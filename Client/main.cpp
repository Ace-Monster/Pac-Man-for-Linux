#include "Global.h"
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
		len = monster.communicate(p->buf);
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
		len = player.communicate(p->buf);
		send(p->sockfd, p->buf, len, 0);
		flag = 1;
		while (true) {
			len = recv(p->sockfd, p->buf, 99999, 0);//player
			p->buf[len] = 0;
			//printf("%s\n", p->buf);
			len = player.communicate(p->buf);
			if (len == -1) break;
			send(p->sockfd, p->buf, len, 0);
		}
	}
	//gameover
	close(p->sockfd);
	pthread_exit(NULL);
}

int main() {
	/*
	auto maze = pGenerator->Generate();
	Monster m(&maze, '0');
	maze.SetMonster(&m);
	Player p1(&maze, '1'), p2(&maze, '2'), p3(&maze, '3');
	maze.AddPlayer(&p1), maze.AddPlayer(&p2), maze.AddPlayer(&p3);
	p1.communicate("0|1|1|120|333|1=1|1|120|333|1=1|1|120|333|1=1|1|120|333|1=12|231|333");
	*/
	Commun commun;
	if (!commun.createConnect()) return 0;
	printf("Connect succsee!\n");
	pthread_t pthread;
	pthread_create(&pthread, NULL, connectread, (void *)&commun);
	pthread_join(pthread, NULL);
	cout << "LOADING...\n";
	while (!flag);
	while (true) {
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
}