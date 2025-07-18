﻿#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "BlackJackGame.h"

int WinMain()
{
	main();
}

int main()
{
	srand(time(NULL));

	Player p = Player_New();
	Dealer d = Dealer_New();

	BlackJackGame game = BlackJackGame_New(&p, &d);
	
	BlackJackGame_RunGame(&game);

	BlackJackGame_Save_Game(&game);
	BlackJackGame_Free(&game);
}