#pragma once
#define DEFAULT_PLAYER_BALANCE 500

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "Hand.h"

typedef struct _player
{
	Hand hand;
	size_t bet;
	size_t balanceDelta;
	int64_t balance;
	uint8_t gameEnd;
	bool won;
} Player;

Player Player_New();
void Player_Draw(Player* player);
