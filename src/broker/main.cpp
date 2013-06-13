#include "../dbg.h"
#include "connections.h"

int main(int argc, char* argv[]){
	SDLNet_Init();
	Connections *c = new Connections();
	c->Open(2000);
	while(true){
		c->Tick();
	}
	c->Close();
	delete c;
	SDLNet_Quit();
}