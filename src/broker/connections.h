#pragma once

#include <map>
#include "SDL/SDL_net.h"

class Node;

class Connections{
public:
	Connections();
	~Connections();
	void Open(int port);
	void Close();
	void Tick();
private:
	TCPsocket _server;
	IPaddress _server_ip;
	int _node_number = 1;
	std::map<int, Node*> _connected_nodes;
};