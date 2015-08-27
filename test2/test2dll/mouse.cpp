#include <windows.h>
#include <stdio.h>
#include <cmath>
#include <vector>
#include "llist.h"
#include "player.h"
#include "table.h"
#include "flags.h"
#include "findwindow.h"
#include "util.h"
#include "macros.h"

#define MAX_POINTS 100
long destX, destY, points[MAX_POINTS];
int mouseMoving = 0;

void LeftClick(void)
{  
	INPUT Input[2];

	::ZeroMemory(&Input, sizeof(INPUT)*2);

	// left down 
	Input[0].type		= INPUT_MOUSE;
	Input[0].mi.dwFlags	= MOUSEEVENTF_LEFTDOWN;
	// left up
	Input[1].type		= INPUT_MOUSE;
	Input[1].mi.dwFlags	= MOUSEEVENTF_LEFTUP;

	::SendInput(2, Input, sizeof(INPUT));
}

void DoubleClick(void)
{  
	INPUT Input[4];

	::ZeroMemory(&Input, sizeof(INPUT)*4);

	for(int i = 0; i < 2; i++)
	{
		// left down 
		Input[(i*2)].type		= INPUT_MOUSE;
		Input[(i*2)].mi.dwFlags	= MOUSEEVENTF_LEFTDOWN;
		// left up
		Input[(i*2)+1].type			= INPUT_MOUSE;
		Input[(i*2)+1].mi.dwFlags	= MOUSEEVENTF_LEFTUP;
	}

	::SendInput(4, Input, sizeof(INPUT));
}


VOID CALLBACK MouseMoveUpdate(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	POINT curPos;
	int x, y;
	INPUT Input	= {0};

	::GetCursorPos(&curPos);

	if(abs(curPos.x-destX) <= 1 && abs(curPos.y-destY) <= 1)
	{
		::KillTimer(hwnd, idEvent);
		mouseMoving = 0;
	}

	x = (long)(::sqrt((float)abs(curPos.x-destX))/randRangef((float)0.8,(float)2.0));
	y = (long)(::sqrt((float)abs(curPos.y-destY))/randRangef((float)0.8,(float)2.0));

	Input.type			= INPUT_MOUSE;
	Input.mi.dwFlags	= MOUSEEVENTF_MOVE;
	Input.mi.dx			= destX > curPos.x ? x : x*-1;
	Input.mi.dy			= destY > curPos.y ? y : y*-1;

	::SendInput(1, &Input, sizeof(INPUT));
}


// setup a move to a point.  width and height is included for some variation
void MoveMouse(int x, int y, int width, int height)
{
	if(!mouseMoving)
	{
		destX = x + randRange((width*-1)/2, width/2);
		destY = y + randRange((height*-1)/2, height/2);

		::SetTimer(NULL, NULL, randRange(5,30), MouseMoveUpdate);
		mouseMoving = 1;
	}
}

// Keyboard stuff
extern BOOL CALLBACK MyEnumWindowsFunc(HWND hwnd, LPARAM lParam);
void AltTab(char *match)
{
	FINDWINDOW	findwin;
	HWND		alttabWnd;
	ALTTABINFO	atinfo;
	char		buf[1024]	= {'\0'};
	int			i, current = 1, total=0;
	static int	failed = 0;

	if(failed) return; // TODO TEMP debugging crap

	atinfo.cbSize = sizeof(ALTTABINFO);

	::keybd_event(VK_MENU, 0, KEYEVENTF_EXTENDEDKEY, 0);
	::keybd_event(VK_TAB, 0, KEYEVENTF_EXTENDEDKEY, 0);
	::keybd_event(VK_TAB, 0, KEYEVENTF_EXTENDEDKEY|KEYEVENTF_KEYUP, 0);

	findwin.find("#32771", 0, 0);
	alttabWnd = findwin.window;

	if(alttabWnd)
	{
		::GetAltTabInfo(alttabWnd, 0, &atinfo, NULL, NULL);

		for(i = 0; i < atinfo.cItems; i++)
		{
			::GetAltTabInfo(alttabWnd, i, &atinfo, buf, 1024);
			if(strstr(buf, match) != NULL)
				break;
		}
		while(current != i)
		{
			if(total++ > 50) // TODO temp ugly crap
			{
				Log("Alt tab failed for [%s]", match);
				failed = 1;
				break;
			}
			::keybd_event(VK_TAB, 0, KEYEVENTF_EXTENDEDKEY, 0);
			::keybd_event(VK_TAB, 0, KEYEVENTF_EXTENDEDKEY|KEYEVENTF_KEYUP, 0);

			if(current == (atinfo.cItems-1))
				current = 0;
			else
				current++;

			::Sleep(randRange(50,300));
		}
	}
	::keybd_event(VK_MENU, 0, KEYEVENTF_EXTENDEDKEY|KEYEVENTF_KEYUP, 0);
	::Sleep(randRange(50,300));
}

// carriage return / enter key
void CR(HWND hwnd)
{
	keybd_event(VK_RETURN, NULL, KEYEVENTF_EXTENDEDKEY|0, 0);
	keybd_event(VK_RETURN, NULL, KEYEVENTF_EXTENDEDKEY|KEYEVENTF_KEYUP, 0);
	::Sleep(randRange(100,350));
}

void AltF4(HWND hwnd)
{
	keybd_event(VK_MENU, NULL, KEYEVENTF_EXTENDEDKEY|0, 0);
	keybd_event(VK_F4, NULL, KEYEVENTF_EXTENDEDKEY|0, 0);
	keybd_event(VK_F4, NULL, KEYEVENTF_EXTENDEDKEY|KEYEVENTF_KEYUP, 0);
	keybd_event(VK_MENU, NULL, KEYEVENTF_EXTENDEDKEY|KEYEVENTF_KEYUP, 0);
}

void Esc(HWND hwnd)
{
	keybd_event(VK_ESCAPE, NULL, KEYEVENTF_EXTENDEDKEY|0, 0);
	keybd_event(VK_ESCAPE, NULL, KEYEVENTF_EXTENDEDKEY|KEYEVENTF_KEYUP, 0);
	::Sleep(randRange(100,350));
}

void TypeText(HWND hwnd, char *text)
{
	extern KEYCODE keycodes[];
	BYTE code;

	while(*text != '\0')
	{
		code = 0;
		for(int i = 0; keycodes[i].character != 0; i++)
		{
			if(UPPER(*text) == keycodes[i].character)
			{
				code = keycodes[i].windows_code;
				break;
			}
		}
		if(code)
		{
			keybd_event(code, NULL, KEYEVENTF_EXTENDEDKEY|0, 0);
			keybd_event(code, NULL, KEYEVENTF_EXTENDEDKEY|KEYEVENTF_KEYUP, 0);
		}
		text++;
	}
}

void MoveMouseAndClick(HWND hwnd, int x, int y)
{
	int ix = ::GetSystemMetrics(SM_CXSCREEN);
	int iy = ::GetSystemMetrics(SM_CYSCREEN);	
	POINT pt = {x,y};
	INPUT Input = {0};
	int destX, destY;

	::ClientToScreen(hwnd, &pt);

	destX = pt.x;
	destY = pt.y;

	Input.type			= INPUT_MOUSE;
	Input.mi.dwFlags	= MOUSEEVENTF_MOVE|MOUSEEVENTF_ABSOLUTE;
	Input.mi.dx			= destX*65535/ix;
	Input.mi.dy			= destY*65535/iy;

	::SendInput(1, &Input, sizeof(INPUT));
	LeftClick();
}
