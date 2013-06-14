#include "../dbg.h"
#include "SDL/SDL_net.h"
#include "node.h"

int main(int argc, char* argv[]){
	SDLNet_Init();
	Node *n = new Node();
	int port = atoi(argv[1]);
	n->Start(port);
	delete n;
	SDLNet_Quit();
}