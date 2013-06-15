#include "task.h"
#include "../dbg.h"
#include "../util.h"
#include <sstream>
#include <unistd.h>
#include <fcntl.h>

Task::Task(std::string file, std::string data){
	_file = file;
	_data = data;
}

Task::~Task(){}

void Task::Run(){
	FILE* _task = popen(BuildString("%s %s", _file.c_str(), _data.c_str()).c_str(), "r");
	_fileno = fileno(_task);
	fcntl(_fileno, F_SETFL, O_NONBLOCK);
}

bool Task::isComplete(){
	char buffer[128];
	ssize_t r = read(_fileno, buffer, 128);
	if( r == -1 && errno == EAGAIN ){
		return false;
	} else if ( r > 0 ){
  	_result = buffer;
		return true;
	} else {
		pclose(_task);
	}
	return false;
}

std::string Task::Result(){ return _result; }