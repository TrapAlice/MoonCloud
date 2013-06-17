#include "../dbg.h"
#include "connections.h"
#include "taskmanager.h"
#include "node.h"
#include <map>
#include "SDL/SDL_net.h"
#include <unistd.h>
#include <iostream>
#include <memory>

int main(int argc, char* argv[]){
	int port = 2000;
	int max_nodes = 20;
	bool help = false;
	int v;

	while ( (v = getopt (argc, argv, "p::n::h::")) != -1 ){
		switch(v){
			case 'p': port = atoi(optarg); break;
			case 'n': max_noes = atoi(optarg); break;
			case 'h': help = true; break;
		}
	}

	if( help ){
		std::cout << "Usage:"<<std::endl<<
			"-p    Port           optional (default 2000)\n"<<
			"-n    Max nodes      optional (default 20)\n";
		exit(0); 
	}

	SDLNet_Init();
	std::shared_ptr<std::map<int, std::shared_ptr<Node>>> _node_map(new std::map<int, std::shared_ptr<Node>>);
	std::shared_ptr<Connections> c(new Connections(_node_map));
	std::shared_ptr<TaskManager> t(new TaskManager(_node_map));
	c->AddTaskManager(t);
	t->AddConnections(c);
	c->Open(port);
	while(!c->isShutdown()){
		c->Tick();
		t->Tick();
	}
	c->Close();
	_node_map->clear();
	SDLNet_Quit();
}