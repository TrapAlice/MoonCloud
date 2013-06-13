#pragma once

#include <map>
#include <string>
#include <vector>
#include "SDL/SDL_net.h"

class Node;

class Connections{
public:
	Connections(std::map<int, Node*> *nodes);
	~Connections();
	void Open(int port);
	void Close();
	void Tick();
private:
	TCPsocket _server;
	IPaddress _server_ip;
	int _node_number = 1;
	std::map<int, Node*> *_connected_nodes;

	void _check_new_connections();
	void _check_new_messages();
	void _process_message(int id, std::vector<std::string> message);
	void _disconnect_node(int id);
};