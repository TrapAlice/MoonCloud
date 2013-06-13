#pragma once

#include "SDL/SDL_net.h"

class Node{
public:
	Node(int id, TCPsocket socket, IPaddress *ip);
	~Node();
	int Id();
	Uint32 Host();
	Uint16 Port();
	TCPsocket Socket();
private:
	int        _id;
	IPaddress *_remote_ip;
	TCPsocket  _client;
	int        _status;
};