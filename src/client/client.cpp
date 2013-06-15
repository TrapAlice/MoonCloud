#include "client.h"
#include "../dbg.h"
#include "../messageid.h"
#include "../util.h"
#include <sstream>
#include <iterator>

Client::Client(){

}

Client::~Client(){

}

void Client::OpenConnection(){
	SDLNet_ResolveHost(&_node_ip,"localhost",2001);
	_node_socket = SDLNet_TCP_Open(&_node_ip);
}

void Client::GetIdleNode(){
	std::stringstream ss;
	ss << GET_IDLE_NODE;
	_send_message(ss.str());
}

int Client::GetJobId(int amount, int type){
	std::stringstream s;
	s << JOB_REQUEST<<" " << 0<<" " << amount<<" " << type<<" ";
	_send_message(s.str());
	auto jobId=Split(_receive_message());
	return atoi(jobId[2].c_str());
}

void Client::ProcessTask(int jobId, std::string file, std::string data){
	std::stringstream ss;
	ss << JOB_REQUEST <<" "<< 1 <<" "<< jobId <<" "<< file <<" "<< data<<" ";
	_send_message(ss.str());
}

std::string Client::GetResults(){
	return Pack(SplitFrom(1,Split(_receive_message())));
}

void Client::CloseConnection(){
	SDLNet_TCP_Close(_node_socket);
}

std::string Client::_receive_message(){
	char buffer[512];
	memset(buffer, 0, 512);
	if (SDLNet_TCP_Recv(_node_socket, buffer, 512) > 0){
		return std::string(buffer);
	}
	return "";
}

void Client::_send_message(std::string message){
	std::stringstream ss;
	ss<<message.length()<< " "<< message;
	debug("Message sent: %s", ss.str().c_str());
	SDLNet_TCP_Send(_node_socket, (void *)(ss.str().c_str()), ss.str().length());
}
