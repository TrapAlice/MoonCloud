#include "connections.h"
#include "../dbg.h"
#include "../messageid.h"
#include "../util.h"
#include "node.h"
#include "taskmanager.h"
#include <string.h>
#include <sstream>
#include <iterator>

Connections::Connections(std::shared_ptr<std::map<int, std::shared_ptr<Node>>> nodes, int max_nodes){
	_connected_nodes = nodes;
	_set = SDLNet_AllocSocketSet(max_nodes);
}

Connections::~Connections(){
	SDLNet_FreeSocketSet(_set);
}

void Connections::Open(int port){
	check(SDLNet_ResolveHost(&_server_ip, NULL, port) != -1, "Error: %s", SDLNet_GetError());
	_server = SDLNet_TCP_Open(&_server_ip);
	check(_server, "Error: %s", SDLNet_GetError());
	log_info("Conncetions Manager opened on port: %d", port);
}

void Connections::Close(){
	SDLNet_TCP_Close(_server);
	log_info("Connections Manager closed");
}

void Connections::AddTaskManager(std::shared_ptr<TaskManager> t){
	_t = t;
}

void Connections::Tick(){
	_check_new_connections();
	if( SDLNet_CheckSockets(_set, 200) > 0 ){
		_check_new_messages();
	}
}

void Connections::SendMessage(int id, std::string message){
	debug("Message sent: %d:%s", id, message.c_str());
	message = BuildString("%d %s", message.length(), message.c_str());
	SDLNet_TCP_Send(_connected_nodes->at(id)->Socket(), (void *)(message.c_str()), message.length());
}

void Connections::_check_new_connections(){
	TCPsocket node;
	if( (node = SDLNet_TCP_Accept(_server)) ){
		IPaddress *remote_ip = SDLNet_TCP_GetPeerAddress(node);
		std::shared_ptr<Node> newNode(new Node(_node_number, node, remote_ip));
		_connected_nodes->insert(std::pair<int, std::shared_ptr<Node>>(_node_number, newNode));
		SDLNet_TCP_AddSocket(_set, newNode->Socket());
		newNode->SetStatus(STATUS_UNKNOWN);
		log_info("Node %d has connected: %x : %d", newNode->Id(), newNode->Host(), newNode->Port());
		_node_number++;
	}
}

void Connections::_check_new_messages(){
	char buffer[512];
	for( auto node : *_connected_nodes ){
		if( SDLNet_SocketReady(node.second->Socket()) ){
			memset(buffer, 0, 512);
			if (SDLNet_TCP_Recv(node.second->Socket(), buffer, 512) > 0){
				_process_message(node.first, Split(std::string(buffer)));
			} else {
				_disconnect_node(node.first);
			}
		}
	}
}



void Connections::_process_message(int id, std::vector<std::string> message){
	if( message.size() == 0 ) return;
	unsigned size = std::stoi(message[0]);
	if( size == 0 ) return;
	std::string temp_string = Pack(SplitFrom(1, message));
	std::string remaining = size != temp_string.length()? temp_string.substr(size) : "";
	message = Split(temp_string.substr(0,size));
	debug("Message received: %d:%s", id, Pack(message).c_str());
	switch(std::stoi(message[0])){
		case DISCONNECT:
			_disconnect_node(id);
			break;
		case JOB_REQUEST:
			_t.lock()->AddJob(id, message);
			break;
		case JOB_RESPONSE:
			_t.lock()->JobResponse(id, message);
			_allocate_idle_node();
			break;
		case NEW_NODE:
			_connected_nodes->at(id)->SetRemotePort(std::stoi(message[1]));
			_change_node_status(id, STATUS_IDLE);
			_allocate_idle_node();
			break;
		case SET_STATUS_IDLE:
			_change_node_status(id, STATUS_IDLE);
			break;
		case SET_STATUS_ACTIVE:
			_change_node_status(id, STATUS_ACTIVE);
			break;
		case SET_STATUS_BUSY:
			_change_node_status(id, STATUS_BUSY);
			break;
		case GET_IDLE_NODE:
			_waiting_for_idle_node.push(_connected_nodes->at(id));
			break;
		case CLOSE:
			_shutdown = true;
			break;
		case NODE_STATUS:
			log_info("NODE \t Status \t Host \t Port");
			for( auto& node : *_connected_nodes ){
				log_info("%d \t %d \t\t %x \t %d",node.second->Id(), node.second->Status(), node.second->Host(), node.second->Port());
			}
			break;
		default:
			log_err("Unknown message: %d - %s", id, Pack(message).c_str());
	}
	if( remaining.size() > 1 ){
		_process_message(id, Split(remaining));
	}
}

void Connections::_change_node_status(int id, int status){
	switch( status ){
		case STATUS_IDLE:
			break;
		case STATUS_BUSY:
		case STATUS_ACTIVE:
			_t.lock()->JobResponse(id, std::vector<std::string>{"3","3"});
			break;
	}
	_connected_nodes->at(id)->SetStatus(status);
}

bool Connections::isShutdown(){ return _shutdown; }

void Connections::_disconnect_node(int id){
	_t.lock()->NodeDisconnected(id);
	auto node = _connected_nodes->at(id);
	SDLNet_TCP_DelSocket(_set, node->Socket());
	_connected_nodes->erase(id);
	log_info("Node %d disconnected", id);
}

void Connections::_allocate_idle_node(){
	if( !_waiting_for_idle_node.empty() ){
		auto idle_node = _t.lock()->FindIdleNode();
		if( idle_node ){
			auto waiting_node = _waiting_for_idle_node.front();
			SendMessage(waiting_node->Id(), BuildString("0x%x %d", idle_node->Host(), idle_node->RemotePort()));
			_waiting_for_idle_node.pop();
		}
	}
}
