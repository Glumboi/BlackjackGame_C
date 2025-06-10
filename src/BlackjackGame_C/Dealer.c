#include "Dealer.h"

Dealer Dealer_New()
{
	Dealer result = { 0 };
	result.hand = Hand_New("Dealer");
	return result;
}

void Dealer_Draw(Dealer* dealer)
{
	if (!dealer) return;
	Hand_Draw(&dealer->hand);
}

