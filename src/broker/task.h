#pragma once

#include <string>

class Node;

class Task{
public:
	Task(int id, std::string data);
	void AssignWorker(Node *worker);
	void Complete(std::string result);
	std::string Data();
	std::string Result();
	bool isComplete();
	int Id();
private:
	int _id;
	Node *_worker;
	std::string _data;
	std::string _result;
	bool _complete;
};