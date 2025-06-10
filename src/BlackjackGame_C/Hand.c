#include "Hand.h"

Hand Hand_New(char* name)
{
	Hand res = { 0 };
	res.handName = name;
	return res;
}

void Hand_Clear(Hand* hand)
{
	if (!hand) return;
	free(hand->cards);
	hand->cards = NULL;
	hand->cardsCount = 0;
	hand->handTotal = 0;
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
}

void Hand_Render(Hand* hand, Texture2D cardMap, bool topRow)
{
	Vector2 cardPos;

	if (!topRow)
	{
		for (size_t i = 0; i < hand->cardsCount; i++)
		{
			// Calculate the position for each card to be drawn next to each other horizontally
			cardPos = (Vector2){
			   .x = 10 + i * (CARD_DEFAULT_WIDTH + 10),  // cardWidth is the width of the card, spacing is the space between cards
			   .y = GetScreenHeight() / 2
			};

			// Render each card next to each other
			Card_Render(
				&hand->cards[i], // Corrected the card index from 0 to i to get the proper card
				cardMap,
				cardPos,
				WHITE
			);
		}
	}
	else
	{
		for (size_t i = 0; i < hand->cardsCount; i++)
		{
			// Calculate the position for each card to be drawn starting from the top-right corner
			cardPos = (Vector2){
				.x = GetScreenWidth() - 10 - CARD_DEFAULT_WIDTH - i * (CARD_DEFAULT_WIDTH + 10), // Start from the right and move left
				.y = 10 // Set Y to the top of the screen
			};

			// Render each card starting from the top-right corner
			Card_Render(
				&hand->cards[i], // Corrected the card index from 0 to i to get the proper card
				cardMap,
				cardPos,
				WHITE
			);
		}
	}


}

uint8_t Hand_GetTotal(Hand* hand)
{
	if (!hand) return;
	hand->handTotal = 0;
	for (size_t i = 0; i < hand->cardsCount; i++)
	{
		hand->handTotal += hand->cards[i].value;
	}
	return hand->handTotal;
}
