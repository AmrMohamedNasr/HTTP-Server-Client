// Design your client, good luck.
#ifndef	CLIENT_H
#define	CLIENT_H

#include <sys/socket.h>
class Client {
	public:
		void start_client(int port, char *server_ip);
};

#endif
