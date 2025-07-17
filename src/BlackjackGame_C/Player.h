#ifndef PLAYER_H
#define PLAYER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "Hand.h"

#define DEFAULT_PLAYER_BALANCE 500
#define DEFAULT_PLAYER_BET_STEP 5

typedef struct _player
{
	struct _playerStats
	{
		size_t bet;
		size_t balanceDelta;
		int64_t balance;
	} playerStats;

	Hand hand;
	uint8_t gameEnd;
	bool won;
	bool holding;
	bool doubledDown;
} Player;

Player Player_New();
void Player_Draw(Player* player);
void Player_IncreaseBet(Player* player);
void Player_DecreaseBet(Player* player);
bool Player_AllowPlay(Player* player);

#endif