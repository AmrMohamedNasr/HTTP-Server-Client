/*
 * server.cpp
 *
 *  Created on: Nov 1, 2018
 *      Author: amrnasr
 */

#include "server.h"
#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <unistd.h>
#include <csignal>
#include "client_worker.h"

using namespace std;

#define RETRIES	3
#define	MAX_CONNECTIONS	1000
#define TIMEOUT	120
#define	MAX_WORKERS	16

static vector<ClientWorker *> workers;

inline int diff_millis(struct timeval t2, struct timeval t1) {
	return (t2.tv_sec - t1.tv_sec) * 1000 + (t2.tv_usec - t1.tv_usec)/1000;
}

void clean_workers() {
	struct timeval cur;
	gettimeofday(&cur, NULL);
	double k, max = 0;
	ClientWorker *cli, *maxC = 0;
	int maxI = -1, items_removed = 0;
	for (int i = workers.size() - 1; i >= 0; i--) {
		cli = workers[i];
		k = diff_millis(cur, cli->getLatestTime());
		if (cli->isFinished()) {
			cli->kill_thread();
			workers.erase(workers.begin() + i);
			items_removed++;
			delete cli;
		} else {
			if (k > max) {
				items_removed = 0;
				maxI = i;
				max = k;
				maxC = cli;
			}
		}
	}
	if (workers.size() > MAX_WORKERS) {
		if (maxI != -1) {
			maxC->kill_thread();
			workers.erase(workers.begin() + maxI - items_removed);
			delete maxC;
		}
	}
}

void kill_workers() {
	for (unsigned int i = workers.size() - 1; i >= 0; i--) {
		workers[i]->kill_thread();
		delete workers[i];
	}
}

void my_handler(int s){
	kill_workers();
	printf("Closed Server%d\n",s);
	exit(1);
}

void Server::start_server(int port) {
	int ntry = 0;
	workers = vector<ClientWorker *>();
	int listenSocket;
	while((listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0 && ntry < RETRIES) {
		ntry++;
		cout << "Failed to open listening port. Retrying again for " << ntry << " time of " << RETRIES << " retries !" << endl;
	}
	if (listenSocket < 0) {
		perror("Socket creation");
		cout << "Could not open listening port. Ending program !" << endl;
		return;
	}
	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0 , sizeof(sockaddr_in));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(port);
	ntry = 0;
	int success = -1;
	while((success = bind(listenSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr))) < 0 && ntry < RETRIES) {
		ntry++;
		cout << "Failed to bind listening port. Retrying again for " << ntry << " time of " << RETRIES << " retries !" << endl;
	}
	if (success < 0) {
		perror("bind");
		cout << "Could not bind listening port. Ending program !" << endl;
		return;
	}
	int enable = 1;
	if (setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
		perror("setsockopt(SO_REUSEADDR) failed");
	}
	ntry = 0;
	while((success = listen(listenSocket, MAX_CONNECTIONS)) < 0 && ntry < RETRIES) {
		ntry++;
		cout << "Failed to listen on port. Retrying again for " << ntry << " time of " << RETRIES << " retries !" << endl;
	}
	if (success < 0) {
		perror("listen");
		cout << "Could not listen on port. Ending program !" << endl;
		return;
	}
	struct sigaction sigIntHandler;

	sigIntHandler.sa_handler = my_handler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;

	sigaction(SIGINT, &sigIntHandler, NULL);
	cout << "Port " << port << " is listening and ready." << endl;
	struct sockaddr_in clientAddr;
	unsigned int clientLen = sizeof(clientAddr);
	int clientSocket;
	for (;;) {
		clean_workers();
		if ((clientSocket = accept(listenSocket, (struct sockaddr *) &clientAddr,&clientLen)) < 0) {
			perror("Socket Accepting");
			cout << "Could not accept connection !" << endl;
			break;
		}
		if (setsockopt(clientSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
		    perror("setsockopt(SO_REUSEADDR) failed");
		}
		//cout << "Handling client " <<  inet_ntoa(clientAddr.sin_addr) << " " << clientSocket << endl;
		ClientWorker * worker = new ClientWorker(clientSocket, TIMEOUT * 1000);
		workers.push_back(worker);
		worker->start_serving();
	}
	close(listenSocket);
}
