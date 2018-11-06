/*
 * file_handler.cpp
 *
 *  Created on: Nov 1, 2018
 *      Author: amrnasr
 */
#include "file_handler.h"
#include <string>
#include <fstream>
#include <streambuf>

using namespace std;

bool FileHandler::check_file(string path) {
	ifstream t(path);
	if (!t) {
		return false;
	}
	return true;
}

string FileHandler::read_file(string path) {
	ifstream t(path);
	if (!t){
		return "";
	}
	string str((std::istreambuf_iterator<char>(t)),
	                 std::istreambuf_iterator<char>());
	return str;
}

bool FileHandler::write_file(string path, string data) {
	ofstream out(path);
	if (!out) {
		return false;
	}
	out << data;
	return true;
}
