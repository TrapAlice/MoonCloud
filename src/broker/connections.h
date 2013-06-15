#pragma once

#include <map>
#include <string>
#include <vector>
#include "SDL/SDL_net.h"

class Node;
class TaskManager;

class Connections{
public:
	Connections(std::map<int, Node*> *nodes);
	~Connections();
	void AddTaskManager(TaskManager *t);
	void Open(int port);
	void Close();
	void Tick();
	void SendMessage(int id, std::string message);
	bool isShutdown();
private:
	TCPsocket _server;
	IPaddress _server_ip;
	int _node_number = 1;
	std::map<int, Node*> *_connected_nodes;
	TaskManager *_t;
	SDLNet_SocketSet _set;
	bool _shutdown = false;

	void _check_new_connections();
	void _check_new_messages();
	void _process_message(int id, std::vector<std::string> message);
	void _disconnect_node(int id);
	void _change_node_status(int id, int status);
};