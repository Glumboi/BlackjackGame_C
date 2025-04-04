#pragma once
#define DEFAULT_PLAYER_BALANCE 500

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "Hand.h"

typedef struct _player
{
	Hand hand;
	size_t playerBet;
	int64_t playerBalance;
	uint8_t gameEnd;
} Player;

Player Player_New();
void Player_Draw(Player* player);
void Player_Win(Player* player);
void Player_Lose(Player* player);
