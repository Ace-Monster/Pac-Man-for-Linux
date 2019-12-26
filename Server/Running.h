#pragma once
#include "Player.h"
#include "MazeGenerator.h"

enum GAMESTATUS
{
	WAIT = 0,
	RUNNING = 1,
	WIN = 2,
	LOSE = 3
};
class Running {
private:
	int fd[10];									//玩家通信地址
	Player player[10];							//玩家状态信息
	int size;									//玩家数量
	GAMESTATUS gamestatus;						//游戏状态
	MazeGenerator *mazeGenerator;				//地图生成器
	Maze maze;									//地图
	pthread_t thread_array[10];					//线程池
	char buf[10][100005];						//缓冲区
	int changeTochar(Player *p, char s[]);
	void changeToplayer(Player *p, char s[]);
	void intToChar(int x, int &t, char s[]);
	friend void *connectread(void *id);			//线程2
	void gamestart();							//游戏主进程
	void gameover();							//游戏结束
public:
	friend void *run(void *p);					//线程1
	void add(int tfd);							//添加玩家
	int getSize();
	void display(){
		changeTochar(&player[0], buf[0]);
		printf("%s\n", buf[0]);
	}
};
