#include "connections.h"
#include "../dbg.h"
#include "node.h"

Connections::Connections(){

}

Connections::~Connections(){
	for( auto i : _connected_nodes ){
		delete i;
	}
	_connected_nodes.clear();
}

void Connections::Open(int port){
	SDLNet_ResolveHost(&_server_ip, NULL, port);
	_server = SDLNet_TCP_Open(&_server_ip);
	log_info("Conncetions Manager opened on port: %d", port);
}

void Connections::Tick(){
	TCPsocket client;
	if( (client = SDLNet_TCP_Accept(_server)) ){
		IPaddress *remote_ip = SDLNet_TCP_GetPeerAddress(client);
		Node *node = new Node(_node_number, client, remote_ip);
		_connected_nodes[_node_number] = node;
		log_info("Node %d has connected: %x : %d", node->Id(), node->Host(), node->Port());
		_node_number++;
	}
}

void Connections::Close(){
	SDLNet_TCP_Close(_server);
	log_info("Connections Manager closed");
}