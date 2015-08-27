#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include <vector>
#include "llist.h"
#include "player.h"
#include "table.h"
#include "flags.h"
#include "findwindow.h"
#include "menu.h"
#include "const.h"
#include "macros.h"
#include "dllmain.h"
#include "util.h"
#include "mouse.h"

wchar_t lastMessageDrawTextEx[4096] = {'\0'};
wchar_t lastMessageExtTextOut[4096] = {'\0'};

double table_get_bb(char *bb)
{
	char buf[32];
	char *p = strrchr(bb, '/');
	int i = 0;
	if(!p) return 10000.0;
	while(!isdigit(*p))
		p++;
	for(; *p != '\0'; p++, i++)
		buf[i] = *p;
	buf[i] = '\0';
	return atof(buf);
}

double get_money_str(char *avg_pot)
{
	char buf[32];
	int i;
	while(!isdigit(*avg_pot))
		avg_pot++;
	for(i = 0; *avg_pot != '\0'; avg_pot++, i++)
		buf[i] = *avg_pot;
	buf[i] = '\0';
	return atof(buf);
}

int table_get_poker_type(char *type)
{
	if(!strcmp(type, "NL"		)) return PTYPE_NOLIMIT_HOLDEM;
	if(!strcmp(type, "PL"		)) return PTYPE_POT_HOLDEM;
	if(!strcmp(type, "Fixed"	)) return PTYPE_LIMIT_HOLDEM;
	return PTYPE_PLAY;
}

int table_get_players_flop(char *players)
{
	char buf[32];
	int i = 0;
	for(; *players != '\0' && isdigit(*players); players++, i++)
		buf[i] = *players;
	buf[i] = '\0';
	return atoi(buf);
}

int table_get_hands_hour(char *avg_hands)
{
	char buf[32];
	int i = 0;
	while(!isdigit(*avg_hands))
		avg_hands++;
	for(; *avg_hands != '\0'; avg_hands++, i++)
		buf[i] = *avg_hands;
	buf[i] = '\0';
	return atoi(buf);
}

void table_set_flags(TABLE *table, char *buf)
{
	if(strstr(buf, "6 max"		) != NULL)	SET_BIT(table->flags, SIXMAX);
	if(strstr(buf, "no all-in"	) != NULL)	SET_BIT(table->flags, NOALLIN);
	if(strstr(buf, "fast"		) != NULL)	SET_BIT(table->flags, FAST);
}

void pstarsHandleText(LPWSTR text, HWND hwnd, LPRECT lpRect, wchar_t *cname, wchar_t *name, double center, double proportion)
{
	POINT			pt			= {0,0};
	TABLE			*t			= NULL;
	PLAYER			*p			= NULL;
	static TABLE	*table		= NULL;
	static PLAYER	*player		= NULL;
	char			*buf		= new char[wcslen(text)+1];
	char			temp[1024]	= {'\0'};

	::wsprintfA(buf, "%S", text);

	if(strstr(buf, "Dealer:") != NULL)
	{
		hwnd = ::GetParent(hwnd);
		::GetWindowText(hwnd, temp, 1024);

		for(t = tables->begin(); t; t = tables->next())
		{
			if(strstr(temp, t->name) != NULL)
				break;
		}
		if(t)
			t->dealer_message(buf);
	}
	
	// proportion:
	//		center:
	//	11+ -- player listing and stack sizes in main lobby
	//	27+ -- main table list
	//		< 36(name) < 47(stakes/bb) < 54(poker type) < 71(#players) < 82(avg pot)
	//		< 93(flop %) < 100(avg hands/hour)
	if(!wcscmp(cname, L"PokerStarsListClass"))
	{
		if(proportion > 11 && proportion < 27 && table)
		{
			/*
			if(strcmp(buf, "Player")
			&& strcmp(buf, "City")
			&& strcmp(buf, "Chips"))
			{
				Log("ADDING PLAYERS (%s) TO (%s)", buf, table->name);
				if(center < 25)
				{
					// check if player is already listed @ table
					for(p = table->players.begin(); p; p = table->players.next())
					{
						if(!strcmp(p->name, buf))
						{
							player = p;
							break;
						}
					}
					// new player
					if(!p)
					{
						p = new PLAYER(buf);
						table->players.insert(p);
						player = p;
					}
				}
				else if(center < 60)	player->set_city(buf);
				else					player->stack = get_money_str(buf);
			}*/
		}
		else if(proportion >= 27) // table list window
		{
			if(center < 36) // name -- new table
			{
				for(t = tables->begin(); t; t = tables->next())
				{
					if(strstr(buf, t->name) != NULL)
					{
						::CopyRect(&t->rect, lpRect);
						table = NULL;
						break;
					}
				}
				if(!t)
				{
					char *pstr;
					table = new TABLE(buf);
					table_set_flags(table, buf);
					// get rid of the flags (x y)
					if((pstr = strchr(table->name,'(')) != 0)
						*(--pstr) = '\0';
					::CopyRect(&table->rect, lpRect);
					tables->insert(table);
				}
			}
			else if(table && center < 47) table->bb					= table_get_bb(buf);
			else if(table && center < 54) table->poker_type			= table_get_poker_type(buf);
			else if(table && center < 71) table->num_players		= atoi(buf);
			else if(table && center < 82) table->avg_pot			= get_money_str(buf);
			else if(table && center < 93) table->avg_players_flop	= table_get_players_flop(buf);
			else if(table && center < 100) table->avg_hands_hour	= table_get_hands_hour(buf);
		}
	}
	delete [] buf;
}

void pstarsDrawTextEx(HDC hdc, LPWSTR lpchText, LPRECT lprc)
{
	wchar_t *dealer = wcsstr(lpchText, L"Dealer:");

	if((dealer == NULL
	|| (dealer != NULL && lprc->bottom == 0 && lprc->right == 0 && lprc->top == 0)
	   )
	&&	 wcsstr(lastMessageDrawTextEx, lpchText) == NULL) // repeating messages
	{
		wchar_t	buf[100]		= {'\0'};
		wchar_t	name[100]		= {'\0'};
		HWND	hwnd			= NULL;
		HWND	parent			= NULL;
		RECT	wndRectCl		= {0,0,0,0};	// GetClientRect
		RECT	mainWndRectCl	= {0,0,0,0};	// GetClientRect
		RECT	wndRect			= {0,0,0,0};	// GetWindowRect
		RECT	mainWndRect		= {0,0,0,0};	// GetWindowRect
		POINT	pt				= {0,0};
		double	proportion		= 0.0;
		double	center			= 0.0;

		::GetClientRect(mainWnd, &mainWndRectCl);
		::GetWindowRect(mainWnd, &mainWndRect);

		if(!(hwnd = ::WindowFromDC(hdc)))
		{
			hwnd = mainWnd;
			pt.x = lprc->left + ((lprc->right - lprc->left)/2);
			pt.y = lprc->top + ((lprc->bottom - lprc->top)/2);
			hwnd = ::ChildWindowFromPoint(hwnd, pt);
		}

		if(hwnd)
		{
//			::SetPixel(hdc, pt.x, pt.y, RGB(255,0,0));
			::GetClientRect(hwnd, &wndRectCl);
			::GetWindowRect(hwnd, &wndRect);

			// get some stats on windows for passing on to main function.  the way they are designed
			// it shouldn't matter if window is resized.  proportion/center will be same
			center		= (float)(pt.x-(wndRect.left-mainWndRect.left)) / (float)wndRectCl.right * 100.0;
			proportion	= (float)(wndRectCl.bottom*wndRectCl.right) / (float)(mainWndRectCl.bottom*mainWndRectCl.right)*100.0;

			::GetWindowTextW(hwnd, name, 100);
			::GetClassNameW(hwnd, buf, 100);
		}

		FILE *fp = fopen("mybor.txt", "a+");
		fwprintf(fp, L"DTEx::%-45s\t\t -- N(%s)C(%s) P/C(%.1f %.1f) Rect(%li %li %li %li) Size(%lih %liw)\n",
			lpchText,
			hwnd != NULL ? name : L"none",
			hwnd != NULL ? buf : L"none",
			proportion, center,
			lprc->top, lprc->bottom, lprc->left, lprc->right,
			wndRectCl.bottom, wndRectCl.right);
		fclose(fp);

		// copy last message to get rid of duplicate or "staggered" messages
		wcscpy(lastMessageDrawTextEx, lpchText);

		pstarsHandleText(lpchText, hwnd, lprc, buf, name, center, proportion);
	}
}

void pstarsExtTextOut(HDC hdc, LPCWSTR lpString, CONST RECT* lprc)
{
	TABLE *table;
	char lpbuf[4096] = {'\0'}, buf[128];
	char *p, *pbuf;

	if(wcsstr(lastMessageExtTextOut, lpString) == NULL)
	{
		FILE *fp = fopen("mybor.txt", "a+");
		fwprintf(fp, L"ETO::%s\t\t -- Chars(%d) Rect(%li %li %li %li)\n", 
			lpString,
			::wcslen(lpString),
			lprc->bottom, lprc->left, lprc->right, lprc->top);
		fclose(fp);

		::wsprintfA(lpbuf, "%S", lpString);

		if((p = strstr(lpbuf, "Table '")) != NULL)
		{
			p += strlen("Table '");
			pbuf = buf;
			while(*p != '\'')
				*pbuf++ = *p++;
			*pbuf = '\0';
			Log("(pstarsExtTextOut) found table string, name(%s)", buf);
		}

		for(table = tables->begin(); table; table = tables->next())
		{
			if(strstr(table->name, buf) != NULL)
				break;
		}
		if(table)
			table->read_history(lpbuf);

		wcscpy(lastMessageExtTextOut, lpString); 
	}
}
 
void pstarsCheckNews()
{
	FINDWINDOW find("#32770", "News", 0);

	if(find.window)
	{
		Log("(Global) Found news window, tabbing to it and escaping");
		AltTab("News");
		Esc(find.window);
	}
}

llist<MENU>	poker_menu;		// main/top poker menu

void pstarsSetupMenu()
{
	pstarsCheckNews();

	AltTab("PokerStars Lobby");
//	PopupWindow(NULL, "PokerStars Lobby");
	MoveMouseAndClick(mainWnd, 43, 111);
	MoveMouseAndClick(mainWnd, 278, 144);
	MoveMouseAndClick(mainWnd, 486, 167);
}
