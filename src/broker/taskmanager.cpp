#include "taskmanager.h"
#include "taskgroup.h"
#include "task.h"
#include "node.h"
#include "connections.h"
#include "../dbg.h"
#include "../messageid.h"
#include "../util.h"
#include <sstream>

TaskManager::TaskManager(std::shared_ptr<std::map<int, std::shared_ptr<Node>>> nodes){
	_connected_nodes = nodes;
}

TaskManager::~TaskManager(){

}

void TaskManager::AddConnections(std::shared_ptr<Connections> c){
	_c = c;
}

void TaskManager::Tick(){
	_process_tasks();
}

void TaskManager::AddJob(int sender, std::vector<std::string> data){
	switch( std::stoi(data[1]) ){
		case 0: //Start
		{
			int amount = std::stoi(data[2]);
			std::shared_ptr<TaskGroup> newTaskGroup(new TaskGroup(_task_group_id, sender, 0, amount));
			_tasks[_task_group_id] = newTaskGroup;
			log_info("New task group from id:%d, task_group_id:%d, amount: %d", sender, _task_group_id, amount);
			_c.lock()->SendMessage(sender, moon::BuildString("%d %d ", JOB_ID, newTaskGroup->Id()));
			++_task_group_id;
		}
			break;
		case 1: //Add
		{
			auto taskGroup = _tasks[std::stoi(data[2])];
			if( taskGroup ){
				std::shared_ptr<Task> newTask(new Task(taskGroup->Id(), moon::Pack(moon::SplitFrom(2, data))));
				if( taskGroup->AddTask(newTask) ){
					_queued_tasks.push(newTask);
					log_info("New task from id:%d, task_group:%d added", sender, taskGroup->Id());
				} else {
					log_err("Task Group %d already full", taskGroup->Id());
				}
			}
		}
			break;
	}
}

void TaskManager::JobResponse(int sender, std::vector<std::string> data){
	switch( std::stoi(data[1]) ){
		case 0://yes
			_job_accepted(sender);
			break;
		case 1://no
			_job_refused(sender);
			break;
		case 2://successful
			_job_successful(sender, moon::Pack(moon::SplitFrom(2, data)));
			break;
		case 3://interrupted
			_job_interrupted(sender);
			break;
	}
}

void TaskManager::NodeDisconnected(int sender){
	_job_interrupted(sender);
}

void TaskManager::_job_accepted(int sender){
	_connected_nodes->at(sender)->SetStatus(STATUS_BUSY);
	auto task = _nodes_task[sender];
	_c.lock()->SendMessage(sender, moon::BuildString("%d %s ",JOB_DATA, task->Data().c_str()));
}

void TaskManager::_job_refused(int sender){
	_connected_nodes->at(sender)->SetStatus(STATUS_UNKNOWN);
	auto task = _nodes_task[sender];
	if( task ){
		_queued_tasks.push(task);
		_nodes_task.erase(sender);
	}
}

void TaskManager::_job_successful(int sender, std::string result){
	_connected_nodes->at(sender)->SetStatus(STATUS_IDLE);
	auto task = _nodes_task[sender];
	if( task ){
		task->Complete(result);
		_nodes_task.erase(sender);
		auto taskGroup = _tasks[task->Id()];
		if( taskGroup->isComplete() ){
			_c.lock()->SendMessage(taskGroup->Client(), moon::BuildString("%d %s", JOB_RESULTS, taskGroup->Results().c_str()));
			_tasks.erase(task->Id());
			taskGroup->Clear();
		}
	}
}

void TaskManager::_job_interrupted(int sender){
	auto task = _nodes_task[sender];
	if( task ){
		log_info("Task %d was interrupted", task->Id());
		_queued_tasks.push(task);
		_nodes_task.erase(sender);
	}
}



std::shared_ptr<Node> TaskManager::FindIdleNode(){
	for( auto node : *_connected_nodes ){
		if( node.second->Status() == STATUS_IDLE ){
			node.second->SetStatus(STATUS_UNKNOWN);
			return node.second;
		}
	}
	return nullptr;
}

void TaskManager::_process_tasks(){
	std::shared_ptr<Task> task;
	std::queue<std::shared_ptr<Task>> spareTasks;
	while( _queued_tasks.size() > 0 ){
		task = _queued_tasks.front();
		_queued_tasks.pop();
		auto selected_node = FindIdleNode();
		if( selected_node ){
			_nodes_task[selected_node->Id()] = task;
			_c.lock()->SendMessage(selected_node->Id(), moon::BuildString("%d ", JOB_RESPONSE));
			log_info("Assigning Task %d to node %d", task->Id(), selected_node->Id());
		} else {
			spareTasks.push(task);
		}
	}

	while( spareTasks.size() > 0 ){
		_queued_tasks.push(spareTasks.front());
		spareTasks.pop();
	}
}