#pragma once

#include "Player.h"
#include "Dealer.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <raylib.h>

#define ASSETS_DIRECTORY "\\Assets\\"
#define SPRITES_DIRECTORY "Sprites"

typedef struct _blackJackGame
{
	Player* player;
	Dealer* dealer;

	struct _algoState
	{
		Hand* nextDrawHand;
		bool hasBetAlready;

		bool dealerTurn;
		bool stopPlayer;
	} algoState;

	uint8_t roundCount;

	char assetDir[1024];
	char spriteDir[1024];

	Texture2D* textureBucket;
	size_t textureCount;

} BlackJackGame;

BlackJackGame BlackJackGame_New(Player* player, Dealer* dealer);
void BlackJackGame_ClearHands(BlackJackGame* game);
void BlackJackGame_RunGame(BlackJackGame* game);
void BlackJackGame_Render(BlackJackGame* game);
void BlackJackGame_Input(BlackJackGame* game);
void BlackJackGame_Algorithm(BlackJackGame* game);
void BlackJackGame_State_Reset(BlackJackGame* game);