/*
 * file_writer.cpp
 *
 *  Created on: Nov 6, 2018
 *      Author: amrnasr
 */
#include "file_writer.h"

void FileWriter::set_file(string path) {
	this->ofs = ofstream(path);
}

bool FileWriter::write_chunk(char * chunk, streamsize size) {
	if (!this->ofs) {
		return false;
	}
	this->ofs.write(chunk, size);
	return true;
}
