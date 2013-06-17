#pragma once

#include <map>
#include <string>
#include <vector>
#include <queue>
#include <memory>
#include "SDL/SDL_net.h"

class Node;
class TaskManager;

class Connections{
public:
	Connections(std::shared_ptr<std::map<int, std::shared_ptr<Node>>> nodes);
	~Connections();
	void AddTaskManager(std::shared_ptr<TaskManager> t);
	void Open(int port);
	void Close();
	void Tick();
	void SendMessage(int id, std::string message);
	bool isShutdown();
private:
	TCPsocket _server;
	IPaddress _server_ip;
	int _node_number = 1;
	std::shared_ptr<std::map<int, std::shared_ptr<Node>>> _connected_nodes;
	std::shared_ptr<TaskManager> _t;
	SDLNet_SocketSet _set;
	bool _shutdown = false;
	std::queue<std::shared_ptr<Node>> _waiting_for_idle_node;

	void _check_new_connections();
	void _check_new_messages();
	void _process_message(int id, std::vector<std::string> message);
	void _disconnect_node(int id);
	void _change_node_status(int id, int status);
};