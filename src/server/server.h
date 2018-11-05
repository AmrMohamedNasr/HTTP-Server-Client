#ifndef	SERVER_H
#define	SERVER_H

class Server {
	public:
		void start_server(int port);
};

void handleClient(int socket);

#endif
