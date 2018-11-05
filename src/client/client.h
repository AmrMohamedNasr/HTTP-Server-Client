// Design your client, good luck.
#ifndef	CLIENT_H
#define	CLIENT_H
#include <iostream>
#include <string>
#include <sys/socket.h>
using namespace std;
class Client {
	public:
		void start_client(int port, char *server_ip, string file);
};

#endif
