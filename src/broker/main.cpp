#include "../dbg.h"
#include "connections.h"
#include <map>

int main(int argc, char* argv[]){
	SDLNet_Init();
	auto *_node_map = new std::map<int, Node*>;
	Connections *c = new Connections(_node_map);
	c->Open(2000);
	while(true){
		c->Tick();
	}
	c->Close();
	delete c;
	SDLNet_Quit();
}