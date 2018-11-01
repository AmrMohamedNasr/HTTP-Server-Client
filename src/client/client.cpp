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


using namespace std;

void Client::start_client(int port, char *server_ip) {
	int fileDesc, portNo, n;
	struct sockaddr_in serverAdd;
	struct hostent *server;
	char buffer[256];
	map <string, int> myConnections;
	//returns pointer to hostent
	server = gethostbyname(server_ip);
	if (server == NULL) {
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}





}





