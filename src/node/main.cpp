#include "../dbg.h"
#include "SDL/SDL_net.h"
#include "node.h"

int main(int argc, char* argv[]){
	SDLNet_Init();
	Node *n = new Node();
	n->Start();
	delete n;
	SDLNet_Quit();
}