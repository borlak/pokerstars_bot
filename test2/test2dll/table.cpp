#include <windows.h>
#include <stdio.h>
#include "llist.h"
#include "player.h"
#include "table.h"
#include "arguments.h"
#include "message.h"
#include "util.h"
#include "mouse.h"
#include "const.h"
#include "dllmain.h"

table::table(char *table_name)
{
	::ZeroMemory(this, sizeof(TABLE));
	name = new char[strlen(table_name)+1];
	strcpy(name, table_name);
}

table::~table()
{
	delete [] name;
}

int table::isLobbyClickable()
{
	return (rect.bottom != 0 && rect.right != 0);
}

int table::isOpen()
{
	return (state > 0);
}

void table::close()
{
	this->state = STATE_NONE;
	AltTab(this->name);
	AltF4(this->hwnd);
	if(current_hand)
		delete [] current_hand;
	current_hand = 0;
	players.clear();
	g_table_count--;
	g_state = STATE_FIND_TABLES;
}

void table::set_hand(char *str)
{
	while(!isdigit(*str))
		str++;
	if(current_hand) delete [] current_hand;
	current_hand = new char[strlen(str)+1];
	strcpy(current_hand, str);
}


// format: hand_id|||table|||player|||action|||amount
void table::dealer_message(char *str)
{
	ARGS args(str);
	MESSAGE message("|||");
	char		buf[128]			= {'\0'};
	char		*player_name		= 0;
	int			index;
	static int	first_hand = 0; // don't message about UNKNOWNs until at least one hand has been found

	if(this->current_hand)
		message.add(MESG_HAND, this->current_hand);

	message.add(MESG_TABLE, this->name);

	if(args.size())
	{
		if(args.last() && !strcmp(args.last(), "folds"))
		{
			player_name = args.cut(1, -2);
			message.add(MESG_PLAYER, player_name);
			message.add(MESG_ACTION, ACTION_FOLDS);
		}
		else if(args.last() && !strcmp(args.last(), "checks"))
		{
			player_name = args.cut(1, -2);
			message.add(MESG_PLAYER, player_name);
			message.add(MESG_ACTION, ACTION_CHECKS);
		}
		else if(args[-2] && !strcmp(args[-2], "calls"))
		{
			player_name = args.cut(1, -3);
			message.add(MESG_PLAYER, player_name);
			message.add(MESG_ACTION, ACTION_CALLS);
			message.add(MESG_AMOUNT, money(args[-1]));
		}
		else if(args[-2] && !strcmp(args[-2], "bets"))
		{
			player_name = args.cut(1, -3);
			message.add(MESG_PLAYER, player_name);
			message.add(MESG_ACTION, ACTION_BETS);
			message.add(MESG_AMOUNT, money(args[-1]));
		}
		else if(args[-4] && !strcmp(args[-4], "raises"))
		{
			player_name = args.cut(1, -5);
			message.add(MESG_PLAYER, player_name);
			message.add(MESG_ACTION, ACTION_RAISES);
			message.add(MESG_AMOUNT, money(args[-1]));
		}
		else if(args[-2] && !strcmp(args[-2], "blind"))
		{
			player_name = args.cut(1, -5);
			message.add(MESG_PLAYER, player_name);
//			message.add(MESG_ACTION, "blind"); small or big blind?
		}
		else if(args[-4] && !strcmp(args[-4], "table"))
		{
			player_name = args.cut(1, -7);
			message.add(MESG_PLAYER, player_name);
			message.add(MESG_ACTION, ACTION_PLAYER_JOINS);
		}
		else if(args[-3] && !strcmp(args[-3], "leaves"))
		{
			player_name = args.cut(1, -4);
			message.add(MESG_PLAYER, player_name);
			message.add(MESG_ACTION, ACTION_PLAYER_LEAVES);
		}
		else if(args[-1] && !strcmp(args[-1], "act"))
		{
			index = args.index_of("it's")-1;
			args[index][strlen(args[index])-1] = '\0';
			message.add(MESG_PLAYER, args.cut(1, index));
			message.add(MESG_ACTION, ACTION_TIME_TO_ACT);
			message.add(MESG_AMOUNT, atoi(args[-4]));
		}
		else if(args[-2] && !strcmp(args[-4], "Flop:"))
		{
			string	card1(card(args[-3])),
					card2(card(args[-2])),
					card3(card(args[-1]));
			sprintf(buf, "%s%s%s", card1.c_str(), card2.c_str(), card3.c_str());
			message.add(MESG_PLAYER, "dealer");
			message.add(MESG_ACTION, ACTION_FLOP);
			message.add(MESG_AMOUNT, buf);
		}
		else if(args[-2] && !strcmp(args[-2], "Hole"))
		{
			message.add(MESG_PLAYER, "dealer");
			message.add(MESG_ACTION, ACTION_DEALING);
		}
		else if(args[-2] && !strcmp(args[-2], "River:"))
		{
			message.add(MESG_PLAYER, "dealer");
			message.add(MESG_ACTION, ACTION_RIVER);
			message.add(MESG_AMOUNT, card(args[-1]));
		}
		else if(args[-2] && !strcmp(args[-2], "Turn:"))
		{
			message.add(MESG_PLAYER, "dealer");
			message.add(MESG_ACTION, ACTION_TURN);
			message.add(MESG_AMOUNT, card(args[-1]));
		}
		else if(args[6] && !strcmp(args.cut(1, 6), "A new game will start"))
		{
			message.add(MESG_PLAYER, "dealer");
//			message.add(MESG_ACTION, "newgame");
		}
		else if(args[4] && !strcmp(args.cut(1, 3), "Starting new hand:"))
		{
			first_hand = 1;
			set_hand(args[-1]);
			message.add(MESG_PLAYER, "dealer");
			message.add(MESG_ACTION, ACTION_NEW_HAND);
			message.add(MESG_AMOUNT, this->current_hand);
		}
		else if(args.size() > 5 && args.find("wins pot") != 0)
		{
			message.add(MESG_PLAYER, args.cut(3, args.index_of("wins")-1));
			message.add(MESG_ACTION, ACTION_WINNER);
		}
		else if(args[-2] && args.size() > 3 && !strcmp(args[-2], "mucks"))
		{// FIX
			message.add(MESG_PLAYER, args.cut(1, -3));
			message.add(MESG_ACTION, ACTION_MUCKS);
		}
		else if(args[-1] && args.size() > 9 && !strcmp(args[-1], "button"))
		{
			message.add(MESG_PLAYER, args.cut(1, -9));
			message.add(MESG_ACTION, ACTION_AFTER_BUTTON);
		}
		else if(args[2] && !strcmp(args.cut(1, 2), "Uncalled bet"))
		{
			message.add(MESG_PLAYER, args.cut(6, args.size()-1));
			message.add(MESG_ACTION, ACTION_BET_RETURNED);
			message.add(MESG_AMOUNT, money(args[3]));
		}
		else if(first_hand)
		{
			// final checking stuff
			if((index = args.index_of("has")) >= 0)
			{
				if(get_player(args.cut(1, index-1)))
				{
					Log("(dealer_message) %s", args.c_str());
				}
				else
					Log("(dealer_message 'has') UNKNOWN(%s)", args.c_str());
			}
			else
				Log("(dealer_message) UNKNOWN(%s)", args.c_str());
		}
		if(message.valid_message())
			Log("(MESSAGE) %s actual(%s)", message.print(), message.prepare_message().c_str());
		if(player_name)
			add_player(player_name);
	} // if args.size
}

void table::read_history(char *buf)
{
	ARGS args;
	PLAYER *player;
	double chips;
	char *player_name;
	char *line;

	Log("READ HISTORY CALLED -- strlen of buf [%d] buf=[%s]", strlen(buf), buf);

	while((line = get_line(buf)) != NULL)
	{
		// strip \n ..
		line[strlen(line)-1] = '\0';
		args.read(line,' ');

//Log("args[0]=[%s]len[%d]   args[-1]=[%s]len[%d]", args[0], strlen(args[0]), args[-1], strlen(args[-1]));
		if(args.size() > 5 && !strcmp(args[0], "Seat") && !strcmp(args[-1], "chips)"))
		{
			player_name	= args.cut(2, -4);
			chips		= money(args[-3]);
			if((player = get_player(player_name)))
				player->stack = chips;
		}
		else if(args.size() > 4 && !strcmp(args[-2], "blind") && !strcmp(args[-4], "posts"))
		{
			player_name = args.cut(0, -5);
			player_name[strlen(player_name)-1] = '\0';
			chips = money(args[-1]);
			if((player = get_player(player_name)))
				player->stack -= chips;
		}
		else if(args.size() >= 3 && (!strcmp(args[-2],"bets") || !strcmp(args[-2],"calls")))
		{
			player_name = args.cut(0, -3);
			player_name[strlen(player_name)-1] = '\0';
			chips = money(args[-1]);
			if((player = get_player(player_name)))
				player->stack -= chips;
		}
		else if(args.size() >= 5 && !strcmp(args[-4],"raises"))
		{
			player_name = args.cut(0, -5);
			player_name[strlen(player_name)-1] = '\0';
			chips = money(args[-1]);
			if((player = get_player(player_name)))
				player->stack -= chips;
		}
		// special case here as this "collecte xx from pot" appears twice -- second time
		// after *** SUMMARY *** so that is what the "Seat" check is for
		else if(args.size() >= 5 && strcmp(args[0],"Seat") && !strcmp(args[-4],"collected"))
		{
			player_name = args.cut(0, -5);
			chips = money(args[-3]);
			if((player = get_player(player_name)))
				player->stack += chips;
		}
		else if(args.size() >= 6 && !strcmp(args[0], "Uncalled"))
		{
			player_name = args.cut(5, -1);
			chips = money(args[2]);
			if((player = get_player(player_name)))
				player->stack += chips;
		}
	}
}

PLAYER *table::add_player(char *name)
{
	PLAYER *player = 0;

	for(player = players.begin(); player; player = players.next())
	{
		if(!strcmp(player->name, name))	
			break;
	}
	// new player
	if(!player)
	{
		player = new PLAYER(name);
		players.insert(player);
	}
	return player;
}

PLAYER *table::get_player(char *name)
{
	PLAYER *player=0;
	for(player = players.begin(); player; player = players.next())
	{
		if(!strcmp(player->name, name))
			return player;
	}
	return player;
}

void table::remove_player(char *name)
{
	PLAYER *player=0;
	for(player = players.begin(); player; player = players.next())
	{
		if(!strcmp(player->name, name))
		{
			players.remove(player);
			break;
		}
	}
}

