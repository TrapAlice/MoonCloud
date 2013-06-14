#pragma once
#include "SDL/SDL_net.h"

#include <vector>
#include <string>

class Client{
public:
	Client();
	~Client();
	void OpenConnection();
	void GetIdleNode();
	int GetJobId(int amount, int type);
	void ProcessTask(int jobId, std::string file, std::string data);
	std::string GetResults();
	void CloseConnection();
private:
	IPaddress _node_ip;
	TCPsocket _node_socket;
	std::string _receive_message();
	void _send_message(std::string message);
};