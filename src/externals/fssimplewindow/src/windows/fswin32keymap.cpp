/* ////////////////////////////////////////////////////////////

File Name: fswin32keymap.cpp
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

#if defined(_WIN32_WINNT) && _WIN32_WINNT<0x0500
#undef _WIN32_WINNT
#endif

#if !defined(_WIN32_WINNT)
#define _WIN32_WINNT 0x500
#endif

#include <stdio.h>
#include <stdlib.h>

#include "fssimplewindow.h"
#include "fswin32keymap.h"


#include <windows.h>





FsWin32KeyMapper::FsWin32KeyMapper()
{
	int i;

	mapVKtoFSKEY=new int [FS_NUM_VK];
	mapFSKEYtoVK=new int [FSKEY_NUM_KEYCODE];

	for(i=0; i<FS_NUM_VK; i++)
	{
		mapVKtoFSKEY[i]=FSKEY_NULL;
	}
	for(i=0; i<FSKEY_NUM_KEYCODE; i++)
	{
		mapFSKEYtoVK[i]=0;
	}

	AddKeyMapping(FSKEY_SPACE,               VK_SPACE);
	AddKeyMapping(FSKEY_0,                   '0');
	AddKeyMapping(FSKEY_1,                   '1');
	AddKeyMapping(FSKEY_2,                   '2');
	AddKeyMapping(FSKEY_3,                   '3');
	AddKeyMapping(FSKEY_4,                   '4');
	AddKeyMapping(FSKEY_5,                   '5');
	AddKeyMapping(FSKEY_6,                   '6');
	AddKeyMapping(FSKEY_7,                   '7');
	AddKeyMapping(FSKEY_8,                   '8');
	AddKeyMapping(FSKEY_9,                   '9');
	AddKeyMapping(FSKEY_A,                   'A');
	AddKeyMapping(FSKEY_B,                   'B');
	AddKeyMapping(FSKEY_C,                   'C');
	AddKeyMapping(FSKEY_D,                   'D');
	AddKeyMapping(FSKEY_E,                   'E');
	AddKeyMapping(FSKEY_F,                   'F');
	AddKeyMapping(FSKEY_G,                   'G');
	AddKeyMapping(FSKEY_H,                   'H');
	AddKeyMapping(FSKEY_I,                   'I');
	AddKeyMapping(FSKEY_J,                   'J');
	AddKeyMapping(FSKEY_K,                   'K');
	AddKeyMapping(FSKEY_L,                   'L');
	AddKeyMapping(FSKEY_M,                   'M');
	AddKeyMapping(FSKEY_N,                   'N');
	AddKeyMapping(FSKEY_O,                   'O');
	AddKeyMapping(FSKEY_P,                   'P');
	AddKeyMapping(FSKEY_Q,                   'Q');
	AddKeyMapping(FSKEY_R,                   'R');
	AddKeyMapping(FSKEY_S,                   'S');
	AddKeyMapping(FSKEY_T,                   'T');
	AddKeyMapping(FSKEY_U,                   'U');
	AddKeyMapping(FSKEY_V,                   'V');
	AddKeyMapping(FSKEY_W,                   'W');
	AddKeyMapping(FSKEY_X,                   'X');
	AddKeyMapping(FSKEY_Y,                   'Y');
	AddKeyMapping(FSKEY_Z,                   'Z');
	AddKeyMapping(FSKEY_ESC,                 VK_ESCAPE);
	AddKeyMapping(FSKEY_F1,                  VK_F1);
	AddKeyMapping(FSKEY_F2,                  VK_F2);
	AddKeyMapping(FSKEY_F3,                  VK_F3);
	AddKeyMapping(FSKEY_F4,                  VK_F4);
	AddKeyMapping(FSKEY_F5,                  VK_F5);
	AddKeyMapping(FSKEY_F6,                  VK_F6);
	AddKeyMapping(FSKEY_F7,                  VK_F7);
	AddKeyMapping(FSKEY_F8,                  VK_F8);
	AddKeyMapping(FSKEY_F9,                  VK_F9);
	AddKeyMapping(FSKEY_F10,                 VK_F10);
	AddKeyMapping(FSKEY_F11,                 VK_F11);
	AddKeyMapping(FSKEY_F12,                 VK_F12);
	AddKeyMapping(FSKEY_PRINTSCRN,           0 /* Unassignable */);
	AddKeyMapping(FSKEY_SCROLLLOCK,          VK_SCROLL);
	AddKeyMapping(FSKEY_PAUSEBREAK,          VK_PAUSE);
	AddKeyMapping(FSKEY_TILDA,               VK_OEM_3);
	AddKeyMapping(FSKEY_MINUS,               VK_OEM_MINUS);
	AddKeyMapping(FSKEY_PLUS,                VK_OEM_PLUS);
	AddKeyMapping(FSKEY_BS,                  VK_BACK);
	AddKeyMapping(FSKEY_TAB,                 VK_TAB);
	AddKeyMapping(FSKEY_LBRACKET,            VK_OEM_4);
	AddKeyMapping(FSKEY_RBRACKET,            VK_OEM_6);
	AddKeyMapping(FSKEY_BACKSLASH,           VK_OEM_5);
	AddKeyMapping(FSKEY_CAPSLOCK,            VK_CAPITAL);
	AddKeyMapping(FSKEY_SEMICOLON,           VK_OEM_1);   // 186(0x00BA)
	AddKeyMapping(FSKEY_SINGLEQUOTE,         VK_OEM_7);
	AddKeyMapping(FSKEY_ENTER,               VK_RETURN);
	AddKeyMapping(FSKEY_SHIFT,               VK_SHIFT);
	AddKeyMapping(FSKEY_COMMA,               VK_OEM_COMMA);
	AddKeyMapping(FSKEY_DOT,                 VK_OEM_PERIOD);
	AddKeyMapping(FSKEY_SLASH,               VK_OEM_2);
	AddKeyMapping(FSKEY_CTRL,                VK_CONTROL);
	AddKeyMapping(FSKEY_ALT,                 VK_MENU);
	AddKeyMapping(FSKEY_INS,                 VK_INSERT);
	AddKeyMapping(FSKEY_DEL,                 VK_DELETE);
	AddKeyMapping(FSKEY_HOME,                VK_HOME);
	AddKeyMapping(FSKEY_END,                 VK_END);
	AddKeyMapping(FSKEY_PAGEUP,              VK_PRIOR);
	AddKeyMapping(FSKEY_PAGEDOWN,            VK_NEXT);
	AddKeyMapping(FSKEY_UP,                  VK_UP);
	AddKeyMapping(FSKEY_DOWN,                VK_DOWN);
	AddKeyMapping(FSKEY_LEFT,                VK_LEFT);
	AddKeyMapping(FSKEY_RIGHT,               VK_RIGHT);
	AddKeyMapping(FSKEY_NUMLOCK,             VK_NUMLOCK);
	AddKeyMapping(FSKEY_TEN0,                VK_NUMPAD0);
	AddKeyMapping(FSKEY_TEN1,                VK_NUMPAD1);
	AddKeyMapping(FSKEY_TEN2,                VK_NUMPAD2);
	AddKeyMapping(FSKEY_TEN3,                VK_NUMPAD3);
	AddKeyMapping(FSKEY_TEN4,                VK_NUMPAD4);
	AddKeyMapping(FSKEY_TEN5,                VK_NUMPAD5);
	AddKeyMapping(FSKEY_TEN6,                VK_NUMPAD6);
	AddKeyMapping(FSKEY_TEN7,                VK_NUMPAD7);
	AddKeyMapping(FSKEY_TEN8,                VK_NUMPAD8);
	AddKeyMapping(FSKEY_TEN9,                VK_NUMPAD9);
	AddKeyMapping(FSKEY_TENDOT,              VK_DECIMAL);
	AddKeyMapping(FSKEY_TENSLASH,            VK_DIVIDE);
	AddKeyMapping(FSKEY_TENSTAR,             VK_MULTIPLY);
	AddKeyMapping(FSKEY_TENMINUS,            VK_SUBTRACT);
	AddKeyMapping(FSKEY_TENPLUS,             VK_ADD);
	AddKeyMapping(FSKEY_TENENTER,            0 /* Unassignable */);
	AddKeyMapping(FSKEY_CONTEXT,             VK_APPS);

	AddKeyMapping(FSKEY_CONVERT,             VK_CONVERT);
	AddKeyMapping(FSKEY_NONCONVERT,          VK_NONCONVERT);
	AddKeyMapping(FSKEY_KANA,                VK_KANA);       // Japanese JIS Keyboard Only
	AddKeyMapping(FSKEY_ZENKAKU,             VK_KANJI);      // Japanese JIS Keyboard ONly
	AddKeyMapping(FSKEY_RO,                  VK_OEM_102);    // Japanese JIS Keyboard Only (Immediate left of right-shift)

	AddKeyMapping(FSKEY_LEFT_CTRL,           VK_LCONTROL);
	AddKeyMapping(FSKEY_RIGHT_CTRL,          VK_RCONTROL);
	AddKeyMapping(FSKEY_LEFT_SHIFT,          VK_LSHIFT);
	AddKeyMapping(FSKEY_RIGHT_SHIFT,         VK_RSHIFT);
	AddKeyMapping(FSKEY_LEFT_ALT,            VK_LMENU);
	AddKeyMapping(FSKEY_RIGHT_ALT,           VK_RMENU);
}

void FsWin32KeyMapper::AddKeyMapping(int fskey,int vk)
{
	if(fskey<0 || FSKEY_NUM_KEYCODE<=fskey)
	{
		printf("FSKEY is out of range\n");
		exit(1);
	}
	if(vk<0 || FS_NUM_VK<=vk)
	{
		printf("VK is out of range\n");
		exit(1);
	}

	mapVKtoFSKEY[vk]=fskey;
	mapFSKEYtoVK[fskey]=vk;
}

FsWin32KeyMapper::~FsWin32KeyMapper()
{
	delete [] mapFSKEYtoVK;
	delete [] mapVKtoFSKEY;
}

int FsWin32KeyMapper::VkToFsKey(int vk)
{
	if(0<=vk && vk<FS_NUM_VK)
	{
		return mapVKtoFSKEY[vk];
	}
	return FSKEY_NULL;
}

int FsWin32KeyMapper::FsKeyToVk(int fsKey)
{
	if(0<=fsKey && fsKey<FSKEY_NUM_KEYCODE)
	{
		return mapFSKEYtoVK[fsKey];
	}
	return 0;
}
