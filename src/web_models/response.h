#ifndef	REQUEST_H
#define	REQUEST

#include <map>
#include <string>
#include "network_enums.h"

using namespace std;



class Response {
	private:
		int statusCode;
		PROTOCOL protocol;
		string statusMessage;
		map<string, string> headers;
		string data;
	public:
		Response();
		Response(int code, PROTOCOL proto, string messageCode);
		string getHeaderValue(string key);
		void addHeaderValue(string key, string value);

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

		int getStatusCode() const {
			return statusCode;
		}

		void setStatusCode(int statusCode) {
			this->statusCode = statusCode;
		}

		const string& getStatusMessage() const {
			return statusMessage;
		}

		void setStatusMessage(const string& statusMessage) {
			this->statusMessage = statusMessage;
		}
};
#endif
