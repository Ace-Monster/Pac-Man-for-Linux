#pragma once
#include "Global.h"
#include "Maze.h"
class MazeGenerator
{
public:
	int m_nSeed;
	int m_nXSize, m_nYSize;
	int m_nMaxFeatures;
	int m_nChanceRoom;
	int m_nChanceCorridor;

	typedef mt19937 RngT;
public:
	MazeGenerator(int XSize, int YSize) :
		m_nSeed(random_device()()),
		m_nXSize(XSize), m_nYSize(YSize),
		m_nMaxFeatures(DEFAULT_FEATURE_NUM),
		m_nChanceRoom(DEFAULT_CREATE_ROOM_CHANCE) {
		m_nChanceCorridor = 100 - m_nChanceRoom;
	}

	Maze Generate() {
		assert(m_nMaxFeatures > 0 && m_nMaxFeatures <= DEFAULT_FEATURE_NUM);
		assert(m_nXSize > 3);
		assert(m_nYSize > 3);

		auto rng = RngT(m_nSeed);
		auto maze = Maze(m_nXSize, m_nYSize, Tile::Unused);

		MakeDungeon(maze, rng);
		SpillBeans(maze, rng);
		return maze;
	}

	int GetPlayerPos(Maze* maze) {
		//m_nSeed = random_device()();
		auto rng = RngT(m_nSeed);
		while (true) {
			int x = GetRandomInt(rng, 1, maze->GetCol() - 2);
			int y = GetRandomInt(rng, 1, maze->GetRow() - 2);

			if (maze->GetCell(x, y) == Tile::DirtFloor || maze->GetCell(x, y) == Tile::Corridor) {
				if (!maze->IsPlayerAt(x, y)) {
					return x + y * maze->GetCol();
				}
			}
		}
	}

private:

	int GetRandomInt(RngT& rng, int min, int max) const {
		return uniform_int_distribution<int>(min, max)(rng);
	}

	Direction GetRandomDirection(RngT& rng) const {
		return Direction(uniform_int_distribution<int>(static_cast<int>(Direction::North), static_cast<int>(Direction::West))(rng));
	}

	void SpillBeans(Maze& maze, RngT& rng) const {
		int NumOfBeans = m_nXSize * m_nYSize / 10;
		while (NumOfBeans--) {
			for (auto tries = 0; tries != MAX_TRY_TIMES; ++tries) {
				int x = GetRandomInt(rng, 1, m_nXSize - 2);
				int y = GetRandomInt(rng, 1, m_nYSize - 2);

				if (maze.GetCell(x, y) == Tile::DirtFloor || maze.GetCell(x, y) == Tile::Corridor) {
					if (!maze.IsBeanAt(x, y)) {
						maze.GetBeans()->push_back(x + y * m_nXSize);
						break;
					}
				}
			}
		}
	}

	bool MakeCorridor(Maze& maze, RngT& rng, int x, int y, int maxLength, Direction direction) const {
		assert(x >= 0 && x < m_nXSize);
		assert(y >= 0 && y < m_nYSize);

		assert(maxLength > 0 && maxLength <= max(m_nXSize, m_nYSize));

		auto length = GetRandomInt(rng, MIN_CORRIDOR_LEN, maxLength);

		auto xStart = x;
		auto yStart = y;

		auto xEnd = x;
		auto yEnd = y;

		if (direction == Direction::North)
			yStart = y - length;
		else if (direction == Direction::East)
			xEnd = x + length;
		else if (direction == Direction::South)
			yEnd = y + length;
		else if (direction == Direction::West)
			xStart = x - length;

		if (!maze.IsXInBounds(xStart) || !maze.IsXInBounds(xEnd) || !maze.IsYInBounds(yStart) || !maze.IsYInBounds(yEnd))
			return false;

		if (!maze.IsAreaUnused(xStart, yStart, xEnd, yEnd))
			return false;

		maze.SetCells(xStart, yStart, xEnd, yEnd, Tile::Corridor);

		return true;
	}

	bool MakeRoom(Maze& maze, RngT& rng, int x, int y, int xMaxLength, int yMaxLength, Direction direction) const {
		assert(xMaxLength >= MIN_ROOM_WIDTH);
		assert(yMaxLength >= MIN_ROOM_HEIGHT);

		auto xLength = GetRandomInt(rng, MIN_ROOM_WIDTH, xMaxLength);
		auto yLength = GetRandomInt(rng, MIN_ROOM_HEIGHT, yMaxLength);

		auto xStart = x;
		auto yStart = y;
		auto xEnd = x;
		auto yEnd = y;

		if (direction == Direction::North) {
			yStart = y - yLength;
			xStart = x - xLength / 2;
			xEnd = x + (xLength + 1) / 2;
		}
		else if (direction == Direction::East) {
			yStart = y - yLength / 2;
			yEnd = y + (yLength + 1) / 2;
			xEnd = x + xLength;
		}
		else if (direction == Direction::South) {
			yEnd = y + yLength;
			xStart = x - xLength / 2;
			xEnd = x + (xLength + 1) / 2;
		}
		else if (direction == Direction::West) {
			yStart = y - yLength / 2;
			yEnd = y + (yLength + 1) / 2;
			xStart = x - xLength;
		}

		if (!maze.IsXInBounds(xStart) || !maze.IsXInBounds(xEnd) || !maze.IsYInBounds(yStart) || !maze.IsYInBounds(yEnd))
			return false;

		if (!maze.IsAreaUnused(xStart, yStart, xEnd, yEnd))
			return false;

		maze.SetCells(xStart, yStart, xEnd, yEnd, Tile::DirtWall);
		maze.SetCells(xStart + 1, yStart + 1, xEnd - 1, yEnd - 1, Tile::DirtFloor);

		return true;
	}

	bool MakeRoomOrCorridor(Maze& maze, RngT& rng, int x, int y, int xmod, int ymod, Direction direction) const {
		auto chance = GetRandomInt(rng, 0, 100);

		if (chance <= m_nChanceRoom) {
			if (MakeRoom(maze, rng, x + xmod, y + ymod, MAX_ROOM_WIDTH, MAX_ROOM_HEIGHT, direction)) {
				maze.SetCell(x, y, Tile::Door);
				maze.SetCell(x + xmod, y + ymod, Tile::DirtFloor);

				return true;
			}

			return false;
		}
		else {
			if (MakeCorridor(maze, rng, x + xmod, y + ymod, MAX_CORRIDOR_LEN, direction)) {
				maze.SetCell(x, y, Tile::Door);
				return true;
			}

			return false;
		}
	}

	bool MakeRandomFeature(Maze& maze, RngT& rng) const {
		for (auto tries = 0; tries != MAX_TRY_TIMES; ++tries) {
			int x = GetRandomInt(rng, 1, m_nXSize - 2);
			int y = GetRandomInt(rng, 1, m_nYSize - 2);

			if (maze.GetCell(x, y) != Tile::DirtWall && maze.GetCell(x, y) != Tile::Corridor)
				continue;

			if (maze.IsAdjacent(x, y, Tile::Door))
				continue;

			if (maze.GetCell(x, y + 1) == Tile::DirtFloor || maze.GetCell(x, y + 1) == Tile::Corridor)
			{
				if (MakeRoomOrCorridor(maze, rng, x, y, 0, -1, Direction::North))
					return true;
			}
			else if (maze.GetCell(x - 1, y) == Tile::DirtFloor || maze.GetCell(x - 1, y) == Tile::Corridor)
			{
				if (MakeRoomOrCorridor(maze, rng, x, y, 1, 0, Direction::East))
					return true;
			}
			else if (maze.GetCell(x, y - 1) == Tile::DirtFloor || maze.GetCell(x, y - 1) == Tile::Corridor)
			{
				if (MakeRoomOrCorridor(maze, rng, x, y, 0, 1, Direction::South))
					return true;
			}
			else if (maze.GetCell(x + 1, y) == Tile::DirtFloor || maze.GetCell(x + 1, y) == Tile::Corridor)
			{
				if (MakeRoomOrCorridor(maze, rng, x, y, -1, 0, Direction::West))
					return true;
			}
		}

		return false;
	}

	bool MakeRandomStairs(Maze& maze, RngT& rng, Tile tile) const {
		auto tries = 0;
		auto maxTries = MAX_TILES_NUM;

		for (; tries != maxTries; ++tries) {
			int x = GetRandomInt(rng, 1, m_nXSize - 2);
			int y = GetRandomInt(rng, 1, m_nYSize - 2);

			if (!maze.IsAdjacent(x, y, Tile::DirtFloor) && !maze.IsAdjacent(x, y, Tile::Corridor))
				continue;

			if (maze.IsAdjacent(x, y, Tile::Door))
				continue;

			maze.SetCell(x, y, tile);
			if (tile == Tile::UpStairs) {
				maze.portalA = x + y * m_nXSize;
			}
			else {
				maze.portalB = x + y * m_nXSize;
			}
			return true;
		}

		return false;
	}

	bool MakeDungeon(Maze& maze, RngT& rng) const {
		MakeRoom(maze, rng, m_nXSize / 2, m_nYSize / 2, MAX_ROOM_WIDTH, MAX_ROOM_HEIGHT, GetRandomDirection(rng));

		for (auto features = 1; features != m_nMaxFeatures; ++features) {
			if (!MakeRandomFeature(maze, rng)) {
				break;
			}
		}

		if (!MakeRandomStairs(maze, rng, Tile::UpStairs))
			cerr << "ERROR A！" << endl;

		if (!MakeRandomStairs(maze, rng, Tile::DownStairs))
			cerr << "ERROR B！" << endl;

		return true;
	}
};