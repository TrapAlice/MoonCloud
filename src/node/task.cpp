#include "task.h"
#include "../dbg.h"
#include "../util.h"
#include <sstream>

Task::Task(std::string file, std::string data){
	_file = file;
	_data = data;
}

Task::~Task(){}

std::string Task::Run(){
	FILE* pipe = popen(BuildString("%s %s", _file.c_str(), _data.c_str()).c_str(), "r");
	char buffer[128];
  while(!feof(pipe)) {
  	if(fgets(buffer, 128, pipe) != NULL)
  		_result += buffer;
  }
  pclose(pipe);
  return _result;
}
