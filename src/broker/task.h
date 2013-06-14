#pragma once

#include <string>

class Node;

class Task{
public:
	Task(int id, std::String data);
	void AssignWorker(Node *worker);
	void Complete(std::String result);
	std::string Data();
	bool isComplete();
private:
	int _id;
	Node *_worker;
	std::string _data;
	std::string _result;
	bool _complete;
};