#include "Player.h"

Player Player_New()
{
	Player result = { 0 };

	struct _playerStats stats = { 0 };
	result.playerStats = (struct _playerStats){ 0 };
	result.playerStats.balance = DEFAULT_PLAYER_BALANCE;

	result.hand = Hand_New("Player");
	return result;
}

void Player_Draw(Player* player)
{
	Hand_Draw(&player->hand);
}

void Player_IncreaseBet(Player* player)
{
	PTR_VALIDATE(player, return;);
	if (player->playerStats.balance >= 5 && player)
	{
		player->playerStats.bet += 5;
		player->playerStats.balance -= 5;
	}
}

void Player_DecreaseBet(Player* player)
{
	PTR_VALIDATE(player, return;);
	if (player->playerStats.bet >= 5 && player)
	{
		player->playerStats.balance += 5;
		player->playerStats.bet -= 5;
	}
}

bool Player_AllowPlay(Player* player)
{
	PTR_VALIDATE(player, return false;);

	return Hand_GetTotal(&player->hand) > 0 && player->playerStats.bet > 0;
}
