#include "connections.h"
#include "../dbg.h"
#include "node.h"
#include <string.h>
#include <sstream>
#include <iterator>

Connections::Connections(std::map<int, Node*> *nodes){
	_connected_nodes = nodes;
}

Connections::~Connections(){
	//for( auto i : _connected_nodes ){
	//	delete i;
	//}
	//_connected_nodes.clear();
}

void Connections::Open(int port){
	SDLNet_ResolveHost(&_server_ip, NULL, port);
	_server = SDLNet_TCP_Open(&_server_ip);
	log_info("Conncetions Manager opened on port: %d", port);
}

void Connections::Close(){
	SDLNet_TCP_Close(_server);
	log_info("Connections Manager closed");
}

void Connections::Tick(){
	_check_new_connections();
	_check_new_messages();
}

void Connections::_check_new_connections(){
	TCPsocket client;
	if( (client = SDLNet_TCP_Accept(_server)) ){
		IPaddress *remote_ip = SDLNet_TCP_GetPeerAddress(client);
		Node *node = new Node(_node_number, client, remote_ip);
		_connected_nodes->insert(std::pair<int, Node*>(_node_number, node));
		log_info("Node %d has connected: %x : %d", node->Id(), node->Host(), node->Port());
		_node_number++;
	}
}

void Connections::_check_new_messages(){
	char buffer[512];
	for( auto& client : *_connected_nodes ){
		memset(buffer, 0, 512);
		if (SDLNet_TCP_Recv(client.second->Socket(), buffer, 512) > 0){
			auto s = std::string(buffer);
			std::stringstream ss(s);
			std::istream_iterator<std::string> begin(ss);
			std::istream_iterator<std::string> end;
			std::vector<std::string> vstrings(begin, end);
			_process_message(client.first, vstrings);
		} else {
			_disconnect_node(client.first);
		}
	}
}

enum MESSAGE_IDS{
	DISCONNECT = 1,
};

void Connections::_process_message(int id, std::vector<std::string> message){
	switch(atoi(message[0].c_str())){
		case DISCONNECT:
			_disconnect_node(id);
			break;
		default:
			log_err("Unknown message: %d - %s", id, [](std::vector<std::string> s){std::stringstream ss; for( auto c : s){ ss<<c.c_str()<<" "; } return ss.str().c_str(); }(message));
	}
}

void Connections::_disconnect_node(int id){
	auto& node = _connected_nodes->at(id);
	delete node;
	_connected_nodes->erase(id);
	log_info("Node %d disconnected", id);
}

