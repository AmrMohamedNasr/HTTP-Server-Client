/*
 * response.cpp
 *
 *  Created on: Nov 5, 2018
 *      Author: amrnasr
 */
#include "response.h"

Response::Response() {
	this->protocol = HTTP1_0;
	this->statusCode = 404;
	this->statusMessage = "";
	this->headers = map<string, string> ();
	this->data = "";
}

Response::Response(int code, PROTOCOL proto, string messageCode) {
	this->protocol = proto;
	this->statusCode = code;
	this->statusMessage = messageCode;
	this->headers = map<string, string> ();
	this->data = "";
}

void Response::addHeader(string key, string value) {
	this->headers[key] = value;
}

string Response::getHeaderValue(string key) {
	return this->headers.find(key)->second;
}

bool Response::hasHeader(string key) {
	return this->headers.find(key) != this->headers.end();
}

string Response::format_response() {
	string s = "";
	if (this->protocol == HTTP1_0) {
		s += "HTTP/1.0 ";
	} else if (this->protocol == HTTP1_1) {
		s += "HTTP/1.1 ";
	}
	s += to_string(this->statusCode) + " ";
	s +=  this->statusMessage + "\r\n";
	map<string, string>::iterator it;
	for (it = this->headers.begin(); it != this->headers.end(); it++) {
		s += it->first + " : " + it->second + "\r\n";
	}
	s += "\r\n";
	return s;
}
