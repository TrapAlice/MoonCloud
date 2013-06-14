#include "../dbg.h"
#include "connections.h"
#include "taskmanager.h"
#include "node.h"
#include <map>
#include "SDL/SDL_net.h"

int main(int argc, char* argv[]){
	SDLNet_Init();
	auto *_node_map = new std::map<int, Node*>;
	Connections *c = new Connections(_node_map);
	TaskManager *t = new TaskManager(_node_map);
	c->AddTaskManager(t);
	t->AddConnections(c);
	c->Open(2000);
	while(!c->isShutdown()){
		c->Tick();
		t->Tick();
	}
	c->Close();
	delete t;
	delete c;
	for( auto& node : *_node_map ){
		delete node.second;
	}
	_node_map->clear();
	delete _node_map;
	SDLNet_Quit();
}