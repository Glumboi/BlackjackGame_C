#include "Card.h"

CARD_SYMBOL CardSymbol_Gen(bool numbered)
{
	return  rand() % (CARD_SYMBOL_HEARTS - 1 - CARD_SYMBOL_DIAMODS + 1) + CARD_SYMBOL_DIAMODS;
}

CARD_TYPE CardType_Gen()
{
	return rand() % (CARD_SYMBOL_TOTAL_TYPE_COUNT - 1 - CARD_TYPE_NUMBERED + 1) + CARD_TYPE_NUMBERED;
}

Card Card_GetRandom(uint8_t handTotal)
{
	Card result = { 0 };

	result.type = CardType_Gen();

	switch (result.type)
	{
	case CARD_TYPE_NUMBERED:
	{
		result.symbol = CardSymbol_Gen(true);
		result.value = rand() % (9 + 1 - 2) + 2;
		break;
	}
	case CARD_TYPE_ACE:
	{
		result.symbol = CardSymbol_Gen(true);
		result.value = 11;
		if (handTotal + result.value >= 21)
			result.value = 1;
		break;
	}
	case CARD_TYPE_JACK:
	case CARD_TYPE_KING:
	case CARD_TYPE_QUEEN:
	{
		result.symbol = CardSymbol_Gen(true);
		result.value = 10;
		break;
	}
	}

	switch (result.symbol)
	{
	case CARD_SYMBOL_CLUBS:
		result.mappedCard = TEXMAP_CLUBS;
		result.cardColor = CARD_COLOR_BLACK;
		break;
	case CARD_SYMBOL_HEARTS:
		result.mappedCard = TEXMAP_HEARTS;
		result.cardColor = CARD_COLOR_RED;
		break;
	case CARD_SYMBOL_DIAMODS:
		result.mappedCard = TEXMAP_DIAMONDS;
		result.cardColor = CARD_COLOR_RED;
		break;
	case CARD_SYMBOL_SPADES:
		result.mappedCard = TEXMAP_SPADES;
		result.cardColor = CARD_COLOR_BLACK;
		break;
	default:
		break;
	}
	return result;
}

void Card_Render(Card* card, Texture mappedTex, Vector2 pos, Color tint)
{
	DrawTextureRec(mappedTex, card->mappedCard, pos, WHITE);

	const char* text = NULL;
	Vector2 textSize = { 0 };
	Vector2 textPos = { 0 };

	// TODO: Don't do this on every render call 

	switch (card->type)
	{
	case CARD_TYPE_ACE:
		text = "ACE";
		break;
	case CARD_TYPE_JACK:
		text = "J";
		break;
	case CARD_TYPE_KING:
		text = "K";
		break;
	case CARD_TYPE_QUEEN:
		text = "Q";
		break;
	default:
		text = TextFormat("%d", card->value);
		break;
	}

	textSize = MeasureTextEx(GetFontDefault(), text, 32, 4);
	textPos = (Vector2){
		   pos.x + (card->mappedCard.width - textSize.x) / 2,
		   pos.y + (card->mappedCard.height - textSize.y) / 2
	};
	DrawTextEx(GetFontDefault(), text, textPos, 32, 4, card->cardColor);
}
