/*
 * network_enums.h
 *
 *  Created on: Oct 31, 2018
 *      Author: amrnasr
 */

#ifndef SRC_WEB_MODELS_NETWORK_ENUMS_H_
#define SRC_WEB_MODELS_NETWORK_ENUMS_H_
#include <string>

using namespace std;

enum REQUEST_TYPE {GET, POST};
enum PROTOCOL {HTTP1_0, HTTP1_1};

inline string request_to_string(REQUEST_TYPE type) {
	if (type == GET) {
		return "GET";
	} else if (type == POST) {
		return "POST";
	}
	return "Undefined";
}

inline string protocol_to_string(PROTOCOL proto) {
	if (proto == HTTP1_0) {
		return "HTTP/1.0";
	} else if (proto == HTTP1_1) {
		return "HTTP/1.1";
	}
	return "Undefined";
}

#endif /* SRC_WEB_MODELS_NETWORK_ENUMS_H_ */
