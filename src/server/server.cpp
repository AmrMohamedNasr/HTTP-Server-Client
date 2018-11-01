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

using namespace std;

#define RETRIES	3
#define	MAX_CONNECTIONS	100
#define RCVBUFSIZE 32

void handleClient(int socket) {
	char echoBuffer[RCVBUFSIZE];
	int recvMsgSize;
	/* Buffer for echo string */
	/* Size of received message */
	/* Receive message from client */
	if ((recvMsgSize = recv(socket, echoBuffer, RCVBUFSIZE, 0)) < 0) {
		perror("recv() failed");
		return;
	}
	/* Send received string and receive again until end of transmission */
	while (recvMsgSize > 0) /* zero indicates end of transmission */
	{
		cout << string(echoBuffer);
		/*
		if (send(socket, echoBuffer, recvMsgSize, 0) != recvMsgSize) {
			perror("send() failed");
			return;
		}
		*/
		if ((recvMsgSize = recv(socket, echoBuffer, RCVBUFSIZE, 0)) < 0) {
			perror("recv() failed");
			return;
		}
	}
	close(socket);
}

void Server::start_server(int port) {
	int ntry = 0;
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
	cout << "Port " << port << " is listening and ready." << endl;
	struct sockaddr_in clientAddr;
	unsigned int clientLen = sizeof(clientAddr);
	int clientSocket;
	vector<thread> working_threads;
	for (;;) {
		if ((clientSocket = accept(listenSocket, (struct sockaddr *) &clientAddr,&clientLen)) < 0) {
			perror("Socket Accepting");
			cout << "Could not accept connection !" << endl;
			break;
		}
		printf("Handling client %s\n", inet_ntoa(clientAddr.sin_addr));
		thread worker(handleClient, clientSocket);
		worker.detach();
	}
	close(listenSocket);
}

