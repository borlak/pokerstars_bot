#include <windows.h>
#include <vector>
#include <stdio.h>
#include "llist.h"
#include "player.h"
#include "table.h"
#include "flags.h"
#include "util.h"
#include "action.h"
#include "findwindow.h"
#include "pstars.h"
#include "socket.h"
#include "dllmain.h"
#include "const.h"
#include "mouse.h"
#include "macros.h"

using namespace std;

// externs
extern llist<ACTION> *actions = new llist<ACTION>;
extern SOCK		client_sock;

// local functions
TABLE *FindTable(double bb, int players);
void OpenTable(TABLE *table);
int remove_bad_tables(std::vector<HWND> &windows, char *name_match, char *class_match);


void ActionBegin(ACTION *action)
{
	actions->insert_last(action);
}

void ActionComplete()
{
	actions->remove(actions->begin());
}


DWORD WINAPI MyThreadProc(LPVOID lpParameter)
{
	int		tick_count	= 0;
	HDC		hdc			= 0;
	RECT	pRect		= {0,0,0,0};
	HWND	ownd		= 0;
	POINT	pt			= {0,0};
	TABLE	*table		= 0;
	PLAYER	*player		= 0;
	COLORREF color		= 0;
	FINDWINDOW findwin;

	// init
	site_init();

	while(1)
	{
		pstarsCheckNews();

		switch(g_state)
		{
		case STATE_LOGIN:
			findwin.find(0, "Log In", 0);
			if(findwin.window)
			{
				Log("(Global) STATE_LOGIN: Found Log In window");
				CR(mainWnd);
				g_state = STATE_LEAVE_CASHIER;
			}
			else
			{
				Log("(Global) STATE_LOGIN: Clicking cashier button");
				MoveMouseAndClick(mainWnd, 41, 455);
			}
			break;
		case STATE_LEAVE_CASHIER:
			findwin.find(0, "Cashier", 0);
			if(findwin.window)
			{
				AltTab("Cashier");
				Log("(Global) STATE_LEAVE_CASHIER: Escaping out of cashier");
				Esc(findwin.window);
				g_state = STATE_CONNECT_MASTER;
			}
			break;
		case STATE_CONNECT_MASTER:
			Log("(Global) STATE_CONNECT_MASTER: Connecting to master with socket %d", client_sock.descriptor);
			client_sock.connect("localhost", 6666);
			g_state = STATE_FIND_TABLES;
			break;
		case STATE_FIND_TABLES:
			if(g_table_count < g_table_max)
			{
				AltTab("PokerStars Lobby");

				if((table = FindTable(0.25, 3)))
				{
					Log("(Global) STATE_FIND_TABLES: Found table %s in lobby to open", table->name);
					OpenTable(table);
					table->state = STATE_SEARCH;
					g_table_count++;
				}
				else
				{
					Log("(Global) STATE_FIND_TABLES: Scrolling looking for table...");
					// scroll a bit
					MoveMouseAndClick(mainWnd, 538, 391);
				}
			}
			else
			{
				Log("(Global) STATE_FIND_TABLES: Opened all tables");
				g_state = STATE_MAIN_FINISHED;
			}
			break;
		case STATE_MAIN_FINISHED:
			Log("(Global) STATE_MAIN_FINISHED: Finished state, going to STATE_MAIN_CHECK");
			g_state = STATE_MAIN_CHECK;
			break;
		case STATE_MAIN_CHECK:
			if(client_sock.output.length() > 0)
			{
//				Log("(Global) STATE_MAIN_CHECK: sending output string length [%s]", client_sock.output.c_str());
				client_sock.send();
			}
			break;
		default:
			break;
		}

		for(table = tables->begin(); table; table = tables->next())
		{
			if(table->state == STATE_NONE)
				continue;

			switch(table->state)
			{
			case STATE_SEARCH:
				findwin.find(0, table->name, 0);
				if(findwin.window)
				{
					Log("(%s) STATE_SEARCH: setting hwnd", table->name);
					table->hwnd		= findwin.window;
					table->state	= STATE_OPENED;
				}
				else
				{
					Log("(%s) STATE_SEARCH: unable to find window", table->name);
					remove_bad_tables(findwin.find_all(0), 0, "PokerStarsTableFrameClass");
				}
				break;
			case STATE_OPENED: // table is open now.. get a screenshot
				findwin.find(0, table->name, 0);
				if(findwin.window)
				{
					Log("(%s) STATE_OPENED: foregrounding window", table->name);
//					AltTab(table->name);
					table->hwnd = findwin.window;
//					::GetWindowRect(findwin.window, &pRect);
//					::ScreenCaptureWindow(::GetWindowDC(findwin.window), &pRect, table->name);
					table->state = STATE_SIT;
				}
				else
					Log("(%s) STATE_OPENED: unable to find window", table->name);
				break;
			case STATE_SIT: // got a screenshot, find a seat, and sit!
				struct seat
				{ 
					int x1, y1, x2, y2;
					seat(int x1, int y1, int x2, int y2) : x1(x1), y1(y1), x2(x2), y2(y2) { }
				};
				struct seat *seats[10];
				int i;
				char buf[256];
				buf[0] = '\0';

				findwin.find(0, table->name, 0);
				if(findwin.window)
				{
					Log("(%s) STATE_SIT: looking for a seat", table->name);
					AltTab(table->name);
					hdc = ::GetWindowDC(table->hwnd);

					if(IS_SET(table->flags, SIXMAX))
					{
						seats[0] = new struct seat(64,225,74,240);
						seats[1] = new struct seat(216,63,226,79);
						seats[2] = new struct seat(585,63,595,78);
						seats[3] = new struct seat(734,225,744,240);
						seats[4] = new struct seat(584,384,594,400);
						seats[5] = new struct seat(216,384,226,400);
						seats[6] = 0;
					}
					else
					{
						seats[0] = new struct seat(61,225,71,239);
						seats[1] = new struct seat(141,108,151,124);
						seats[2] = new struct seat(304,63,314,79);
						seats[3] = new struct seat(493,63,503,78);
						seats[4] = new struct seat(649,108,659,123);
						seats[5] = new struct seat(732,225,741,240);
						seats[6] = new struct seat(654,365,664,381);
						seats[7] = new struct seat(399,384,409,399);
						seats[8] = new struct seat(146,362,156,378);
						seats[9] = 0;
					}
					for(i = 0; seats[i]; i++)
					{
						color = ::GetPixel(hdc, seats[i]->x1, seats[i]->y1);
						if(GetRValue(color) == 11 && GetGValue(color) == 11 && GetBValue(color) == 11)
						{
							color = ::GetPixel(hdc, seats[i]->x2, seats[i]->y2);
							if(GetRValue(color) == 11 && GetGValue(color) == 11 && GetBValue(color) == 11)
								break;
						}
					}
					if(seats[i]) // we found a seat!  sit down.
					{
						Log("(%s) STATE_SIT: found seat %d.  Clicking to sit...", table->name, i);
						MoveMouseAndClick(table->hwnd, seats[i]->x1, seats[i]->y1);
						table->state = STATE_BUYIN;
					}
					else
					{
						Log("(%s) STATE_SIT: all seats taken, closing table", table->name);
						table->close();
					}
				}
				else
					Log("(%s) STATE_SIT: unable to find table", table->name);
				break;
			case STATE_BUYIN:
				findwin.find(0, "Buy-in", 0);
				if(findwin.window)
				{
					AltTab("Buy-in");

					// check for final buyin/enter $ amount
					color = ::GetPixel(::GetWindowDC(findwin.window), 35, 53);
					if(GetRValue(color) == 255 && GetGValue(color) == 255 && GetBValue(color) == 255)
					{
						Log("(%s) STATE_BUYIN: enter dollar amount (%d)", table->name, 5);
						MoveMouseAndClick(findwin.window, 184, 185-::GetSystemMetrics(4/*SY_CYCAPTION*/));
						LeftClick();
						TypeText(findwin.window, "5");
//						table->get_player("bubni")->stack = 5; // set stack for self
						CR(findwin.window);
						table->state = STATE_GET_HISTORY;
					}
					else
					{
						Log("(%s) STATE_BUYIN: hitting enter for random message", table->name);
						CR(findwin.window);
					}
				}
				else
					Log("(%s) STATE_BUYIN: unable to find buyin window...waiting", table->name);
				break;
			case STATE_GET_HISTORY:
				findwin.find(0, "Instant Hand History", 0);
				if(findwin.window)
				{
					Log("(%s) STATE_GET_HISTORY: closing history window", table->name);
					AltTab("Instant Hand History");
					AltF4(findwin.window);
					table->state = STATE_PLAY;
					break;
				}
				else
				{
					findwin.find(0, table->name, 0);
					if(findwin.window)
					{
						Log("(%s) STATE_GET_HISTORY: clicking on previous history link", table->name);
						AltTab(table->name);
						MoveMouseAndClick(findwin.window, 63, 47-::GetSystemMetrics(4/*SY_CYCAPTION*/));
						break;
					}
					else
						Log("(%s) STATE_GET_HISTORY: unable to find window", table->name);
				}
				break;
			case STATE_PLAY:
				Log("(%s) STATE_PLAY: Listing players", table->name);
				for(player = table->players.begin(); player; player = table->players.next())
					Log("(%s) STATE_PLAY: Player %s has %f chips", table->name, player->name, player->stack);
				table->state = STATE_PLAY+1;
				break;
			default:
				break;
			} // table state
		} // for loop

		// sleep timer/tick
		if(++tick_count % 5 == 0)
			Log("Tick(%d)", tick_count);
		::Sleep(500);
	}
	return 1;
}

void PopupWindow(HWND hwnd)
{
	DWORD thread1, thread2, hold;

	if(::GetForegroundWindow() == hwnd)
		return;

//	::SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
//	::SetWindowPos(hForeWnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

	thread1 = ::GetWindowThreadProcessId(::GetForegroundWindow(), &hold);
	thread2 = ::GetWindowThreadProcessId(hwnd, &hold);
	
	if(thread1 != thread2)
	{
		::AttachThreadInput(thread1, thread2, TRUE);
		::SetForegroundWindow(hwnd);
		::AttachThreadInput(thread1, thread2, FALSE);
	}
	else
		::SetForegroundWindow(hwnd);

	if(::IsIconic(hwnd))
		::ShowWindow(hwnd, SW_RESTORE);
	else
		::ShowWindow(hwnd, SW_SHOW);

	::Sleep(1000);
}

void PopupWindow(char *window_class, char *window_name)
{
	FINDWINDOW findwin(window_class, window_name, 0);

	if(findwin.window)
		PopupWindow(findwin.window);
	else
		MB("PopupWindow: can't find window");
}

TABLE *FindTable(double bb, int players)
{
	TABLE *table = 0;

	for(table = tables->begin(); table; table = tables->next())
	{
		if(!table->isOpen()
		&& table->isLobbyClickable()
		&& table->bb == bb
		&& table->num_players >= players
		&& ((IS_SET(table->flags, SIXMAX) && table->num_players < 6) || (table->num_players < 9)))
			break;
	}
	return table;
}

void OpenTable(TABLE *table)
{
	POINT pt;

	MidPoint(&table->rect, &pt);
	MoveMouseAndClick(mainWnd, pt.x, pt.y);
	LeftClick();
}

int remove_bad_tables(vector<HWND> &windows, char *name_match, char *class_match)
{
	TABLE *table;
	vector<HWND>::iterator it;
	char wname[1024], cname[1024];
	int found, total=0;

	for(it = windows.begin(); it != windows.end(); it++)
	{
		wname[0] = '\0';
		cname[0] = '\0';
		::GetWindowText(*it, wname, 1024);
		::GetClassName(*it, cname, 1024);

		// make sure we match one of the passed in strings
		if((wname[0] != '\0' && name_match && ::strstr(wname, name_match) != NULL)
		|| (cname[0] != '\0' && class_match && ::strstr(cname, class_match) != NULL))
		{
			found = 0;
			Log("(remove_bad_tables()) found table wname(%s)", wname);

			for(table = tables->begin(); table; table = tables->next())
			{
				if(((wname[0] != '\0' && ::strstr(wname, table->name) != NULL)
				|| (cname[0] != '\0' && ::strstr(cname, table->name) != NULL))
				&& table->state > 0) // make sure table is not supposed to be open
				{
					found = 1;
					break; 
				}
			}
			if(!found && wname[0] != '\0')
			{
				Log("(remove_bad_tables()) Tabbing to name (%s)(%s) and closing window", wname, cname);
				AltTab(wname);
				AltF4(*it);
				CR(*it);
				total++;
			}
		}
	}
	return total;
}

