#include "pub.h"

void setdaemon(){
	pid_t pid, sid;

	pid = fork();
	if(pid < 0){
		perror("fork error:");
		exit(EXIT_FAILURE);
	}
	if(pid > 0){
		exit(EXIT_SUCCESS);		// 父进程退出
	}
	// 子进程调用setsid函数，将进程和控制台脱离关系
	if((sid = setsid()) < 0){
		perror("setsid error:");
		exit(EXIT_FAILURE);
	}

	/*
	if(chdir("/") < 0){
		perror("chdir failed:");
		exit(EXIT_FAILURE);
	}
	umask(0);
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	*/
}

void catch_Signal(int sign){
	switch(sign){
		case SIGINT:
			//print("signal SIGINT\n");
			break;
		// 如果一个非阻塞的socket已经关闭，在这个socket上
		// 调用send函数，会触发一个SIGPIPE消息
		case SIGPIPE:
			//printf("signal SIGPIPE\n");
			//signal(SIGPIPE, catch_Signal);
			break;
	}
}

int signal1(int signo, void (*func)(int)){
	struct sigaction act, oact;
	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	return sigaction(signo, &act, &oact);
}

int socket_create(int port){
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	int on = 1;
	if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1){
		perror("setsockopt failed:");
		return 0;
	}
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0){
		perror("bind error:");
		return 0;
	}

	if(listen(sock, 256) == -1){
		perror("listen error:");
		return 0;
	}
	return sock;
}

