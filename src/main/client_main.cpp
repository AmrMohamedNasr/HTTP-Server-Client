#include <iostream>
#include "../client/client.h"

using namespace std;
bool isNum(string num);


int main(int argc, char** argv) {
	if (argc == 3) {
			if (!isNum(argv[2])) {
				cout << "Invalid port number. Must be a valid port number !" << endl;
				return 0;
			}
			string file;
			cout << "Enter File path to read requests: " << endl;
			cin >> file;
			int port = atoi(argv[2]);
			char *server_ip = argv[1];
			Client client;
			client.start_client(port, server_ip, file);
		} else {
			cout << "Invalid command. Should be called like this \"" << argv[0] << " <server ip> <port number> \"" << endl;
		}
		return 0;
}

bool isNum(string num) {
	for (int i = 0; i < num.size(); i++) {
		if (!isdigit(num[i])) {
			return false;
		}
	}
	return true;
}
