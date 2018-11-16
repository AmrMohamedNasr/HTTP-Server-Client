
#include "requestAndPortNo.h"
using namespace std;


RequestAndPortNo::RequestAndPortNo(Request req) {
	this->request = req;
	this->portNo = 80;
}
RequestAndPortNo::RequestAndPortNo(Request req , int portNo) {
	this->request = req;
	this->portNo = portNo;
}

