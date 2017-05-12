.SUFFIXES:.cpp .o

CC = g++

SRCS = server.cpp\
	   pub.cpp\
	   work.cpp
OBJS = $(SRCS:.cpp=.o)
EXE = server

all:start clean

start:$(OBJS)
	@$(CC) -o $(EXE) $(OBJS)

.cpp .o:
	@$(CC) -Wall -g -o $@ -c $<

.PHONY:clean
clean:
	@rm -f *.o
