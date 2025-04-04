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
} Hand;

Hand Hand_New();
void Hand_Draw(Hand* hand);