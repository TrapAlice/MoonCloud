#include "../dbg.h"
#include "connections.h"
#include "taskmanager.h"
#include <map>

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
	SDLNet_Quit();
}