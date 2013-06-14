#pragma once

#include <map>
#include <queue>
#include <string>
#include <vector>

class Node;
class Task;
class TaskGroup;

class TaskManager{
public:
	TaskManager(std::map<int, Node*> *nodes);
	~TaskManager();
	void AddJob(int sender, std::vector<std::string> data);
	void JobResponse(int sender, std::vector<std::string> data);
	void NodeDisconnected();
private:
	std::map<int, Node*> *_connected_nodes;
	std::queue<Task*> _queued_tasks;
	std::map<int, TaskGroup*> _tasks;
	int _task_group_id = 1;
	std::map<int, Task*> _nodes_task;
	
	Node *_find_idle_node();
	void _process_tasks();
	void _job_accepted(int sender);
	void _job_refused(int sender);
	void _job_successful(int sender, std::string result);
	void _job_interrupted(int sender);
};