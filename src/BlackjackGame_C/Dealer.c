#include "Dealer.h"

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