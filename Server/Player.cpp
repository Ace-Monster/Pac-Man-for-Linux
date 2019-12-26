#include "Player.h"

void Player::init(int id, int initx) {
	icon = id;
	x = initx;
	y = 1;
	points = 0;
	status = STATUS::LIVE;
}