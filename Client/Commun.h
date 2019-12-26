#pragma once
#include "Maze.h"

class Commun {
private:
	int sockfd;             //通信地址
	int post = 2224;
	char chost[20] = "127.0.0.1";
	char buf[100000];       //缓冲区
public:
	friend void *connectread(void *p);
	bool createConnect();
	void setPost(int tpost){
		post = tpost;	
	}
};
