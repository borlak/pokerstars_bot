const int SITE_PSTARS	= 1;	// poker stars
const int SITE_BODOG	= 2;	
const int SITE_PARTY	= 3;	// party poker
const int SITE_FT		= 4;	// full tilt

const int PTYPE_PLAY			= 0;
const int PTYPE_NOLIMIT_HOLDEM	= 1;
const int PTYPE_LIMIT_HOLDEM	= 2;
const int PTYPE_POT_HOLDEM		= 3;

// table->state
const int STATE_NONE		= 0;
const int STATE_SEARCH		= 1;
const int STATE_OPENED		= 2;
const int STATE_SIT			= 3;
const int STATE_BUYIN		= 4;
const int STATE_GET_HISTORY	= 5;
const int STATE_PLAY		= 6;
// g_state
const int STATE_LOGIN			= 0;
const int STATE_LEAVE_CASHIER	= 1;
const int STATE_CONNECT_MASTER	= 2;
const int STATE_FIND_TABLES		= 3;
const int STATE_MAIN_FINISHED	= 4;
const int STATE_MAIN_CHECK		= 5;

// global vars
extern int site_id;
extern int g_state;
extern int g_table_count;
extern int g_table_max;
extern int g_menu_bar_size;
extern int g_popup;
extern HDC gCaptureDC;
extern HWND mainWnd;
extern llist<TABLE> *tables;
extern SOCK	client_sock;


#include "util.h"

// global functions
// dllmain.cpp
void site_init(void);
// flags.cpp
LPWSTR get_flags_dtfformat(UINT format);
// mouse.cpp
void MoveMouse(int x, int y, int width, int height);
void MoveMouseAndClick(HWND hwnd, int x, int y);
void LeftClick(void);
void DoubleClick(void);
void AltTab(char *match);
void CR(HWND hwnd);
void AltF4(HWND hwnd);
void Esc(HWND hwnd);
void TypeText(HWND hwnd, char *text);
// pstars.cpp
void pstarsDrawTextEx(HDC hdc, LPWSTR lpchText, LPRECT lprc);
void pstarsExtTextOut(HDC hdc, LPCWSTR lpString, CONST RECT* lprc);
void pstarsCheckNews();
void pstarsSetupMenu();

// macros
#define UMIN(a, b)				((a) < (b) ? (a) : (b))
#define UMAX(a, b)				((a) > (b) ? (a) : (b))
#define URANGE(a, b, c)			((b) < (a) ? (a) : ((b) > (c) ? (c) : (b)))
#define IS_SET(flag, bit)		((flag) & (bit))
#define SET_BIT(var, bit)		((var) |= (bit))
#define REMOVE_BIT(var, bit)	((var) &= ~(bit))
#define LOWER(c)				((c) >= 'A' && (c) <= 'Z' ? (c)+'a'-'A' : (c))
#define UPPER(c)				((c) >= 'a' && (c) <= 'z' ? (c)+'A'-'a' : (c))

#define MB(str)					(MessageBox(NULL, (str), (str), NULL))
