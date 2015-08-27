#include <windows.h>
#include <Richedit.h>
#include <stdio.h>
#include <time.h>
#include <vector>
#include "detours.h"
#include "llist.h"
#include "player.h"
#include "table.h"
#include "flags.h"
#include "socket.h"
#include "const.h"
#include "util.h"
#include "macros.h"
#include "pstars.h"


static BOOL (WINAPI * TrueTranslate)(const MSG *lpMsg) = TranslateMessage;
// text interception functions
static int (WINAPI * TrueDrawTextW)(HDC hDC, LPCWSTR lpString, int nCount, LPRECT lpRect, UINT uFormat) = DrawTextW;
static int (WINAPI * TrueDrawTextExW)(HDC hdc, LPWSTR lpchText, int cchText, LPRECT lprc, UINT dwDTFormat, LPDRAWTEXTPARAMS lpDTParams) = DrawTextExW;
static BOOL (WINAPI * TrueTextOutW)(HDC hdc, int nXStart, int nYStart, LPCWSTR lpString, int cbString) = TextOutW;
static BOOL (WINAPI * TrueExtTextOutW)(HDC hdc, int X, int Y, UINT fuOptions, CONST RECT* lprc, LPCWSTR lpString, UINT cbCount, CONST INT* lpDx) = ExtTextOutW;

// extern decs
DWORD WINAPI MyThreadProc(LPVOID lpParameter);

HINSTANCE	hDll				= NULL;
HWND		mainWnd				= NULL;
long		mesgCount			= 0;
long		TimeForScreenCap	= 0;
int			site_id				= 0;
int			g_menu_bar_size		= 0; 
int			g_table_count		= 0;
int			g_table_max			= 1;
int			g_state				= 0;
int			g_popup				= 0;
HANDLE		hg_thread			= 0;
SOCK		client_sock(SOCK_STREAM, IPPROTO_TCP);
llist<TABLE> *tables = new llist<TABLE>;

BOOL WINAPI MyTextOutW(HDC hdc, int nXStart, int nYStart, LPCWSTR lpString, int cbString)
{
/*	FILE *fp = fopen("mybor.txt", "a+");
	fwprintf(fp, L"ExtTextOutW::%s\t\t -- %li\n", lpString, mesgCount++);
	fclose(fp);
*/	return TrueTextOutW(hdc, nXStart, nYStart, lpString, cbString);
}

BOOL WINAPI MyExtTextOutW(HDC hdc, int X, int Y, UINT fuOptions, CONST RECT* lprc, LPCWSTR lpString, UINT cbCount, CONST INT* lpDx)
{
	if(lpString)
	{
		switch(site_id)
		{
		case SITE_PSTARS:
			pstarsExtTextOut(hdc, lpString, lprc);
			break;
		default:
			MB("mydrawtextexw: no site_id");
			break;
		}
	}
	return TrueExtTextOutW(hdc, X, Y, fuOptions, lprc, lpString, cbCount, lpDx);
}

int WINAPI MyDrawTextW(HDC hDC, LPCWSTR lpString, int nCount, LPRECT lpRect, UINT uFormat)
{
	return TrueDrawTextW(hDC, lpString, nCount, lpRect, uFormat);
}

int WINAPI MyDrawTextExW(HDC hdc, LPWSTR lpchText, int cchText, LPRECT lprc, UINT dwDTFormat, LPDRAWTEXTPARAMS lpDTParams)
{
	if(lpchText)
	{
		switch(site_id)
		{
		case SITE_PSTARS:
			pstarsDrawTextEx(hdc, lpchText, lprc);
			break;
		default:
			MessageBox(NULL, "mydrawtextexw: no site_id", "error", NULL);
			break;
		}
	}
	return TrueDrawTextExW(hdc, lpchText, cchText, lprc, dwDTFormat, lpDTParams);
}

BOOL CALLBACK MyEnumWindowsChildFunc(HWND hwnd, LPARAM lParam)
{
	RECT pRect		= {0,0,0,0};
	TCHAR buf[100]	= {'\0'};
	HDC wndDC		= NULL;

	::GetClassName(hwnd, buf, 100);
	::GetWindowRect(hwnd, &pRect);
	wndDC = ::GetWindowDC(hwnd);

	FILE *fp = fopen("myborwindows.txt", "a+");
	fprintf(fp, "       Class(%-25s) B(%-4d) T(%-3d) b-t(%-3d) L(%-3d) R(%-4d) r-l(%-3d) DC(%d)\n", 
		buf,
		pRect.bottom, pRect.top, (pRect.bottom - pRect.top),
		pRect.left, pRect.right, (pRect.right - pRect.left),
		wndDC);
	fclose(fp);

	return TRUE;
}

BOOL CALLBACK MyEnumWindowsFunc(HWND hwnd, LPARAM lParam)
{
	HDC wndDC		= NULL;
	TCHAR buf[100]	= {'\0'};
	TCHAR name[100]	= {'\0'};
	RECT pRect		= {0,0,0,0};

	::GetClassName(hwnd, buf, 100);
	::GetWindowText(hwnd, name, 100);
	::GetWindowRect(hwnd, &pRect);
	wndDC = ::GetWindowDC(hwnd);

	FILE *fp = fopen("myborwindows.txt", "a+");
	fprintf(fp, "WINDOW Class(%-25s) (%-25s) B(%-4d) T(%-3d) b-t(%-3d) L(%-3d) R(%-4d) r-l(%-3d) DC(%d)\n", 
		buf, name,
		pRect.bottom, pRect.top, (pRect.bottom - pRect.top),
		pRect.left, pRect.right, (pRect.right - pRect.left),
		wndDC);
	fclose(fp);

	if(!strcmp("PokerStarsTableFrameClass", buf)
	|| !strcmp("PokerStars Lobby", name)
	|| !strcmp("Shell_TrayWnd", buf))
	{
//		::ScreenCaptureWindow(wndDC, &pRect, "myborwindow");
		::EnumChildWindows(hwnd, MyEnumWindowsChildFunc, 0);
	}

	return TRUE;
}


BOOL WINAPI MyTranslate(const MSG *lpMsg)
{
	TABLE *t = NULL;
	FILE *fp = NULL;

	switch(lpMsg->message)
	{
	case WM_KEYUP:
/*		fp = fopen("mybor.txt", "a+");
		fprintf(fp, "-----LISTING TABLES\n");
		for(t = tables->begin(); t; t = tables->next())
			fprintf(fp, "-----TABLE %-40s bb(%-2.2f) numP(%d) type(%d) avg_hands(%-3d) avg_flop(%d) avg_pot(%.2f)\n", 
				t->name, t->bb, t->num_players, t->poker_type, 
				t->avg_hands_hour, t->avg_players_flop, t->avg_pot);
		fclose(fp);

		::EnumWindows(MyEnumWindowsFunc, 0);
*/
		break;
	default:
		break;
	}
	return TrueTranslate(lpMsg);
}


// initialize stuff per site... called right after main window is found
void site_init()
{
	HWND frameWnd	= NULL;
	RECT pRect		= {0,0,0,0};

	::GetWindowRect(mainWnd, &pRect);

	switch(site_id)
	{
	case SITE_PSTARS:
		g_menu_bar_size = 41;
		// a PokerStarsFrameClass hides the ListClass that shows the players and their stack sizes 
		// after you click on a table.  we put it to the bottom window stack so the ListClass is found first.
		if((frameWnd = ::FindWindowEx(mainWnd, NULL, "PokerStarsFrameClass", NULL)))
			::SetWindowPos(frameWnd, HWND_BOTTOM, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE);
		pstarsSetupMenu();
		break;
	default:
		MessageBox(NULL, "site_init: no site_id", "error", NULL);
		break;
	}

//	ScreenCaptureWindow(::GetWindowDC(mainWnd), &pRect, "mybor_main_wnd");
}


BOOL APIENTRY DllMain( HINSTANCE hModule, DWORD fdwReason, LPVOID lpReserved )
{
	hDll = hModule;

	srand((int)time(0));

	switch(fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		Log("\n\n\nProgram starting up!");
		mainWnd = ::FindWindow(NULL,"PokerStars Lobby");
		site_id = SITE_PSTARS; // hack for now

		hg_thread = ::CreateThread(NULL, 0, MyThreadProc, 0, 0, 0);

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)TrueTranslate,	MyTranslate);
		DetourAttach(&(PVOID&)TrueDrawTextExW,	MyDrawTextExW);
		DetourAttach(&(PVOID&)TrueExtTextOutW,	MyExtTextOutW);
//		DetourAttach(&(PVOID&)TrueDrawTextW,	MyDrawTextW);
//		DetourAttach(&(PVOID&)TrueTextOutW,		MyTextOutW);
		DetourTransactionCommit();
		break;
	case DLL_THREAD_ATTACH:
		// I'm assuming this is what will happen when the programs launch subwindows (like when you open a table)
		// not necessary for pokerstars at least.
		break;
	case DLL_PROCESS_DETACH:
		// cleanup
		::CloseHandle(hg_thread);

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach(&(PVOID&)TrueTranslate,	MyTranslate);
		DetourDetach(&(PVOID&)TrueDrawTextExW,	MyDrawTextExW);
		DetourDetach(&(PVOID&)TrueExtTextOutW,	MyExtTextOutW);
//		DetourDetach(&(PVOID&)TrueDrawTextW,	MyDrawTextW);
//		DetourDetach(&(PVOID&)TrueTextOutW,		MyTextOutW);
		DetourTransactionCommit();
		break;
	default:
		break;
	}
	return TRUE;
} 
