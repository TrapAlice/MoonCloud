#include "task.h"
#include "../dbg.h"
#include <sstream>

Task::Task(std::string file, std::string data){
	_file = file;
	_data = data;
}

Task::~Task(){}

std::string Task::Run(){
	std::stringstream s;
	s<< _file << " " << _data; 
	FILE* pipe = popen(s.str().c_str(), "r");
	char buffer[128];
  while(!feof(pipe)) {
  	if(fgets(buffer, 128, pipe) != NULL)
  		_result += buffer;
  }
  pclose(pipe);
  return _result;
}
