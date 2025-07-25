#include "BlackJackGame.h"

#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

#ifdef _DEBUG
void* MallocLog(size_t sz)
{
	TraceLog(LOG_INFO, TextFormat("[%s call]: %lld", __FUNCTION__, sz)); 
	return malloc(sz);
}
#define malloc(sz) MallocLog(sz)

void FreeLog(void* mem)
{
	TraceLog(LOG_INFO, TextFormat("[%s call]", __FUNCTION__));
	free(mem);
}
#define free(mem) FreeLog(mem)
#endif

BlackJackGame BlackJackGame_New(Player* player, Dealer* dealer)
{
	PTR_VALIDATE(player, abort(););
	PTR_VALIDATE(dealer, abort(););

	BlackJackGame result = { 0 };
	if (!dealer || !player) return result;

	result.player = player;
	result.dealer = dealer;
	result.shouldRenderMainMenu = true;
	result.saveGame = true;

	result.wonAction[0] = '-';
	result.wonAction[1] = '+';

	result.algoState.drawDelay = 1000; // Set the dealer delay to 1 second
	result.algoState.wonHand = NULL;

	strcpy_s(result.assetDir, 260, GetApplicationDirectory());
	strcat_s(result.assetDir, 260, ASSETS_DIRECTORY);

	strcpy_s(result.spriteDir, 260, result.assetDir);
	strcat_s(result.spriteDir, 260, SPRITES_DIRECTORY);

	strcpy_s(result.saveFile, 260, result.assetDir);
	strcat_s(result.saveFile, 260, SAVE_FILE);

	if (DirectoryExists(result.assetDir))
	{
		SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);

		InitWindow(800, 600, "GlumJack");

		char iconBuff[260] = { 0 };
		sprintf_s(iconBuff, 260, "%s%s", result.assetDir, "Icon.png");

		Image icon = LoadImage(iconBuff);
		SetWindowIcon(icon);
		UnloadImage(icon);

		TraceLog(LOG_INFO, "Asset directory found!\nFound in: %s", result.assetDir);
		TraceLog(LOG_INFO, "Assumed sprites directory: %s", result.spriteDir);
		TraceLog(LOG_INFO, "Assumed save file: %s", result.saveFile);

		FilePathList sprites = LoadDirectoryFiles(result.spriteDir);

		// TODO: Remove malloc maybe, I only use a single map of textures, so it will always be one.
		// Which would allow for a stack allocation instead

		result.textureCount = sprites.count;
		result.textureBucket = malloc(result.textureCount * sizeof(Texture2D));

		for (size_t i = 0; i < sprites.count; i++)
		{
			TraceLog(LOG_INFO, "Loading sprite: %s", sprites.paths[i]);
			result.textureBucket[i] = LoadTexture(sprites.paths[i]);
		}
	}
	else
	{
		TraceLog(LOG_ERROR, "Asset directory not found!\nExpected in: %s", result.assetDir);
	}

	BlackJackGame_InitGuiVars(&result);
	return result;
}

void BlackJackGame_InitGuiVars(BlackJackGame* game)
{
	// gui init
	game->guiVars.lowerBandBounds = LOWER_BAND_BOUNDS;
	game->guiVars.drawButtonBounds =
		(Rectangle)
	{
		.width = DEFAULT_GUI_BUTTON_WIDTH,
		.height = DEFAULT_GUI_BUTTON_HEIGHT,
		.x = game->guiVars.lowerBandBounds.x + 10,
		.y = game->guiVars.lowerBandBounds.y + 10
	};

	game->guiVars.holdButtonBounds =
		(Rectangle)
	{
		.width = DEFAULT_GUI_BUTTON_WIDTH,
		.height = DEFAULT_GUI_BUTTON_HEIGHT,
		.x = game->guiVars.drawButtonBounds.x + game->guiVars.drawButtonBounds.width + 10,
		.y = game->guiVars.lowerBandBounds.y + 10
	};

	game->guiVars.doubleDownButtonBounds =
		(Rectangle)
	{
		.width = DEFAULT_GUI_BUTTON_WIDTH + 15,
		.height = DEFAULT_GUI_BUTTON_HEIGHT,
		.x = game->guiVars.holdButtonBounds.x + game->guiVars.holdButtonBounds.width + 10,
		.y = game->guiVars.lowerBandBounds.y + 10
	};

	game->guiVars.textStartBounds = (Rectangle){ .x = game->guiVars.doubleDownButtonBounds.x + game->guiVars.doubleDownButtonBounds.width + 10, .y = game->guiVars.lowerBandBounds.y + 10 };

	game->guiVars.increaseButtonBounds =
		(Rectangle)
	{
		.width = 15,
		.height = 15,
		.x = game->guiVars.textStartBounds.x + game->guiVars.betMeasure + 10,
		.y = game->guiVars.textStartBounds.y + DEFAULT_GUI_FONT_SIZE
	};

	game->guiVars.decreaseButtonBounds =
		(Rectangle)
	{
		.width = 15,
		.height = 15,
		.x = game->guiVars.increaseButtonBounds.x + 20,
		.y = game->guiVars.textStartBounds.y + DEFAULT_GUI_FONT_SIZE
	};

	game->guiVars.balanceFormat = TextFormat("Balance: %lld$", game->player->playerStats.balance);
	game->guiVars.balanceMeasure = MeasureText(game->guiVars.balanceFormat, DEFAULT_GUI_FONT_SIZE);

	game->guiVars.betFormat = TextFormat("Current bet: %lld$", game->player->playerStats.bet);
	game->guiVars.betMeasure = MeasureText(game->guiVars.betFormat, DEFAULT_GUI_FONT_SIZE);

	game->guiVars.playerHandFormat = TextFormat("Your hand: %lld",
		game->player->hand.handTotal);
	game->guiVars.playerHandMeasure = MeasureText(game->guiVars.playerHandFormat, DEFAULT_GUI_FONT_SIZE);

	game->guiVars.dealerHandFormat = TextFormat("Dealer hand: %lld",
		game->dealer->hand.handTotal);
	game->guiVars.dealerHandMeasure = MeasureText(game->guiVars.dealerHandFormat, DEFAULT_GUI_FONT_SIZE);

}

void BlackJackGame_ClearHands(BlackJackGame* game)
{
	Hand_Clear(&game->player->hand);
	Hand_Clear(&game->dealer->hand);
}

void BlackJackGame_RunGame(BlackJackGame* game)
{
	PTR_VALIDATE(game, return;);
	game->algoState.dealerTurn = false;
	while (!WindowShouldClose() && !game->exitFromMenu)
	{
		if (game->shouldRenderMainMenu)
		{
			BlackJackGame_Render_MainMenu(game);
		}
		else
		{
			BlackJackGame_Render_Game(game);
			BlackJackGame_Algorithm(game);
			BlackJackGame_State_Reset(game);
		}
	}
}

size_t fcExpect = 0;
void BlackJackGame_RenderPart_Won(BlackJackGame* game)
{
	if (game->algoState.wonHand != NULL)
	{
		game->algoState.stopPlayerInput = true;

		char* message = TextFormat("%s has won!\n%c %lld$",
			game->algoState.wonHand,
			game->wonAction[game->player->won],
			game->player->playerStats.bet);

		Color textColor = game->player->won ? GREEN : RED;
		int fontSize = 30;
		int textWidth = MeasureText(message, fontSize);
		int textX = (GetScreenWidth() - textWidth) / 2;
		int textY = (GetScreenHeight() - fontSize) / 2;

		// Wait 5 seconds while showing the message
		if (fcExpect > 0)
		{
			if (game->fc < fcExpect)
			{
				DrawText(message, textX, textY, fontSize, textColor);
				if (++game->fc == SIZE_MAX)
					game->fc = 0;
				return;

			}
			fcExpect = 0;
		}
		else
		{
			fcExpect = game->fc + 5 * 1.0f / (GetFrameTime());
			return;
		}

		BlackJackGame_Reset(game);
	}
}

void BlackJackGame_RenderPart_GUI(BlackJackGame* game)
{
	BlackJackGame_InitGuiVars(game);

	DrawRectangleRec(game->guiVars.lowerBandBounds, BLACK);

	if ((GuiButton(game->guiVars.drawButtonBounds, "Draw (A)") || CONTROLLER_PRESSED(CONTROLLER_A))
		&& !game->algoState.stopPlayerInput
		&& game->player->playerStats.bet > 0)
	{
		Player_Draw(game->player);
		game->algoState.stopPlayerInput = true;
		game->algoState.dealerTurn = true;
		game->algoState.nextDrawHand = &game->dealer->hand;
	}

	if ((GuiButton(game->guiVars.holdButtonBounds, "Hold (X)") || CONTROLLER_PRESSED(CONTROLLER_X))
		&& !game->algoState.stopPlayerInput && Player_AllowPlay(game->player))
	{
		game->algoState.stopPlayerInput = true;
		game->algoState.dealerTurn = true;
		game->algoState.nextDrawHand = &game->dealer->hand;
		game->player->holding = true;
	}

	if ((GuiButton(game->guiVars.doubleDownButtonBounds, "Double down (Y)") || CONTROLLER_PRESSED(CONTROLLER_Y))
		&& !game->algoState.stopPlayerInput && Player_AllowPlay(game->player))
	{
		if (game->player->playerStats.balance * 2 >= game->player->playerStats.bet * 2)
		{
			Player_Draw(game->player);
			game->algoState.stopPlayerInput = true;
			game->algoState.dealerTurn = true;
			game->algoState.nextDrawHand = &game->dealer->hand;
			game->player->playerStats.bet *= 2;
			game->player->playerStats.balance -= game->player->playerStats.bet;
			game->player->holding = true;
			game->player->doubledDown = true;
		}
	}

	DrawText(game->guiVars.balanceFormat, game->guiVars.textStartBounds.x, game->guiVars.textStartBounds.y, DEFAULT_GUI_FONT_SIZE, LIME);
	DrawText(game->guiVars.betFormat, game->guiVars.textStartBounds.x, game->guiVars.textStartBounds.y + DEFAULT_GUI_FONT_SIZE, DEFAULT_GUI_FONT_SIZE, WHITE);
	DrawText(game->guiVars.playerHandFormat, game->guiVars.textStartBounds.x + game->guiVars.betMeasure + 60 /*offset relative to the buttons below*/, game->guiVars.textStartBounds.y, DEFAULT_GUI_FONT_SIZE, WHITE);
	DrawText(game->guiVars.dealerHandFormat, game->guiVars.textStartBounds.x + game->guiVars.betMeasure + 60 /*offset relative to the buttons below*/, game->guiVars.textStartBounds.y + DEFAULT_GUI_FONT_SIZE, DEFAULT_GUI_FONT_SIZE, WHITE);

	if ((GuiButton(game->guiVars.increaseButtonBounds, "+") || CONTROLLER_PRESSED(CONTROLLER_CROSS_UP))
		&& !game->algoState.stopPlayerInput)
	{
		Player_IncreaseBet(game->player);
		game->algoState.nextDrawHand = &game->player->hand;
	}

	if ((GuiButton(game->guiVars.decreaseButtonBounds, "-") || CONTROLLER_PRESSED(CONTROLLER_CROSS_DOWN))
		&& !game->algoState.stopPlayerInput)
	{
		Player_DecreaseBet(game->player);
	}
}

void BlackJackGame_RenderPart_Hands(BlackJackGame* game)
{
	Hand_Render(&game->player->hand, game->textureBucket[0], false);
	Hand_Render(&game->dealer->hand, game->textureBucket[0], true);
}

void BlackJackGame_RenderPart_Stats(BlackJackGame* game)
{
	Rectangle lowerBandBounds = LOWER_BAND_BOUNDS;
	DrawText(TextFormat("Your hand: %lld",
		game->player->hand.handTotal), 10, lowerBandBounds.y - 24, 24, BLACK);
	DrawText(TextFormat("Dealer hand: %lld",
		game->dealer->hand.handTotal), 10, lowerBandBounds.y - 48, 24, BLACK);
}

void BlackJackGame_Render_Game(BlackJackGame* game)
{
	RENDER_BEGIN(GRAY);
	BlackJackGame_RenderPart_GUI(game);
	BlackJackGame_RenderPart_Hands(game);
	BlackJackGame_RenderPart_Won(game);
	RENDER_END;

	if (++game->fc == SIZE_MAX)
		game->fc = 0;
}

void BlackJackGame_Render_MainMenu(BlackJackGame* game)
{
	RENDER_BEGIN(GRAY);

	DrawText("'GlumJack', made by Glummy", SCREEN_CENT.x - 150, SCREEN_CENT.y - 20, 22, BLACK);

	Rectangle playBounds =
	{
		.x = SCREEN_CENT.x - DEFAULT_GUI_BUTTON_WIDTH ,
		.y = SCREEN_CENT.y + 20,
		.width = DEFAULT_GUI_BUTTON_WIDTH * 2,
		.height = DEFAULT_GUI_BUTTON_HEIGHT * 2
	};

	if (GuiButton(playBounds, "PLAY! (A)") || CONTROLLER_PRESSED(CONTROLLER_A))
	{
		game->shouldRenderMainMenu = false;
		BlackJackGame_LoadSave(game);
	}

	Rectangle exitBounds =
	{
		.x = SCREEN_CENT.x - DEFAULT_GUI_BUTTON_WIDTH ,
		.y = SCREEN_CENT.y + playBounds.height + 20,
		.width = DEFAULT_GUI_BUTTON_WIDTH * 2,
		.height = DEFAULT_GUI_BUTTON_HEIGHT * 2
	};

	if (GuiButton(exitBounds, "EXIT (B)") || CONTROLLER_PRESSED(CONTROLLER_B))
	{
		game->exitFromMenu = true;
	}

	Rectangle saveGameCheckboxBounds = {

		.x = SCREEN_CENT.x - DEFAULT_GUI_BUTTON_WIDTH ,
		.y = SCREEN_CENT.y + exitBounds.height * 2 + 20,
		.width = 25,
		.height = 25
	};

	GuiCheckBox(saveGameCheckboxBounds, "Save game?", &game->saveGame);

	RENDER_END;
}

void BlackJackGame_Algorithm(BlackJackGame* game)
{
	if (game->player->playerStats.bet <= 0)
		return;

	int playerTotal = Hand_GetTotal(&game->player->hand);
	int dealerTotal = Hand_GetTotal(&game->dealer->hand);
	bool playerBust = playerTotal > 21;
	bool dealerBust = dealerTotal > 21;

	bool playerTurnOver = game->algoState.stopPlayerInput || playerBust;

	if (game->algoState.wonHand == NULL && (playerBust || dealerBust || (playerTurnOver && !game->algoState.dealerTurn && game->algoState.nextDrawHand == NULL)))
	{

		if (playerBust)
		{
			game->player->playerStats.balance -= game->player->playerStats.bet;
			game->algoState.wonHand = game->dealer->hand.handName;
			game->player->won = false;
		}
		else if (dealerBust || playerTotal > dealerTotal)
		{
			game->player->playerStats.balance += game->player->playerStats.bet;
			game->algoState.wonHand = game->player->hand.handName;
			game->player->won = true;

		}
		else if (playerTotal == dealerTotal)
		{
			game->player->playerStats.balance += game->player->playerStats.bet;
		}


		return;
	}

	bool playerBlackjack = playerTotal == 21;
	bool dealerBlackjack = dealerTotal == 21;

	if (game->algoState.wonHand == NULL && playerBlackjack || dealerBlackjack)
	{
		game->algoState.stopPlayerInput = true;
		game->algoState.dealerTurn = false;

		if (playerBlackjack && dealerBlackjack)
		{
			game->player->playerStats.balance += game->player->playerStats.bet;
		}
		else if (playerBlackjack)
		{
			game->player->playerStats.balance += game->player->playerStats.bet + (int)(game->player->playerStats.bet * 1.5);
			game->player->won = true;
			game->algoState.wonHand = game->player->hand.handName;
		}
		else if (dealerBlackjack)
		{
			game->player->won = false;
			game->algoState.wonHand = game->dealer->hand.handName;
		}

		return;
	}

	if (!game->algoState.dealerTurn && game->algoState.nextDrawHand != NULL)
	{
		//Hand_Draw(game->algoState.nextDrawHand);
		game->algoState.nextDrawHand = NULL;

		if (playerTotal > 21)
		{
			game->algoState.stopPlayerInput = true;
		}
		else
		{
			game->algoState.dealerTurn = true;
			game->algoState.lastDrawTime = clock();
		}
	}

	if (game->algoState.dealerTurn && game->algoState.stopPlayerInput)
	{
		if (game->player->holding)
		{
			Dealer_Draw(game->dealer);
			game->algoState.stopPlayerInput = false;
			game->algoState.dealerTurn = false;
		}
		else
		{
			clock_t currentTime = clock();
			if ((currentTime - game->algoState.lastDrawTime) * 1000 / CLOCKS_PER_SEC >= game->algoState.drawDelay)
			{
				if (dealerTotal < 17)
				{
					Dealer_Draw(game->dealer);
					game->algoState.lastDrawTime = currentTime;
					return;
				}
				game->algoState.stopPlayerInput = false;

			}
		}

	}
}

void BlackJackGame_Reset(BlackJackGame* game)
{
	PTR_VALIDATE(game, return;);

	// Reset for next round

	game->algoState.wonHand = NULL;
	game->player->won = false;

	BlackJackGame_ClearHands(game);

	game->algoState.stopPlayerInput = false;
	game->algoState.dealerTurn = false;
	game->algoState.nextDrawHand = NULL;
	game->algoState.lastDrawTime = clock();
	game->player->holding = false;

	if (game->player->doubledDown)
	{
		game->player->doubledDown = false;
		game->player->playerStats.bet /= 2;
	}
}

void BlackJackGame_State_Reset(BlackJackGame* game)
{
	game->algoState.nextDrawHand = NULL;
}

void BlackJackGame_LoadSave(BlackJackGame* game)
{
	PTR_VALIDATE(game, return;);

	if (!FileExists(game->saveFile) || !game->saveGame) return;

	size_t sizeOut = 0;
	uint8_t* saveFileBytes = LoadFileData(game->saveFile, &sizeOut);

	if (game->player)
		memcpy(&game->player->playerStats, saveFileBytes, sizeOut);
	UnloadFileData(saveFileBytes);
}

void BlackJackGame_Save_Game(BlackJackGame* game)
{
	if (!game || !game->saveGame) return;
	SaveFileData(game->saveFile, &game->player->playerStats, sizeof(struct _playerStats));
}

void BlackJackGame_Free(BlackJackGame* game)
{
	PTR_VALIDATE(game, return;);
	BlackJackGame_ClearHands(game);
	for (size_t i = 0; i < game->textureCount; i++)
	{
		UnloadTexture(game->textureBucket[i]);
	}
	PTR_FREE(game->textureBucket);
	game->textureCount = 0;
}