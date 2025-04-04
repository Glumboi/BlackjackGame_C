#include "BlackJackGame.h"

BlackJackGame BlackJackGame_New(Player* player, Dealer* dealer)
{
	BlackJackGame result = { 0 };
	if (!dealer || !player) return result;

	result.player = player;
	result.dealer = dealer;

	strcpy_s(result.assetDir, 1024, GetWorkingDirectory());
	strcat_s(result.assetDir, 1024, ASSETS_DIRECTORY);

	strcpy_s(result.spriteDir, 1024, result.assetDir);
	strcat_s(result.spriteDir, 1024, SPRITES_DIRECTORY);

	if (DirectoryExists(result.assetDir))
	{
		SetConfigFlags(FLAG_WINDOW_RESIZABLE);
		InitWindow(800, 600, "Blackjack");

		TraceLog(LOG_INFO, "Asset directory found!\nFound in: %s", result.assetDir);
		TraceLog(LOG_INFO, "Assumed sprites directory: %s", result.spriteDir);

		FilePathList sprites = LoadDirectoryFiles(result.spriteDir);

		result.textureBucket = malloc(sprites.count * sizeof(Texture2D));
		result.textureCount = sprites.count;

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

	return result;
}

void BlackJackGame_ClearHands(BlackJackGame* game)
{
	if (!game) return;
	game->player->hand = Hand_New();
	game->dealer->hand = Hand_New();
}

void BlackJackGame_RunGame(BlackJackGame* game)
{
	if (!game) return;

	while (!WindowShouldClose())
	{
		BlackJackGame_Input(game);
		BlackJackGame_Algorithm(game);
		BlackJackGame_Render(game);
		BlackJackGame_State_Reset(game);
	}
}

void BlackJackGame_Render(BlackJackGame* game)
{
	BeginDrawing();
	ClearBackground(GRAY);

	// Display balance and bet
	DrawText(TextFormat("Balance: %lld$", game->player->playerBalance), 10, 10, 24, LIME);
	DrawText(TextFormat("Current bet: %lld$", game->player->playerBet), 10, 35, 24, BLACK);

	Rectangle lowerBandBounds = (Rectangle){ .x = 0,.y = GetScreenHeight() - 50, .width = GetScreenWidth(),.height = 200 };
	DrawRectangleRec(lowerBandBounds, BLACK);

	if (!game->algoState.hasBetAlready)
	{
		DrawText("To increase the bet, press '+' on your keyboard, to decrease press '-'!",
			lowerBandBounds.x + 10, lowerBandBounds.y + 10, 20, WHITE);
		EndDrawing();
		return;
	}
	else if (!game->algoState.dealerTurn)
	{
		DrawText("Enter to draw a card, space to hold or shift to double down!",
			lowerBandBounds.x + 10, lowerBandBounds.y + 10, 20, WHITE);
	}
	else
	{
		DrawText("Dealer turn! (Press space to advance)",
			lowerBandBounds.x + 10, lowerBandBounds.y + 10, 20, WHITE);
	}

	DrawText(TextFormat("Your hand: %lld",
		game->player->hand.handTotal), 10, lowerBandBounds.y - 24, 24, BLACK);

	Hand_Render(&game->player->hand, game->textureBucket[0], false);
	Hand_Render(&game->dealer->hand, game->textureBucket[0], true);

	EndDrawing();
}

void BlackJackGame_Input(BlackJackGame* game)
{
	int key = GetKeyPressed();

	if (key == KEY_KP_ADD && !game->algoState.dealerTurn)
	{
		if (game->player->playerBalance >= 5)
		{
			game->player->playerBet += 5;
			game->player->playerBalance -= game->player->playerBet;
			return;
		}
	}

	if (key == KEY_MINUS && !game->algoState.dealerTurn)
	{
		if (game->player->playerBet >= 5)
		{
			game->player->playerBalance += 5;
			game->player->playerBet -= game->player->playerBet;
			return;
		}
	}

	if (key == KEY_ENTER && !game->algoState.dealerTurn)
	{
		game->algoState.nextDrawHand = &game->player->hand;
		game->algoState.dealerTurn = true;
		return;
	}

	if (key == KEY_LEFT_SHIFT || key == KEY_RIGHT_SHIFT && !game->algoState.dealerTurn)
	{
		if (game->player->playerBalance * 2 >= game->player->playerBet * 2)
		{
			game->algoState.nextDrawHand = &game->player->hand;
			game->algoState.dealerTurn = true;
			game->player->playerBet *= 2;
			game->player->playerBalance -= game->player->playerBet;
			game->algoState.stopPlayer = true;
			return;
		}
	}

	if (key == KEY_SPACE && !game->algoState.dealerTurn)
	{
		game->algoState.nextDrawHand = &game->dealer->hand;
		game->algoState.stopPlayer = true;
		game->algoState.dealerTurn = true;
		return;
	}

	if (key == KEY_SPACE && game->algoState.dealerTurn)
	{
		game->algoState.nextDrawHand = &game->dealer->hand;
		game->algoState.dealerTurn = false;
		return;
	}

}

void BlackJackGame_Algorithm(BlackJackGame* game)
{
	game->algoState.hasBetAlready = game->player->playerBet > 0;

	if (!game->algoState.hasBetAlready)
		return;

	if (game->algoState.nextDrawHand != NULL)
	{
		if (game->algoState.stopPlayer && !game->algoState.dealerTurn) return;
		Hand_Draw(game->algoState.nextDrawHand);
	}
}

void BlackJackGame_State_Reset(BlackJackGame* game)
{
	game->algoState.nextDrawHand = NULL;
}




