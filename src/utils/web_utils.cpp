/*
 * web_utils.cpp
 *
 *  Created on: Nov 5, 2018
 *      Author: amrnasr
 */
#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <unistd.h>
#include "string_utils.h"

using namespace std;

string recv_headers_chunk(int socket, int size, char *rem_data, int *rem_size, string last_time_buffered) {
	char echoBuffer[size + 1];
	memset(echoBuffer, 0, size);
	string request = last_time_buffered;
	string d = "\r\n\r\n";
	int recvMsgSize = 0;
	if (!ends_with(last_time_buffered, d) && last_time_buffered.find(d) == string::npos) {
		if ((recvMsgSize = recv(socket, echoBuffer, size, 0)) < 0) {
			perror(("recv 1 () failed " + to_string(socket)).c_str() );
			return "";
		}
	}
	echoBuffer[size] = '\0';
	string temp(echoBuffer);
	string k = request + temp;
	while (!ends_with(k, d) && k.find(d) == string::npos && recvMsgSize > 0) {
		request.append(echoBuffer);
		memset(echoBuffer, 0, size);
		if ((recvMsgSize = recv(socket, echoBuffer, size, 0)) < 0) {
			perror("recv 2 () failed");
			return "";
		}
		echoBuffer[size] = '\0';
		temp = string(echoBuffer);
		k = request + temp;
	}
	size_t found = k.find(d);
	if (found == string::npos) {
		request.append(temp);
		*rem_size = 0;
	} else {
		memcpy(rem_data, echoBuffer + found - request.size() + d.size(), recvMsgSize - found + request.size() - d.size());
		*rem_size = recvMsgSize - found + request.size() - d.size();
		request = k.substr(0, found + d.size());
	}
	return request;
}
string recv_headers(int socket) {
	char buff;
	int d;
	return recv_headers_chunk(socket, 1, &buff, &d, "");
}

string recv_data(int socket, int num_bytes) {
	char echoBuffer[num_bytes + 1];
	string data= string();
	int recvMsgSize;
	memset(echoBuffer, 0, num_bytes + 1);
	if ((recvMsgSize = recv(socket, echoBuffer, num_bytes, 0)) < 0) {
		perror("Recv data failed");
		return "";
	}
	data.append(echoBuffer);
	return data;
}
int recv_data_bytes(int socket, int num_bytes, char * buff) {
	int recvMsgSize;
	memset(buff, 0, num_bytes);
	if ((recvMsgSize = recv(socket, buff, num_bytes, 0)) < 0) {
		perror("Recv data failed");
		return 0;
	}
	return recvMsgSize;
}
bool send_data(int socket, string data) {
	if (send(socket, data.c_str(), data.size(), 0) < 0) {
		perror("Sending Data Error");
		return false;
	}
	return true;
}

bool send_data(int socket, void * data, int size) {
	if ((send(socket, data, size, 0)) < 0) {
		perror("Sending Data Error");
		return false;
	}
	return true;
}

