/*
 * client_worker.cpp
 *
 *  Created on: Nov 6, 2018
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
#include <csignal>
#include <sys/ioctl.h>
#include "client_worker.h"

#include "../utils/string_utils.h"
#include "../utils/web_utils.h"
#include "../file_system/file_handler.h"
#include "../web_models/request.h"
#include "../web_models/response.h"

using namespace std;

#define RCV_MSG_SIZE	512

void handleClient(int socket, ClientWorker *worker) {
	string s;
	char rem_data[RCV_MSG_SIZE];
	int rem_size = 0;
	time_t cur;
	while ((s = recv_headers_chunk(socket, RCV_MSG_SIZE, rem_data, &rem_size)) != "") {
		time(&worker->time);
		if (s.size() == 0) {
			close(socket);
			return;
		}
		Request r = Request(s);
		cout << request_to_string(r.getType()) << " " << r.getUrl() << " " << protocol_to_string(r.getProtocol()) << endl;
		FileHandler handler = FileHandler();
		if (r.getType() == GET) {
			string rel_path = "." + r.getUrl();
			if (handler.check_file(rel_path)) {
				handler.set_read_file(rel_path);
				Response res = Response(200, r.getProtocol(), "OK");
				size_t file_bytes = handler.get_file_size(rel_path);
				res.addHeader("Content-Length", to_string(file_bytes));
				res.addHeader("Connection", "Keep-Alive");
				string res_s = res.format_response();
				size_t bytes_sent = 0;
				char buff[RCV_MSG_SIZE];
				send_data(socket, res_s);
				while (bytes_sent < file_bytes) {
					size_t bytes_read_in = handler.read_chunk(RCV_MSG_SIZE, buff);
					if (bytes_read_in == 0) {
						break;
					}
					send_data(socket, buff, bytes_read_in);
					bytes_sent += bytes_read_in;
				}
			} else {
				Response res = Response(404, r.getProtocol(), "Not Found");
				res.addHeader("Content-Length", to_string(0));
				res.addHeader("Connection", "Keep-Alive");
				string res_s = res.format_response();
				send_data(socket, res_s);
			}
		} else {
			string rel_path = "." + r.getUrl();
			if (!r.hasHeader("Content-Length")) {
				Response res = Response(404, r.getProtocol(), "Not Found");
				res.addHeader("Content-Length", to_string(0));
				res.addHeader("Connection", "Keep-Alive");
				string res_s = res.format_response();
				send_data(socket, res_s);
			} else {
				size_t len_bytes = stoi(r.getHeaderValue("Content-Length"));
				Response res = Response(200, r.getProtocol(), "OK");
				res.addHeader("Content-Length", to_string(0));
				res.addHeader("Connection", "Keep-Alive");
				string res_s = res.format_response();
				send_data(socket, res_s);
				handler.set_write_file(rel_path);
				size_t bytes_recieved = rem_size;
				handler.write_chunk(rem_data, rem_size);
				char buff[RCV_MSG_SIZE];
				while (bytes_recieved < len_bytes) {
					streamsize data_len = recv_data_bytes(socket, RCV_MSG_SIZE, buff);
					if (data_len == 0) {
						break;
					}
					handler.write_chunk(buff, data_len);
					bytes_recieved += data_len;
				}
			}
		}
		int bytes_available;
		ioctl(socket,FIONREAD,&bytes_available);
		while (bytes_available == 0) {
			time(&cur);
			if (worker->finish_work || difftime(cur, worker->getLatestTime()) >= worker->getTimeout()) {
				worker->finish_work = true;
				close(socket);
				return;
			}
			ioctl(socket,FIONREAD,&bytes_available);
		}
	}
}

ClientWorker::ClientWorker(int socket, unsigned long timeOut) {
	this->socket = socket;
	std::time(&(this->time));
	this->finish_work = false;
	this->timeout = timeOut;
}

void ClientWorker::start_serving() {
	this->thd = thread(handleClient, this->socket, this);
}

void ClientWorker::kill_thread() {
	this->finish_work = true;
	this->thd.join();
	close(this->socket);
}

time_t ClientWorker::getLatestTime() {
	return this->time;
}

double ClientWorker::getTimeout() {
	return this->timeout;
}

bool ClientWorker::isFinished() {
	return this->finish_work;
}
