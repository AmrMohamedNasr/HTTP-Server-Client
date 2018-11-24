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
#include <mutex>

using namespace std;

class ClientWorker {
	private:
		int socket;
		double timeout;
		thread thd;
	public:
		struct timeval time;
		volatile bool finish_work;
		volatile bool wait_for_job;
		mutex close_seq;
		ClientWorker(int socket, unsigned long timeout);
		void start_serving();
		void kill_thread();
		struct timeval getLatestTime();
		double getTimeout();
		void setTimeout(double tmeOut);
		bool isFinished();
		int getSocket() {
			return socket;
		}
};

void handleClient(int socket, ClientWorker *worker);

#endif /* SRC_SERVER_CLIENT_WORKER_H_ */
