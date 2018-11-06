/*
 * file_reader.h
 *
 *  Created on: Nov 6, 2018
 *      Author: amrnasr
 */

#ifndef SRC_FILE_SYSTEM_FILE_READER_H_
#define SRC_FILE_SYSTEM_FILE_READER_H_

#include <string>
#include <fstream>
#include <streambuf>

using namespace std;

class FileReader {
	private:
		ifstream ifs;
	public:
		void set_file(string path);
		size_t read_chunk(int size, char * buff);
};



#endif /* SRC_FILE_SYSTEM_FILE_READER_H_ */
