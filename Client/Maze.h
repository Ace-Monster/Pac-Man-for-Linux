#pragma once
#include "Global.h"

class Maze;

class Monster
{
public:
	~Monster();
	Monster(Maze* maze, char icon);
	Monster(){}
	void DisplayMaze();
	int GetX() const;
	int GetY() const;
	char GetIcon() const;
	void SetX(int x);
	void SetY(int y);
	void SetDir(Direction dir);
	void MoveUp();
	void MoveDown();
	void MoveRight();
	void MoveLeft();
	bool IsInView(int x, int y) const;
	void UpdateView();
	int communicate(char* msg);
private:
	char icon;
	int x, y;
	Maze* maze;
	Direction dir;
	vector<int> view;
};

class Player
{
public:
	~Player();
	Player(Maze* maze, char icon);
	Player(){}
	void DisplayMaze();
	int GetX() const;
	int GetY() const;
	char GetIcon() const;
	bool IsAlive() const;
	void SetX(int x);
	void SetY(int y);
	void SetAlive(bool alive);
	void SetDir(Direction dir);
	void kill();
	void MoveUp();
	void MoveDown();
	void MoveRight();
	void MoveLeft();
	bool IsInView(int x, int y) const ;
	void UpdateView();
	int communicate(char* msg);
	void IncPoints();
	void SetPoints(int points);
	int GetPoints() const;
private:
	char icon;
	int x, y, points;
	bool alive;
	Maze* maze;
	Direction dir;
	vector<int> view;
};

class Maze
{
public:
	Maze();
	Maze(string filepath);
	Maze(int col, int row, Tile value);

	void SetCell(int x, int y, Tile celltype);
	Tile GetCell(int x, int y) const;
	void SetCells(int xStart, int yStart, int xEnd, int yEnd, Tile cellType);
	bool IsXInBounds(int x) const;
	bool IsYInBounds(int y) const;
	bool IsAreaUnused(int xStart, int yStart, int xEnd, int yEnd) const;
	bool IsAdjacent(int x, int y, Tile tile) const;
	bool IsPlayerAt(int x, int y) const;
	bool IsMonsterAt(int x, int y) const;
	bool IsObstacleAt(int x, int y) const;
	bool IsAbleToMove(int x, int y) const;
	bool IsBeanAt(int x, int y) const;
	Player* GetPlayer(int x, int y);
	int GetRow() const;
	int GetCol() const;
	void SetRow(int row);
	void SetCol(int col);
	vector<Tile>* GetMap();
	vector<int>* GetBeans();
	vector<Player*>* GetPlayers();
	Monster* GetMonster();
	void SetMonster(Monster* monster);
	void AddPlayer(Player* player);
	int portalA, portalB;
	void transport(Player* player);
	void transport(Monster* monster);
	void EatBeans(Player* player);
	void EatPlayers(Monster* monster);
	void ReceiveMazeMap(const char* msg);
	~Maze();
	void display();
	void ShowBoard();
	void GameOver(bool win);
	int winner = 0;
private:
	int row, col;
	vector<Tile> mazeMap;
	vector<int> beans;
	vector<Player*> players;
	Monster* monster;
};

