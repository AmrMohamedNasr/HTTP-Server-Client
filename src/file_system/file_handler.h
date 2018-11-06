#ifndef	FILE_HANDLER_H
#define	FILE_HANDLER_H

#include <string>
#include "file_reader.h"
#include "file_writer.h"

using namespace std;

class FileHandler {
	private:
		FileReader reader;
		FileWriter writer;
	public:
		bool check_file(string path);
		size_t get_file_size(string path);
		void set_read_file(string path);
		void set_write_file(string path);
		size_t read_chunk(int size, char * buff);
		bool write_chunk(char * chunk, streamsize size);
};
#endif
