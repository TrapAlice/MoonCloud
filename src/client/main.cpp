#include "../dbg.h"
#include "SDL/SDL_net.h"
#include "client.h"
#include <unistd.h>
#include <iostream>

int main(int argc, char* argv[]){
	int target_port = -1;
	std::string target_host = "localhost";
	int v;

	while ( (v = getopt (argc, argv, "p:h::")) != -1 ){
		switch(v){
			case 'p':
				target_port = atoi(optarg);
				break;
			case 'h':
				target_host = optarg;
				break;
		}
	}

	if( target_port < 0 ){
		std::cout << "Usage:"<<std::endl<<
			"-p    Broker port    required"<<std::endl<<
			"-h    Broker host    optional (default localhost)\n";
		exit(0); 
	}


	SDLNet_Init();
	int amount = 5;
	Client *c = new Client();
	c->OpenConnection(target_host, target_port);
	c->GetIdleNode();
	int jobId = c->GetJobId(amount, 0);
	for( int x = 0; x < amount; ++x ){
		c->ProcessTask(jobId, "bin/add5", "5");
	}
	
	log_info("Results: %s",c->GetResults().c_str());
	c->CloseConnection();
	delete c;
	SDLNet_Quit();
}