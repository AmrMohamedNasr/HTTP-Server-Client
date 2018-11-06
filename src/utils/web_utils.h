/*
 * web_utils.h
 *
 *  Created on: Nov 5, 2018
 *      Author: amrnasr
 */

#ifndef SRC_UTILS_WEB_UTILS_H_
#define SRC_UTILS_WEB_UTILS_H_

string recv_headers(int socket);
string recv_data(int socket, int num_bytes);

#endif /* SRC_UTILS_WEB_UTILS_H_ */
