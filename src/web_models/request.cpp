/*
 * request.cpp
 *
 *  Created on: Nov 2, 2018
 *      Author: michael
 */
#include "request.h"
#include <map>
#include <string>

string Request::getHeaderValue(string key) {

}

Request::Request() {

}

 *  Created on: Nov 4, 2018
 *      Author: amrnasr
 */
#include <vector>
#include "request.h"

#include "../utils/string_utils.h"

Request::Request() {
	this->protocol = HTTP1_0;
	this->type = GET;
	this->url = "";
	this->headers = map<string, string> ();
	this->data = "";
}

Request::Request(string command) {
	vector<string> lines = vector<string>();
	lines = split_string(command, "\r\n");
	vector<string> request_words = vector<string>();
	request_words = split_string(lines[0], " ");
	this->headers = map<string, string> ();
	if (request_words[0] == "GET") {
		this->type = GET;
	} else if (request_words[0] == "POST") {
		this->type = POST;
	}
	this->url = request_words[1];
	if (request_words[2] == "HTTP/1.0") {
		this->protocol = HTTP1_0;
	} else if (request_words[2] == "HTTP/1.1") {
		this->protocol = HTTP1_1;
	}
	for (unsigned int i = 1; i < lines.size(); i++) {
		request_words = split_string(lines[i], ":", 2);
		trim(request_words[0]);
		trim(request_words[1]);
		this->headers[request_words[0]] = request_words[1];
	}
	this->data = "";
}

void Request::addHeader(string key, string value) {
	this->headers[key] = value;
}

string Request::getHeaderValue(string key) {
	return this->headers.find(key)->second;
}

bool Request::hasHeader(string key) {
	return this->headers.find(key) != this->headers.end();
}
string Request::format_request() {
	string s = "";
	if (this->type == GET) {
		s += "GET ";
	} else if (this->type == POST) {
		s += "POST ";
	}
	s +=  this->url + " ";
	if (this->protocol == HTTP1_0) {
		s += "HTTP/1.0\r\n";
	} else if (this->protocol == HTTP1_1) {
		s += "HTTP/1.1\r\n";
	}
	map<string, string>::iterator it;
	for (it = this->headers.begin(); it != this->headers.end(); it++) {
		s += it->first + " : " + it->second + "\r\n";
	}
	s += "\r\n";
	return s;
}
