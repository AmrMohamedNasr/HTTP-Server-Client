/*
 * file_reader.cpp
 *
 *  Created on: Nov 6, 2018
 *      Author: amrnasr
 */

#include "file_reader.h"
#include <string.h>

void FileReader::set_file(string path) {
	this->ifs = ifstream(path);
}

size_t FileReader::read_chunk(int size, char * buff) {
	if (!ifs) {
		memset(buff, 0, size);
		return 0;
	}
	memset(buff, 0, size);
	ifs.read(buff, size);
	return ifs.gcount();
}
