#pragma once
#include "SDL/SDL_net.h"

#include <vector>
#include <string>

class Task;

class Node{
public:
	Node();
	~Node();
	void Start();
private:
	IPaddress _broker_ip;
	TCPsocket _broker_socket;
	int _status;
	Task* _task;
	void _process_message(std::vector<std::string> message);
	void _send_message(std::string message);
};