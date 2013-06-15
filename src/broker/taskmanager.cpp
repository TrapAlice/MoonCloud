#include "taskmanager.h"
#include "taskgroup.h"
#include "task.h"
#include "node.h"
#include "connections.h"
#include "../dbg.h"
#include "../messageid.h"
#include "../util.h"
#include <sstream>

TaskManager::TaskManager(std::map<int, Node*> *nodes){
	_connected_nodes = nodes;
}

TaskManager::~TaskManager(){

}

void TaskManager::AddConnections(Connections *c){
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
			TaskGroup *newTaskGroup = new TaskGroup(_task_group_id, sender, 0, amount);
			_tasks[_task_group_id] = newTaskGroup;
			log_info("New task group from id:%d, task_group_id:%d, amount: %d", sender, _task_group_id, amount);
			_c->SendMessage(sender, BuildString("%d %d ", JOB_ID, newTaskGroup->Id()));
			++_task_group_id;
		}
			break;
		case 1: //Add
		{
			TaskGroup *temp = _tasks[std::stoi(data[2])];
			if( temp ){
				Task *newTask = new Task(temp->Id(), Pack(SplitFrom(2, data)));
				if( temp->AddTask(newTask) ){
					_queued_tasks.push(newTask);
					log_info("New task from id:%d, task_group:%d added", sender, temp->Id());
				} else {
					delete newTask;
					log_err("Task Group %d already full", temp->Id());
				}
				if( temp->isFull() ){
					_process_tasks();
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
			_job_successful(sender, Pack(SplitFrom(2, data)));
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
	Task *task = _nodes_task[sender];
	_c->SendMessage(sender, BuildString("%d %s ",JOB_DATA, task->Data().c_str()));
}

void TaskManager::_job_refused(int sender){
	_connected_nodes->at(sender)->SetStatus(STATUS_UNKNOWN);
	Task *task = _nodes_task[sender];
	if( task ){
		task->AssignWorker(nullptr);
		_queued_tasks.push(task);
		_nodes_task.erase(sender);
		_process_tasks();
	}
}

void TaskManager::_job_successful(int sender, std::string result){
	_connected_nodes->at(sender)->SetStatus(STATUS_IDLE);
	Task *task = _nodes_task[sender];
	if( task ){
		task->Complete(result);
		task->AssignWorker(nullptr);
		_nodes_task.erase(sender);
		TaskGroup *taskGroup = _tasks[task->Id()];
		if( taskGroup->isComplete() ){
			_c->SendMessage(taskGroup->Client(), BuildString("%d %s", JOB_RESULTS, taskGroup->Results().c_str()));
			_tasks.erase(task->Id());
			taskGroup->Clear();
			delete taskGroup;
		}
	}
}

void TaskManager::_job_interrupted(int sender){
	Task *task = _nodes_task[sender];
	if( task ){
		log_info("Task %d was interrupted", task->Id());
		task->AssignWorker(nullptr);
		_queued_tasks.push(task);
		_nodes_task.erase(sender);
		_process_tasks();
	}
}



Node* TaskManager::FindIdleNode(){
	for( auto node : *_connected_nodes ){
		if( node.second->Status() == STATUS_IDLE ){
			node.second->SetStatus(STATUS_UNKNOWN);
			return node.second;
		}
	}
	return nullptr;
}

void TaskManager::_process_tasks(){
	Task *task;
	std::queue<Task*> spareTasks;
	while( _queued_tasks.size() > 0 ){
		task = _queued_tasks.front();
		_queued_tasks.pop();
		Node *selected_node;
		selected_node = FindIdleNode();
		if( selected_node != nullptr ){
			task->AssignWorker(selected_node);
			_nodes_task[selected_node->Id()] = task;
			_c->SendMessage(selected_node->Id(), BuildString("%d ", JOB_RESPONSE));
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