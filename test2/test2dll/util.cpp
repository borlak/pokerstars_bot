#include <windows.h>
#include <stdio.h>
#include <ctime>
#include <sstream>
#include "llist.h"
#include "player.h"
#include "table.h"
#include "flags.h"
#include "menu.h"
#include "const.h"
#include "dllmain.h"

// global vars
HDC		gCaptureDC		= NULL;
HBITMAP gCaptureBitmap	= NULL;

int randRange(int low, int high)
{
	return ((int)rand() % (high - low + 1) + low);
}
float randRangef(float low, float high)
{
	float ret;
	do {
		ret = ((float)rand() / (float)(RAND_MAX + 1) * (float)((high+1) - low) + low);
	} while(ret < low || ret > high);
	return ret;
}
// fuzzy returns true(1) if num is +/- 1 from compare, else returns false(0)
int fuzzy(const int num, const int compare)
{
	int x = compare - (num-1);
	return (x <= 2 && x >= 0);
}
void MidPoint(LPRECT rect, LPPOINT point)
{
	point->x = rect->left + ((rect->right-rect->left)/2);
	point->y = rect->top + ((rect->bottom-rect->top)/2);
}

std::string int_to_string(int num)
{
	std::ostringstream stm;
	stm << num;
	return stm.str();
}

std::string float_to_string(double num)
{
	std::ostringstream stm;
	stm << num;
	return stm.str();
}

int string_to_int(std::string s)
{
	std::istringstream instr(s);
	int n;
	instr >> n;
	return n;
}

double money(char *str)
{
	static char buf[128] = {'\0'};
	char *p = buf;
	while((!isdigit(*str) && *str != '\0') || *str == '.')
		str++;
	while(isdigit(*str) || *str == '.')
		*p++ = *str++;
	*p = '\0';
	return atof(buf);
}

int number(char *str)
{
	static char buf[128] = {'\0'};
	char *p = buf;
	while(!isdigit(*str) && *str != '\0')
			str++;
	while(isdigit(*str))
		*p++ = *str++;
	*p = '\0';
	return atoi(buf);
}

char *card(char *str)
{
	static char buf[3] = {'\0'};
	while(!isdigit(*str) && *str != 'A' && *str != 'K' 
		&& *str != 'Q' && *str != 'J' && *str != 'T'
		&& *str != '\0')
		str++;
	buf[0] = *str++;
	buf[1] = *str;
	buf[2] = '\0';
	return buf;
}

char *get_line(char *&p)
{
	static char line[4096];
	char *lp = line;
	*lp = '\0';
	while(isspace(*p) && *p != '\0')
		p++;
	while(*p != '\0' && *p != '\n')
		*lp++ = *p++;
	*lp = '\0';
	return line[0] == '\0' ? NULL : line;
}

void SaveBitmap(char *szFilename, HBITMAP hBitmap)
{
	HDC					hdc		= NULL;
	FILE*				fp		= NULL;
	LPVOID				pBuf	= NULL;
	BITMAPINFO			bmpInfo;
	BITMAPFILEHEADER	bmpFileHeader;

	do{
		hdc = ::GetDC(NULL);
		::ZeroMemory(&bmpInfo,sizeof(BITMAPINFO));
		bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		::GetDIBits(hdc,hBitmap,0,0,NULL,&bmpInfo,DIB_RGB_COLORS);

		if(bmpInfo.bmiHeader.biSizeImage <= 0)
			bmpInfo.bmiHeader.biSizeImage = bmpInfo.bmiHeader.biWidth*abs(bmpInfo.bmiHeader.biHeight)*(bmpInfo.bmiHeader.biBitCount+7)/8;

		if((pBuf=malloc(bmpInfo.bmiHeader.biSizeImage))==NULL)
		{
			MessageBox(NULL,"Unable to Allocate Bitmap Memory","Error",MB_OK|MB_ICONERROR);
			break;
		}

		bmpInfo.bmiHeader.biCompression=BI_RGB;
		GetDIBits(hdc,hBitmap,0,bmpInfo.bmiHeader.biHeight,pBuf,&bmpInfo,DIB_RGB_COLORS);	

		if((fp=fopen(szFilename,"wb+")) == NULL)
		{
			MessageBox(NULL,"Unable to Create Bitmap File","Error",MB_OK|MB_ICONERROR);
			break;
		}

		bmpFileHeader.bfReserved1=0;
		bmpFileHeader.bfReserved2=0;
		bmpFileHeader.bfSize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+bmpInfo.bmiHeader.biSizeImage;
		bmpFileHeader.bfType='MB';
		bmpFileHeader.bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);

		fwrite(&bmpFileHeader,sizeof(BITMAPFILEHEADER),1,fp);
		fwrite(&bmpInfo.bmiHeader,sizeof(BITMAPINFOHEADER),1,fp);
		fwrite(pBuf,bmpInfo.bmiHeader.biSizeImage,1,fp);
	}while(false);

	if(hdc)		ReleaseDC(NULL,hdc);
	if(pBuf)	free(pBuf);
	if(fp)		fclose(fp);
}
VOID ScreenCaptureWindow(HDC hDC, LPRECT pRect, char *name)
{
	char filename[128]		= {'\0'};
	int nScreenWidth		= pRect->right - pRect->left;
	int nScreenHeight		= pRect->bottom - pRect->top;

	sprintf(filename, "%s.bmp", name);

	if(gCaptureDC)
	{
		::DeleteDC(gCaptureDC);
		::DeleteObject(gCaptureBitmap);
	}

	gCaptureDC		= ::CreateCompatibleDC(hDC);
	gCaptureBitmap	= ::CreateCompatibleBitmap(hDC, nScreenWidth, nScreenHeight);

	::SelectObject(gCaptureDC, gCaptureBitmap); 
	::BitBlt(gCaptureDC, 0, 0, nScreenWidth, nScreenHeight, hDC, 0, 0, SRCCOPY); 

	SaveBitmap(filename, gCaptureBitmap);
}

void CheckActive(llist<MENU> *menu)
{
	COLORREF	colr = -1;
	MENU		*cur = 0;

	for(cur = menu->begin(); cur; cur = menu->next())
	{
		CheckActive(&cur->sub);
		colr = ::GetPixel(gCaptureDC, cur->x, cur->y);
		switch(site_id)
		{
		case SITE_PSTARS:
			cur->active = (GetBValue(colr) == GetBValue(cur->active_color))
						&&(GetGValue(colr) == GetGValue(cur->active_color));
			break;
		default:
			MessageBox(NULL, "CheckActive: no site_id", "error", NULL);
			return;
		}

		if(cur->active)
		{
			char buf[300];sprintf(buf,"%s -- x:%d y:%d   colr:%d %d %d",
				cur->name,cur->x,cur->y,GetRValue(colr),GetGValue(colr),GetBValue(colr));
//			MessageBox(NULL,buf,"is active!",NULL);
		}
	}
}

void ClearTableRects(void)
{
	TABLE *table;

	for(table = tables->begin(); table; table = tables->next())
		::ZeroMemory(&table->rect, sizeof(RECT));
}

// game logging to std error.. now with sprintf support
void Log(const char *str, ...)
{
	FILE	*fp				= fopen("myborlog.txt", "a+");
	char	logstr[2056]	= {'\0'};
	char	buf[2056]		= {'\0'};
	va_list ap;

	va_start(ap, str);
	(void)vsnprintf(buf, 2056, str, ap);

//	strftime(logstr, 2056, "%m/%d/%Y %H:%M:%S :: ",  localtime(0));
	strcat(logstr,buf);

	fprintf(fp, "%s\n", logstr);
	fclose(fp);

	va_end(ap);
}


