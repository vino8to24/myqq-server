#ifndef WORK_H
#define WORK_H

#define CLIENTCOUNT 256
#define BODYBUF 1024

struct msg_t{
	unsigned char head[4];
	char body[1024];
};

class work{
public:
	work(int port);
	~work();
	void run();
private:
	int listen_fd;
	// 声明socket_client数组，管理client的socket连接
	int socket_client[CLIENTCOUNT];


	// 将socket设置为非阻塞
	int setnonblocking(int sock);
	int socket_accept();
	int socket_recv(int sock);
	// client socket连接断开
	void user_logout(int);
	// send消息
	void send_msg(const char *, int);
	// login消息
	void login_msg(int, int, const char*);
	// 将accept的客户端连接安装到socket_client[10]的数组中
	void fix_socket_client(int index, int sock);
};


#endif
