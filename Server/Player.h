#pragma once

enum STATUS
{
	LIVE = 1, DEAD = 0
};
struct Player{
	int icon;		//玩家类型
	int x, y;		//玩家坐标
	int points;		//玩家吃豆子的数量
	STATUS status;	//玩家状态
	void init(int id, int initx);
};