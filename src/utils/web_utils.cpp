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

#define RCVBUFSIZE 1

string recv_headers(int socket) {
	char echoBuffer[RCVBUFSIZE];
	string request = string();
	int recvMsgSize;
	if ((recvMsgSize = recv(socket, echoBuffer, RCVBUFSIZE, 0)) < 0) {
		perror("recv() failed");
		return "";
	}
	request.append(echoBuffer);
	while (!ends_with(request,"\r\n\r\n") && recvMsgSize > 0) {
		memset(echoBuffer, 0, RCVBUFSIZE);
		if ((recvMsgSize = recv(socket, echoBuffer, RCVBUFSIZE, 0)) < 0) {
			perror("recv() failed");
			return "";
		}
		request.append(echoBuffer);
	}
	return request;
}

string recv_data(int socket, int num_bytes) {
	char echoBuffer[num_bytes + 1];
	string data= string();
	int recvMsgSize;
	memset(echoBuffer, 0, num_bytes + 1);
	if ((recvMsgSize = recv(socket, echoBuffer, num_bytes, 0) < 0)) {
		perror("Recv data failed");
		return "";
	}
	data.append(echoBuffer);
	return data;
}


