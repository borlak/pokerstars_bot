#define _CRT_SECURE_NO_WARNINGS // get rid of annoying warnings

#include <windows.h> 
#include <stdio.h> 
#include <tlhelp32.h> 
#include <shlwapi.h>

#define WINDOW_NAME "PokerStars Lobby"
#define DLL_NAME "C:\\Documents and Settings\\Administrator\\My Documents\\Visual Studio 2008\\Projects\\test2\\Debug\\test2dll.dll" 

//I could just use PROCESS_ALL_ACCESS but it's always best to use the absolute bare minimum of priveleges, so that your code works in as 
//many circumstances as possible. 
#define CREATE_THREAD_ACCESS (PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ) 
  
BOOL WriteProcessBYTES(HANDLE hProcess,LPVOID lpBaseAddress,LPCVOID lpBuffer,SIZE_T nSize); 
BOOL LoadDll(char *procName, char *dllName); 

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow) 
{
	LoadDll(WINDOW_NAME, DLL_NAME);
	return 0;
} 

unsigned long GetTargetProcessIdFromWindow(char *windowName) 
{ 
    unsigned long procID; 
    HWND targetWnd; 

    targetWnd = FindWindow(NULL, windowName);
	if(!targetWnd)
      MessageBox(NULL, "Unable to find window", "Loader", NULL); 

	GetWindowThreadProcessId(targetWnd, &procID); 
    
    return procID; 
} 


BOOL LoadDll(char *windowName, char *dllName) 
{ 
	DWORD ProcID = 0, hLibModule;
	HANDLE Proc, hThread; 
	char buf[50]={0}; 
	LPVOID RemoteString, LoadLibAddy; 

	ProcID = GetTargetProcessIdFromWindow(windowName); 

	if(!ProcID) 
		return FALSE;

	Proc = OpenProcess(CREATE_THREAD_ACCESS, FALSE, ProcID); 

	if(!Proc) 
	{
		sprintf(buf, "OpenProcess() failed: %d", GetLastError()); 
		MessageBox(NULL, buf, "Loader", NULL); 
		return FALSE;
	} 

	LoadLibAddy = (LPVOID)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA"); 

	RemoteString = (LPVOID)VirtualAllocEx(Proc, NULL, strlen(DLL_NAME), MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE); 
	WriteProcessMemory(Proc, (LPVOID)RemoteString, dllName, strlen(dllName), NULL); 
	hThread = CreateRemoteThread(Proc, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibAddy, (LPVOID)RemoteString, NULL, NULL);

	::WaitForSingleObject(hThread, INFINITE);
	::GetExitCodeThread(hThread, &hLibModule);
	::CloseHandle(hThread);
	::VirtualFreeEx(Proc, RemoteString, strlen(dllName), MEM_RELEASE);
	return TRUE; 
} 
