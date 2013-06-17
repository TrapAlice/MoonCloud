#include "client.h"
#include "../dbg.h"
#include "../messageid.h"
#include "../util.h"
#include <sstream>
#include <iterator>
#include <arpa/inet.h>

Client::Client(){

}

Client::~Client(){

}

void Client::OpenConnection(std::string host, int port){
	check(SDLNet_ResolveHost(&_node_ip,host.c_str(),port) != -1, "Error: %s", SDLNet_GetError());
	_node_socket = SDLNet_TCP_Open(&_node_ip);
	check(_node_socket, "Error: %s", SDLNet_GetError());
}

void Client::GetIdleNode(){
	log_info("Requesting idle node");
	_send_message(BuildString("%d ", GET_IDLE_NODE));
	std::vector<std::string> idle_node = Split(_receive_message());
	SDLNet_TCP_Close(_node_socket);
	struct in_addr addr;inet_aton(idle_node[1].c_str(), &addr);
	SDLNet_ResolveHost(&_node_ip, inet_ntoa(addr), std::stoi(idle_node[2]));
	_node_socket = SDLNet_TCP_Open(&_node_ip);
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
		debug("Message received: %s", buffer);
		return std::string(buffer);
	}
	return "";
}

void Client::_send_message(std::string message){
	debug("Message sent: %s", message.c_str());
	message = BuildString("%d %s", message.length(), message.c_str());
	SDLNet_TCP_Send(_node_socket, (void *)(message.c_str()), message.length());
}
