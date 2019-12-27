#pragma once
#include "Global.h"

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
	int GetRow() const;
	int GetCol() const;
	void SetRow(int row);
	void SetCol(int col);
	bool IsBeanAt(int x, int y) const;
	bool IsPlayerAt(int x, int y) const;
	void addPlayerPos(int pos);
	vector<Tile>* GetMap();
	vector<pair<int, int> >* GetBeans();
	int portalA, portalB;
	~Maze();
	void display() {
		for (int i = 0; i < row; i++) {
			for (int j = 0; j < col; j++) {
				putchar(toChar(GetCell(j, i)));
			}
			cout << endl;
		}
	}
private:
	int row, col;
	vector<Tile> mazeMap;
	vector<pair<int, int> > beans;
	vector<int> players;
};

