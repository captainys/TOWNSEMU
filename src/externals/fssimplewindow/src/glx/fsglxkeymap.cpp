/* ////////////////////////////////////////////////////////////

File Name: fsglxkeymap.cpp
Copyright (c) 2017 Soji Yamakawa.  All rights reserved.
http://www.ysflight.com

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS 
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE 
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

//////////////////////////////////////////////////////////// */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/poll.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/keysymdef.h>
#include <X11/Xatom.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>

#include "fssimplewindow.h"

#define FS_NUM_XK 65536

class XKtoFSKEYMap
{
public:
	int fskeyForInkey=FSKEY_NULL;
	int fskeyForKeyState=FSKEY_NULL;
};

static XKtoFSKEYMap mapXKtoFSKEY[FS_NUM_XK];
static char mapXKtoChar[FS_NUM_XK];
static int mapFSKEYtoXK[FSKEY_NUM_KEYCODE];


static void FsXAddKeyMapping(int fskey1,int fskey2,int keysym)
{
	if(fskey1<0 || FSKEY_NUM_KEYCODE<=fskey1)
	{
		fprintf(stderr,"FSKEY is out of range\n");
		return;
	}
	if(keysym<0 || FS_NUM_XK<=keysym)
	{
		fprintf(stderr,"XK is out of range\n");
		return;
	}

	mapXKtoFSKEY[keysym].fskeyForInkey=fskey1;
	mapXKtoFSKEY[keysym].fskeyForKeyState=fskey2;
	mapFSKEYtoXK[fskey1]=keysym;
	mapFSKEYtoXK[fskey2]=keysym;
}

static void FsXAddKeyMapping(int fskey,int keysym)
{
	if(fskey<0 || FSKEY_NUM_KEYCODE<=fskey)
	{
		fprintf(stderr,"FSKEY is out of range\n");
		return;
	}
	if(keysym<0 || FS_NUM_XK<=keysym)
	{
		fprintf(stderr,"XK is out of range\n");
		return;
	}

	mapXKtoFSKEY[keysym].fskeyForInkey=fskey;
	mapXKtoFSKEY[keysym].fskeyForKeyState=FSKEY_NULL;
	mapFSKEYtoXK[fskey]=keysym;
}

static void FsXAddKeysymToCharMapping(int keysym,char c)
{
	if(0<keysym && keysym<FS_NUM_XK)
	{
		mapXKtoChar[keysym]=c;
	}
}

void FsXCreateKeyMapping(void)
{
	int i;
	for(i=0; i<FS_NUM_XK; i++)
	{
		mapXKtoFSKEY[i].fskeyForInkey=0;
		mapXKtoFSKEY[i].fskeyForKeyState=0;
		mapXKtoChar[i]=0;
	}
	for(i=0; i<FSKEY_NUM_KEYCODE; i++)
	{
		mapFSKEYtoXK[i]=0;
	}

	FsXAddKeyMapping(FSKEY_SPACE,               XK_space);
	FsXAddKeyMapping(FSKEY_0,                   XK_0);
	FsXAddKeyMapping(FSKEY_1,                   XK_1);
	FsXAddKeyMapping(FSKEY_2,                   XK_2);
	FsXAddKeyMapping(FSKEY_3,                   XK_3);
	FsXAddKeyMapping(FSKEY_4,                   XK_4);
	FsXAddKeyMapping(FSKEY_5,                   XK_5);
	FsXAddKeyMapping(FSKEY_6,                   XK_6);
	FsXAddKeyMapping(FSKEY_7,                   XK_7);
	FsXAddKeyMapping(FSKEY_8,                   XK_8);
	FsXAddKeyMapping(FSKEY_9,                   XK_9);
	FsXAddKeyMapping(FSKEY_A,                   XK_A);
	FsXAddKeyMapping(FSKEY_B,                   XK_B);
	FsXAddKeyMapping(FSKEY_C,                   XK_C);
	FsXAddKeyMapping(FSKEY_D,                   XK_D);
	FsXAddKeyMapping(FSKEY_E,                   XK_E);
	FsXAddKeyMapping(FSKEY_F,                   XK_F);
	FsXAddKeyMapping(FSKEY_G,                   XK_G);
	FsXAddKeyMapping(FSKEY_H,                   XK_H);
	FsXAddKeyMapping(FSKEY_I,                   XK_I);
	FsXAddKeyMapping(FSKEY_J,                   XK_J);
	FsXAddKeyMapping(FSKEY_K,                   XK_K);
	FsXAddKeyMapping(FSKEY_L,                   XK_L);
	FsXAddKeyMapping(FSKEY_M,                   XK_M);
	FsXAddKeyMapping(FSKEY_N,                   XK_N);
	FsXAddKeyMapping(FSKEY_O,                   XK_O);
	FsXAddKeyMapping(FSKEY_P,                   XK_P);
	FsXAddKeyMapping(FSKEY_Q,                   XK_Q);
	FsXAddKeyMapping(FSKEY_R,                   XK_R);
	FsXAddKeyMapping(FSKEY_S,                   XK_S);
	FsXAddKeyMapping(FSKEY_T,                   XK_T);
	FsXAddKeyMapping(FSKEY_U,                   XK_U);
	FsXAddKeyMapping(FSKEY_V,                   XK_V);
	FsXAddKeyMapping(FSKEY_W,                   XK_W);
	FsXAddKeyMapping(FSKEY_X,                   XK_X);
	FsXAddKeyMapping(FSKEY_Y,                   XK_Y);
	FsXAddKeyMapping(FSKEY_Z,                   XK_Z);
	FsXAddKeyMapping(FSKEY_ESC,                 XK_Escape);
	FsXAddKeyMapping(FSKEY_F1,                  XK_F1);
	FsXAddKeyMapping(FSKEY_F2,                  XK_F2);
	FsXAddKeyMapping(FSKEY_F3,                  XK_F3);
	FsXAddKeyMapping(FSKEY_F4,                  XK_F4);
	FsXAddKeyMapping(FSKEY_F5,                  XK_F5);
	FsXAddKeyMapping(FSKEY_F6,                  XK_F6);
	FsXAddKeyMapping(FSKEY_F7,                  XK_F7);
	FsXAddKeyMapping(FSKEY_F8,                  XK_F8);
	FsXAddKeyMapping(FSKEY_F9,                  XK_F9);
	FsXAddKeyMapping(FSKEY_F10,                 XK_F10);
	FsXAddKeyMapping(FSKEY_F11,                 XK_F11);
	FsXAddKeyMapping(FSKEY_F12,                 XK_F12);
	FsXAddKeyMapping(FSKEY_PRINTSCRN,           0);
	FsXAddKeyMapping(FSKEY_SCROLLLOCK,          XK_Scroll_Lock);
	FsXAddKeyMapping(FSKEY_PAUSEBREAK,          XK_Cancel);
	FsXAddKeyMapping(FSKEY_TILDA,               XK_grave);
	FsXAddKeyMapping(FSKEY_MINUS,               XK_minus);
	FsXAddKeyMapping(FSKEY_PLUS,                XK_equal); // There was a mix up.
	FsXAddKeyMapping(FSKEY_BS,                  XK_BackSpace);
	FsXAddKeyMapping(FSKEY_TAB,                 XK_Tab);
	FsXAddKeyMapping(FSKEY_LBRACKET,            XK_bracketleft);
	FsXAddKeyMapping(FSKEY_RBRACKET,            XK_bracketright);
	FsXAddKeyMapping(FSKEY_BACKSLASH,           XK_backslash);
	FsXAddKeyMapping(FSKEY_CAPSLOCK,            0);
	FsXAddKeyMapping(FSKEY_SEMICOLON,           ';');
	FsXAddKeyMapping(FSKEY_COLON,               ':');
	FsXAddKeyMapping(FSKEY_SINGLEQUOTE,         '\'');
	FsXAddKeyMapping(FSKEY_ENTER,               XK_Return);
	FsXAddKeyMapping(FSKEY_SHIFT,               FSKEY_LEFT_SHIFT,   XK_Shift_L);
	FsXAddKeyMapping(FSKEY_SHIFT,               FSKEY_RIGHT_SHIFT,  XK_Shift_R);
	FsXAddKeyMapping(FSKEY_COMMA,               XK_comma);
	FsXAddKeyMapping(FSKEY_DOT,                 XK_period);
	FsXAddKeyMapping(FSKEY_SLASH,               XK_slash);
	FsXAddKeyMapping(FSKEY_CTRL,                FSKEY_LEFT_CTRL,    XK_Control_L);
	FsXAddKeyMapping(FSKEY_CTRL,                FSKEY_RIGHT_CTRL,   XK_Control_R);
	FsXAddKeyMapping(FSKEY_ALT,                 FSKEY_LEFT_ALT,     XK_Alt_L);
	FsXAddKeyMapping(FSKEY_ALT,                 FSKEY_RIGHT_ALT,    XK_Alt_R);
	FsXAddKeyMapping(FSKEY_INS,                 XK_Insert);
	FsXAddKeyMapping(FSKEY_DEL,                 XK_Delete);
	FsXAddKeyMapping(FSKEY_HOME,                XK_Home);
	FsXAddKeyMapping(FSKEY_END,                 XK_End);
	FsXAddKeyMapping(FSKEY_PAGEUP,              XK_Page_Up);
	FsXAddKeyMapping(FSKEY_PAGEDOWN,            XK_Page_Down);
	FsXAddKeyMapping(FSKEY_UP,                  XK_Up);
	FsXAddKeyMapping(FSKEY_DOWN,                XK_Down);
	FsXAddKeyMapping(FSKEY_LEFT,                XK_Left);
	FsXAddKeyMapping(FSKEY_RIGHT,               XK_Right);
	FsXAddKeyMapping(FSKEY_NUMLOCK,             XK_Num_Lock);
	FsXAddKeyMapping(FSKEY_TEN0,                XK_KP_0);
	FsXAddKeyMapping(FSKEY_TEN1,                XK_KP_1);
	FsXAddKeyMapping(FSKEY_TEN2,                XK_KP_2);
	FsXAddKeyMapping(FSKEY_TEN3,                XK_KP_3);
	FsXAddKeyMapping(FSKEY_TEN4,                XK_KP_4);
	FsXAddKeyMapping(FSKEY_TEN5,                XK_KP_5);
	FsXAddKeyMapping(FSKEY_TEN6,                XK_KP_6);
	FsXAddKeyMapping(FSKEY_TEN7,                XK_KP_7);
	FsXAddKeyMapping(FSKEY_TEN8,                XK_KP_8);
	FsXAddKeyMapping(FSKEY_TEN9,                XK_KP_9);
	FsXAddKeyMapping(FSKEY_TENDOT,              XK_KP_Decimal);
	FsXAddKeyMapping(FSKEY_TENSLASH,            XK_KP_Divide);
	FsXAddKeyMapping(FSKEY_TENSTAR,             XK_KP_Multiply);
	FsXAddKeyMapping(FSKEY_TENMINUS,            XK_KP_Subtract);
	FsXAddKeyMapping(FSKEY_TENPLUS,             XK_KP_Add);
	FsXAddKeyMapping(FSKEY_TENENTER,            XK_KP_Enter);

	FsXAddKeyMapping(FSKEY_TEN0,                XK_KP_Insert);
	FsXAddKeyMapping(FSKEY_TEN1,                XK_KP_End);
	FsXAddKeyMapping(FSKEY_TEN2,                XK_KP_Down);
	FsXAddKeyMapping(FSKEY_TEN3,                XK_KP_Page_Down);
	FsXAddKeyMapping(FSKEY_TEN4,                XK_KP_Left);
	FsXAddKeyMapping(FSKEY_TEN5,                XK_KP_Begin);
	FsXAddKeyMapping(FSKEY_TEN6,                XK_KP_Right);
	FsXAddKeyMapping(FSKEY_TEN7,                XK_KP_Home);
	FsXAddKeyMapping(FSKEY_TEN8,                XK_KP_Up);
	FsXAddKeyMapping(FSKEY_TEN9,                XK_KP_Page_Up);
	FsXAddKeyMapping(FSKEY_TENDOT,              XK_KP_Delete);
	// FsXAddKeyMapping(FSKEY_TENSLASH,            XK_KP_Divide);
	// FsXAddKeyMapping(FSKEY_TENSTAR,             XK_KP_Multiply);
	// FsXAddKeyMapping(FSKEY_TENMINUS,            XK_KP_Subtract);
	// FsXAddKeyMapping(FSKEY_TENPLUS,             XK_KP_Add);
	// FsXAddKeyMapping(FSKEY_TENENTER,            XK_KP_Enter);

	FsXAddKeyMapping(FSKEY_CONVERT,             XK_Henkan);
	FsXAddKeyMapping(FSKEY_NONCONVERT,          XK_Muhenkan);
	FsXAddKeyMapping(FSKEY_KANA,                XK_Kana_Lock);
	FsXAddKeyMapping(FSKEY_CONTEXT,             XK_Menu);


	FsXAddKeysymToCharMapping(XK_space,         ' ');
	FsXAddKeysymToCharMapping(XK_0,             '0');
	FsXAddKeysymToCharMapping(XK_1,             '1');
	FsXAddKeysymToCharMapping(XK_2,             '2');
	FsXAddKeysymToCharMapping(XK_3,             '3');
	FsXAddKeysymToCharMapping(XK_4,             '4');
	FsXAddKeysymToCharMapping(XK_5,             '5');
	FsXAddKeysymToCharMapping(XK_6,             '6');
	FsXAddKeysymToCharMapping(XK_7,             '7');
	FsXAddKeysymToCharMapping(XK_8,             '8');
	FsXAddKeysymToCharMapping(XK_9,             '9');
	FsXAddKeysymToCharMapping(XK_A,             'A');
	FsXAddKeysymToCharMapping(XK_B,             'B');
	FsXAddKeysymToCharMapping(XK_C,             'C');
	FsXAddKeysymToCharMapping(XK_D,             'D');
	FsXAddKeysymToCharMapping(XK_E,             'E');
	FsXAddKeysymToCharMapping(XK_F,             'F');
	FsXAddKeysymToCharMapping(XK_G,             'G');
	FsXAddKeysymToCharMapping(XK_H,             'H');
	FsXAddKeysymToCharMapping(XK_I,             'I');
	FsXAddKeysymToCharMapping(XK_J,             'J');
	FsXAddKeysymToCharMapping(XK_K,             'K');
	FsXAddKeysymToCharMapping(XK_L,             'L');
	FsXAddKeysymToCharMapping(XK_M,             'M');
	FsXAddKeysymToCharMapping(XK_N,             'N');
	FsXAddKeysymToCharMapping(XK_O,             'O');
	FsXAddKeysymToCharMapping(XK_P,             'P');
	FsXAddKeysymToCharMapping(XK_Q,             'Q');
	FsXAddKeysymToCharMapping(XK_R,             'R');
	FsXAddKeysymToCharMapping(XK_S,             'S');
	FsXAddKeysymToCharMapping(XK_T,             'T');
	FsXAddKeysymToCharMapping(XK_U,             'U');
	FsXAddKeysymToCharMapping(XK_V,             'V');
	FsXAddKeysymToCharMapping(XK_W,             'W');
	FsXAddKeysymToCharMapping(XK_X,             'X');
	FsXAddKeysymToCharMapping(XK_Y,             'Y');
	FsXAddKeysymToCharMapping(XK_Z,             'Z');
	FsXAddKeysymToCharMapping(XK_a,             'a');
	FsXAddKeysymToCharMapping(XK_b,             'b');
	FsXAddKeysymToCharMapping(XK_c,             'c');
	FsXAddKeysymToCharMapping(XK_d,             'd');
	FsXAddKeysymToCharMapping(XK_e,             'e');
	FsXAddKeysymToCharMapping(XK_f,             'f');
	FsXAddKeysymToCharMapping(XK_g,             'g');
	FsXAddKeysymToCharMapping(XK_h,             'h');
	FsXAddKeysymToCharMapping(XK_i,             'i');
	FsXAddKeysymToCharMapping(XK_j,             'j');
	FsXAddKeysymToCharMapping(XK_k,             'k');
	FsXAddKeysymToCharMapping(XK_l,             'l');
	FsXAddKeysymToCharMapping(XK_m,             'm');
	FsXAddKeysymToCharMapping(XK_n,             'n');
	FsXAddKeysymToCharMapping(XK_o,             'o');
	FsXAddKeysymToCharMapping(XK_p,             'p');
	FsXAddKeysymToCharMapping(XK_q,             'q');
	FsXAddKeysymToCharMapping(XK_r,             'r');
	FsXAddKeysymToCharMapping(XK_s,             's');
	FsXAddKeysymToCharMapping(XK_t,             't');
	FsXAddKeysymToCharMapping(XK_u,             'u');
	FsXAddKeysymToCharMapping(XK_v,             'v');
	FsXAddKeysymToCharMapping(XK_w,             'w');
	FsXAddKeysymToCharMapping(XK_x,             'x');
	FsXAddKeysymToCharMapping(XK_y,             'y');
	FsXAddKeysymToCharMapping(XK_z,             'z');
	FsXAddKeysymToCharMapping(XK_Escape,        0x1b);
	FsXAddKeysymToCharMapping(XK_BackSpace,     0x08);
	FsXAddKeysymToCharMapping(XK_Tab,           '\t');
	FsXAddKeysymToCharMapping(XK_Return,        '\n');

	
	FsXAddKeysymToCharMapping(XK_grave,         '`');
	FsXAddKeysymToCharMapping(XK_asciitilde,    '~');
	FsXAddKeysymToCharMapping(XK_exclam,        '!');
	FsXAddKeysymToCharMapping(XK_at,            '@');
	FsXAddKeysymToCharMapping(XK_numbersign,    '#');
	FsXAddKeysymToCharMapping(XK_dollar,        '$');
	FsXAddKeysymToCharMapping(XK_percent,       '%');
	FsXAddKeysymToCharMapping(XK_asciicircum,   '^');
	FsXAddKeysymToCharMapping(XK_ampersand,     '&');
	FsXAddKeysymToCharMapping(XK_asterisk,      '*');
	FsXAddKeysymToCharMapping(XK_parenleft,     '(');
	FsXAddKeysymToCharMapping(XK_parenright,    ')');
	FsXAddKeysymToCharMapping(XK_minus,         '-');
	FsXAddKeysymToCharMapping(XK_underscore,    '_');
	FsXAddKeysymToCharMapping(XK_equal,         '=');
	FsXAddKeysymToCharMapping(XK_plus,          '+');
	FsXAddKeysymToCharMapping(XK_bracketleft,   '[');
	FsXAddKeysymToCharMapping(XK_braceleft,     '{');
	FsXAddKeysymToCharMapping(XK_bracketright,  ']');
	FsXAddKeysymToCharMapping(XK_braceright,    '}');
	FsXAddKeysymToCharMapping(XK_backslash,     '\\');
	FsXAddKeysymToCharMapping(XK_bar,           '|');
	FsXAddKeysymToCharMapping(XK_semicolon,     ';');
	FsXAddKeysymToCharMapping(XK_colon,         ':');
	FsXAddKeysymToCharMapping(XK_apostrophe,    '\'');
	FsXAddKeysymToCharMapping(XK_quotedbl,      '\"');
	FsXAddKeysymToCharMapping(XK_comma,         ',');
	FsXAddKeysymToCharMapping(XK_less,          '<');
	FsXAddKeysymToCharMapping(XK_period,        '.');
	FsXAddKeysymToCharMapping(XK_greater,       '>');
	FsXAddKeysymToCharMapping(XK_slash,         '/');
	FsXAddKeysymToCharMapping(XK_question,      '?');

	FsXAddKeysymToCharMapping(XK_KP_0,          '0');
	FsXAddKeysymToCharMapping(XK_KP_1,          '1');
	FsXAddKeysymToCharMapping(XK_KP_2,          '2');
	FsXAddKeysymToCharMapping(XK_KP_3,          '3');
	FsXAddKeysymToCharMapping(XK_KP_4,          '4');
	FsXAddKeysymToCharMapping(XK_KP_5,          '5');
	FsXAddKeysymToCharMapping(XK_KP_6,          '6');
	FsXAddKeysymToCharMapping(XK_KP_7,          '7');
	FsXAddKeysymToCharMapping(XK_KP_8,          '8');
	FsXAddKeysymToCharMapping(XK_KP_9,          '9');
	FsXAddKeysymToCharMapping(XK_KP_Decimal,    '.');
	FsXAddKeysymToCharMapping(XK_KP_Divide,     '/');
	FsXAddKeysymToCharMapping(XK_KP_Multiply,   '*');
	FsXAddKeysymToCharMapping(XK_KP_Subtract,   '-');
	FsXAddKeysymToCharMapping(XK_KP_Add,        '+');
	FsXAddKeysymToCharMapping(XK_KP_Enter,      '\n');
}

int FsXKeySymToFsInkey(int keysym)
{
	if(0<=keysym && keysym<FS_NUM_XK)
	{
		return mapXKtoFSKEY[keysym].fskeyForInkey;
	}
	return 0;
}

int FsXKeySymToFsGetKeyState(int keysym)
{
	if(0<=keysym && keysym<FS_NUM_XK)
	{
		return mapXKtoFSKEY[keysym].fskeyForKeyState;
	}
	return 0;
}

char FsXKeySymToChar(int keysym)
{
	if(0<=keysym && keysym<FS_NUM_XK)
	{
		return mapXKtoChar[keysym];
	}
	return 0;
}

int FsXFskeyToKeySym(int fskey)
{
	if(0<=fskey && fskey<FSKEY_NUM_KEYCODE)
	{
		return mapFSKEYtoXK[fskey];
	}
	return 0;
}
