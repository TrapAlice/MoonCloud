#include "taskmanager.h"
#include "taskgroup.h"
#include "node.h"
#include "../dbg.h"

TaskManager::TaskManager(std::map<int, Node*> *nodes){
	_connected_nodes = nodes;
}

TaskManager::~TaskManager(){

}

void TaskManager::AddJob(int sender, std::vector<std::string> data){
	switch( atoi(data[1].c_str()) ){
		case 0: //Start
			int amount = atoi(data[2].c_str());
			TaskGroup newTask = TaskGroup(_task_group_id, sender, 0, amount);
			_tasks[_task_group_id] = newTask;
			log_info("New task group from id:%d, task_group_id:%d, amount: %d", sender, _task_group_id, amount);
			//TODO: Send message back to client
			/*MessageSystem.AddMessage(new Message("Connection","SendMessage", new String[]{pFrom, "JobID "+Integer.toString(_TaskNumber)}));*/
			++_task_group_id;
			break;
		case 1: //Add
			TaskGroup *temp = _tasks[atoi(data[2])];
			Task *newTask = new Task(temp->Id(), [data](){std::stringstream s; for(auto x = 2; x < data.size(); ++x){s<<data[x]<<" ";} return s.str();}())
			if( temp->AddTask(newTask) ){
				_queued_tasks->push_back(newTask);
				log_info("New task from id:%d, task_group:%d added", sender, temp->Id());
			} else {
				delete newTask;
				log_err("Task Group %d already full", temp->Id());
			}
			if( temp->isFull() ){
				_process_tasks();
			} 
			break;
	}
}

void TaskManager::JobResponse(int sender, std::vector<std::string> data){

}

void TaskManager::NodeDisconnected(){

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

}