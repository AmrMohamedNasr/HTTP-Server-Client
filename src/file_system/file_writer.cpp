/*
 * file_writer.cpp
 *
 *  Created on: Nov 6, 2018
 *      Author: amrnasr
 */
#include "file_writer.h"
#include <libgen.h>
#include <sys/stat.h>
#include <cstring>

void FileWriter::set_file(string path) {
	char pathC[path.size() + 1];
	memcpy(pathC, path.c_str(), (size_t)path.size());
	pathC[path.size()] = '\0';
	char * pathOnly = dirname(pathC);
	mkdir(pathOnly, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	this->ofs = ofstream(path);
}

bool FileWriter::write_chunk(char * chunk, streamsize size) {
	if (!this->ofs) {
		return false;
	}
	this->ofs.write(chunk, size);
	return true;
}
