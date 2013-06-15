#pragma once

#include <vector>
#include <string>

class Task;

class TaskGroup{
public:
	TaskGroup(int id, int client, int type, int amount);
	bool AddTask(Task *task);
	bool isFull();
	bool isOwner(Task *task);
	bool isComplete();
	void Clear();
	std::string Results();
	int Id();
	int Client();
private:
	std::vector<Task*> _tasks;
	int _type;
	int _client;
	int _amount;
	int _id;
};