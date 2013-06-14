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
	log_info("Closing down");
}

void Node::_check_new_connections(){
	TCPsocket node;
	if( (node = SDLNet_TCP_Accept(_node_socket)) ){
		_client_ip = SDLNet_TCP_GetPeerAddress(node);
		_client_socket = node;
		SDLNet_TCP_AddSocket(_set, node);
		_status = 2;
		std::stringstream ss;
		ss<<SET_STATUS_ACTIVE;
		_send_message(_broker_socket,  ss.str());
		log_info("Client has connected");
	}
}

void Node::_check_new_messages(){
	char buffer[512];
	memset(buffer, 0, 512);
	if( SDLNet_SocketReady(_broker_socket) ){
		if (SDLNet_TCP_Recv(_broker_socket, buffer, 512) > 0){
			auto s = std::string(buffer);
			std::stringstream ss(s);
			std::istream_iterator<std::string> begin(ss);
			std::istream_iterator<std::string> end;
			std::vector<std::string> vstrings(begin, end);
			_process_message(vstrings);
		} else {
			log_info("Connection to Broker lost");
			_shutdown = true;
		}
	} else if( _client_socket ) {
		if (SDLNet_TCP_Recv(_client_socket, buffer, 512) > 0){
			auto s = std::string(buffer);
			std::stringstream ss(s);
			std::istream_iterator<std::string> begin(ss);
			std::istream_iterator<std::string> end;
			std::vector<std::string> vstrings(begin, end);
			_process_message(vstrings);
		} else {
			log_info("Connection to client lost");
			SDLNet_TCP_DelSocket(_set, _client_socket);
			_client_socket = nullptr;
			_status = 0;
			std::stringstream ss;
			ss<<SET_STATUS_IDLE;
			_send_message(_broker_socket,  ss.str());
		}
	}
	
}

void Node::_process_message(std::vector<std::string> message){
	if( message.size() == 0 ) return;
	unsigned int size = atoi(message[0].c_str());
	message = SplitFrom(1, message);
	std::string temp_string = Pack(message);
	std::string remaining;
	if( size != temp_string.length() ) remaining = temp_string.substr(size);
	temp_string = temp_string.substr(0,size);
	message.clear();
	message = Split(temp_string);

	debug("Message received: %s", [message](){std::stringstream ss; for( auto c : message){ ss<<c.c_str()<<" "; } return ss.str().c_str(); }());
	if( message.size() == 0 ) return;
	switch( atoi(message[0].c_str()) ){
		case JOB_REQUEST:
		{
			_forward_message(_broker_socket, message);
		}
			break;
		case JOB_RESPONSE:
		{
			std::stringstream s;
			if( _status == 0 ){
				s<<JOB_RESPONSE<<" 0";
			} else {
				s<<JOB_RESPONSE<<" 1";
			}
			_send_message(_broker_socket, s.str());
		}
			break;
		case JOB_ID:
		{
			_forward_message(_client_socket, message);
		}
			break;
		case JOB_DATA:
		{
			std::stringstream s;
			_task = new Task(message[2], message[3]);
			std::string result = _task->Run();
			s<<JOB_RESPONSE<<" 2 "<< result;
			delete _task;
			_send_message(_broker_socket, s.str());
		}
			break;
		case JOB_RESULTS:
		{
			std::stringstream ss;
			for( unsigned int x = 1; x < message.size(); ++x ){
				ss << message[x]<<" ";
			}
			_send_message(_client_socket, ss.str());
		}
			break;
		default:
			log_err("Unknown message: %s", [message](){std::stringstream ss; for( auto c : message){ ss<<c.c_str()<<" "; } return ss.str().c_str(); }());
	}
	if( remaining.size() > 1 ){
		_process_message(Split(remaining));
	}
}

void Node::_send_message(TCPsocket to, std::string message){
	debug("Message sent: %s", message.c_str());
	std::stringstream ss;
	ss<<message.length()<< " "<< message;
	//debug("Message sent: %s", ss.str().c_str());
	SDLNet_TCP_Send(to, (void *)(ss.str().c_str()), ss.str().length());
}

void Node::_forward_message(TCPsocket to, std::vector<std::string> message){
	_send_message(to, [message](){std::stringstream ss; for( auto c : message){ ss<<c.c_str()<<" "; } return ss.str(); }());
}