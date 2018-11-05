/*
 * client.cpp
 *
 *  Created on: Nov 1, 2018
 *      Author: michael
 */
#include "client.h"
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

void Client::start_client(int port, char *server_ip, string file) {
	int listenSocket;
	int ntry = 0;
	struct sockaddr_in serverAdd;
	char echoBuffer[RCVBUFSIZE];
	map <string, int> myConnections;
	/* Create a reliable, stream socket using TCP */
	while((listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0 && ntry < RETRIES) {
		ntry++;
		cout << "Failed to open listening port. Retrying again for " << ntry << " time of " << RETRIES << " retries !" << endl;
	}


	memset(&serverAdd, 0 , sizeof(sockaddr_in));
	serverAdd.sin_family = AF_INET;
	serverAdd.sin_addr.s_addr = htonl(INADDR_ANY);
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

	/// this part should be in another function ///
	int stringLen;
	char *stringToSend = "e";
	stringLen = strlen(stringToSend);

	/* Send the string to the server */
	ntry = 0;
	success = -1;
	while ((success = send(listenSocket, stringToSend, stringLen, 0))!= stringLen && ntry < RETRIES) {
		ntry++;
		cout << "Failed to send to server. Retrying again for " << ntry << " time of " << RETRIES << " retries !" << endl;
	}
	if (success < 0) {
		perror("send");
		cout << "Could not send to server. Ending program !" << endl;
		return;
	}

	unsigned int bytesRcvd, totalBytesRcvd;

	while (totalBytesRcvd < stringLen)
	{
		if ((bytesRcvd = recv(listenSocket, echoBuffer, RCVBUFSIZE - 1, 0)) <= 0) {
			perror("recv() failed");
			return;
		}
		totalBytesRcvd += bytesRcvd;
		echoBuffer[bytesRcvd] = '\0';
		cout << echoBuffer << endl;
	}

	close(listenSocket);
	exit(0);




}





