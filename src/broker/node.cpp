#include "node.h"

Node::Node(int id, TCPsocket socket, IPaddress *ip){
	_id = id;
	_client = socket;
	_remote_ip = ip;
}

Node::~Node(){

}

int Node::Id(){ return _id; }
Uint32 Node::Host(){ return SDLNet_Read32(&_remote_ip->host); }
Uint16 Node::Port(){ return SDLNet_Read16(&_remote_ip->port); }
TCPsocket Node::Socket(){ return _client; }