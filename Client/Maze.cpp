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

void Maze::display() {
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			putchar(toChar(GetCell(j, i)));
		}
		cout << endl;
	}
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

vector<Player*>* Maze::GetPlayers() {
	return &players;
}

Monster* Maze::GetMonster() {
	return monster;
}

bool Maze::IsPlayerAt(int x, int y) const {
	for (int i = 0; i < (int)players.size(); i++) {
		if (players[i]->GetX() == x && players[i]->GetY() == y /*&& players[i]->IsAlive()*/) {
			return true;
		}
	}
	return false;
}

bool Maze::IsMonsterAt(int x, int y) const {
	return monster != nullptr && monster->GetX() == x && monster->GetY() == y;
}

Player* Maze::GetPlayer(int x, int y) {
	for (int i = 0; i < (int)players.size(); i++) {
		if (players[i]->GetX() == x && players[i]->GetY() == y) {
			return players[i];
		}
	}
	return nullptr;
}

vector<int>* Maze::GetBeans() {
	return &beans;
}

void Maze::AddPlayer(Player* player) {
	players.push_back(player);
}

void Maze::SetMonster(Monster* monster) {
	this->monster = monster;
}

bool Maze::IsObstacleAt(int x, int y) const {
	Tile tile = GetCell(x, y);
	return tile == Tile::DirtWall || tile == Tile::Unused;
}

bool Maze::IsAbleToMove(int x, int y) const {
	return IsXInBounds(x) && IsYInBounds(y) && !IsObstacleAt(x, y);
}

bool Maze::IsBeanAt(int x, int y) const {
	int pos = x + y * col;
	for (int i = 0; i < (int)beans.size(); i++) {
		if (pos == beans[i]) {
			return true;
		}
	}
	return false;
}

void Maze::transport(Player* player) {
	if (player->GetX() == portalA % col && player->GetY() == portalA / col) {
		player->SetX(portalB % col), player->SetY(portalB / col);
	}
	else if (player->GetX() == portalB % col && player->GetY() == portalB / col) {
		player->SetX(portalA % col), player->SetY(portalA / col);
	}
}

void Maze::transport(Monster* monster) {
	if (monster->GetX() == portalA % col && monster->GetY() == portalA / col) {
		monster->SetX(portalB % col), monster->SetY(portalB / col);
	}
	else if (monster->GetX() == portalB % col && monster->GetY() == portalB / col) {
		monster->SetX(portalA % col), monster->SetY(portalA / col);
	}
}

void Maze::EatBeans(Player* player) {
	int pos = player->GetX() + player->GetY() * col;
	for (int i = 0; i < (int)beans.size(); i++) {
		if (pos == beans[i]) {
			player->IncPoints();
			swap(beans[i], beans.back());
			beans.pop_back();
			return;
		}
	}
}

void Maze::EatPlayers(Monster* monster) {
	int x = monster->GetX(), y = monster->GetY();
	for (int i = 0; i < (int)players.size(); i++) {
		if (x == players[i]->GetX() && y == players[i]->GetY()) {
			players[i]->kill();
			return;
		}
	}
}

void Maze::ShowBoard() {
#ifdef __linux__
	printf("\033[30;47m");
#endif
#ifdef _WIN32 
	SetConsoleColor(0 + 7 * 0x10);
#endif
	cout << endl << "┌───────────────────────┐";
	cout << endl << "│    【Score Board】    │";
	for (int i = 0; i < (int)players.size(); i++) {
		cout << endl << "│    Player (" << i + 1 << ") :  " << players[i]->GetPoints() << "\t│";
	}
	cout << endl << "└───────────────────────┘" << endl;
#ifdef __linux__
	printf("\033[0m");
#endif
#ifdef _WIN32 
	SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#endif
}

void Maze::GameOver(bool win) {
#ifdef __linux__
	printf("\033[31;47;1m");
#endif
#ifdef _WIN32 
	SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_RED | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
#endif
	cout << endl << "┌─────────────┐" << endl;
	if (win) cout << "│ Player WIN! │" << endl;
	else cout << "│ Monster WIN!│" << endl;
	cout << "└─────────────┘" << endl;
#ifdef __linux__
	printf("\033[0m");
#endif
#ifdef _WIN32 
	SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#endif
}

Monster::~Monster() {
}

Monster::Monster(Maze* maze, char icon) : maze(maze), icon(icon), dir(Direction::South) {
	int pos = (new MazeGenerator(0, 0))->GetPlayerPos(maze);
	x = pos % maze->GetCol(), y = pos / maze->GetCol();
}

void Monster::DisplayMaze() {
	for (int i = 0; i < maze->GetRow(); i++) {
		for (int j = 0; j < maze->GetCol(); j++) {
			if (IsInView(j, i)) {
			#ifdef __linux__
				printf("\033[30;47m");
			#endif
			#ifdef _WIN32 
				SetConsoleColor(0 + 7 * 0x10);
			#endif
				if (maze->IsMonsterAt(j, i)) {
				#ifdef __linux__
					printf("\033[31;47;1m");
				#endif
				#ifdef _WIN32 
					SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_RED | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
				#endif
					putchar(maze->GetMonster()->GetIcon());
				}
				else if (maze->IsPlayerAt(j, i)) {
				#ifdef __linux__
					printf("\033[31;47m");
				#endif
				#ifdef _WIN32 
					SetConsoleColor(FOREGROUND_RED | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
				#endif
					putchar(maze->GetPlayer(j, i)->GetIcon());
				}
				else if (maze->IsBeanAt(j, i)) {
				#ifdef __linux__
					printf("\033[33;47m");
				#endif
				#ifdef _WIN32 
					SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
				#endif
					putchar('@');
				}
				else {
					putchar(toChar(maze->GetCell(j, i)));
				}
			}
			else {
				putchar(toChar(maze->GetCell(j, i)));
			}
		#ifdef __linux__
			printf("\033[0m");
		#endif
		#ifdef _WIN32 
			SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		#endif
		}
		cout << endl;
	}
}

int Monster::GetX() const {
	return this->x;
}

int Monster::GetY() const {
	return this->y;
}

void Monster::SetX(int x) {
	this->x = x;
}

void Monster::SetY(int y) {
	this->y = y;
}

char Monster::GetIcon() const {
	return this->icon;
}

void Monster::MoveUp() {
	if (dir != Direction::North) {
		dir = Direction::North;
	}
	else if (maze->IsAbleToMove(x, y - 1)) {
		y--;
		maze->transport(this);
		maze->EatPlayers(this);
	}
}

void Monster::MoveDown() {
	if (dir != Direction::South) {
		dir = Direction::South;
	}
	else if (maze->IsAbleToMove(x, y + 1)) {
		y++;
		maze->transport(this);
		maze->EatPlayers(this);
	}
}

void Monster::MoveRight() {
	if (dir != Direction::East) {
		dir = Direction::East;
	}
	else if (maze->IsAbleToMove(x + 1, y)) {
		x++;
		maze->transport(this);
		maze->EatPlayers(this);
	}
}

void Monster::MoveLeft() {
	if (dir != Direction::West) {
		dir = Direction::West;
	}
	else if (maze->IsAbleToMove(x - 1, y)) {
		x--;
		maze->transport(this);
		maze->EatPlayers(this);
	}
}

bool Monster::IsInView(int x, int y) const {
	int pos = x + maze->GetCol() * y;
	for (int i = 0; i < (int)view.size(); i++) {
		if (pos == view[i]) {
			return true;
		}
	}
	return false;
}

void Monster::UpdateView() {
	view.clear();
	int col = maze->GetCol();
	switch (dir)
	{
	case Direction::North:
		for (int i = 0, j = 1; i < 6; i++) {
			if (maze->IsAbleToMove(x, y - i) && (!i || IsInView(x, y - (i - 1)))) view.push_back(x + col * (y - i));
			for (int k = 1; k <= j; k++) {
				if (maze->IsAbleToMove(x - k, y - i) && (!i || maze->IsAbleToMove(x - k + 1, y - (i - 1)) && IsInView(x - k + 1, y - (i - 1)))) view.push_back(x - k + col * (y - i));
				if (maze->IsAbleToMove(x + k, y - i) && (!i || maze->IsAbleToMove(x + k - 1, y - (i - 1)) && IsInView(x + k - 1, y - (i - 1)))) view.push_back(x + k + col * (y - i));
			}
			if (i & 1) j++;
		}
		break;
	case Direction::South:
		for (int i = 0, j = 1; i < 6; i++) {
			if (maze->IsAbleToMove(x, y + i) && (!i || IsInView(x, y + (i - 1)))) view.push_back(x + col * (y + i));
			for (int k = 1; k <= j; k++) {
				if (maze->IsAbleToMove(x - k, y + i) && (!i || maze->IsAbleToMove(x - k + 1, y + (i - 1)) && IsInView(x - k + 1, y + (i - 1)))) view.push_back(x - k + col * (y + i));
				if (maze->IsAbleToMove(x + k, y + i) && (!i || maze->IsAbleToMove(x + k - 1, y + (i - 1)) && IsInView(x + k - 1, y + (i - 1)))) view.push_back(x + k + col * (y + i));
			}
			if (i & 1) j++;
		}
		break;
	case Direction::East:
		for (int i = 0, j = 1; i < 6; i++) {
			if (maze->IsAbleToMove(x + i, y) && (!i || IsInView(x + (i - 1), y))) view.push_back(x + i + col * y);
			for (int k = 1; k <= j; k++) {
				if (maze->IsAbleToMove(x + i, y - k) && (!i || maze->IsAbleToMove(x + (i - 1), y - k + 1) && IsInView(x + (i - 1), y - k + 1))) view.push_back(x + i + col * (y - k));
				if (maze->IsAbleToMove(x + i, y + k) && (!i || maze->IsAbleToMove(x + (i - 1), y + k - 1) && IsInView(x + (i - 1), y + k - 1))) view.push_back(x + i + col * (y + k));
			}
			if (i & 1) j++;
		}
		break;
	case Direction::West:
		for (int i = 0, j = 1; i < 6; i++) {
			if (maze->IsAbleToMove(x - i, y) && (!i || IsInView(x - (i - 1), y))) view.push_back(x - i + col * y);
			for (int k = 1; k <= j; k++) {
				if (maze->IsAbleToMove(x - i, y - k) && (!i || maze->IsAbleToMove(x - (i - 1), y - k + 1) && IsInView(x - (i - 1), y - k + 1))) view.push_back(x - i + col * (y - k));
				if (maze->IsAbleToMove(x - i, y + k) && (!i || maze->IsAbleToMove(x - (i - 1), y + k - 1) && IsInView(x - (i - 1), y + k - 1))) view.push_back(x - i + col * (y + k));
			}
			if (i & 1) j++;
		}
		break;
	default:
		break;
	}
}

Player::~Player() {
}

Player::Player(Maze* maze, char icon) : maze(maze), icon(icon), alive(true), dir(Direction::South), points(0) {
	int pos = (new MazeGenerator(0, 0))->GetPlayerPos(maze);
	x = pos % maze->GetCol(), y = pos / maze->GetCol();
}

void Player::DisplayMaze() {
	for (int i = 0; i < maze->GetRow(); i++) {
		for (int j = 0; j < maze->GetCol(); j++) {
			if (IsInView(j, i)) {
			#ifdef __linux__
				printf("\033[30;47m");
			#endif
			#ifdef _WIN32 
				SetConsoleColor(0 + 7 * 0x10);
			#endif
				if (maze->IsMonsterAt(j, i)) {
				#ifdef __linux__
					printf("\033[31;47;1m");
				#endif
				#ifdef _WIN32 
					SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_RED | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
				#endif
					putchar(maze->GetMonster()->GetIcon());
				}
				else if (maze->IsPlayerAt(j, i)) {
				#ifdef __linux__
					printf("\033[31;47m");
				#endif
				#ifdef _WIN32 
					SetConsoleColor(FOREGROUND_RED | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
				#endif
					putchar(maze->GetPlayer(j, i)->GetIcon());
				}
				else if (maze->IsBeanAt(j, i)) {
				#ifdef __linux__
					printf("\033[33;47m");
				#endif
				#ifdef _WIN32 
					SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
				#endif
					putchar('@');
				}
				else {
					putchar(toChar(maze->GetCell(j, i)));
				}
			}
			else {
				putchar(toChar(maze->GetCell(j, i)));
			}
		#ifdef __linux__
			printf("\033[0m");
		#endif
		#ifdef _WIN32 
			SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		#endif
		}
		cout << endl;
	}
}

void Player::IncPoints() {
	points++;
}

int Player::GetX() const {
	return this->x;
}

int Player::GetY() const {
	return this->y;
}

void Player::SetX(int x) {
	this->x = x;
}

void Player::SetY(int y) {
	this->y = y;
}

char Player::GetIcon() const {
	return this->icon;
}

int Player::GetPoints() const {
	return this->points;
}

bool Player::IsAlive() const {
	return this->alive;
}

void Player::kill() {
	this->alive = false;
	this->icon = 'X';
}

void Player::MoveUp() {
	if (dir != Direction::North) {
		dir = Direction::North;
	}
	else if (maze->IsAbleToMove(x, y - 1) && (!maze->IsPlayerAt(x, y - 1) || !maze->GetPlayer(x, y - 1)->IsAlive())) {
		y--;
		maze->transport(this);
		maze->EatBeans(this);
		maze->EatPlayers(maze->GetMonster());
	}
}

void Player::MoveDown() {
	if (dir != Direction::South) {
		dir = Direction::South;
	}
	else if (maze->IsAbleToMove(x, y + 1) && (!maze->IsPlayerAt(x, y + 1) || !maze->GetPlayer(x, y + 1)->IsAlive())) {
		y++;
		maze->transport(this);
		maze->EatBeans(this);
		maze->EatPlayers(maze->GetMonster());
	}
}

void Player::MoveRight() {
	if (dir != Direction::East) {
		dir = Direction::East;
	}
	else if (maze->IsAbleToMove(x + 1, y) && (!maze->IsPlayerAt(x + 1, y) || !maze->GetPlayer(x + 1, y)->IsAlive())) {
		x++;
		maze->transport(this);
		maze->EatBeans(this);
		maze->EatPlayers(maze->GetMonster());
	}
}

void Player::MoveLeft() {
	if (dir != Direction::West) {
		dir = Direction::West;
	}
	else if (maze->IsAbleToMove(x - 1, y) && (!maze->IsPlayerAt(x - 1, y) || !maze->GetPlayer(x - 1, y)->IsAlive())) {
		x--;
		maze->transport(this);
		maze->EatBeans(this);
		maze->EatPlayers(maze->GetMonster());
	}
}

bool Player::IsInView(int x, int y) const {
	int pos = x + maze->GetCol() * y;
	for (int i = 0; i < (int)view.size(); i++) {
		if (pos == view[i]) {
			return true;
		}
	}
	return false;
}

void Player::UpdateView() {
	view.clear();
	int col = maze->GetCol();
	switch (dir)
	{
	case Direction::North:
		for (int i = 0, j = 1; i < 6; i++) {
			if (maze->IsAbleToMove(x, y - i) && (!i || IsInView(x, y - (i - 1)))) view.push_back(x + col * (y - i));
			for (int k = 1; k <= j; k++) {
				if (maze->IsAbleToMove(x - k, y - i) && (!i || maze->IsAbleToMove(x - k + 1, y - (i - 1)) && IsInView(x - k + 1, y - (i - 1)))) view.push_back(x - k + col * (y - i));
				if (maze->IsAbleToMove(x + k, y - i) && (!i || maze->IsAbleToMove(x + k - 1, y - (i - 1)) && IsInView(x + k - 1, y - (i - 1)))) view.push_back(x + k + col * (y - i));
			}
			if (i & 1) j++;
		}
		break;
	case Direction::South:
		for (int i = 0, j = 1; i < 6; i++) {
			if (maze->IsAbleToMove(x, y + i) && (!i || IsInView(x, y + (i - 1)))) view.push_back(x + col * (y + i));
			for (int k = 1; k <= j; k++) {
				if (maze->IsAbleToMove(x - k, y + i) && (!i || maze->IsAbleToMove(x - k + 1, y + (i - 1)) && IsInView(x - k + 1, y + (i - 1)))) view.push_back(x - k + col * (y + i));
				if (maze->IsAbleToMove(x + k, y + i) && (!i || maze->IsAbleToMove(x + k - 1, y + (i - 1)) && IsInView(x + k - 1, y + (i - 1)))) view.push_back(x + k + col * (y + i));
			}
			if (i & 1) j++;
		}
		break;
	case Direction::East:
		for (int i = 0, j = 1; i < 6; i++) {
			if (maze->IsAbleToMove(x + i, y) && (!i || IsInView(x + (i - 1), y))) view.push_back(x + i + col * y);
			for (int k = 1; k <= j; k++) {
				if (maze->IsAbleToMove(x + i, y - k) && (!i || maze->IsAbleToMove(x + (i - 1), y - k + 1) && IsInView(x + (i - 1), y - k + 1))) view.push_back(x + i + col * (y - k));
				if (maze->IsAbleToMove(x + i, y + k) && (!i || maze->IsAbleToMove(x + (i - 1), y + k - 1) && IsInView(x + (i - 1), y + k - 1))) view.push_back(x + i + col * (y + k));
			}
			if (i & 1) j++;
		}
		break;
	case Direction::West:
		for (int i = 0, j = 1; i < 6; i++) {
			if (maze->IsAbleToMove(x - i, y) && (!i || IsInView(x - (i - 1), y))) view.push_back(x - i + col * y);
			for (int k = 1; k <= j; k++) {
				if (maze->IsAbleToMove(x - i, y - k) && (!i || maze->IsAbleToMove(x - (i - 1), y - k + 1) && IsInView(x - (i - 1), y - k + 1))) view.push_back(x - i + col * (y - k));
				if (maze->IsAbleToMove(x - i, y + k) && (!i || maze->IsAbleToMove(x - (i - 1), y + k - 1) && IsInView(x - (i - 1), y + k - 1))) view.push_back(x - i + col * (y + k));
			}
			if (i & 1) j++;
		}
		break;
	default:
		break;
	}
}

int Player::communicate(char* msg) {
	string str(msg);
	maze->GetBeans()->clear();
	if (str[0] != '0') {
#ifdef __linux__
		printf("\033c");
#endif
#ifdef _WIN32
		clrscr();
#endif
		DisplayMaze();
		if (str[0] == '1') maze->GameOver(1);
		else maze->GameOver(0);
#ifdef __linux__
		sleep(5);
#endif
#ifdef _WIN32
		Sleep(5000);
#endif

		return -1;
	}
	int idx = 1, st[4], pts[4] = { 0 }, pos[4] = { 0 };
	char ic[4];
	Direction d[4];
	for (int i = 0; i < 4; i++, idx++) {
		ic[i] = str[++idx]; idx++;
		st[i] = str[++idx] - '0'; idx++;
		while (str[++idx] != '|') pts[i] = pts[i] * 10 + str[idx] - '0';
		while (str[++idx] != '|') pos[i] = pos[i] * 10 + str[idx] - '0';
		d[i] = Direction(str[++idx] - '0');
	}
	if (maze->GetPlayers()->empty()) {
		icon = ic[0];
		alive = st[0];
		points = pts[0];
		x = pos[0] % maze->GetCol(), y = pos[0] / maze->GetCol();
		dir = d[0];
		maze->GetPlayers()->push_back(this);
		for (int i = 1; i < 4; i++) {
			Player *np = new Player(maze, ic[i]);
			np->alive = st[i];
			np->points = pts[i];
			np->x = pos[i] % maze->GetCol(), np->y = pos[i] / maze->GetCol();
			np->dir = d[i];
			maze->GetPlayers()->push_back(np);
		}
	}
	else {
		for (int i = 1; i < 4; i++) {
			for (int j = 0; j < (int)maze->GetPlayers()->size(); j++) {
				Player* now = maze->GetPlayers()->at(j);
				if (ic[i] == now->GetIcon()) {
					now->alive = st[i];
					now->points = pts[i];
					now->x = pos[i] % maze->GetCol(), now->y = pos[i] / maze->GetCol();
					now->dir = d[i];
					break;
				}
			}
		}
	}
	while (idx < str.size()) {
		int bean = 0;
		while (++idx < str.size() && str[idx] != '|') {
			bean = bean * 10 + str[idx] - '0';
		}
		maze->GetBeans()->push_back(bean);
	}
	int t = 0;
	msg[t++] = alive + '0';
	msg[t++] = '|';
	int flag = 0;
	for (int i = 1000; i > 0; i /= 10) {
		if (flag == 0 && points / i == 0) continue;
		flag = 1;
		msg[t++] = points / i + '0';
		points %= i;
	}
	if (!flag) msg[t++] = '0';
	msg[t++] = '|';
	flag = 0;
	for (int i = 100000; i > 0; i /= 10) {
		if (flag == 0 && x / i == 0) continue;
		flag = 1;
		msg[t++] = x / i + '0';
		x %= i;
	}
	if (!flag) msg[t++] = '0';
	msg[t++] = '|';
	msg[t++] = (int)dir + '0';
	msg[t] = 0;
	return t;
}

void Player::SetAlive(bool alive) {
	this->alive = alive;
}

void Player::SetPoints(int points) {
	this->points = points;
}

void Player::SetDir(Direction dir) {
	this->dir = dir;
}

void Maze::ReceiveMazeMap(const char* msg) {
	string str(msg);
	mazeMap.clear();
	portalA = portalB = 0;
	col = 0, row = 0;
	int idx = -1;
	while (str[++idx] != '|') col = col * 10 + str[idx] - '0';
	while (str[++idx] != '|') row = row * 10 + str[idx] - '0';
	while (str[++idx] != '|') mazeMap.push_back((Tile)(str[idx] - '0'));
	while (str[++idx] != '|') portalA = (portalA * 10) + str[idx] - '0';
	while (++idx < str.size()) portalB = (portalB * 10) + str[idx] - '0';
}

int Monster::communicate(char* msg) {
	string str(msg);
	maze->GetBeans()->clear();
	if (str[0] != '0') {
#ifdef __linux__
		printf("\033c");
#endif
#ifdef _WIN32
		clrscr();
#endif
		DisplayMaze();
		if (str[0] == '1') maze->GameOver(1);
		else maze->GameOver(0);
#ifdef __linux__
		sleep(5);
#endif
#ifdef _WIN32
		Sleep(5000);
#endif
		return -1;
	}
	int idx = 1, st[4], pts[4] = { 0 }, pos[4] = { 0 };
	char ic[4];
	Direction d[4];
	for (int i = 0; i < 4; i++, idx++) {
		ic[i] = str[++idx]; idx++;
		st[i] = str[++idx] - '0'; idx++;
		while (str[++idx] != '|') pts[i] = pts[i] * 10 + str[idx] - '0';
		while (str[++idx] != '|') pos[i] = pos[i] * 10 + str[idx] - '0';
		d[i] = Direction(str[++idx] - '0');
	}
	if (maze->GetPlayers()->empty()) {
		icon = ic[0];
		x = pos[0] % maze->GetCol(), y = pos[0] / maze->GetCol();
		dir = d[0];
		maze->SetMonster(this);
		for (int i = 1; i < 4; i++) {
			Player *np = new Player(maze, ic[i]);
			np->SetAlive(st[i]);
			np->SetPoints(pts[i]);
			np->SetX(pos[i] % maze->GetCol()), np->SetY(pos[i] / maze->GetCol());
			np->SetDir(d[i]);
			maze->GetPlayers()->push_back(np);
		}
	}
	else {
		for (int i = 1; i < 4; i++) {
			for (int j = 0; j < (int)maze->GetPlayers()->size(); j++) {
				Player* now = maze->GetPlayers()->at(j);
				if (ic[i] == now->GetIcon()) {
					now->SetAlive(st[i]);
					now->SetPoints(pts[i]);
					now->SetX(pos[i] % maze->GetCol()), now->SetY(pos[i] / maze->GetCol());
					now->SetDir(d[i]);
					break;
				}
			}
		}
	}
	while (idx < str.size()) {
		int bean = 0;
		while (++idx < str.size() && str[idx] != '|') {
			bean = bean * 10 + str[idx] - '0';
		}
		maze->GetBeans()->push_back(bean);
	}
	int t = 0;
	msg[t++] = 0 + '0';
	msg[t++] = '|';
	int flag = 0;
	for (int points = 0, i = 1000; i > 0; i /= 10) {
		if (flag == 0 && points / i == 0) continue;
		flag = 1;
		msg[t++] = points / i + '0';
		points %= i;
	}
	if (!flag) msg[t++] = '0';
	msg[t++] = '|';
	flag = 0;
	for (int i = 100000; i > 0; i /= 10) {
		if (flag == 0 && x / i == 0) continue;
		flag = 1;
		msg[t++] = x / i + '0';
		x %= i;
	}
	if (!flag) msg[t++] = '0';
	msg[t++] = '|';
	msg[t++] = (int)dir + '0';
	msg[t] = 0;
	return t;
}