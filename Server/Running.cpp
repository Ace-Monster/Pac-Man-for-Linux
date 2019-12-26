#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <pthread.h>
#include "Running.h"
#include "MazeGenerator.h"

struct thread_data{
	int id;
	Running *p;
	thread_data(int x, Running *b) : id(x),p(b) { }
};

void Running::intToChar(int x, int &t, char s[]){
	int flag = 0;
	for (int i = 100000;i > 0;i /= 10) {
		if (flag == 0 && x / i == 0) continue;
		flag = 1;
		s[t++] = x / i + '0';
		x %= i;
	}
	if(!flag) s[t++] = '0';
	return;
}

int Running::changeTochar(Player *p, char s[]) {
	int t = 0;
	s[t++] = '0';
	s[t++] = '|';
	s[t++] = p->icon + '0';
	s[t++] = '|';
	s[t++] = p->status + '0';
	s[t++] = '|';
	intToChar(p->points, t, s);
	s[t++] = '|';
	//printf("%d\n", p->x);
	intToChar(p->x, t, s);
	s[t++] = '|';
	s[t++] = p->y + '0';
	s[t] = 0;
	return t;
}

void Running::changeToplayer(Player *p, char s[]) {
	int t = 0;
	p->status = STATUS((int)(s[t++] - '0'));
	int point = 0, x = 0;
	while (s[++t] != '|') {
		point = point * 10 + s[t] - '0';
	}
	while (s[++t] != '|') {
		x = x * 10 + s[t] - '0';
	}
	p->points = point;
	p->x = x;
	p->y = s[++t] - '0';
	return;
}

int ttt = 0;
void *connectread(void *data) {
	int id = ((thread_data *)data)->id;
	Running *p = ((thread_data *)data)->p;
	delete (thread_data *)data;
	while (p->gamestatus == GAMESTATUS::WAIT);
	send(p->fd[id], "start", p->size, 0);
	sleep(1);
	int t = 0;
	p->intToChar(p->maze.GetCol(), t, p->buf[5+id]);
	p->buf[5+id][t++] = '|';
	p->intToChar(p->maze.GetRow(), t, p->buf[5+id]);
	p->buf[5+id][t++] = '|';
	for (auto i : *(p->maze).GetMap()) {
		p->buf[5+id][t++] = (int)i + '0';
	}
	p->buf[5+id][t++] = '|';
	p->intToChar(p->maze.portalA, t, p->buf[5+id]);
	p->buf[5+id][t++] = '|';
	p->intToChar(p->maze.portalB, t, p->buf[5+id]);
	send(p->fd[id], p->buf[5+id], t, 0);
	sleep(1);
	while (p->gamestatus == GAMESTATUS::RUNNING) {
		int l = p->changeTochar(&(p->player[id]), p->buf[5+id]);
		p->buf[5+id][l] = 0;		
		//if(ttt < 5) printf("%s\n", p->buf[5+id]);
		p->buf[5+id][l++] = '=';
		for (int i = 0;i < p->size;i++) {
			if(i == id) continue;
			int len = p->changeTochar(&(p->player[i]), p->buf[id]);
			for (int j = 2;j < len;j++) p->buf[5+id][l++] = p->buf[id][j];
			p->buf[5+id][l++] = '=';
		}
		vector<int> Beans(*(p->maze.GetBeans()));
		for(auto i : Beans){
			p->intToChar(i, l, p->buf[5+id]);
			p->buf[5+id][l++] = '|';
		}
		p->buf[5+id][--l] = 0;
		send(p->fd[id], p->buf[5+id], l, 0);
		
		l = recv(p->fd[id], p->buf[id], 99999, 0);
		p->buf[id][l] = 0;
		//printf("%s\n", p->buf[id]);
		p->changeToplayer(&(p->player[id]), p->buf[id]);
		usleep(100);
	}
	char ans[2];
	ans[0] = p->gamestatus + '0';
	send(p->fd[id], ans, 1, 0);
	pthread_exit(NULL);
	return NULL;
}

void Running::add(int tfd) {
	int id = size;
	fd[id] = tfd;
	player[id].init(size, mazeGenerator->GetPlayerPos(&maze));
	//printf("%d\n", player[id].x);
	maze.addPlayerPos(player[id].x);
	thread_data *t = new thread_data(id, this);
	pthread_create(&thread_array[id], NULL, connectread, (void *)t);
	size++;
}

void Running::gamestart() {
	//maze = mazeGenerator->Generate();
	gamestatus = GAMESTATUS::RUNNING;
	while (gamestatus == GAMESTATUS::RUNNING) {
		int d = 0, s = 0;
		for (int i = 1;i < size;i++) {
			if (player[i].status == STATUS::DEAD) d++;
			s += player[i].points;
		}
		if (s == maze.GetBeans()->size()) {//s == map.point
			gamestatus = GAMESTATUS::WIN;
		}
		if (d == 3) {
			gamestatus = GAMESTATUS::LOSE;
		}
	}
	return;
}

void Running::gameover() {
	for (int i = 0;i < size;i++) {
		pthread_join(thread_array[i], NULL);
		close(fd[i]);
	}
	size = 0;
	gamestatus = GAMESTATUS::WAIT;
	return;
}

int Running::getSize(){
	return size;
}
