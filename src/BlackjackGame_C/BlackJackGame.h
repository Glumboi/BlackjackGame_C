#pragma once

#include "Player.h"
#include "Dealer.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h> // For timing

#include <raylib.h>

#define ASSETS_DIRECTORY "\\Assets\\"
#define SPRITES_DIRECTORY "Sprites"

#define RENDER_BEGIN BeginDrawing();\
					 ClearBackground(GRAY);

#define RENDER_END	EndDrawing();
#define COLOR_TRANSPARENT (Color){.a = 0, .b = 255, .g = 255, .r = 255}

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
		bool gameEnd;

		char* wonHand;

		clock_t lastDrawTime; // Track the last time a card was drawn
		int drawDelay;        // Delay in milliseconds between draws
	} algoState;

	uint8_t roundCount;

	char assetDir[1024];
	char spriteDir[1024];

	char wonAction[2]; // {'-', '+'}

	Texture2D* textureBucket;
	size_t textureCount;
	size_t fc; // frame counter

} BlackJackGame;

BlackJackGame BlackJackGame_New(Player* player, Dealer* dealer);

void BlackJackGame_ClearHands(BlackJackGame* game);
void BlackJackGame_RunGame(BlackJackGame* game);
void BlackJackGame_Render(BlackJackGame* game);
void BlackJackGame_RenderPart_Won(BlackJackGame* game);
void BlackJackGame_RenderPart_Stats(BlackJackGame* game);
void BlackJackGame_Input(BlackJackGame* game);
void BlackJackGame_Algorithm(BlackJackGame* game);
void BlackJackGame_Reset(BlackJackGame* game);
void BlackJackGame_State_Reset(BlackJackGame* game);