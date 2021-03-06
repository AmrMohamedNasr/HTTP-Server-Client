#ifndef	BATCH_PARSER_H
#define	BATCH_PARSER_H
#include "../../web_models/requestAndPortNo.h"
#include <vector>
class BatchParser {
	private:
		vector<RequestAndPortNo> requests;
		Request convert_str_to_req(string line);
	public:
		bool read_input(string file);
		bool has_next();
		RequestAndPortNo next();
};
#endif
