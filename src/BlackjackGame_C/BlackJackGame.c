#include "BlackJackGame.h"

BlackJackGame BlackJackGame_New(Player* player, Dealer* dealer)
{
	BlackJackGame result = { 0 };
	if (!dealer || !player) return result;

	result.player = player;
	result.dealer = dealer;

	char wonActionTemp[2] = { '-', '+' };
	memcpy(result.wonAction, wonActionTemp, sizeof(wonActionTemp));

	result.algoState.drawDelay = 1000; // Set the delay to 1 second (1000 milliseconds)
	result.algoState.wonHand = NULL;

	strcpy_s(result.assetDir, 1024, GetWorkingDirectory());
	strcat_s(result.assetDir, 1024, ASSETS_DIRECTORY);

	strcpy_s(result.spriteDir, 1024, result.assetDir);
	strcat_s(result.spriteDir, 1024, SPRITES_DIRECTORY);

	if (DirectoryExists(result.assetDir))
	{
		SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);

		InitWindow(800, 600, "GlumJack");

		char buff[260] = { 0 };
		sprintf_s(buff, 260, "%s%s", result.assetDir, "Icon.png");
		Image icon = LoadImage(buff);
		SetWindowIcon(icon);
		UnloadImage(icon);

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
		Hand_Clear(&game->player->hand);
		Hand_Clear(&game->dealer->hand);
	}

	void BlackJackGame_RunGame(BlackJackGame* game)
	{
		if (!game) return;
		game->algoState.dealerTurn = false;
		while (!WindowShouldClose())
		{
			if (game->algoState.wonHand == NULL) // Block on end
				BlackJackGame_Input(game);

			BlackJackGame_Algorithm(game);
			BlackJackGame_Render(game);
			BlackJackGame_State_Reset(game);
		}
	}

	size_t fcExpect = 0;
	void BlackJackGame_RenderPart_Won(BlackJackGame* game)
	{
		if (game->algoState.wonHand != NULL)
		{
			char* message = TextFormat("%s has won!\n%c %ld$",
				game->algoState.wonHand,
				game->wonAction[game->player->won],
				game->player->bet);

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

			game->algoState.wonHand = NULL;
			game->player->won = false;
			BlackJackGame_Reset(game);
		}
	}

	void BlackJackGame_RenderPart_Stats(BlackJackGame* game)
	{
		// Display balance and bet
		DrawText(TextFormat("Balance: %lld$", game->player->balance), 10, 10, 24, LIME);
		DrawText(TextFormat("Current bet: %lld$", game->player->bet), 10, 35, 24, BLACK);

		Rectangle lowerBandBounds = (Rectangle){ .x = 0,.y = GetScreenHeight() - 50, .width = GetScreenWidth(),.height = 200 };
		DrawRectangleRec(lowerBandBounds, BLACK);

		if (!game->algoState.hasBetAlready && game->player->bet <= 0)
		{
			DrawText("To increase the bet, press '+' on your keyboard, to decrease press '-'!",
				lowerBandBounds.x + 10, lowerBandBounds.y + 10, 20, WHITE);
			return;
		}
		else if (!game->algoState.dealerTurn && Hand_GetTotal(&game->player->hand) == 0)
		{
			DrawText("Enter to draw your frist card!",
				lowerBandBounds.x + 10, lowerBandBounds.y + 10, 20, WHITE);
			return;
		}


		if (!game->algoState.dealerTurn)
		{
			DrawText("Enter to draw a card, space to hold or shift to double down!",
				lowerBandBounds.x + 10, lowerBandBounds.y + 10, 20, WHITE);
		}
		else
			DrawText("Please wait...",
				lowerBandBounds.x + 10, lowerBandBounds.y + 10, 20, WHITE);


		DrawText(TextFormat("Your hand: %lld",
			game->player->hand.handTotal), 10, lowerBandBounds.y - 24, 24, BLACK);

		DrawText(TextFormat("Dealer hand: %lld",
			game->dealer->hand.handTotal), 10, lowerBandBounds.y - 48, 24, BLACK);

		Hand_Render(&game->player->hand, game->textureBucket[0], false);
		Hand_Render(&game->dealer->hand, game->textureBucket[0], true);
	}

	void BlackJackGame_Render(BlackJackGame* game)
	{
		RENDER_BEGIN;
		BlackJackGame_RenderPart_Stats(game);
		BlackJackGame_RenderPart_Won(game);
		RENDER_END;

		if (++game->fc == SIZE_MAX)
			game->fc = 0;
	}

	void BlackJackGame_Input(BlackJackGame* game)
	{
		if (game->algoState.dealerTurn) return; // Only allow input during the player's turn
		int key = GetKeyPressed();

		if (!game->algoState.hasBetAlready)
		{
			if (key == KEY_KP_ADD && !game->algoState.dealerTurn)
			{
				if (game->player->balance >= 5)
				{
					game->player->bet += 5;
					game->player->balance -= 5;
					return;
				}
			}

			if (key == KEY_MINUS && !game->algoState.dealerTurn)
			{
				if (game->player->bet >= 5)
				{
					game->player->balance += 5;
					game->player->bet -= 5;
					return;
				}
			}
		}

		if (key == KEY_ENTER && !game->algoState.stopPlayer)
		{
			game->algoState.nextDrawHand = &game->player->hand;
			game->algoState.hasBetAlready = true;
			return;
		}

		if (key == KEY_LEFT_SHIFT || key == KEY_RIGHT_SHIFT && !game->algoState.stopPlayer)
		{
			if (game->player->balance * 2 >= game->player->bet * 2)
			{
				game->algoState.nextDrawHand = &game->player->hand;
				game->player->bet *= 2;
				game->player->balance -= game->player->bet;
				game->algoState.stopPlayer = true;
				return;
			}
		}

		if (key == KEY_SPACE && !game->algoState.stopPlayer)
		{
			game->algoState.stopPlayer = true;
			game->algoState.nextDrawHand = &game->dealer->hand;

			return;
		}

	}

	void BlackJackGame_Algorithm(BlackJackGame* game)
	{
		if (!game->algoState.hasBetAlready && game->player->bet <= 0)
			return;

		int playerTotal = Hand_GetTotal(&game->player->hand);
		int dealerTotal = Hand_GetTotal(&game->dealer->hand);
		bool playerBust = playerTotal > 21;
		bool dealerBust = dealerTotal > 21;

		bool playerTurnOver = game->algoState.stopPlayer || playerBust;

		if (game->algoState.wonHand == NULL && (playerBust || dealerBust || (playerTurnOver && !game->algoState.dealerTurn && game->algoState.nextDrawHand == NULL)))
		{

			if (playerBust)
			{
				game->player->balance -= game->player->bet;
				game->algoState.wonHand = game->dealer->hand.handName;
				game->player->won = false;
			}
			else if (dealerBust || playerTotal > dealerTotal)
			{
				game->player->balance += game->player->bet;
				game->algoState.wonHand = game->player->hand.handName;
				game->player->won = true;

			}
			else if (playerTotal == dealerTotal)
			{
				game->player->balance += game->player->bet;
			}


			return;
		}

		bool playerBlackjack = playerTotal == 21;
		bool dealerBlackjack = dealerTotal == 21;

		if (game->algoState.wonHand == NULL && playerBlackjack || dealerBlackjack)
		{
			game->algoState.stopPlayer = true;
			game->algoState.dealerTurn = false;

			if (playerBlackjack && dealerBlackjack)
			{
				game->player->balance += game->player->bet;
			}
			else if (playerBlackjack)
			{
				game->player->balance += game->player->bet + (int)(game->player->bet * 1.5);
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
			Hand_Draw(game->algoState.nextDrawHand);
			game->algoState.nextDrawHand = NULL;

			if (playerTotal > 21)
			{
				game->algoState.stopPlayer = true;
			}
			else
			{
				game->algoState.dealerTurn = true;
				game->algoState.lastDrawTime = clock();
			}
		}

		if (game->algoState.dealerTurn)
		{
			clock_t currentTime = clock();
			if ((currentTime - game->algoState.lastDrawTime) * 1000 / CLOCKS_PER_SEC >= game->algoState.drawDelay)
			{
				if (dealerTotal < 17)
				{
					Dealer_Draw(game->dealer);
					game->algoState.lastDrawTime = currentTime;
				}
				else
				{
					game->algoState.dealerTurn = false;
				}
			}
		}
	}

	void BlackJackGame_Reset(BlackJackGame* game)
	{
		if (!game)return;
		// Reset for next round
		BlackJackGame_ClearHands(game);
		game->algoState.hasBetAlready = false;
		game->algoState.stopPlayer = false;
		game->algoState.dealerTurn = false;
		game->algoState.nextDrawHand = NULL;
		game->algoState.lastDrawTime = clock();
	}

	void BlackJackGame_State_Reset(BlackJackGame* game)
	{
		game->algoState.nextDrawHand = NULL;
	}




