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

void BlackJackGame_DisplayHands(BlackJackGame* game)
{
	printf("== Your balance: %lld ==\n== Your hand: %d | Dealer hand: %d ==\n", game->player->playerBalance, game->player->hand.handTotal, game->dealer->hand.handTotal);
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
		BlackJackGame_Render(game);
		//BlackJackGame_BlackJackAlgorithm(game);
	}

}

void BlackJackGame_Render(BlackJackGame* game)
{
	BeginDrawing();
	ClearBackground(GRAY);

	// Display balance and bet
	DrawText(TextFormat("Balance: %lld$", game->player->playerBalance), 10, 10, 24, LIME);
	DrawText(TextFormat("Current bet: %lld$", game->player->playerBet), 10, 42, 24, BLACK);

	Rectangle lowerBandBounds = (Rectangle){ .x = 0,.y = GetScreenHeight() - 50, .width = GetScreenWidth(),.height = 200 };
	DrawRectangleRec(lowerBandBounds, BLACK);
	DrawText("To increase the bet, press '+' on your keyboard, to decrease press '-'!", lowerBandBounds.x + 10, lowerBandBounds.y + 10, 20, WHITE);


	// Game (testing things here currently)
	if (game->roundCount == 0)
	{
		Card c = Card_GetRandom(0);
		Card_Render(&c, game->textureBucket[0], (Vector2) { .x = GetScreenWidth() / 2, .y = GetScreenHeight() / 2 }, WHITE);
		EndDrawing();
		
		return;

	}

	EndDrawing();
}


void BlackJackGame_Input(BlackJackGame* game)
{
	int key = GetKeyPressed();
	if (key == KEY_KP_ADD)
	{
		if (game->player->playerBalance > game->player->playerBet)
		{
			game->player->playerBet += 5;
			game->player->playerBalance -= 5;
		}
	}

	if (key == KEY_MINUS)
	{
		if (game->player->playerBet > 0)
		{
			game->player->playerBalance += 5;
			game->player->playerBet -= 5;
		}
	}

	if (key == KEY_ENTER && game->roundCount == 0)
	{
		game->roundCount = 1;
	}

}

void BlackJackGame_BlackJackAlgorithm(BlackJackGame* game)
{
	// If player has no balance, exit the game
	if (game->player->playerBalance <= 0)
	{
		printf("You have no balance left. Game over!\n");
		return;
	}

	// Round setup: Place bet
	if (game->roundCount == 0)
	{
		printf("Please place your bet, your current balance is %lld$: ", game->player->playerBalance);
		int scanfRes = scanf_s("%lld", &game->player->playerBet);
		if (scanfRes == EOF)
			return;
		else if (scanfRes == 0)
			return;
		else
		{
			if (game->player->playerBet > game->player->playerBalance)
			{
				printf("You don't have enough balance to place that bet.\n");
				return;  // Prompt the user to place a valid bet
			}

			game->player->playerBalance -= game->player->playerBet;
			printf("Bet placed: %lld$\nNew balance: %lld$\n", game->player->playerBet, game->player->playerBalance);
		}

		// Draw cards
		Player_Draw(game->player);  // Player's face-up card
		Dealer_Draw(game->dealer);  // Dealer's face-up card
	}

	// Round actions
	int action = 0;
	while (action == 0)
	{
		BlackJackGame_DisplayHands(game);
		printf("== Action (1 to stand, 2 to draw, 3 to double down) ==\n");
		printf("Your choice: ");
		scanf_s("%d", &action);

		switch (action)
		{
		case 1: // Stand
			Dealer_Draw(game->dealer);
			break;
		case 2: // Draw
			Player_Draw(game->player);
			break;
		case 3: // Double down
			if (game->player->playerBet * 2 <= game->player->playerBalance)
			{
				game->player->playerBet *= 2;
				game->player->playerBalance -= game->player->playerBet;
				printf("Doubled down: -%lld$, new balance: %lld$\n",
					game->player->playerBet,
					game->player->playerBalance);
				Player_Draw(game->player);
				break;
			}
			else
			{
				printf("Not enough balance to double down!\n");
				action = 0;
				continue;
			}
			break;
		default:
			break;
		}

		// Dealer draws their face-down card after player action
		Dealer_Draw(game->dealer);

		// End the loop after dealer's final action
		if (action == 1 || action == 3)
			break; // Once player stands or doubles down, break to evaluate round

		game->roundCount++;
	}

	// End of round - evaluate winner
	if (game->dealer->hand.handTotal == 21)
	{
		printf("Dealer has 21! You lose.\n");
		Player_Lose(game->player);
		return;
	}
	if (game->dealer->hand.handTotal > 21)
	{
		printf("Dealer busts! You win.\n");
		Player_Win(game->player);
		return;
	}

	if (game->player->hand.handTotal == 21)
	{
		printf("You have 21! You win.\n");
		Player_Win(game->player);
		return;
	}
	if (game->player->hand.handTotal > 21)
	{
		printf("You bust! You lose.\n");
		Player_Lose(game->player);
		return;
	}

	if (game->player->hand.handTotal > game->dealer->hand.handTotal)
	{
		Player_Win(game->player);
	}
	else if (game->player->hand.handTotal == game->dealer->hand.handTotal)
	{
		printf("Draw!\n");
		game->player->playerBalance += game->player->playerBet / 2; // Return half bet for draw
	}
	else
	{
		Player_Lose(game->player);
	}

	// Reset hands and start a new round
	game->roundCount = 0;
	BlackJackGame_DisplayHands(game);
	BlackJackGame_ClearHands(game);
}



