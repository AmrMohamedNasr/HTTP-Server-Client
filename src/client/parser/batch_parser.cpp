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


