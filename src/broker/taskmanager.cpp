#include "taskmanager.h"
#include "taskgroup.h"
#include "task.h"
#include "node.h"
#include "../dbg.h"
#include <sstream>

TaskManager::TaskManager(std::map<int, Node*> *nodes){
	_connected_nodes = nodes;
}

TaskManager::~TaskManager(){

}

void TaskManager::AddJob(int sender, std::vector<std::string> data){
	switch( atoi(data[1].c_str()) ){
		case 0: //Start
		{
			int amount = atoi(data[2].c_str());
			TaskGroup *newTaskGroup = new TaskGroup(_task_group_id, sender, 0, amount);
			_tasks[_task_group_id] = newTaskGroup;
			log_info("New task group from id:%d, task_group_id:%d, amount: %d", sender, _task_group_id, amount);
			//TODO: Send message back to client
			/*MessageSystem.AddMessage(new Message("Connection","SendMessage", new String[]{pFrom, "JobID "+Integer.toString(_TaskNumber)}));*/
			++_task_group_id;
		}
			break;
		case 1: //Add
		{
			TaskGroup *temp = _tasks[atoi(data[2].c_str())];
			Task *newTask = new Task(temp->Id(), [data](){std::stringstream s; for(auto x = 2; x < (int)data.size(); ++x){s<<data[x]<<" ";} return s.str();}());
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
			break;
	}
}

void TaskManager::JobResponse(int sender, std::vector<std::string> data){
	switch( atoi(data[1].c_str()) ){
		case 0://yes
			_job_accepted(sender);
			break;
		case 1://no
			_job_refused(sender);
			break;
		case 2://successful
			_job_successful(sender, [data](){std::stringstream s; for(auto x = 2; x < (int)data.size(); ++x){s<<data[x]<<" ";} return s.str();}());
			break;
		case 3://interrupted
			_job_interrupted(sender);
			break;
	}
}

void TaskManager::NodeDisconnected(){

}

void TaskManager::_job_accepted(int sender){
	_connected_nodes->at(sender)->SetStatus(STATUS_BUSY);
	//Task *task = _nodes_task[sender];
	//TODO: send message to node
	/*MessageSystem.AddMessage(new Message("Connection", "SendMessage", new String[]{pFrom, "JobResponse Data "+temp.Data}));*/
}

void TaskManager::_job_refused(int sender){
	_connected_nodes->at(sender)->SetStatus(STATUS_UNKNOWN);
	Task *task = _nodes_task[sender];
	task->AssignWorker(nullptr);
	_queued_tasks.push(task);
	_nodes_task.erase(sender);
	_process_tasks();
}

void TaskManager::_job_successful(int sender, std::string result){
	Task *task = _nodes_task[sender];
	_connected_nodes->at(sender)->SetStatus(STATUS_IDLE);
	task->Complete(result);
	task->AssignWorker(nullptr);
	_nodes_task.erase(sender);
	if( _tasks[task->Id()]->isComplete() ){
		//TODO: Results are sent back to client
		/*MessageSystem.AddMessage(new Message("Connection", "SendMessage", new String[]{_Tasks.get(k).Client, "JobResponse Complete "+_Tasks.get(k).getResult()}));*/
	}
	_process_tasks();
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



Node* TaskManager::_find_idle_node(){
	for( auto& node : *_connected_nodes ){
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
		selected_node = _find_idle_node();
		if( selected_node != nullptr ){
			task->AssignWorker(selected_node);
			_nodes_task[selected_node->Id()] = task;
			//TODO: Send message to selected_node
			/*MessageSystem.AddMessage(new Message("Connection","SendMessage",new String[]{selectedAgent,"JobRequest"}));*/
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