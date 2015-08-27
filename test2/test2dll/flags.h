#pragma once
#ifndef __MY_FLAGS__
#define __MY_FLAGS__

typedef struct DTFFormat_struct DTFFORMAT_STRUCT;
typedef struct keycode KEYCODE;

struct DTFFormat_struct
{
	int type;
	wchar_t *name;
};

struct keycode
{
	char character;
	BYTE windows_code;
};

// table flags
const int SIXMAX	= 0x00000001;
const int NOALLIN	= 0x00000002;
const int FAST		= 0x00000004;

#endif