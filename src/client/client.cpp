#include "client.h"
#include "../dbg.h"
#include "../messageid.h"
#include <sstream>

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
	std::stringstream ss;
	ss << JOB_REQUEST << 0 << amount << type;
	_send_message(ss.str());
	std::string jobId = _receive_message.c_str();
	std::stringstream ss(jobId);
	std::istream_iterator<std::string> begin(ss);
	std::istream_iterator<std::string> end;
	std::vector<std::string> vstrings(begin, end);
	return atoi(vstrings[1].c_str());
}

void Client::ProcessTask(int jobId, std::string file, std::string data){
	std::stringstream ss;
	ss << JOB_REQUEST << 1 << jobId << file << data;
	_send_message(ss.str());
}

std::string Client::GetResults(){
	_receive_message();
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
	debug("Message sent: %s", message.c_str());
	SDLNet_TCP_Send(_node_socket, (void *)(message.c_str()), message.length());
}
