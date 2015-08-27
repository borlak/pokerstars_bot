#include <windows.h>
#include <vector>
#include "findwindow.h"

using namespace std;

findwindow::findwindow()
{
	::ZeroMemory(this, sizeof(FINDWINDOW));
}

findwindow::findwindow(char *class_str, char *window_str, int children)
{
	::ZeroMemory(this, sizeof(FINDWINDOW));
	find(class_str, window_str, children);
}

findwindow::~findwindow()
{
	cleanup();
}

void findwindow::cleanup()
{
	if(window_name) delete [] window_name;
	if(class_name) delete [] class_name;
	windows.clear();
	::ZeroMemory(this, sizeof(FINDWINDOW));
}

void findwindow::setup(char *class_str, char *window_str)
{
	if(class_str)
	{
		class_name = new char[strlen(class_str)+1];
		strcpy(class_name, class_str);
	}

	if(window_str)
	{
		window_name = new char[strlen(window_str)+1];
		strcpy(window_name, window_str);
	}
}

int findwindow::isActive()
{
	return ::GetActiveWindow() == window;
}

int findwindow::hasFocus()
{
	return ::GetFocus() == window;
}

void findwindow::find(char *class_str, char *window_str, int children)
{
	cleanup();
	search_children	= children;
	setup(class_str, window_str);

	::EnumWindows(FindEnumWindowsFunc, (LPARAM)this);
}

int findwindow::CheckWnd(HWND hwnd, FINDWINDOW *findwin)
{
	int		found			= 0;
	TCHAR	class_buf[512]	= {'\0'};
	TCHAR	name_buf[512]	= {'\0'};

	::GetClassName(hwnd, class_buf, 512);
	::GetWindowText(hwnd, name_buf, 512);

	if(findwin->class_name && strstr(class_buf, findwin->class_name) != NULL)
		found++;
	if(findwin->window_name && strstr(name_buf, findwin->window_name) != NULL)
		found++;

	if((findwin->window_name && findwin->class_name && found == 2)
	|| (((findwin->window_name && !findwin->class_name)
		||(findwin->class_name && !findwin->window_name))
		&& found == 1))
	{
		findwin->window = hwnd;
		return 1;
	}
	return 0;
}

int findwindow::FindEnumChildWindowsFunc(HWND hwnd, LPARAM lParam)
{
	if(CheckWnd(hwnd, (FINDWINDOW*)lParam))
		return 0;

	return 1;
}

int findwindow::FindEnumWindowsFunc(HWND hwnd, LPARAM lParam)
{
	FINDWINDOW *findwin = (FINDWINDOW*)lParam;

	if(CheckWnd(hwnd, findwin))
		return 0;

	if(findwin->search_children)
	{
		::EnumChildWindows(hwnd, FindEnumChildWindowsFunc, (LPARAM)findwin);

		if(findwin->window)
			return 0;
	}
	return 1;
}

vector<HWND> &findwindow::find_all(int children)
{
	cleanup();

	search_children = children;

	::EnumWindows(FindAllEnumWindowsFunc, (LPARAM)this);

	return windows;
}

int findwindow::FindAllEnumWindowsFunc(HWND hwnd, LPARAM lParam)
{
	FINDWINDOW *findwin = (FINDWINDOW*)lParam;

	findwin->windows.push_back(hwnd);
	if(findwin->search_children)
		::EnumChildWindows(hwnd, FindAllEnumChildWindowsFunc, (LPARAM)findwin);

	return 1;
}

int findwindow::FindAllEnumChildWindowsFunc(HWND hwnd, LPARAM lParam)
{
	FINDWINDOW *findwin = (FINDWINDOW*)lParam;
	findwin->windows.push_back(hwnd);
	return 1;
}

HWND findwindow::has_window(char *class_str, char *window_str)
{
	vector<HWND>::iterator it;

	// run setup so CheckWnd works
	setup(class_str, window_str);

	for(it = windows.begin(); it != windows.end(); it++)
	{
		if(CheckWnd(*it, this))
			return *it;
	}
	return 0;
}