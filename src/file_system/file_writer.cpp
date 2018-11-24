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
#include <climits>

static void _mkdir(const char *dir) {
        char tmp[PATH_MAX];
        char *p = NULL;
        size_t len;

        snprintf(tmp, sizeof(tmp),"%s",dir);
        len = strlen(tmp);
        if(tmp[len - 1] == '/')
                tmp[len - 1] = 0;
        for(p = tmp + 1; *p; p++)
                if(*p == '/') {
                        *p = 0;
                        mkdir(tmp, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
                        *p = '/';
                }
        mkdir(tmp, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

void FileWriter::set_file(string path) {
	char pathC[path.size() + 1];
	memcpy(pathC, path.c_str(), (size_t)path.size());
	pathC[path.size()] = '\0';
	char * pathOnly = dirname(pathC);
	_mkdir(pathOnly);
	this->ofs = ofstream(path);
}

bool FileWriter::write_chunk(char * chunk, streamsize size) {
	if (!this->ofs) {
		return false;
	}
	this->ofs.write(chunk, size);
	return true;
}
