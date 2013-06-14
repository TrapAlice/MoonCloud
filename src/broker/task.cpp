#include "task.h"
#include "node.h"

Task::Task(int id, std::String data){
	_id = id;
	_data = data;
	_complete = false;
	_worker = nullptr;
}

void Task::Assign_Worker(Node *worker){
	_worker = worker;
}

void Task::Complete(std::String result){
	_result = result;
	_complete = true;
}

std::string Task::Data(){ return _data; }

bool Task::isComplete(){ return _complete; }
