#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "Hand.h"

typedef struct _dealer
{
	Hand hand;
} Dealer;

Dealer Dealer_New();
void Dealer_Draw(Dealer* dealer);
void Dealer_Win(Dealer* dealer);
void Dealer_Lose(Dealer* dealer);

