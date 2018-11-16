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

using namespace std;

class ClientWorker {
	private:
		int socket;
		double timeout;
		thread thd;
	public:
		time_t time;
		volatile bool finish_work;
		ClientWorker(int socket, unsigned long timeout);
		void start_serving();
		void kill_thread();
		time_t getLatestTime();
		double getTimeout();
		bool isFinished();
};

void handleClient(int socket, ClientWorker *worker);

#endif /* SRC_SERVER_CLIENT_WORKER_H_ */
