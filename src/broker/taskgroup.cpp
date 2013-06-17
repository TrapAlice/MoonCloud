#include "taskgroup.h"
#include "task.h"
#include <sstream>
#include "../dbg.h"

TaskGroup::TaskGroup(int id, int client, int type, int amount){
	_id = id;
	_client = client;
	_type = type;
	_amount = amount;
}

bool TaskGroup::AddTask(std::shared_ptr<Task> task){
	if( (int)_tasks.size() < _amount ){
		_tasks.push_back(task);
		return true;
	}
	return false;
}

bool TaskGroup::isFull(){
	return (int)_tasks.size() == _amount;
}

bool TaskGroup::isOwner(std::shared_ptr<Task> task){
	for( auto t : _tasks ){
		if( t == task ) return true;
	}
	return false;
}

bool TaskGroup::isComplete(){
	if(!isFull()) return false;
	for( auto t : _tasks ) {
		if( !t->isComplete() ) return false;
	}
	return true;
}

void TaskGroup::Clear(){
}

std::string TaskGroup::Results(){
	std::ostringstream ss;
	for( auto t : _tasks ) {
		ss << t->Result();
	}
	return ss.str();
}

int TaskGroup::Id(){ return _id; }
int TaskGroup::Client(){ return _client; }
