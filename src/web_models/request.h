#ifndef	REQUEST_H
#define	REQUEST

#include <map>
#include <string>
#include "network_enums.h"

using namespace std;

class Request {
	private:
		REQUEST_TYPE type;
		PROTOCOL protocol;
		string url;
		map<string, string> headers;
		string data;
	public:
		Request();
		Request(string command);
		Request(char * recieved_data);
		string getHeaderValue(string key);

		const string& getData() const {
			return data;
		}

		void setData(const string& data) {
			this->data = data;
		}

		const map<string, string>& getHeaders() const {
			return headers;
		}

		void setHeaders(const map<string, string>& headers) {
			this->headers = headers;
		}

		PROTOCOL getProtocol() const {
			return protocol;
		}

		void setProtocol(PROTOCOL protocol) {
			this->protocol = protocol;
		}

		REQUEST_TYPE getType() const {
			return type;
		}

		void setType(REQUEST_TYPE type) {
			this->type = type;
		}

	const string& getUrl() const {
		return url;
	}

	void setUrl(const string& url) {
		this->url = url;
	}
};
#endif
