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

void Client::OpenConnection(int port){
	SDLNet_ResolveHost(&_node_ip,"localhost",port);
	_node_socket = SDLNet_TCP_Open(&_node_ip);
}

void Client::GetIdleNode(){
	_send_message(BuildString("%d ", GET_IDLE_NODE));
}

int Client::GetJobId(int amount, int type){
	_send_message(BuildString("%d 0 %d %d ", JOB_REQUEST, amount, type));
	auto jobId=Split(_receive_message());
	return std::stoi(jobId[2]);
}

void Client::ProcessTask(int jobId, std::string file, std::string data){
	_send_message(BuildString("%d 1 %d %s %s", JOB_REQUEST, jobId, file.c_str(), data.c_str()));
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
	debug("Message sent: %s", message.c_str());
	message = BuildString("%d %s", message.length(), message.c_str());
	SDLNet_TCP_Send(_node_socket, (void *)(message.c_str()), message.length());
}
