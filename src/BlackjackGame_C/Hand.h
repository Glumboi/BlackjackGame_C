#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "Card.h"

typedef struct _hand
{
	Card* cards;

	uint8_t cardsCount;
	uint8_t handTotal;

	char* handName;
} Hand;

Hand Hand_New(char* name);
void Hand_Clear(Hand* hand);
void Hand_Draw(Hand* hand);
void Hand_Render(Hand* hand, Texture2D cardMap, bool topRow);
uint8_t Hand_GetTotal(Hand* hand);
