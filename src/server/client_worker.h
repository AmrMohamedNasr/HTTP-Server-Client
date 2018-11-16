/*
 * client_worker.h
 *
 *  Created on: Nov 6, 2018
 *      Author: amrnasr
 */

#ifndef SRC_SERVER_CLIENT_WORKER_H_
#define SRC_SERVER_CLIENT_WORKER_H_

#include <ctime>
#include <thread>
#include <sys/time.h>

using namespace std;

class ClientWorker {
	private:
		int socket;
		double timeout;
		thread thd;
	public:
		struct timeval time;
		volatile bool finish_work;
		ClientWorker(int socket, unsigned long timeout);
		void start_serving();
		void kill_thread();
		struct timeval getLatestTime();
		double getTimeout();
		bool isFinished();
		int getSocket() {
			return socket;
		}
};

void handleClient(int socket, ClientWorker *worker);

#endif /* SRC_SERVER_CLIENT_WORKER_H_ */
