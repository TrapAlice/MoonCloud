#pragma once

#include <string>

class Task{
public:
	Task(std::string file, std::string data);
	~Task();
	std::string Run();
private:
	std::string _data;
	std::string _file;
	std::string _result;
};