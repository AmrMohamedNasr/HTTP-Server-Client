/*
 * server.cpp
 *
 *  Created on: Nov 1, 2018
 *      Author: amrnasr
 */

#include "server.h"
#include "../file_system/file_handler.h"
#include "../web_models/request.h"
#include "../web_models/response.h"
#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <unistd.h>

#include "../utils/string_utils.h"
#include "../utils/web_utils.h"

using namespace std;

#define RETRIES	3
#define	MAX_CONNECTIONS	100


void handleClient(int socket) {
	string s;
	while ((s = recv_headers(socket)) != "") {
		if (s.size() == 0) {
			close(socket);
			return;
		}
		Request r = Request(s);
		FileHandler handler = FileHandler();
		if (r.getType() == GET) {
			string rel_path = "." + r.getUrl();
			if (handler.check_file(rel_path)) {
				string data = handler.read_file(rel_path);
				Response res = Response(200, r.getProtocol(), "OK");
				res.addHeader("Content-Length", to_string(data.size()));
				string res_s = res.format_response();
				if (send(socket, res_s.c_str(), res_s.size(), 0) < 0) {
					perror("Response Header Error");
				}
				if (send(socket, data.c_str(), data.size(), 0) < 0) {
					perror("Response Data Error");
				}
			} else {
				Response res = Response(404, r.getProtocol(), "Not Found");
				res.addHeader("Content-Length", to_string(0));
				string res_s = res.format_response();
				if (send(socket, res_s.c_str(), res_s.size(), 0) < 0) {
					perror("Response Error");
				}
			}
		} else {
			string rel_path = "." + r.getUrl();
			for (auto it = r.getHeaders().begin(); it != r.getHeaders().end(); it++) {
				cout << it->first << " " << it->second << endl;
			}
			if (!r.hasHeader("Content-Length")) {
				Response res = Response(404, r.getProtocol(), "Not Found");
				res.addHeader("Content-Length", to_string(0));
				string res_s = res.format_response();
				if (send(socket, res_s.c_str(), res_s.size(), 0) < 0) {
					perror("Response Error");
				}
			} else {
				int len_bytes = stoi(r.getHeaderValue("Content-Length"));
				cout << len_bytes << endl;
				Response res = Response(200, r.getProtocol(), "OK");
				res.addHeader("Content-Length", to_string(0));
				string res_s = res.format_response();
				if (send(socket, res_s.c_str(), res_s.size(), 0) < 0) {
					perror("Response Header Error");
				}
				string data = recv_data(socket, len_bytes);
				handler.write_file(rel_path, data);
			}
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
		cout << "Handling client " <<  inet_ntoa(clientAddr.sin_addr) << " " << clientSocket << endl;
		thread worker(handleClient, clientSocket);
		worker.detach();
	}
	close(listenSocket);
}

