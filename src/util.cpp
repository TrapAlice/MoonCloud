#include "util.h"
#include <stdarg.h>

std::string Pack(std::vector<std::string> v){
	std::string result;
	for( auto s : v ){
		result += s + " ";
	}
	return result;
}

std::vector<std::string> Split(std::string s){
	std::stringstream ss(s);
	std::istream_iterator<std::string> begin(ss);
	std::istream_iterator<std::string> end;
	std::vector<std::string> vstrings(begin, end);
	return vstrings;
}

std::vector<std::string> SplitFrom(unsigned int from, std::vector<std::string> source){
	std::vector<std::string> result;
	for( unsigned int x = from; x < source.size(); ++x ){
		result.push_back(source[x]);
	}
	return result;
}

std::vector<std::string> SplitTo(unsigned int to, std::vector<std::string> source){
	std::vector<std::string> result;
	for( unsigned int x = 0; x < to; ++x ){
		result.push_back(source[x]);
	}
	return result;
}

std::string BuildString(std::string s, ...){
	char *text = new char[512];
	va_list ap;
	va_start(ap, s);
	vsprintf(text, s.c_str(), ap);
	va_end(ap);
	std::string result(text);
	free(text);
	return result;
}