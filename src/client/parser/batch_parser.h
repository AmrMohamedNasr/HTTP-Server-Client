#ifndef	BATCH_PARSER_H
#define	BATCH_PARSER_H

#include "../../web_models/request.h"

class BatchParser {
	private:
	public:
		bool read_input(string file);
		bool has_next();
		Request next();
};
#endif
