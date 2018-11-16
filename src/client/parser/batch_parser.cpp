/*

 * patch_parser.cpp
 *
 *  Created on: Nov 1, 2018
 *      Author: michael
 */
#include "batch_parser.h"
#include "../../file_system/file_handler.h"
#include <fstream>
#include <sstream>
#include <iostream>

vector<string> split_spaces(string str) {
	string buf;
	stringstream strs(str);
	vector<string> tokens;
	while (strs >> buf) {
		tokens.push_back(buf);
	}
	return tokens;
}

bool BatchParser:: read_input(string file) {
	ifstream ifstr;
	ifstr.open(file);
	if (!ifstr) {
		return false;
	}
	string line, s="";

	while (getline(ifstr, line)) {
		vector<string> words = split_spaces(line);
		if (!(words.size() == 4 || words.size() == 3)){
			cout << "Invalid command format" << endl;
			return false;
		}
		if (words[0] == "GET" || words[0] == "POST"){
			s = words[0]+" "+words[1]+" "+"HTTP/1.1"+"\r\n";
			Request req = Request(s);
			req.addHeader("host",words[2]);
			if(words.size() == 3)
				requests.push_back(RequestAndPortNo(req));
			else
				requests.push_back(RequestAndPortNo(req,stoi(words[3])));
		}else cout << "Invalid command format" << endl;
	}
	return true;
}

bool BatchParser::has_next() {
	return !(requests.empty());
}

Request BatchParser::next() {
	Request req = requests.front();
	requests.erase(requests.begin());
	return req;
}


