#pragma once
#include <string>
#include <iostream>
#include <map>
#include <queue>
#include <fstream>
#include <random> 
#include <assert.h>
#include <cstdio>
#include <cstdlib>
#ifdef __linux__
#include <termios.h> 
#include <unistd.h>
char getch(void);
#endif
#ifdef _WIN32 
#include <windows.h>
#include <conio.h>
#define PERR(bSuccess, api){if(!(bSuccess)) printf("%s:Error %d from %s on line %d\n", __FILE__, GetLastError(), api, __LINE__);}
inline void clrscr() {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD coordScreen = { 0,0 };//设置清屏后光标返回的屏幕左上角坐标
	BOOL bSuccess;
	DWORD cCharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO csbi;//保存缓冲区信息

	DWORD dwConSize;//当前缓冲区可容纳的字符数

	bSuccess = GetConsoleScreenBufferInfo(hConsole, &csbi);//获得缓冲区信息
	PERR(bSuccess, "GetConsoleScreenBufferInfo");
	dwConSize = csbi.dwSize.X * csbi.dwSize.Y;//缓冲区容纳字符数目

											  //用空格填充缓冲区
	bSuccess = FillConsoleOutputCharacter(hConsole, (TCHAR)' ', dwConSize, coordScreen, &cCharsWritten);
	PERR(bSuccess, "FillConsoleOutputCharacter");

	bSuccess = GetConsoleScreenBufferInfo(hConsole, &csbi);//获得缓冲区信息
	PERR(bSuccess, "ConsoleScreenBufferInfo");

	//填充缓冲区属性
	bSuccess = FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten);
	PERR(bSuccess, "FillConsoleOutputAttribute");

	//光标返回屏幕左上角坐标
	bSuccess = SetConsoleCursorPosition(hConsole, coordScreen);
	PERR(bSuccess, "SetConsoleCursorPosition");
	return;
}
BOOL SetConsoleColor(WORD wAttributes);
#endif

#define MAX_TILES_NUM  10000

#define MAX_ROOM_WIDTH  8
#define MAX_ROOM_HEIGHT 8
#define MIN_ROOM_WIDTH  4
#define MIN_ROOM_HEIGHT 4

// 房间和走廊的合计最大个数
#define DEFAULT_FEATURE_NUM 1000

// 尝试生成房间和走廊的测试次数（即步长）
#define MAX_TRY_TIMES  1000

// 默认创建房间的概率（100-该值则为创建走廊的概率）
#define DEFAULT_CREATE_ROOM_CHANCE  70

// 走廊长度
#define MIN_CORRIDOR_LEN   3
#define MAX_CORRIDOR_LEN   6

using namespace std;
enum class Direction
{
	North,  // 北
	South,  // 南
	East,  // 东
	West,  // 西
};
enum class Tile
{
	Unused, // 没用的格子（土块）
	DirtWall,  // 墙壁
	DirtFloor,  // 房间地板
	Corridor,  // 走廊
	Door,  // 房门
	UpStairs,  // 入口
	DownStairs // 出口
};

Tile toTile(char ch);
char toChar(Tile tile);