#ifndef PUB_H
#define PUB_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>

void setdaemon();
int socket_create(int);

void catch_Signal(int sign);
int signal1(int, void (*)(int));

#endif
