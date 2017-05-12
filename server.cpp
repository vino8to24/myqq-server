#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "pub.h"
#include "work.h"

int main(int args, char **argv){
	if(args < 2){
		printf("Usage: server port\n");
		return -1;
	}

	int iport = atoi(argv[1]);
	if(iport == 0){
		printf("port %d invalid!", iport);
		return -1;
	}
	
	// 将程序设置为daemon状态
	//setdaemon();
	work w(iport);
	printf("server begins...\n");
	
	signal1(SIGINT, catch_Signal);
	signal1(SIGPIPE, catch_Signal);
	w.run();

	printf("server ends...\n");
	return EXIT_SUCCESS;
}
