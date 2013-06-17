#include "../dbg.h"
#include "SDL/SDL_net.h"
#include "node.h"
#include <unistd.h>
#include <iostream>
#include <memory>

int main(int argc, char* argv[]){
	int broker_port = 2000;
	std::string broker_host = "localhost";
	int node_port = -1;
	bool help = false;
	int c;

	while ( (c = getopt (argc, argv, "p:b::l::h::")) != -1 ){
		switch(c){
			case 'p': node_port = atoi(optarg); break;
			case 'b': broker_port = atoi(optarg); break;
			case 'l': broker_host = optarg; break;
			case 'h': help = true; break;
		}
	}

	if( node_port < 0 || help ){
		std::cout << "Usage:"<<std::endl<<
			"-p    port           required\n"<<
			"-b    Broker port    optional (default 2000)\n"<<
			"-l    Broker host    optional (default localhost)\n";
		exit(0); 
	}

	check(SDLNet_Init() != -1, "Error: %s", SDLNet_GetError());
	std::unique_ptr<Node> n(new Node());
	n->Start(node_port, broker_host, broker_port);
	n->Stop();
	SDLNet_Quit();
}