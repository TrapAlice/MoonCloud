#include "taskgroup.h"
#include "task.h"
#include <sstream>

TaskGroup(int id, int client, int type, int amount){
	_id = id;
	_client = client;
	_type = type;
	_amount = amount;
}

bool AddTask(Task task){
	if( _tasks.size() < _amount ){
		_tasks.push_back(task);
		return true;
	}
	return false;
}

bool isFull(){
	return _tasks.size == _amount;
}

bool isOwner(Task task){
	for( auto t : _tasks ){
		if( t == task ) return true;
	}
	return false;
}

bool isComplete(){
	for( auto t : _tasks ) {
		if( !t->isComplete() ) return false;
	}
	return true;
}

std::string Results(){
	std::stringstream ss()
	for( auto t : _tasks ) {
		ss << t->Result() << " ";
	}
	return ss->str();
}
