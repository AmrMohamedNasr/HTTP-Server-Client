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
#define RCV_MSG_SIZE	128

void handleClient(int socket) {
	string s;
	char rem_data[RCV_MSG_SIZE];
	int rem_size;
	while ((s = recv_headers_chunk(socket, RCV_MSG_SIZE, rem_data, &rem_size)) != "") {
		if (s.size() == 0) {
			close(socket);
			return;
		}
		Request r = Request(s);
		cout << request_to_string(r.getType()) << " " << r.getUrl() << " " << protocol_to_string(r.getProtocol()) << endl;
		FileHandler handler = FileHandler();
		if (r.getType() == GET) {
			string rel_path = "." + r.getUrl();
			if (handler.check_file(rel_path)) {
				handler.set_read_file(rel_path);
				Response res = Response(200, r.getProtocol(), "OK");
				size_t file_bytes = handler.get_file_size(rel_path);
				res.addHeader("Content-Length", to_string(file_bytes));
				string res_s = res.format_response();
				send_data(socket, res_s);
				size_t bytes_sent = 0;
				char buff[RCV_MSG_SIZE];
				while (bytes_sent < file_bytes) {
					size_t bytes_read_in = handler.read_chunk(RCV_MSG_SIZE, buff);
					send_data(socket, buff, bytes_read_in);
					bytes_sent += bytes_read_in;
				}
			} else {
				Response res = Response(404, r.getProtocol(), "Not Found");
				res.addHeader("Content-Length", to_string(0));
				string res_s = res.format_response();
				send_data(socket, res_s);
			}
		} else {
			string rel_path = "." + r.getUrl();
			if (!r.hasHeader("Content-Length")) {
				Response res = Response(404, r.getProtocol(), "Not Found");
				res.addHeader("Content-Length", to_string(0));
				string res_s = res.format_response();
				send_data(socket, res_s);
			} else {
				size_t len_bytes = stoi(r.getHeaderValue("Content-Length"));
				Response res = Response(200, r.getProtocol(), "OK");
				res.addHeader("Content-Length", to_string(0));
				string res_s = res.format_response();
				send_data(socket, res_s);
				handler.set_write_file(rel_path);
				size_t bytes_recieved = rem_size;
				handler.write_chunk(rem_data, rem_size);
				char buff[RCV_MSG_SIZE];
				while (bytes_recieved < len_bytes) {
					int data_len = recv_data_bytes(socket, RCV_MSG_SIZE, buff);
					handler.write_chunk(buff, data_len);
					bytes_recieved += data_len;
				}
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

