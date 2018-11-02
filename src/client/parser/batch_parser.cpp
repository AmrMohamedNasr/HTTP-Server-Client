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
	string line;
	while (getline(ifstr, line)) {
		vector<string> words = split_spaces(line);
		Request req = Request();
		if(words.size() == 4 || words.size() == 3 ) {
			if(words[0] == "GET") {

			} else if (words[0] == "POST") {

			} else {
				cout << "Invalid command format" << endl;
				return false;
			}
		} else {
			cout << "Invalid command format" << endl;
			return false;
		}
		//TODO
	}
	// TODO
	return true;
}



bool BatchParser::has_next() {
	// To be implemented.
	return true;
}

Request BatchParser::next() {
	// To be implemented.
	return Request();
}


