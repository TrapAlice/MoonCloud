#pragma once

#include <map>
#include <queue>
#include <string>
#include <vector>
#include <memory>

class Node;
class Task;
class TaskGroup;
class Connections;

class TaskManager{
public:
	TaskManager(std::shared_ptr<std::map<int, std::shared_ptr<Node>>> nodes);
	~TaskManager();
	void Tick();
	void AddConnections(std::shared_ptr<Connections> c);
	void AddJob(int sender, std::vector<std::string> data);
	void JobResponse(int sender, std::vector<std::string> data);
	void NodeDisconnected(int sender);
	std::shared_ptr<Node> FindIdleNode();
private:
	std::shared_ptr<std::map<int, std::shared_ptr<Node>>> _connected_nodes;
	std::queue<std::shared_ptr<Task>> _queued_tasks;
	std::map<int, std::shared_ptr<TaskGroup>> _tasks;
	int _task_group_id = 1;
	std::map<int, std::shared_ptr<Task>> _nodes_task;
	std::weak_ptr<Connections> _c;
	
	void _process_tasks();
	void _job_accepted(int sender);
	void _job_refused(int sender);
	void _job_successful(int sender, std::string result);
	void _job_interrupted(int sender);
};