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
#include "../file_system/file_handler.h"
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
#define LIMIT_PIPE	20
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

bool receive_response(int listenSocket, struct sockaddr_in serverAdd, bool first, string flag, Request req, int *code) {
	char rem_data[RCVBUFSIZE];
	int rem_size = 0;
	string resp_string = recv_headers_chunk(listenSocket, RCVBUFSIZE, rem_data, &rem_size, "");
	if (resp_string == "") {
		if (first && connect_server(listenSocket, serverAdd)) {
			return receive_response(listenSocket, serverAdd, false, flag, req,code);
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
	if (flag == "GET" && resp.getStatusCode() == 200) {
		FileHandler fileHandler = FileHandler();
		fileHandler.set_write_file("." + req.getUrl());
		size_t size = atoi(resp.getHeaderValue("Content-Length").c_str());
		size_t bytes_recieved = rem_size;
		fileHandler.write_chunk(rem_data, rem_size);
		char buff[RCVBUFSIZE];
		while (bytes_recieved < size) {
			int ask_for_num = size - bytes_recieved < RCVBUFSIZE ? size - bytes_recieved : RCVBUFSIZE;
			streamsize data_len = recv_data_bytes(listenSocket, ask_for_num, buff);
			fileHandler.write_chunk(buff, data_len);
			bytes_recieved += data_len;
		}
	} else {
		*code = resp.getStatusCode();
	}
	cout << resp.getStatusCode() << " " << resp.getStatusMessage() << endl;
	return true;
}

void create_Socket(int * listenSocket, struct sockaddr_in *serverAdd, int port, struct in_addr * inAdd, string key) {
	int ntry = 0;
	/* Create a reliable, stream socket using TCP */
	while((*listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0 && ntry < RETRIES) {
		ntry++;
		cout << "Failed to open listening port. Retrying again for " << ntry << " time of " << RETRIES << " retries !" << endl;
	}

	memset(serverAdd, 0 , sizeof(sockaddr_in));
	serverAdd->sin_family = AF_INET;
	serverAdd->sin_addr.s_addr = inet_addr(inet_ntoa((*inAdd)));
	serverAdd->sin_port = htons(port);

	connect_server(*listenSocket, *serverAdd);
	cout << "new Connection is established " + key << endl;
	myConnections.insert(pair<string, int>(key, *listenSocket));
}

void get_Socket(RequestAndPortNo req_data, int *listenSocket, struct sockaddr_in *serverAdd) {
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
	string ip(inet_ntoa((*inAdd)));
	string key = ip + ":" + to_string(req_data.getPortNo());
	bool create_new = true;
	if (myConnections.find(key) != myConnections.end()) {

		*listenSocket = myConnections.find(key)->second;
		create_new = false;
	}
	if (create_new) {
		create_Socket(listenSocket, serverAdd, req_data.getPortNo(),inAdd, key);
	}
}


#include "../utils/string_utils.h"

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
		vector<Request> reqs;
		// format this request.
		bool end = false;
		int req_sent_num = 0;
		while(!end && req.getType() == GET) {
			get_Socket(req_data, &listenSocket, &serverAdd);
			reqs.push_back(req_data.getRequest());
			socket_map.push_back(pair<int, struct sockaddr_in>(listenSocket, serverAdd));
			if (!send_message(listenSocket, serverAdd, req, true)) {
				perror("send");
				cout << "request not sent" << endl;
				return;
			}
			cout << "GET " << req.getUrl() << endl;
			req_sent_num++;
			if (req_sent_num == LIMIT_PIPE) {
				int code;
				for (unsigned int i=0;i<socket_map.size();i++) {
					if (!receive_response(socket_map[i].first, socket_map[i].second, true, "GET", reqs[i], &code)) {
						perror("receive");
						cout << "response not received from server" << endl;
						return;
					}
				}
				socket_map.clear();
				reqs.clear();
				req_sent_num = 0;
			}
			if (parser.has_next()) {
				req_data = parser.next();
				req = req_data.getRequest();
			} else {
				end = true;
			}
		}
		int code;
		for (unsigned int i=0;i<socket_map.size();i++) {
			if (!receive_response(socket_map[i].first, socket_map[i].second, true, "GET", reqs[i], &code)) {
				perror("receive");
				cout << "response not received from server" << endl;
				return;
			}
		}
		socket_map.clear();
		reqs.clear();
		if (!end && req.getType() == POST) {
			std::ifstream t("." + req.getUrl());
			std::string str((std::istreambuf_iterator<char>(t)),
							 std::istreambuf_iterator<char>());
			get_Socket(req_data, &listenSocket, &serverAdd);
			req.addHeader("Content-Length", to_string(str.size()));
			if (!send_message(listenSocket, serverAdd, req, true)) {
				perror("send");
				cout << "request not sent to server" << endl;
				return;
			}
			cout << "POST " << req.getUrl() << endl;
			receive_response(listenSocket, serverAdd, true, "POST", req, &code);
			if (code == 200) {
				if (!send_data(listenSocket, str)) {
					perror("send");
					cout << "data not sent to server" << endl;
					return;
				}
			}
		}

		if (!parser.has_next()) {
			map<string, int>::iterator it = myConnections.begin();
			while(it != myConnections.end()) {
				close(it->second);
				it++;
			}
		}
	}
	exit(0);
}





