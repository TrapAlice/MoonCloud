#include "node.h"
#include <sstream>
#include <iterator>
#include "../dbg.h"
#include "../messageid.h"
#include "../util.h"
#include <string.h>
#include "task.h"

Node::Node(){
	_set = SDLNet_AllocSocketSet(2);
	_client_socket = 0;
}

Node::~Node(){
	SDLNet_TCP_Close(_node_socket);
	SDLNet_FreeSocketSet(_set);
}

void Node::Start(int port){
	_status = 0;
	log_info("Starting up node");
	log_info("Connecting to Broker");
	SDLNet_ResolveHost(&_broker_ip,"localhost",2000);
	_broker_socket = SDLNet_TCP_Open(&_broker_ip);
	SDLNet_TCP_AddSocket(_set, _broker_socket);
	log_info("Connected to Broker");

	log_info("Opening connection for client");
	SDLNet_ResolveHost(&_node_ip, NULL, port);
	_node_socket = SDLNet_TCP_Open(&_node_ip);
	log_info("Connection opened for client on port %d", port);

	while( !_shutdown ){
		_check_new_connections();
		if( SDLNet_CheckSockets(_set, 200) > 0 ){
			_check_new_messages();
		}
	}
	SDLNet_TCP_Close(_broker_socket);
	SDLNet_TCP_Close(_client_socket);
	log_info("Closing down");
}

void Node::_check_new_connections(){
	if( !_client_socket ){
		if( (_client_socket = SDLNet_TCP_Accept(_node_socket)) ){
			_client_ip = SDLNet_TCP_GetPeerAddress(_client_socket);
			SDLNet_TCP_AddSocket(_set, _client_socket);
			_status = 2;
			_send_message(_broker_socket, BuildString("%d ", SET_STATUS_ACTIVE));
			log_info("Client has connected");
		}
	}
}

void Node::_check_new_messages(){
	char buffer[512];
	memset(buffer, 0, 512);
	if( SDLNet_SocketReady(_broker_socket) ){
		if (SDLNet_TCP_Recv(_broker_socket, buffer, 512) > 0){
			_process_message(Split(std::string(buffer)));
		} else {
			log_info("Connection to Broker lost");
			_shutdown = true;
		}
	} else if( _client_socket ) {
		if( SDLNet_SocketReady(_client_socket) ){
			if (SDLNet_TCP_Recv(_client_socket, buffer, 512) > 0){
				_process_message(Split(std::string(buffer)));
			} else {
				log_info("Connection to client lost");
				SDLNet_TCP_DelSocket(_set, _client_socket);
				SDLNet_TCP_Close(_client_socket);
				_client_socket = 0;
				_status = 0;
				_send_message(_broker_socket, BuildString("%d ", SET_STATUS_IDLE));
			}
		}
	}
	
}

void Node::_process_message(std::vector<std::string> message){
	if( message.size() == 0 ) return;
	unsigned size = std::stoi(message[0]);
	if( size == 0 ) return;
	std::string temp_string = Pack(SplitFrom(1, message));
	std::string remaining = size < temp_string.length()? temp_string.substr(size) : "";
	message = Split(temp_string.substr(0,size));
	debug("Message received: %s", Pack(message).c_str());
	if( message.size() == 0 ) return;
	switch( std::stoi(message[0]) ){
		case JOB_REQUEST:
		{
			_forward_message(_broker_socket, message);
		}
			break;
		case JOB_RESPONSE:
		{
			if( _status == 0 ){
				_send_message(_broker_socket, BuildString("%d 0 ", JOB_RESPONSE));
			} else {
				_send_message(_broker_socket, BuildString("%d 1 ", JOB_RESPONSE));
			}
		}
			break;
		case JOB_ID:
		{
			_forward_message(_client_socket, message);
		}
			break;
		case JOB_DATA:
		{
			_task = new Task(message[2], message[3]);
			std::string result = _task->Run();
			delete _task;
			_send_message(_broker_socket, BuildString("%d 2 %s", JOB_RESPONSE, result.c_str()));
		}
			break;
		case JOB_RESULTS:
		{
			_send_message(_client_socket, Pack(SplitFrom(1, message)));
		}
			break;
		default:
			log_err("Unknown message: %s", Pack(message).c_str());
	}
	if( remaining.size() > 1 ){
		_process_message(Split(remaining));
	}
}

void Node::_send_message(TCPsocket to, std::string message){
	debug("Message sent: %s", message.c_str());
	message = BuildString("%d %s", message.length(), message.c_str());
	SDLNet_TCP_Send(to, (void *)(message.c_str()), message.length());
}

void Node::_forward_message(TCPsocket to, std::vector<std::string> message){
	_send_message(to, Pack(message));
}