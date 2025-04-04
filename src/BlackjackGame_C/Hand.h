#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "Card.h"

typedef struct _hand
{
	Card* cards;

	uint8_t cardsCount;
	uint8_t handTotal;

	char handName[64];
} Hand;

Hand Hand_New();
void Hand_Draw(Hand* hand);
void Hand_Render(Hand* hand, Texture2D cardMap, bool topRow);