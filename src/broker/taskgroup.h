#pragma once

#include <vector>
#include <string>
#include <memory>

class Task;

class TaskGroup{
public:
	TaskGroup(int id, int client, int type, int amount);
	bool AddTask(std::shared_ptr<Task> task);
	bool isFull();
	bool isComplete();
	void Clear();
	std::string Results();
	int Id();
	int Client();
private:
	std::vector<std::shared_ptr<Task>> _tasks;
	int _type;
	int _client;
	int _amount;
	int _id;
};