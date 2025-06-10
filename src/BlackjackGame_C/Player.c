#include "Player.h"

Player Player_New()
{
	Player result = { 0 };
	result.hand = Hand_New("Player");
	result.balance = DEFAULT_PLAYER_BALANCE;
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
	player->balance += player->bet;
	player->gameEnd = 1;
}

void Player_Lose(Player* player)
{
	if (!player) return;
	printf("Player has lost!\n");
	player->balance -= player->bet;
	player->gameEnd = 1;
}
