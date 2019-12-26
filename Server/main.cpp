#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>
#include <signal.h>
#include <pthread.h>
#include "Running.h"
int sockfd;
int post = 2223;
Running running;

void *run(void *p) {
	Running *r = (Running *)p;
	r->mazeGenerator = new MazeGenerator(50, 20);
	while (true) {
		r->maze = r->mazeGenerator->Generate();
		while (r->size < 4);						//等待玩家到齐
		r->gamestart();
		r->gameover();
	}
}

int main(){
	//running.display();
	//return 0;
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(post);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror("socket error");
		exit(-1);
	}
   	if(bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
   		perror("bind error");
   		exit(-1);
	}
    printf("bind ok!\n");
    if(listen(sockfd, 4) == -1){
		perror("listen error");
		exit(-1);
	}
	pthread_t mrun;
	pthread_create(&mrun, NULL, run, (void *)&running);
	while(true){
		int socksz = sizeof(struct sockaddr_in);
		int fd = accept(sockfd, NULL, (socklen_t *)&socksz);
		if(fd == -1){
			perror("Accept error\n");
			continue;
		}
		printf("Connect success\n");
		if(running.getSize() >= 4){
			printf("Player is more then 4, connect will be closed.\n");
			close(fd);
			continue;
		}
		running.add(fd);
	}
	return 0;
}
