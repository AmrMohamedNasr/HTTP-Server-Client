#ifndef	FILE_HANDLER_H
#define	FILE_HANDLER_H
#include <string>

using namespace std;

class FileHandler {
	public:
		bool check_file(string path);
		string read_file(string path);
		bool write_file(string path, string data);
};
#endif
