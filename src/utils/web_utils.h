/*
 * web_utils.h
 *
 *  Created on: Nov 5, 2018
 *      Author: amrnasr
 */

#ifndef SRC_UTILS_WEB_UTILS_H_
#define SRC_UTILS_WEB_UTILS_H_

string recv_headers_chunk(int socket, int size, char *rem_data, int *rem_size);
string recv_headers(int socket);
string recv_data(int socket, int num_bytes);
int recv_data_bytes(int socket, int num_bytes, char * buff);
bool send_data(int socket, string data);
bool send_data(int socket, void * data, int size);

#endif /* SRC_UTILS_WEB_UTILS_H_ */
