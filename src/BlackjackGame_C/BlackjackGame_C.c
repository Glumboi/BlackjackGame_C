#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/*
Diamonds: Represented by the symbol ♦.
Clubs: Represented by the symbol ♣.
Spades: Represented by the symbol ♠.
Hearts: Represented by the symbol ♥.
*/

typedef enum _CARD_TYPE
{
	CARD_TYPE_NUMBERED,
	CARD_TYPE_FACE,
	CARD_TYPE_ACE,

	CARD_SYMBOL_TOTAL_TYPE_COUNT
} CARD_TYPE;

typedef enum _CARD_SYMBOL
{
	// Numbered
	CARD_SYMBOL_DIAMODS = 256,
	CARD_SYMBOL_CLUBS,
	CARD_SYMBOL_SPADES,
	CARD_SYMBOL_HEARTS,

	// Ace
	CARD_SYMBOL_ACE,

	// Face
	CARD_SYMBOL_JACK,
	CARD_SYMBOL_QUEEN,
	CARD_SYMBOL_KING,


	CARD_SYMBOL_TOTAL_SYMBOL_COUNT
} CARD_SYMBOL;

typedef struct _card
{
	CARD_TYPE type;
	CARD_SYMBOL symbol;

	uint8_t value;
} Card;

Card Card_GetRandom(uint8_t handTotal)
{
	Card result = { 0 };

	result.type = rand() % (CARD_SYMBOL_TOTAL_TYPE_COUNT - 1 - CARD_TYPE_NUMBERED + 1) + CARD_TYPE_NUMBERED;

	switch (result.type)
	{
	case CARD_TYPE_NUMBERED:
	{
		result.symbol = rand() % (CARD_SYMBOL_HEARTS - 1 - CARD_SYMBOL_DIAMODS + 1) + CARD_SYMBOL_DIAMODS;
		result.value = rand() % (9 + 1 - 2) + 2;
		break;
	}
	case CARD_TYPE_FACE:
	{
		result.symbol = rand() % (CARD_SYMBOL_TOTAL_SYMBOL_COUNT - 1 - CARD_SYMBOL_JACK + 1) + CARD_SYMBOL_JACK;
		result.value = 10;
		break;
	}
	case CARD_TYPE_ACE:
	{
		result.symbol = CARD_SYMBOL_ACE;
		result.value = 11;
		if (handTotal + result.value >= 21)
			result.value = 1;
		break;
	}
	default:
		break;
	}
	return result;
}

void Card_Display(Card* card)
{
	char msg[64];

	switch (card->type)
	{
	case CARD_TYPE_NUMBERED:
		sprintf_s(msg, 64, "== Numbered card ==\n");
		sprintf_s(msg + strlen(msg), 64 - strlen(msg), "== %d ==\n", card->value);
		break;

	case CARD_TYPE_FACE:
		sprintf_s(msg, 64, "== Face card ==\n");
		sprintf_s(msg + strlen(msg), 64 - strlen(msg), "== %d ==\n", card->value);
		break;

	case CARD_TYPE_ACE:
		sprintf_s(msg, 64, "== ACE ==\n");
		sprintf_s(msg + strlen(msg), 64 - strlen(msg), "== %d ==\n", card->value);
		break;

	default:
		// Handle default case if necessary
		break;
	}

	switch (card->symbol)
	{
	case CARD_SYMBOL_ACE:
		sprintf_s(msg + strlen(msg), 64 - strlen(msg), "== ACE ==\n");
		break;

	case CARD_SYMBOL_CLUBS:
		sprintf_s(msg + strlen(msg), 64 - strlen(msg), "== CLUBS ==\n");
		break;

	case CARD_SYMBOL_DIAMODS:
		sprintf_s(msg + strlen(msg), 64 - strlen(msg), "== DIAMONDS ==\n");
		break;

	case CARD_SYMBOL_HEARTS:
		sprintf_s(msg + strlen(msg), 64 - strlen(msg), "== HEARTS ==\n");
		break;

	case CARD_SYMBOL_JACK:
		sprintf_s(msg + strlen(msg), 64 - strlen(msg), "== JACK ==\n");
		break;

	case CARD_SYMBOL_KING:
		sprintf_s(msg + strlen(msg), 64 - strlen(msg), "== KING ==\n");
		break;

	case CARD_SYMBOL_QUEEN:
		sprintf_s(msg + strlen(msg), 64 - strlen(msg), "== QUEEN ==\n");
		break;

	case CARD_SYMBOL_SPADES:
		sprintf_s(msg + strlen(msg), 64 - strlen(msg), "== SPADES ==\n");
		break;

	default:
		// Handle default case if necessary
		break;
	}

	printf("%s", msg);
}


typedef struct _hand
{
	Card* cards;

	uint8_t cardsCount;
	uint8_t handTotal;
} Hand;

Hand Hand_New()
{
	return (Hand) { 0 };
}

void Hand_Draw(Hand* hand)
{
	if (!hand) return;

	Card* temp = realloc(hand->cards, ++hand->cardsCount * sizeof(Card));
	if (!temp) return;

	Card newCard = Card_GetRandom(hand->handTotal);

	hand->handTotal += newCard.value;
	temp[hand->cardsCount - 1] = newCard;
	hand->cards = temp;

	Card_Display(&newCard);

}

typedef struct _dealer
{
	Hand hand;
} Dealer;

Dealer Dealer_New()
{
	Dealer result = { 0 };
	result.hand = Hand_New();
	return result;
}

void Dealer_Draw(Dealer* dealer)
{
	if (!dealer) return;
	printf("\nDealer drew: \n");
	Hand_Draw(&dealer->hand);
	printf("================\n");
	printf("\n");
}

void Dealer_Win(Dealer* dealer)
{
	if (!dealer) return;
	printf("Dealer has won!\n");
}

void Dealer_Lose(Dealer* dealer)
{
	if (!dealer) return;
	printf("Dealer has bust!\n");
}

#define DEFAULT_PLAYER_BALANCE 500

typedef struct _player
{
	Hand hand;
	size_t playerBet;
	int64_t playerBalance;
	uint8_t gameEnd;
} Player;

Player Player_New()
{
	Player result = { 0 };
	result.hand = Hand_New();
	result.playerBalance = DEFAULT_PLAYER_BALANCE;
	return result;
}

void Player_Draw(Player* player)
{
	if (!player) return;
	printf("\nYou drew: \n");
	Hand_Draw(&player->hand);
	printf("\n");
	printf("================\n");
}

void Player_Win(Player* player)
{
	if (!player) return;
	printf("Player has won!\n");
	player->playerBalance += player->playerBet;
	player->gameEnd = 1;
}

void Player_Lose(Player* player)
{
	if (!player) return;
	printf("Player has lost!\n");
	player->playerBalance -= player->playerBet;
	player->gameEnd = 1;
}

typedef struct _blackJackGame
{
	Player* player;
	Dealer* dealer;

	uint8_t roundCount;
} BlackJackGame;

BlackJackGame BlackJackGame_New(Player* player, Dealer* dealer)
{
	BlackJackGame result = { 0 };
	if (!dealer || !player) return result;

	result.player = player;
	result.dealer = dealer;
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

	while (1)
	{
		// If player has no balance, exit the game
		if (game->player->playerBalance <= 0)
		{
			printf("You have no balance left. Game over!\n");
			break;
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
					continue;  // Prompt the user to place a valid bet
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
			break;
		}
		if (game->dealer->hand.handTotal > 21)
		{
			printf("Dealer busts! You win.\n");
			Player_Win(game->player);
			break;
		}

		if (game->player->hand.handTotal == 21)
		{
			printf("You have 21! You win.\n");
			Player_Win(game->player);
			break;
		}
		if (game->player->hand.handTotal > 21)
		{
			printf("You bust! You lose.\n");
			Player_Lose(game->player);
			break;
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
}


int main()
{
	srand(time(NULL));

	Player p = Player_New();
	Dealer d = Dealer_New();

	BlackJackGame game = BlackJackGame_New(&p, &d);
	BlackJackGame_RunGame(&game);
	system("pause");
}