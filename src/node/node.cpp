#include "node.h"
#include <sstream>
#include <iterator>
#include "../dbg.h"
#include "../messageid.h"
#include <string.h>
#include "task.h"

Node::Node(){}
Node::~Node(){}

void Node::Start(){
	_status = 0;
	log_info("Starting up node");
	log_info("Connecting to Broker");
	SDLNet_ResolveHost(&_broker_ip,"localhost",2000);
	_broker_socket = SDLNet_TCP_Open(&_broker_ip);
	log_info("Connected to Broker");
	while( true ){
		char buffer[512];
		memset(buffer, 0, 512);
		if (SDLNet_TCP_Recv(_broker_socket, buffer, 512) > 0){
			auto s = std::string(buffer);
			std::stringstream ss(s);
			std::istream_iterator<std::string> begin(ss);
			std::istream_iterator<std::string> end;
			std::vector<std::string> vstrings(begin, end);
			_process_message(vstrings);
		} else {
			log_info("Connection to Broker lost");
			break;
		}
	}
	log_info("Closing down");
}

void Node::_process_message(std::vector<std::string> message){
	debug("Message received: %s", [message](){std::stringstream ss; for( auto c : message){ ss<<c.c_str()<<" "; } return ss.str().c_str(); }());
	if( message.size() == 0 ) return;
	switch( atoi(message[0].c_str()) ){
		case JOB_RESPONSE:
		{
			std::stringstream s;
			if( _status == 0 ){
				s<<JOB_RESPONSE<<" 0";
			} else {
				s<<JOB_RESPONSE<<" 1";
			}
			_send_message(s.str());
		}
			break;
		case JOB_DATA:
		{
			std::stringstream s;
			_task = new Task(message[2], message[3]);
			std::string result = _task->Run();
			s<<JOB_RESPONSE<<" 2 "<< result;
			delete _task;
			_send_message(s.str());
		}
			break;
		default:
			log_err("Unknown message: %s", [message](){std::stringstream ss; for( auto c : message){ ss<<c.c_str()<<" "; } return ss.str().c_str(); }());
	}
}

void Node::_send_message(std::string message){
	debug("Message sent: %s", message.c_str());
	SDLNet_TCP_Send(_broker_socket, (void *)(message.c_str()), message.length());
}