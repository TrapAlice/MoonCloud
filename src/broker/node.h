#pragma once

#include "SDL/SDL_net.h"

enum Status{
	STATUS_IDLE,
	STATUS_BUSY,
	STATUS_ACTIVE,
	STATUS_UNKNOWN,
};

class Node{
public:
	Node(int id, TCPsocket socket, IPaddress *ip);
	~Node();
	int Id();
	Uint32 Host();
	Uint16 Port();
	int RemotePort();
	void SetRemotePort(int port);
	TCPsocket Socket();
	int Status();
	void SetStatus(int status);
private:
	int        _id;
	IPaddress *_remote_ip;
	TCPsocket  _client;
	int        _status;
	int        _port;
};