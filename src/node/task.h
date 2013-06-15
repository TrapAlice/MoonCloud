#pragma once

#include <string>

class Task{
public:
	Task(std::string file, std::string data);
	~Task();
	void Run();
	bool isComplete();
	std::string Result();
private:
	std::string _data;
	std::string _file;
	std::string _result;
	int _fileno;
	FILE* _task;
};