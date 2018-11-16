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

string recv_headers_chunk(int socket, int size, char *rem_data, int *rem_size) {
	char echoBuffer[size];
	string request = string();
	int recvMsgSize;
	if ((recvMsgSize = recv(socket, echoBuffer, size, 0)) < 0) {
		perror("recv() failed");
		return "";
	}
	string temp(echoBuffer);
	string d = "\r\n\r\n";
	while (!ends_with(request + temp, d) && temp.rfind(d) == string::npos && recvMsgSize > 0) {
		request.append(echoBuffer);
		memset(echoBuffer, 0, size);
		if ((recvMsgSize = recv(socket, echoBuffer, size, 0)) < 0) {
			perror("recv() failed");
			return "";
		}
		temp = string(echoBuffer);
	}
	size_t found = temp.rfind(d);
	if (found == string::npos) {
		request.append(temp);
		*rem_size = 0;
	} else {
		request.append(temp.substr(0, found + d.size()));
		memcpy(rem_data, echoBuffer + found + d.size(), recvMsgSize - found - d.size());
		*rem_size = recvMsgSize - found - d.size();
	}
	return request;
}
string recv_headers(int socket) {
	char buff;
	int d;
	return recv_headers_chunk(socket, 1, &buff, &d);
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

