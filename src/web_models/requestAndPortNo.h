
#include "request.h"
class RequestAndPortNo {
	private:
	Request request;
	int portNo;
	public:
		RequestAndPortNo(Request req);
		RequestAndPortNo(Request req,int portNo);
		Request getRequest();
		int getPortNo();
};
