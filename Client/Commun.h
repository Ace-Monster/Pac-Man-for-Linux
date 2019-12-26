#pragma once
#include "Maze.h"

class Commun {
private:
	int sockfd;             //通信地址
	int post = 2223;
	char chost[20] = "127.0.0.1";
	char buf[100000];       //缓冲区
public:
	friend void *connectread(void *p);
	bool createConnect();
};