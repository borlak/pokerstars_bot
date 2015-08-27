#include <windows.h>
#include "flags.h"

DTFFORMAT_STRUCT DTFFormat_types[] = {
	{DT_BOTTOM,				L"BOTTOM"			},
	{DT_CALCRECT,			L"CALCRECT"			},
	{DT_CENTER,				L"CENTER"			},
	{DT_EDITCONTROL,		L"EDITCONTROL"		},
	{DT_END_ELLIPSIS,		L"END_ELLIPSIS"		},
	{DT_EXPANDTABS,			L"EXPANDTABS"		},
	{DT_EXTERNALLEADING,	L"EXTERNALLEADING"	},
	{DT_HIDEPREFIX,			L"HIDEPREFIX"		},
	{DT_INTERNAL,			L"INTERNAL"			},
	{DT_LEFT,				L"LEFT"				},
	{DT_MODIFYSTRING,		L"MODIFYSTRING"		},
	{DT_NOCLIP,				L"NOCLIP"			},
	{DT_NOFULLWIDTHCHARBREAK, L"NOFULLWIDTHCHARBREAK" },
	{DT_NOPREFIX,			L"NOPREFIX"			},
	{DT_PATH_ELLIPSIS,		L"PATH_ELLIPSIS"	},
	{DT_PREFIXONLY,			L"PREFIXONLY"		},
	{DT_RIGHT,				L"RIGHT"			},
	{DT_RTLREADING,			L"RTLREADING"		},
	{DT_SINGLELINE,			L"SINGLELINE"		},
	{DT_TABSTOP,			L"TABSTOP"			},
	{DT_TOP,				L"TOP"				},
	{DT_VCENTER,			L"VCENTER"			},
	{DT_WORDBREAK,			L"WORDBREAK"		},
	{DT_WORD_ELLIPSIS,		L"WORD_ELLIPSIS"	}
};


LPWSTR get_flags_dtfformat(UINT format)
{
	static wchar_t buf[2056];
	buf[0] = '\0';
	int length = sizeof(DTFFormat_types) / sizeof(*DTFFormat_types);
	for(int i = 0; i < length; i++)
		if(DTFFormat_types[i].type & format)
		{
			wcscat(buf, DTFFormat_types[i].name);
			wcscat(buf, L" ");
		}
	if(format)
		buf[wcslen(buf)-1] = '\0';
	return buf;
}

KEYCODE keycodes[] = 
{
	{' ', 0x20},
	{'0', 0x30},
	{'1', 0x31},
	{'2', 0x32},
	{'3', 0x33},
	{'4', 0x34},
	{'5', 0x35},
	{'6', 0x36},
	{'7', 0x37},
	{'8', 0x38},
	{'9', 0x39},
	{'A', 0x41},
	{'B', 0x42},
	{'C', 0x43},
	{'D', 0x44},
	{'E', 0x45},
	{'F', 0x46},
	{'G', 0x47},
	{'H', 0x48},
	{'I', 0x49},
	{'J', 0x4A},
	{'K', 0x4B},
	{'L', 0x4C},
	{'M', 0x4D},
	{'N', 0x4E},
	{'O', 0x4F},
	{'P', 0x50},
	{'Q', 0x51},
	{'R', 0x52},
	{'S', 0x53},
	{'T', 0x54},
	{'U', 0x55},
	{'V', 0x56},
	{'W', 0x57},
	{'X', 0x58},
	{'Y', 0x59},
	{'Z', 0x5A},
	{0,0}
};