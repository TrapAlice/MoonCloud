#include "taskgroup.h"
#include "task.h"
#include <sstream>

TaskGroup::TaskGroup(int id, int client, int type, int amount){
	_id = id;
	_client = client;
	_type = type;
	_amount = amount;
}

bool TaskGroup::AddTask(Task *task){
	if( (int)_tasks.size() < _amount ){
		_tasks.push_back(task);
		return true;
	}
	return false;
}

bool TaskGroup::isFull(){
	return (int)_tasks.size() == _amount;
}

bool TaskGroup::isOwner(Task *task){
	for( auto t : _tasks ){
		if( t == task ) return true;
	}
	return false;
}

bool TaskGroup::isComplete(){
	for( auto t : _tasks ) {
		if( !t->isComplete() ) return false;
	}
	return true;
}

std::string TaskGroup::Results(){
	std::stringstream ss;
	for( auto t : _tasks ) {
		ss << t->Result() << " ";
	}
	return ss.str();
}

int TaskGroup::Id(){ return _id; }
