#include "task.h"
#include "node.h"

Task::Task(int id, std::string data){
	_id = id;
	_data = data;
	_complete = false;
}

void Task::Complete(std::string result){
	_result = result;
	_complete = true;
}

std::string Task::Data(){ return _data; }
std::string Task::Result(){ return _result; }

bool Task::isComplete(){ return _complete; }
int Task::Id(){ return _id; }
