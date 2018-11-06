/*
 * web_utils.cpp
 *
 *  Created on: Nov 5, 2018
 *      Author: amrnasr
 */
#include "../utils/string_utils.h"

using namespace std;


void print_raw_string(string s) {
	for (char& p : s)
	{
	    int c = (unsigned char) p;
	    switch (c)
	    {
	        case '\\':
	            printf("\\\\");
	            break;
	        case '\n':
	            printf("\\n");
	            break;
	        case '\r':
	            printf("\\r");
	            break;
	        case '\t':
	            printf("\\t");
	            break;
	        default:
	            if (isprint(c))
	            {
	                putchar(c);
	            }
	            else
	            {
	                printf("\\x%X", c);
	            }
	            break;
	    }
	}
	printf("\n");
}

vector<string> split_string(string line, string delimiter) {
	vector<string> strings = vector<string>();
	size_t pos = 0;
	string token;
	while ((pos = line.find(delimiter)) != string::npos) {
		token = line.substr(0, pos);
		if (token.size() > 0) {
			strings.push_back(token);
		}
		line.erase(0, pos + delimiter.length());
	}
	if (line.size() > 0) {
		strings.push_back(line);
	}
	return strings;
}

vector<string> split_string(string line, string delimiter, int partitions) {
	vector<string> strings = vector<string>();
	size_t pos = 0;
	string token;
	int partition = 1;
	while ((pos = line.find(delimiter)) != string::npos) {
		token = line.substr(0, pos);
		if (token.size() > 0) {
			strings.push_back(token);
		}
		partition++;
		line.erase(0, pos + delimiter.length());
		if (partition == partitions) {
			break;
		}
	}
	if (line.size() > 0) {
		strings.push_back(line);
	}
	return strings;
}

