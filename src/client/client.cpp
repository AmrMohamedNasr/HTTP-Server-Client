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
#include "../web_models/network_enums.h"
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
map <string, int> myConnections;
vector <pair<int, struct sockaddr_in>> socket_map;


bool connect_server(int listenSocket,struct sockaddr_in serverAdd ) {
	int ntry = 0;
	int success = -1;
	while((success = connect(listenSocket, (struct sockaddr *) &serverAdd, sizeof(serverAdd))) < 0 && ntry < RETRIES) {
		ntry++;
		cout << "Failed to connect to server. Retrying again for " << ntry << " time of " << RETRIES << " retries !" << endl;
	}
	if (success < 0) {
		perror("connect");
		cout << "Could not connect to server. Ending program !" << endl;
		return false;
	}
	cout << " is connected to server and ready." << endl;
	return true;
}
bool send_message(int listenSocket, struct sockaddr_in serverAdd, Request req, bool first ) {
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
		if (first && connect_server(listenSocket, serverAdd)) {
			return send_message(listenSocket, serverAdd, req, false);
		} else {
			perror("send");
			cout << "Could not send to server. Ending program !" << endl;
			return false;
		}
	}
	return true;
}

bool receive_response(int listenSocket, struct sockaddr_in serverAdd, bool first, string flag) {
	string resp_string = recv_headers(listenSocket);
	if (resp_string == "") {
		if (first && connect_server(listenSocket, serverAdd)) {
			return receive_response(listenSocket, serverAdd, false, flag);
		}
		perror("receive");
		cout << "Could not receive from server. Ending program !" << endl;
		return false;
	}
	Response resp = Response(resp_string);
	if (!resp.hasHeader("Content-Length")) {
		perror("receive");
		cout << "Could not receive Content-length from server. Ending program !" << endl;
		return false;
	}
	first = true;
	if (flag == "GET") {
		string data_string = recv_data(listenSocket,
			atoi(resp.getHeaderValue("Content-Length").c_str()));
		if (data_string == "") {
			if (first && connect_server(listenSocket, serverAdd)) {
				data_string = recv_data(listenSocket,
						atoi(resp.getHeaderValue("Content-Length").c_str()));
				if (data_string == "") {
					perror("receive");
					cout << "Could not receive from server. Ending program !" << endl;
					return false;
				}
			} else {
				perror("receive");
				cout << "Could not receive from server. Ending program !" << endl;
				return false;
			}
		}
		resp.setData(data_string);
		// TODO write to file
	}
	cout << resp.format_response() << endl;
	return true;
}

void create_Socket(RequestAndPortNo req_data, int *listenSocket, struct sockaddr_in *serverAdd) {
	struct in_addr * inAdd;
	hostent * record;
	Request req = req_data.getRequest();
	if (!req.hasHeader("host")) {
		cout << "Failed to fetch host name. Ending program !" << endl;
		return;
	} else {
		record = gethostbyname(req.getHeaderValue("host").c_str());
		inAdd = (struct in_addr *)record->h_addr_list[0];
	}
	string key = req.getHeaderValue("host").c_str() + to_string(req_data.getPortNo());
	if (myConnections.find(req.getHeaderValue("host").c_str()) != myConnections.end()) {
		*listenSocket = myConnections.find(key)->second;
	} else {
		int ntry = 0;
		/* Create a reliable, stream socket using TCP */
		while((*listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0 && ntry < RETRIES) {
			ntry++;
			cout << "Failed to open listening port. Retrying again for " << ntry << " time of " << RETRIES << " retries !" << endl;
		}

		memset(serverAdd, 0 , sizeof(sockaddr_in));
		serverAdd->sin_family = AF_INET;
		serverAdd->sin_addr.s_addr = inet_addr(inet_ntoa((*inAdd)));
		serverAdd->sin_port = htons(req_data.getPortNo());

		connect_server(*listenSocket, *serverAdd);
		myConnections.insert(pair<string, int>(key, *listenSocket));
	}
}


void Client::start_client(int port, char *server_ip, string file) {
	int listenSocket;
	BatchParser parser;
	if (!parser.read_input(file)) {
		perror("File reading");
		cout << "Could not read requests. Ending program !" << endl;
		return;
	}
	Request req;
	struct sockaddr_in serverAdd;

	while(parser.has_next()) {
		RequestAndPortNo req_data = parser.next();
		// request to be sent.
		req = req_data.getRequest();
		// format this request.
		bool end = false;
		while(!end && req.getType() == GET) {
			create_Socket(req_data, &listenSocket, &serverAdd);
			socket_map.push_back(pair<int, struct sockaddr_in>(listenSocket, serverAdd));
			if (!send_message(listenSocket, serverAdd, req, true)) {
				perror("send");
				cout << "request not sent" << endl;
				return;
			}
			if (parser.has_next()) {
				req_data = parser.next();
				req = req_data.getRequest();
			} else {
				end = true;
			}
		}
		for(int i=0;i<socket_map.size();i++) {
			if (!receive_response(socket_map[i].first, socket_map[i].second, true, "GET")) {
				perror("receive");
				cout << "response not received from server" << endl;
				return;
			}
		}
		socket_map.clear();
		if (!end && req.getType() == POST) {
			create_Socket(req_data, &listenSocket, &serverAdd);
			if (!send_message(listenSocket, serverAdd, req, true)) {
				perror("send");
				cout << "request not sent to server" << endl;
				return;
			}
			receive_response(listenSocket, serverAdd, true, "POST");
		}

		if (!parser.has_next()) {
			map<string, int>::iterator it = myConnections.begin();
			while(it != myConnections.end()) {
				close(it->second);
			}
		}
	}

	exit(0);




}





