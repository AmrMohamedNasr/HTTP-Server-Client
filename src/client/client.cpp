/*
 * client.cpp
 *
 *  Created on: Nov 1, 2018
 *      Author: michael
 */
#include "client.h"
#include "parser/batch_parser.h"
#include "../utils/web_utils.h"
#include "../web_models/response.h"
#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <unistd.h>
#include <map>
#include <netinet/in.h>
#include <netdb.h>

#define RETRIES 3
#define RCVBUFSIZE 32
using namespace std;
void send_message(int listenSocket, Request req ) {
	string str = req.format_request();
	// get length of request.
	int stringLen = str.length();
	int ntry = 0;
	int success = -1;
	while ((success = send(listenSocket, str.c_str() , stringLen, 0))!= stringLen && ntry < RETRIES) {
		ntry++;
		cout << "Failed to send to server. Retrying again for " << ntry << " time of " << RETRIES << " retries !" << endl;
	}
	if (success < 0) {
		perror("send");
		cout << "Could not send to server. Ending program !" << endl;
		return;
	}


	string resp_string = recv_headers(listenSocket);
	if (resp_string == "") {
		perror("receive");
		cout << "Could not receive response from server. Ending program !" << endl;
		return;
	}
	Response resp = Response(resp_string);
	if (!resp.hasHeader("Content-Length")) {
		perror("receive");
		cout << "Could not receive Content-length from server. Ending program !" << endl;
		return;
	}
	string data_string = recv_data(listenSocket,
			atoi(resp.getHeaderValue("Content-Length").c_str()));
	resp.setData(data_string);
	cout << resp.format_response() << endl;

}


void Client::start_client(int port, char *server_ip, string file) {
	int listenSocket;
	map <string, int> myConnections;
	BatchParser parser;
	if (!parser.read_input(file)) {
		perror("File reading");
		cout << "Could not read requests. Ending program !" << endl;
		return;
	}
	Request req;
	while(parser.has_next()) {
		// request to be sent.
		req = parser.next();
		// format this request.
		if (!req.hasHeader("host")) {
			cout << "Failed to fetch host name. Ending program !" << endl;
			return;
		}
		if (myConnections.find(req.getHeaderValue("host").c_str()) != myConnections.end()) {
			listenSocket = myConnections.find(req.getHeaderValue("host").c_str())->second;
		} else {
			int ntry = 0;
			struct sockaddr_in serverAdd;
			/* Create a reliable, stream socket using TCP */
			while((listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0 && ntry < RETRIES) {
				ntry++;
				cout << "Failed to open listening port. Retrying again for " << ntry << " time of " << RETRIES << " retries !" << endl;
			}

			memset(&serverAdd, 0 , sizeof(sockaddr_in));
			serverAdd.sin_family = AF_INET;
			serverAdd.sin_addr.s_addr = htonl(INADDR_ANY);
			//TODO port number should be given with request using a map.
			serverAdd.sin_port = htons(port);
			ntry = 0;
			int success = -1;

			while((success = connect(listenSocket, (struct sockaddr *) &serverAdd, sizeof(serverAdd))) < 0 && ntry < RETRIES) {
				ntry++;
				cout << "Failed to connect to server. Retrying again for " << ntry << " time of " << RETRIES << " retries !" << endl;
			}
			if (success < 0) {
				perror("connect");
				cout << "Could not connect to server. Ending program !" << endl;
				return;
			}
			cout << "Port " << port << " is connected to server and ready." << endl;
			myConnections.insert(pair<string, int>(req.getHeaderValue("host").c_str(), listenSocket));
		}
		send_message(listenSocket, req);
		//TODO To Be Written in file.

		// TODO This part need to be optimized ..
		// connections are open for long time with nothing useful.
		if (!parser.has_next()) {
			map<string, int>::iterator it = myConnections.begin();
			while(it != myConnections.end()) {
				close(it->second);
			}
		}
	}

	exit(0);




}





