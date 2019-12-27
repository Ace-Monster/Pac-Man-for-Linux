#include "Maze.h"
#include "MazeGenerator.h"

Maze::Maze() : col(0), row(0), mazeMap() {
}

Maze::Maze(string filepath) {
	this->row = col = 0;
	ifstream fin(filepath);
	if (!fin.good()) {
		cerr << "read file error!\n";
		return;
	}
	char ch;
	bool firstLine = true;
	while ((ch = fin.get()) != EOF) {
#ifdef __linux__
		if (ch == '\n') {
			this->row++;
			firstLine = false;
		}
		else {
			this->mazeMap.push_back(toTile(ch));
			if (firstLine) this->col++;
		}
#endif

#ifdef _WIN32 
		if (ch == '\r') continue;
		if (ch == '\n') {
			this->row++;
			firstLine = false;
		}
		else {
			this->mazeMap.push_back(toTile(ch));
			if (firstLine) this->col++;
		}
#endif
	}
	this->row++;
	fin.close();
}

Maze::Maze(int col, int row, Tile value = Tile::Unused) : col(col), row(row), mazeMap(row * col, value) {
}

Maze::~Maze() {
}

void Maze::SetCell(int x, int y, Tile celltype) {
	assert(IsXInBounds(x));
	assert(IsYInBounds(y));

	mazeMap[x + col * y] = celltype;
}

Tile Maze::GetCell(int x, int y) const {
	assert(IsXInBounds(x));
	assert(IsYInBounds(y));
	return mazeMap[x + col * y];
}

void Maze::SetCells(int xStart, int yStart, int xEnd, int yEnd, Tile cellType) {
	assert(IsXInBounds(xStart) && IsXInBounds(xEnd));
	assert(IsYInBounds(yStart) && IsYInBounds(yEnd));

	assert(xStart <= xEnd);
	assert(yStart <= yEnd);

	for (auto y = yStart; y != yEnd + 1; ++y) {
		for (auto x = xStart; x != xEnd + 1; ++x) {
			SetCell(x, y, cellType);
		}
	}
}

bool Maze::IsXInBounds(int x) const {
	return x >= 0 && x < col;
}

bool Maze::IsYInBounds(int y) const {
	return y >= 0 && y < row;
}

bool Maze::IsAreaUnused(int xStart, int yStart, int xEnd, int yEnd) const {
	assert(IsXInBounds(xStart) && IsXInBounds(xEnd));
	assert(IsYInBounds(yStart) && IsYInBounds(yEnd));

	assert(xStart <= xEnd);
	assert(yStart <= yEnd);

	for (auto y = yStart; y != yEnd + 1; ++y) {
		for (auto x = xStart; x != xEnd + 1; ++x) {
			if (GetCell(x, y) != Tile::Unused) {
				return false;
			}
		}
	}
	return true;
}

bool Maze::IsAdjacent(int x, int y, Tile tile) const {
	assert(IsXInBounds(x - 1) && IsXInBounds(x + 1));
	assert(IsYInBounds(y - 1) && IsYInBounds(y + 1));

	return (GetCell(x - 1, y) == tile || GetCell(x + 1, y) == tile ||
		GetCell(x, y - 1) == tile || GetCell(x, y + 1) == tile);
}

int Maze::GetRow() const {
	return this->row;
}

int Maze::GetCol() const {
	return this->col;
}

void Maze::SetRow(int row) {
	this->row = row;
}

void Maze::SetCol(int col) {
	this->col = col;
}

vector<Tile>* Maze::GetMap() {
	return &mazeMap;
}

vector<pair<int, int> >* Maze::GetBeans() {
	return &beans;
}

bool Maze::IsBeanAt(int x, int y) const {
	int pos = x + y * col;
	for (int i = 0; i < (int)beans.size(); i++) {
		if (pos == beans[i].first) {
			return true;
		}
	}
	return false;
}

bool Maze::IsPlayerAt(int x, int y) const {
	int pos = x + y * col;
	for (int i = 0; i < (int)players.size(); i++) {
		if (players[i] == pos) {
			return true;
		}
	}
	return false;
}

void Maze::addPlayerPos(int pos) {
	players.push_back(pos);
}