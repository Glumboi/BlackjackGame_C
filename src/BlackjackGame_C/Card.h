#ifndef CARD_H
#define CARD_H

#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define CARD_DEFAULT_HEIGHT 195
#define CARD_DEFAULT_WIDTH 96

#define TEXMAP_CLUBS (Rectangle){ .width = CARD_DEFAULT_WIDTH,.height = CARD_DEFAULT_HEIGHT };
#define TEXMAP_DIAMONDS (Rectangle){ .width = CARD_DEFAULT_WIDTH,.height = CARD_DEFAULT_HEIGHT, .x = CARD_DEFAULT_WIDTH };
#define TEXMAP_HEARTS (Rectangle){ .width = CARD_DEFAULT_WIDTH,.height = CARD_DEFAULT_HEIGHT, .x = CARD_DEFAULT_WIDTH * 2 };
#define TEXMAP_SPADES (Rectangle){ .width = CARD_DEFAULT_WIDTH,.height = CARD_DEFAULT_HEIGHT, .x = CARD_DEFAULT_WIDTH * 3 };

#define CARD_COLOR_RED CLITERAL(Color){ 172, 50, 50, 255 } 
#define CARD_COLOR_BLACK BLACK

/*
Diamonds: Represented by the symbol ♦.
Clubs: Represented by the symbol ♣.
Spades: Represented by the symbol ♠.
Hearts: Represented by the symbol ♥.
*/

typedef enum _CARD_TYPE
{
	CARD_TYPE_NUMBERED,
	CARD_TYPE_ACE,

	CARD_TYPE_JACK,
	CARD_TYPE_QUEEN,
	CARD_TYPE_KING,

	CARD_SYMBOL_TOTAL_TYPE_COUNT
} CARD_TYPE;

typedef enum _CARD_SYMBOL
{
	// Numbered
	CARD_SYMBOL_DIAMODS = 256,
	CARD_SYMBOL_CLUBS,
	CARD_SYMBOL_SPADES,
	CARD_SYMBOL_HEARTS,
	CARD_SYMBOL_TOTAL_SYMBOL_COUNT
} CARD_SYMBOL;

CARD_SYMBOL CardSymbol_Gen(bool numbered);

typedef struct _card
{
	CARD_TYPE type;
	CARD_SYMBOL symbol;
	Color cardColor;

	uint8_t value;
	Rectangle mappedCard;

	char* text;
} Card;

Card Card_GetRandom(uint8_t handTotal);
void Card_Render(Card* card, Texture mappedTex, Vector2 pos, Color tint);

#endif