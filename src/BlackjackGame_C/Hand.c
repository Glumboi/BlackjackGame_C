#include "Hand.h"

Hand Hand_New(char* name)
{
	PTR_VALIDATE(name, name = "NULL";);
	Hand res = { 0 };
	res.handName = name;
	return res;
}

void Hand_Clear(Hand* hand)
{
	PTR_VALIDATE(hand, return;);
	PTR_FREE(hand->cards);
	hand->cardsCount = 0;
	hand->handTotal = 0;
}

void Hand_Draw(Hand* hand)
{
	PTR_VALIDATE(hand, return;);

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
			cardPos = (Vector2){
			   .x = 10 + i * (CARD_DEFAULT_WIDTH + 10),
			   .y = GetScreenHeight() / 2
			};

			Card_Render(
				&hand->cards[i],
				cardMap,
				cardPos,
				WHITE
			);
		}
		return;
	}

	for (size_t i = 0; i < hand->cardsCount; i++)
	{
		cardPos = (Vector2){
			.x = GetScreenWidth() - 10 - CARD_DEFAULT_WIDTH - i * (CARD_DEFAULT_WIDTH + 10),
			.y = 10
		};

		Card_Render(
			&hand->cards[i],
			cardMap,
			cardPos,
			WHITE
		);
	}
}

uint8_t Hand_GetTotal(Hand* hand)
{
	PTR_VALIDATE(hand, return;);
	hand->handTotal = 0;
	for (size_t i = 0; i < hand->cardsCount; i++)
	{
		hand->handTotal += hand->cards[i].value;
	}
	return hand->handTotal;
}
