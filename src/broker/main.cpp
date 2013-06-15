#include "../dbg.h"
#include "connections.h"
#include "taskmanager.h"
#include "node.h"
#include <map>
#include "SDL/SDL_net.h"
#include <unistd.h>
#include <iostream>

int main(int argc, char* argv[]){
	int port = 2000;
	bool help = false;
	int v;

	while ( (v = getopt (argc, argv, "p::h::")) != -1 ){
		switch(v){
			case 'p':
				port = atoi(optarg);
				break;
			case 'h':
				help = true;
				break;
		}
	}

	if( help ){
		std::cout << "Usage:"<<std::endl<<
			"-p    port           optional (default 2000)\n";
		exit(0); 
	}

	SDLNet_Init();
	auto *_node_map = new std::map<int, Node*>;
	Connections *c = new Connections(_node_map);
	TaskManager *t = new TaskManager(_node_map);
	c->AddTaskManager(t);
	t->AddConnections(c);
	c->Open(port);
	while(!c->isShutdown()){
		c->Tick();
		t->Tick();
	}
	c->Close();
	delete t;
	delete c;
	for( auto node : *_node_map ){
		delete node.second;
	}
	_node_map->clear();
	delete _node_map;
	SDLNet_Quit();
}