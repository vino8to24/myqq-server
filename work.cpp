#include "work.h"
#include "pub.h"
#include <sys/epoll.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>

work::work(int port){
	memset(socket_client, 0, sizeof(socket_client));
	if((listen_fd = socket_create(port)) == 0){
		exit(-1);
	}
}

work::~work(){
}

int work::socket_accept(){
	struct sockaddr_in client_addr;
	socklen_t len = sizeof(client_addr);
	char str[INET_ADDRSTRLEN];
	int client_sock = accept(listen_fd, (struct sockaddr*)&client_addr, &len);
	if(client_sock < 0){
		perror("accept error:");
	}else{
		printf("accept %s \n", inet_ntop(AF_INET, &client_addr.sin_addr, str, sizeof(str)));
	}
	return client_sock;
}

void work::login_msg(int sock, int o_userid, const char *passwd){
	struct msg_t msg;
	memset(&msg, 0, sizeof(msg));
	msg.head[0] = 2;	// 系统消息
	msg.head[2] = 0;	// 保留
	msg.head[3] = 0;	// 保留

	if((o_userid < 0) || (o_userid >= CLIENTCOUNT)){
		printf("login failed, %d: invalid userid\n", o_userid);
		msg.head[1] = 1;	// 无效userid
		send(sock, (const char*)&msg, sizeof(msg), 0);
		close(sock);
		return;
	}
	// 验证用户登陆ID和密码
	if(!auth_passwd(o_userid, passwd)){
		
	}
}

int work::socket_recv(int sock){
	struct msg_t msg;
	memset(&msg, 0, sizeof(msg));
	//接收来自client socket发送来的消息
	ssize_t recv_len = recv(sock, (char *)&msg, sizeof(msg));
	if(recv_len <= 0){
		if(recv_len < 0){
			perror("receive failed:");
		}
	}else{
		switch(msg.head[0]){
			case 0:		// send消息
				send_msg(&msg, recv_len);
				break;
			case 1:		// login消息
				login_msg(socket, msg.head[1], msg.body);
				break;
			default:	// 无法识别的消息
				printf("loging fail, it's not login message, %s\n", (const char*)&msg);
				msg.head[0] = 2;	// 系统消息
				msg.head[1] = 0;	// 无法识别的消息
				msg.head[2] = 0;	// 保留
				msg.head[3] = 0;	// 保留
				// 给client端socket下发系统消息
				send(sock, (const char*)&msg, sizeof(msg.body), 0);
				return 0;
		}
	}

	return recv_len;
}

void work::user_logout(int sock){
	
}

int work::setnonblocking(int sock){
	int opts = fcntl(sock, F_GETFL);
	if(opts < 0){
		perror("fcntl error:");
		return 0;
	}
	opts = opts | O_NONBLOCK;
	if(fcntl(sock, F_SETFL, opts) < 0){
		perror("fcntl error:");
		return 0;
	}
	return 1;
}

void work::run(){
	// 声明epoll_event结构体的变量，ev用于注册事件
	// 数组用于回传要处理的事件
	struct epoll_event ev, events[CLIENTCOUNT];
	// 把socket设置为非阻塞模式
	setnonblocking(listen_fd);
	// 生成用于处理accept的epoll专用的文件描述符
	int epfd = epoll_create(CLIENTCOUNT);
	// 设置与要处理的事件相关的文件描述符
	ev.data.fd = listen_fd;
	// 设置要处理的事件类型
	ev.events = EPOLLIN | EPOLLERR | EPOLLHUP;
	// 注册epoll事件
	epoll_ctl(epfd, EPOLL_CTL_ADD, listen_fd, &ev);

	int sock_fd = 0;
	while(1){
		// 等待epoll事件的发生
		int nfds = epoll_wait(epfd, events, CLIENTCOUNT, -1);
		if(nfds == -1){
			perror("epoll_wait error:");
			break;
		}
		for(int i=0; i<nfds; ++i){
			if(events[i].data.fd < 0){
				continue;
			}
			// 监测到一个socket用户连接到绑定的socket端口，建立新的连接
			if(events[i].data.fd == listen_fd){
				// 将来自client端的socket描述符设置为非阻塞
				sock_fd = socket_accept();
				if(sock_fd >= 0){
					setnonblocking(sock_fd);
					ev.data.fd = sock_fd;
					ev.events = EPOLLIN | EPOLLERR | EPOLLHUP;
					epoll_ctl(epfd, EPOLL_CTL_ADD, sock_fd, &ev);
					continue;
				}
			}
			if(events[i].events & EPOLLIN){
				sock_fd = events[i].data.fd;
				if(socket_recv(sock_fd) <= 0){
					user_logout(sock_fd);
					events[i].data.fd = -1;
				}
			}
			if(events[i].events & EPOLLERR){
				sock_fd = events[i].data.fd;
				user_logout(sock_fd);
				events[i].data.fd = -1;			
			}
			if(events[i].events & EPOLLHUP){
				sock_fd = events[i].data.fd;
				user_logout(sock_fd);
				events[i].data.fd = -1;			
			}
		}
	}
	close(epfd);
}
