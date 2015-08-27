#pragma once
#ifndef _MY_VARS_
#define _MY_VARS_

const int SITE_PSTARS	= 1;	// poker stars
const int SITE_BODOG	= 2;	
const int SITE_PARTY	= 3;	// party poker
const int SITE_FT		= 4;	// full tilt

const int PTYPE_PLAY			= 0;
const int PTYPE_NOLIMIT_HOLDEM	= 1;
const int PTYPE_LIMIT_HOLDEM	= 2;
const int PTYPE_POT_HOLDEM		= 3;

// global functions
// dllmain.cpp
int randRange(int low, int high);
// mouse.cpp
void MouseMove(int x, int y, int width, int height);
void LeftClick(void);

// macros
#define UMIN(a, b)		((a) < (b) ? (a) : (b))
#define UMAX(a, b)		((a) > (b) ? (a) : (b))
#define URANGE(a, b, c)	((b) < (a) ? (a) : ((b) > (c) ? (c) : (b)))

#endif 