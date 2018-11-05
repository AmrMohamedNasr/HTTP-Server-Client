/*
 * web_utils.h
 *
 *  Created on: Nov 5, 2018
 *      Author: amrnasr
 */

#ifndef SRC_UTILS_STRING_UTILS_H_
#define SRC_UTILS_STRING_UTILS_H_

#include <algorithm>
#include <cctype>
#include <locale>
#include <string>
#include <vector>

using namespace std;

inline bool ends_with(std::string const & value, std::string const & ending) {
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

// trim from start (in place)
inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}
vector<string> split_string(string line, string delimiter);
vector<string> split_string(string line, string delimiter, int partitions);
void print_raw_string(string s);

#endif /* SRC_UTILS_STRING_UTILS_H_ */
