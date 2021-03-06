﻿#include "Global.h"
#include "Maze.h"
#include "MazeGenerator.h"
#include "Commun.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <pthread.h>
using namespace std;

Maze maze;
Player player;
Monster monster;
bool flag, pm;

void *connectread(void *t) {
	Commun *p = (Commun *)t;
	recv(p->sockfd, p->buf, 99999, 0);//start
	printf("gamestart\n");
	int len = 0;
	len = recv(p->sockfd, p->buf, 99999, 0);//map size
	p->buf[len] = 0;
	//printf("%s\n", p->buf);
	maze.ReceiveMazeMap(p->buf);
	len = recv(p->sockfd, p->buf, 99999, 0);//player
	p->buf[len] = 0;
	if (p->buf[2] == '0') {
		pm = 1;
		monster = Monster(&maze, '0');		
		len = monster.communicate(p->buf);
		p->buf[len] = 0;
		printf("%s\n", p->buf);
		send(p->sockfd, p->buf, len, 0);
		flag = 1;
		while (true) {
			len = recv(p->sockfd, p->buf, 99999, 0);//player
			p->buf[len] = 0;
			//printf("%s\n", p->buf);
			len = monster.communicate(p->buf);
			if (len == -1) break;
			send(p->sockfd, p->buf, len, 0);
		}
	}
	else {
		pm = 0;
		player = Player(&maze, p->buf[2]);
		len = player.communicate(p->buf);
		send(p->sockfd, p->buf, len, 0);
		flag = 1;
		while (true) {
			len = recv(p->sockfd, p->buf, 99999, 0);//player
			p->buf[len] = 0;
			//printf("%s\n", p->buf);
			//sleep(3);
			len = player.communicate(p->buf);
			if (len == -1) break;
			send(p->sockfd, p->buf, len, 0);
		}
	}
	//gameover
	close(p->sockfd);
	pthread_exit(NULL);
}

void *winflush(void* p) {
	while (!maze.winner) {
		usleep(200000);
	#ifdef __linux__
		printf("\033c");
	#endif
	#ifdef _WIN32
		clrscr();
	#endif
		if (pm) monster.UpdateView();
		else player.UpdateView();
		if (pm) monster.DisplayMaze();
		else player.DisplayMaze();
		maze.ShowBoard();
	}
	#ifdef __linux__
		printf("\033c");
	#endif
	#ifdef _WIN32
		clrscr();
	#endif
	if (pm) monster.UpdateView();
	else player.UpdateView();
	if (pm) monster.DisplayMaze();
	else player.DisplayMaze();
	if (maze.winner == 2) maze.GameOver(1);
	else maze.GameOver(0);
	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
	/*
	auto maze = pGenerator->Generate();
	Monster m(&maze, '0');
	maze.SetMonster(&m);
	Player p1(&maze, '1'), p2(&maze, '2'), p3(&maze, '3');
	maze.AddPlayer(&p1), maze.AddPlayer(&p2), maze.AddPlayer(&p3);
	p1.communicate("0|1|1|120|333|1=1|1|120|333|1=1|1|120|333|1=1|1|120|333|1=12|231|333");
	*/
	Commun commun;
	if(argc >= 2){
		int post = 0;
		int l = strlen(argv[1]);
		for(int i = 0;i < l;i++)
			post = post*10 + argv[1][i]-'0';
		commun.setPost(post);	
	}
	if (!commun.createConnect()) return 0;
	printf("Connect succsee!\n");
	pthread_t pthread;
	pthread_create(&pthread, NULL, connectread, (void *)&commun);
	cout << "LOADING...\n";
	while (!flag);
	pthread_t pthreadwin;
	pthread_create(&pthreadwin, NULL, winflush, NULL);
	while (true) {
		if (maze.winner) {
			break;
		}
		if (pm) monster.UpdateView();
		else player.UpdateView();
		if (pm) monster.DisplayMaze();
		else player.DisplayMaze();
		maze.ShowBoard();
		char op;
	#ifdef __linux__
		op = getch();
	#endif
	#ifdef _WIN32 
		op = _getch();
	#endif
		if (pm || player.IsAlive())
		switch (op)
		{
		case 'w':
			if (pm) monster.MoveUp();
			else player.MoveUp();
			break;
		case 'a':
			if (pm) monster.MoveLeft();
			else player.MoveLeft();
			break;
		case 's':
			if (pm) monster.MoveDown();
			else player.MoveDown();
			break;
		case 'd':
			if (pm) monster.MoveRight();
			else player.MoveRight();
			break;
		default:
			break;
		}
	#ifdef __linux__
		printf("\033c");
	#endif
	#ifdef _WIN32
		clrscr();
	#endif
	}
	pthread_join(pthread, NULL);
	pthread_join(pthreadwin, NULL);
}
