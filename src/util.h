#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <iterator>

std::string Pack(std::vector<std::string> v);
std::vector<std::string> Split(std::string s);
std::vector<std::string> SplitFrom(unsigned int from, std::vector<std::string> source);
std::vector<std::string> SplitTo(unsigned int to, std::vector<std::string> source);
std::string BuildString(std::string s, ...);