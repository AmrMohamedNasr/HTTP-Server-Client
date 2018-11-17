//#include <iostream>
//#include "../server/server.h"
//#include <cstdlib>
//
//using namespace std;
//
//bool isNum(string num) {
//	for (unsigned int i = 0; i < num.size(); i++) {
//		if (!isdigit(num[i])) {
//			return false;
//		}
//	}
//	return true;
//}
//int main(int argc, char** argv) {
//	if (argc == 2) {
//		if (!isNum(argv[1])) {
//			cout << "Invalid port number. Must be a valid port number !" << endl;
//			return 0;
//		}
//		int port = atoi(argv[1]);
//		Server server;
//		server.start_server(port);
//	} else {
//		cout << "Invalid command. Should be called like this \"" << argv[0] << " <port number> \"" << endl;
//	}
//	return 0;
//}
