#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "BlackJackGame.h"

int main()
{
	srand(time(NULL));

	Player p = Player_New();
	Dealer d = Dealer_New();

	BlackJackGame game = BlackJackGame_New(&p, &d);
	BlackJackGame_RunGame(&game);
	system("pause");
}