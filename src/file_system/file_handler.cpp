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

void FileHandler::set_read_file(string path) {
	this->reader.set_file(path);
}

void FileHandler::set_write_file(string path) {
	this->writer.set_file(path);
}

size_t FileHandler::read_chunk(int size, char * buff) {
	return this->reader.read_chunk(size, buff);
}

size_t FileHandler::get_file_size(string path) {
	ifstream in(path, ifstream::ate | ifstream::binary);
	if (!in) {
		return 0;
	}
	return in.tellg();
}
bool FileHandler::write_chunk(char * chunk, streamsize size) {
	return this->writer.write_chunk(chunk, size);
}
