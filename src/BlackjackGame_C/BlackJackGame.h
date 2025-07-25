#pragma once

#ifndef BLACKJACKGAME_H
#define BLACKJACKGAME_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h> // For timing

#include <raylib.h>

#include "Player.h"
#include "Dealer.h"
#include "HelperDefinitions.h"
#include "Controller.h"

#define ASSETS_DIRECTORY "\\Assets\\"
#define SPRITES_DIRECTORY "Sprites"
#define SAVE_FILE "saveFile.sav"

#define DEFAULT_GUI_FONT_SIZE 17
#define DEFAULT_GUI_BUTTON_WIDTH 80
#define DEFAULT_GUI_BUTTON_HEIGHT 30

#define LOWER_BAND_BOUNDS (Rectangle){ .x = 0,.y = GetScreenHeight() - 50, .width = GetScreenWidth(),.height = 200 }

typedef struct _blackJackGame
{
	Player* player;
	Dealer* dealer;

	struct _algoState
	{
		Hand* nextDrawHand;

		bool dealerTurn;
		bool stopPlayerInput;
		bool gameEnd;

		char* wonHand;

		clock_t lastDrawTime; 
		int drawDelay;        // Dealer delay in milliseconds between draws
	} algoState;

	struct _guiVars
	{
		Rectangle lowerBandBounds;

		Rectangle drawButtonBounds;
		Rectangle holdButtonBounds;
		Rectangle doubleDownButtonBounds;
		Rectangle textStartBounds;
		Rectangle increaseButtonBounds;
		Rectangle decreaseButtonBounds;

		char* balanceFormat;
		int balanceMeasure;

		char* betFormat;
		int betMeasure;

		char* playerHandFormat;
		int playerHandMeasure;

		char* dealerHandFormat;
		int dealerHandMeasure;
	} guiVars;

	uint8_t roundCount;

	char assetDir[260];
	char spriteDir[260];
	char saveFile[260];

	char wonAction[2]; // {'-', '+'}

	Texture2D* textureBucket; // 0 = card map
	size_t textureCount;
	size_t fc; // frame counter

	bool shouldRenderMainMenu;
	bool exitFromMenu;

	bool saveGame;
} BlackJackGame;

BlackJackGame BlackJackGame_New(Player* player, Dealer* dealer);

void BlackJackGame_InitGuiVars(BlackJackGame* game);

void BlackJackGame_ClearHands(BlackJackGame* game);
void BlackJackGame_RunGame(BlackJackGame* game);

void BlackJackGame_Render_Game(BlackJackGame* game);

void BlackJackGame_RenderPart_Won(BlackJackGame* game);
void BlackJackGame_RenderPart_GUI(BlackJackGame* game);
void BlackJackGame_RenderPart_Hands(BlackJackGame* game);
void BlackJackGame_RenderPart_Stats(BlackJackGame* game);

void BlackJackGame_Render_MainMenu(BlackJackGame* game);

void BlackJackGame_Algorithm(BlackJackGame* game);
void BlackJackGame_Reset(BlackJackGame* game);
void BlackJackGame_State_Reset(BlackJackGame* game);

void BlackJackGame_LoadSave(BlackJackGame* game);
void BlackJackGame_Save_Game(BlackJackGame* game);

void BlackJackGame_Free(BlackJackGame* game);

#endif