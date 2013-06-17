#pragma once

#include <string>

class Task{
public:
	Task(int id, std::string data);
	void Complete(std::string result);
	std::string Data();
	std::string Result();
	bool isComplete();
	int Id();
private:
	int _id;
	std::string _data;
	std::string _result;
	bool _complete;
};