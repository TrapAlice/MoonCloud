#pragma once
#include "SDL/SDL_net.h"

#include <vector>
#include <string>

class Task;

class Node{
public:
	Node();
	~Node();
	void Start(int port, std::string broker_host, int broker_port);
private:
	IPaddress _broker_ip;
	TCPsocket _broker_socket;
	IPaddress _node_ip;
	TCPsocket _node_socket;
	IPaddress *_client_ip;
	TCPsocket _client_socket;
	SDLNet_SocketSet _set;
	int _status;
	Task* _task;
	bool _shutdown = false;
	void _check_new_connections();
	void _check_new_messages();
	void _process_message(std::vector<std::string> message);
	void _send_message(TCPsocket to, std::string message);
	void _forward_message(TCPsocket to, std::vector<std::string> message);
};