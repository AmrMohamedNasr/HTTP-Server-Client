/*
 * response.cpp
 *
 *  Created on: Nov 5, 2018
 *      Author: amrnasr
 */
#include "response.h"
#include <vector>
#include <string>
#include "../utils/string_utils.h"
using namespace std;
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

Response::Response(string command) {
	vector<string> lines = vector<string>();
	lines = split_string(command, "\r\n");
	vector<string> request_words = vector<string>();
	request_words = split_string(lines[0], " ");
	this->headers = map<string, string> ();
	if (request_words[0] == "HTTP/1.0") {
		this->protocol = HTTP1_0;
	} else if (request_words[0] == "HTTP/1.1") {
		this->protocol = HTTP1_1;
	}
	this->statusCode = atoi(request_words[1].c_str());
	string msg = "";
	for (int i = 2; i < request_words.size(); i++) {
		msg += request_words[i];
		if (i != request_words.size() - 1) {
			msg += " ";
		}
	}
	this->statusMessage = msg;
	unsigned int i = 1;
	for (; i < lines.size() && lines[i] != "\r\n"; i++) {
		request_words = split_string(lines[i], ":", 2);
		trim(request_words[0]);
		trim(request_words[1]);
		this->headers[request_words[0]] = request_words[1];
	}
	for(; i < lines.size(); i++){
		this->data += lines[i];
	}
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
