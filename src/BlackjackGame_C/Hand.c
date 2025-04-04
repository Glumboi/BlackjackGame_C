#include "Hand.h"

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

