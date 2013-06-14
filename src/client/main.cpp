#include "../dbg.h"
#include "SDL/SDL_net.h"
#include "client.h"

int main(int argc, char* argv[]){
	SDLNet_Init();
	int amount = 5;
	Client *c = new Client();
	c->OpenConnection();
	//c->GetIdleNode();
	int jobId = c->GetJobId(amount, 0);
	for( int x = 0; x < amount; ++x ){
		c->ProcessTask(jobId, "bin/add5", "5");
	}
	
	log_info("Results: %s",c->GetResults().c_str());
	c->CloseConnection();
	SDLNet_Quit();
}