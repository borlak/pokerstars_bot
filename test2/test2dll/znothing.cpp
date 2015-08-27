 /*
void message::operator+=(char *input)
{
	add_deli();
	mesg += input;
}

void message::operator+=(std::string input)
{
	add_deli();
	mesg += input;
}

void message::operator+=(int input)
{
	add_deli();
	mesg += int_to_string(input);
}
*/

/*
void table::dealer_message(char *str)
{
	PLAYER *player;
	char text[128], nothing[128], player_name[128];
	char card1[3], card2[3], card3[3];
	float chips;

	if(::sscanf(str, "Dealer: Starting new hand: #%s", text) == 1)
		set_hand(text);
	else if(::sscanf(str, "Dealer: %s leaves %s", player_name, nothing) == 2)
		remove_player(player_name);
	else if(::sscanf(str, "Dealer: %s joins the %s", player_name, nothing) == 2)
		add_player(player_name);
	else if(::sscanf(str, "Dealer: Dealing Hole %s", text) == 1)
		;
	else if(::sscanf(str, "Dealer: Dealing Flop: [%s %s %s]", card1, card2, card3) == 3)
		;
	else if(::sscanf(str, "Dealer: Dealing %s: [%s]", text, card1) == 2)
		;
	else if(::sscanf(str, "Dealer: %s has %s", player_name, text) == 2)
		;
	else if(::sscanf(str, "Dealer: %s %s", player_name, text) == 2)
	{
		if(!strcmp(text, "folds"))
			add_player(player_name);
		else if(!strcmp(text, "checks"))
			;
		else
			Log("(dealer_message) No action for message (%s)", str);
	}
	else if(::sscanf(str, "Dealer: %s posts %s blind $%f", player_name, text, &chips) == 3)
		;
	else if(::sscanf(str, "Dealer: %s bets $%f", player_name, &chips) == 2)
		;
	else if(::sscanf(str, "Dealer: %s calls $%f", player_name, &chips) == 2)
		;
	else if(::sscanf(str, "Dealer: %s raises $%*f to $%f and is %s", player_name, &chips, text) == 3)
	{
		player			= get_player(player_name);
		player->all_in	= 1;
	}
	else if(::sscanf(str, "Dealer: %s raises $%*f to $%f", player_name, &chips) == 2)
		;
	else if(::sscanf(str, "Dealer: Uncalled bet ($%f) returned to %s", &chips, player_name) == 2)
		;
	else if(::sscanf(str, "Dealer: %s mucks %s", player_name, text) == 2)
		;
	else if(::sscanf(str, "Dealer: %s sits %s", player_name, text) == 2)
	{
		player				= add_player(player_name);
		player->sitting_out	= 1;
	}
	else if(::sscanf(str, "Dealer: %s has %s", player_name, text) == 2)
	{
		if(!strcmp(text, "returned"))
		{
			player				= get_player(player_name);
			player->sitting_out	= 1;
		}
	}
	else
		Log("(dealer_message) No action for message (%s)", str);
}

void table::read_history(char *buf)
{
	PLAYER *player;
	float chips;
	char player_name[64];
	char nothing[64];
	char *line;
	int seat;

	while((line = get_line(buf)) != NULL)
	{
Log("line %s", line);
		if(::sscanf(line, "Seat %d: %s ($%f in %s", &seat, player_name, &chips, nothing) == 4)
		{
			player = add_player(player_name);
Log("Player %s found with %f chips", player_name, chips);
			// if there is no player it could be old history and they've left the table...
			if((player = get_player(player_name)))
				player->stack = chips;
			continue;
		}
		else if(::sscanf(line, "%s posts %s blind $%f", player_name, nothing, &chips) == 3)
		{
			// strip off the colon on the name
			player_name[strlen(player_name)-1] = '\0';
Log("Player %s posts %s blind %f", player_name, nothing, chips);
			if((player = get_player(player_name)))
				player->stack -= chips;
			continue;
		}
		else if(::sscanf(line, "%s calls $%f", player_name, &chips) == 2
		|| ::sscanf(line, "%s bets $%f", player_name, &chips) == 2)
		{
			player_name[strlen(player_name)-1] = '\0';
Log("Player %s calls or bets %f", player_name, chips);
			if((player = get_player(player_name)))
				player->stack -= chips;
			continue;
		}
		else if(::sscanf(line, "%s raises $%*f to $%f", player_name, &chips) == 2)
		{
			player_name[strlen(player_name)-1] = '\0';
Log("Player %s raises to %f", player_name, chips);
			if((player = get_player(player_name)))
				player->stack -= chips;
			continue;
		}
		else if(::sscanf(line, "%s collected $%f from %s", player_name, &chips, nothing) == 3)
		{
Log("Player %s collects %f", player_name, chips);
			if((player = get_player(player_name)))
				player->stack += chips;
			continue;			
		}
		else if(::sscanf(line, "Uncalled bet ($%f) returned to %s", &chips, player_name) == 2)
		{
Log("Player %s gets returned %f", player_name, chips);
			if((player = get_player(player_name)))
				player->stack += chips;
			continue;
		}
	}
}


setuppstarsmenu....
	HWND		frameWnd	= ::FindWindowEx(mainWnd, NULL, "PokerStarsSelectorClass", NULL);
	COLORREF	colr		= 0;
	RECT		pRect		= {0,0,0,0};
	int			width		= 0;
	int			prop		= 0;
	MENU		*menu		= 0;
	MENU		*sub		= 0;


	::GetWindowRect(mainWnd, &pRect);
	width = pRect.right - pRect.left;
	// Holdem
	menu = new MENU( 45, 133, 0, width, RGB(57,26,0), "Holdem");	poker_menu.insert(menu);
	sub = new MENU( 92, 167, 0, width, RGB(218,177,49), "Fixed");	menu->sub.insert(sub);
	sub = new MENU(275, 167, 1, width, RGB(218,177,49), "Nolimit");	menu->sub.insert(sub);
	sub = new MENU(464, 167, 2, width, RGB(218,177,49), "Play");	menu->sub.insert(sub);

	menu = poker_menu[0]->sub[0]; // fixed
	sub = new MENU( 57, 193, 0, width, RGB(187,175,125), "All");	menu->sub.insert(sub);
	sub = new MENU(154, 193, 1, width, RGB(187,175,125), "High");	menu->sub.insert(sub);
	sub = new MENU(269, 193, 2, width, RGB(187,175,125), "Medium");	menu->sub.insert(sub);
	sub = new MENU(382, 193, 3, width, RGB(187,175,125), "Low");	menu->sub.insert(sub);
	sub = new MENU(482, 193, 4, width, RGB(187,175,125), "Micro");	menu->sub.insert(sub);
	menu = poker_menu[0]->sub[1]; // nolimit
	sub = new MENU( 57, 193, 0, width, RGB(187,175,125), "All");	menu->sub.insert(sub);
	sub = new MENU(154, 193, 1, width, RGB(187,175,125), "High");	menu->sub.insert(sub);
	sub = new MENU(269, 193, 2, width, RGB(187,175,125), "Medium");	menu->sub.insert(sub);
	sub = new MENU(382, 193, 3, width, RGB(187,175,125), "Low");	menu->sub.insert(sub);
	sub = new MENU(482, 193, 4, width, RGB(187,175,125), "Micro");	menu->sub.insert(sub);
	menu = poker_menu[0]->sub[2]; // play
	sub = new MENU( 61, 193, 0, width, RGB(187,175,125), "All");		menu->sub.insert(sub);
	sub = new MENU(187, 193, 1, width, RGB(187,175,125), "Fixed");		menu->sub.insert(sub);
	sub = new MENU(330, 193, 2, width, RGB(187,175,125), "Nolimit");	menu->sub.insert(sub);
	sub = new MENU(470, 193, 3, width, RGB(187,175,125), "Potlimit");	menu->sub.insert(sub);

	// Omaha
	menu = new MENU(121, 133, 1, width, RGB(57,26,0), "Omaha");		poker_menu.insert(menu);
	sub = new MENU(135, 167, 0, width, RGB(218,177,49), "Omaha");	menu->sub.insert(sub);
	sub = new MENU(401, 167, 1, width, RGB(218,177,49), "Hilo");	menu->sub.insert(sub);

	// Stud
	menu = new MENU(184, 133, 2, width, RGB(57,26,0), "Stud");		poker_menu.insert(menu);
	sub = new MENU(103, 167, 0, width, RGB(218,177,49), "7Card");	menu->sub.insert(sub);
	sub = new MENU(286, 167, 1, width, RGB(218,177,49), "Hilo");	menu->sub.insert(sub);
	sub = new MENU(472, 167, 2, width, RGB(218,177,49), "Razz");	menu->sub.insert(sub);

	// Other Games
	menu = new MENU(266, 133, 3, width, RGB(57,26,0), "Other");		poker_menu.insert(menu);
	sub = new MENU( 86, 167, 0, width, RGB(218,177,49), "5Card");	menu->sub.insert(sub);
	sub = new MENU(230, 167, 1, width, RGB(218,177,49), "2-7");		menu->sub.insert(sub);
	sub = new MENU(371, 167, 2, width, RGB(218,177,49), "HORSE");	menu->sub.insert(sub);
	sub = new MENU(487, 167, 3, width, RGB(218,177,49), "HOSE");	menu->sub.insert(sub);

	// Tournaments
	menu = new MENU(363, 133, 4, width, RGB(57,26,0), "Tourney");		poker_menu.insert(menu);
	sub = new MENU( 38, 167, 0, width, RGB(218,177,49), "All");			menu->sub.insert(sub);
	sub = new MENU( 98, 167, 1, width, RGB(218,177,49), "VIP");			menu->sub.insert(sub);
	sub = new MENU(168, 167, 2, width, RGB(218,177,49), "Regular");		menu->sub.insert(sub);
	sub = new MENU(255, 167, 3, width, RGB(218,177,49), "Satellite");	menu->sub.insert(sub);
	sub = new MENU(335, 167, 4, width, RGB(218,177,49), "Special");		menu->sub.insert(sub);
	sub = new MENU(425, 167, 5, width, RGB(218,177,49), "Freeroll");	menu->sub.insert(sub);
	sub = new MENU(503, 167, 6, width, RGB(218,177,49), "Private");		menu->sub.insert(sub);
	menu = poker_menu[4]->sub[1]; // VIP
	sub = new MENU( 38, 193, 0, width, RGB(187,175,125), "All");		menu->sub.insert(sub);
	sub = new MENU(113, 193, 1, width, RGB(187,175,125), "Bronze");		menu->sub.insert(sub);
	sub = new MENU(208, 193, 2, width, RGB(187,175,125), "Silver");		menu->sub.insert(sub);
	sub = new MENU(295, 193, 3, width, RGB(187,175,125), "Gold");		menu->sub.insert(sub);
	sub = new MENU(395, 193, 4, width, RGB(187,175,125), "Platinum");	menu->sub.insert(sub);
	sub = new MENU(495, 193, 5, width, RGB(187,175,125), "Supernova");	menu->sub.insert(sub);
	menu = poker_menu[4]->sub[2]; // Regular
	sub = new MENU( 44, 193, 0, width, RGB(187,175,125), "All");		menu->sub.insert(sub);
	sub = new MENU(104, 193, 1, width, RGB(187,175,125), "High");		menu->sub.insert(sub);
	sub = new MENU(184, 193, 2, width, RGB(187,175,125), "Medium");		menu->sub.insert(sub);
	sub = new MENU(266, 193, 3, width, RGB(187,175,125), "Low");		menu->sub.insert(sub);
	sub = new MENU(345, 193, 4, width, RGB(187,175,125), "Micro");		menu->sub.insert(sub);
	sub = new MENU(422, 193, 5, width, RGB(187,175,125), "Rebuy");		menu->sub.insert(sub);
	sub = new MENU(501, 193, 6, width, RGB(187,175,125), "Turbo");		menu->sub.insert(sub);
	menu = poker_menu[4]->sub[3]; // Satellite
	sub = new MENU( 58, 193, 0, width, RGB(187,175,125), "All");		menu->sub.insert(sub);
	sub = new MENU(157, 193, 1, width, RGB(187,175,125), "Cash");		menu->sub.insert(sub);
	sub = new MENU(258, 193, 2, width, RGB(187,175,125), "FPP");		menu->sub.insert(sub);
	sub = new MENU(369, 193, 3, width, RGB(187,175,125), "Rebuy");		menu->sub.insert(sub);
	sub = new MENU(483, 193, 4, width, RGB(187,175,125), "Events");		menu->sub.insert(sub);

	// Sit N Go
	menu = new MENU(440, 133, 5, width, RGB(57,26,0), "SNG");			poker_menu.insert(menu);
	sub = new MENU( 39, 167, 0, width, RGB(218,177,49), "All");			menu->sub.insert(sub);
	sub = new MENU(106, 167, 1, width, RGB(218,177,49), "1Table");		menu->sub.insert(sub);
	sub = new MENU(196, 167, 2, width, RGB(218,177,49), "Multi");		menu->sub.insert(sub);
	sub = new MENU(304, 167, 3, width, RGB(218,177,49), "HU");			menu->sub.insert(sub);
	sub = new MENU(390, 167, 4, width, RGB(218,177,49), "Satellite");	menu->sub.insert(sub);
	sub = new MENU(488, 167, 5, width, RGB(218,177,49), "Play");		menu->sub.insert(sub);
	menu = poker_menu[5]->sub[1]; // 1Table
	sub = new MENU( 51, 193, 0, width, RGB(187,175,125), "All");		menu->sub.insert(sub);
	sub = new MENU(128, 193, 1, width, RGB(187,175,125), "High");		menu->sub.insert(sub);
	sub = new MENU(224, 193, 2, width, RGB(187,175,125), "Medium");		menu->sub.insert(sub);
	sub = new MENU(316, 193, 3, width, RGB(187,175,125), "Low");		menu->sub.insert(sub);
	sub = new MENU(400, 193, 4, width, RGB(187,175,125), "Micro");		menu->sub.insert(sub);
	sub = new MENU(495, 193, 5, width, RGB(187,175,125), "Turbo");		menu->sub.insert(sub);
	menu = poker_menu[5]->sub[2]; // Multi
	sub = new MENU( 51, 193, 0, width, RGB(187,175,125), "All");		menu->sub.insert(sub);
	sub = new MENU(128, 193, 1, width, RGB(187,175,125), "High");		menu->sub.insert(sub);
	sub = new MENU(224, 193, 2, width, RGB(187,175,125), "Medium");		menu->sub.insert(sub);
	sub = new MENU(316, 193, 3, width, RGB(187,175,125), "Low");		menu->sub.insert(sub);
	sub = new MENU(400, 193, 4, width, RGB(187,175,125), "Micro");		menu->sub.insert(sub);
	sub = new MENU(495, 193, 5, width, RGB(187,175,125), "Turbo");		menu->sub.insert(sub);
	menu = poker_menu[5]->sub[3]; // HU
	sub = new MENU( 58, 193, 0, width, RGB(187,175,125), "All");		menu->sub.insert(sub);
	sub = new MENU(154, 193, 1, width, RGB(187,175,125), "High");		menu->sub.insert(sub);
	sub = new MENU(270, 193, 2, width, RGB(187,175,125), "Medium");		menu->sub.insert(sub);
	sub = new MENU(380, 193, 3, width, RGB(187,175,125), "Low");		menu->sub.insert(sub);
	sub = new MENU(485, 193, 4, width, RGB(187,175,125), "Micro");		menu->sub.insert(sub);
	menu = poker_menu[5]->sub[4]; // Satellite
	sub = new MENU( 75, 193, 0, width, RGB(187,175,125), "All");		menu->sub.insert(sub);
	sub = new MENU(200, 193, 1, width, RGB(187,175,125), "Cash");		menu->sub.insert(sub);
	sub = new MENU(333, 193, 2, width, RGB(187,175,125), "FPP");		menu->sub.insert(sub);
	sub = new MENU(466, 193, 3, width, RGB(187,175,125), "Events");		menu->sub.insert(sub);
	menu = poker_menu[5]->sub[5]; // Play
	sub = new MENU( 75, 193, 0, width, RGB(187,175,125), "All");		menu->sub.insert(sub);
	sub = new MENU(200, 193, 1, width, RGB(187,175,125), "High");		menu->sub.insert(sub);
	sub = new MENU(333, 193, 2, width, RGB(187,175,125), "Medium");		menu->sub.insert(sub);
	sub = new MENU(466, 193, 3, width, RGB(187,175,125), "Low");		menu->sub.insert(sub);

	// Events
	menu = new MENU(516, 133, 6, width, RGB(57,26,0), "Events");		poker_menu.insert(menu);
	sub = new MENU( 42, 167, 0, width, RGB(218,177,49), "WSOP");		menu->sub.insert(sub);
	sub = new MENU(119, 167, 1, width, RGB(218,177,49), "WorldCup");	menu->sub.insert(sub);
	sub = new MENU(201, 167, 2, width, RGB(218,177,49), "WCOOP");		menu->sub.insert(sub);
	sub = new MENU(272, 167, 3, width, RGB(218,177,49), "LAPT");		menu->sub.insert(sub);
	sub = new MENU(335, 167, 4, width, RGB(218,177,49), "APPT");		menu->sub.insert(sub);
	sub = new MENU(390, 167, 5, width, RGB(218,177,49), "EPT");			menu->sub.insert(sub);
	sub = new MENU(445, 167, 6, width, RGB(218,177,49), "PSM");			menu->sub.insert(sub);
	sub = new MENU(505, 167, 7, width, RGB(218,177,49), "Special");		menu->sub.insert(sub);
	for(int i = 0; i < 8; i++)
	{
	if(i == 1) continue; // 1 (world cup) is the only one that differs currently
	menu = poker_menu[6]->sub[i];
	sub = new MENU( 40, 193, 0, width, RGB(187,175,125), "All");				menu->sub.insert(sub);
	sub = new MENU(115, 193, 1, width, RGB(187,175,125), "Qualifiers");			menu->sub.insert(sub);
	sub = new MENU(225, 193, 2, width, RGB(187,175,125), "FPP Satellites");		menu->sub.insert(sub);
	sub = new MENU(342, 193, 3, width, RGB(187,175,125), "Cash Satellites");	menu->sub.insert(sub);
	sub = new MENU(470, 193, 4, width, RGB(187,175,125), "SNG Satellites");		menu->sub.insert(sub);
	}
	menu = poker_menu[6]->sub[1];
	sub = new MENU( 62, 193, 0, width, RGB(187,175,125), "US");				menu->sub.insert(sub);
	sub = new MENU(155, 193, 1, width, RGB(187,175,125), "CA");				menu->sub.insert(sub);
	sub = new MENU(257, 193, 2, width, RGB(187,175,125), "DE");				menu->sub.insert(sub);
	sub = new MENU(370, 193, 3, width, RGB(187,175,125), "National");		menu->sub.insert(sub);
	sub = new MENU(485, 193, 4, width, RGB(187,175,125), "Division Finals");menu->sub.insert(sub);
	CheckActive(&poker_menu);
*/
