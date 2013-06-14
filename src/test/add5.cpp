#include <stdlib.h>
#include <iostream>
#include <ctime>

int main(int argc, char* argv[]){
	int result = atoi(argv[1]) + 5;
	clock_t start_time = clock();
	clock_t end_time = 5000 * 1000 + start_time;
	while(clock() != end_time);
	std::cout << result;
}