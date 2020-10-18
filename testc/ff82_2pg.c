/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include <stdio.h>

#include "egb.h"
#include "snd.h"
#include "io.h"

static char EGB_work[EgbWorkSize];

// Test I/O FF82H

#define EGB_FOREGROUND_COLOR 0
#define EGB_BACKGROUND_COLOR 1
#define EGB_FILL_COLOR 2
#define EGB_TRANSPARENT_COLOR 3

#define EGB_PSET 0
#define EGB_PRESET 1
#define EGB_OR 2
#define EGB_AND 3
#define EGB_XOR 4
#define EGB_NOT 5
#define EGB_MATTE 6
#define EGB_PASTEL 7
#define EGB_OPAGUE 9
#define EGB_MASKSET 13
#define EGB_MASKRESET 14
#define EGB_MASKNOT 15


void WaitPadRelease(void)
{
	for(;;)
	{
		int pad;
		SND_joy_in_2(0,&pad);
		if(0xFF==(pad&0xFF))
		{
			return;
		}
	}
}

void Print(unsigned int byte)
{
	int y;
	for(y=0; y<800; y+=16)
	{
		struct param
		{
			short x,y,len;
			char str[256];
		} p;
		p.x=32;
		p.y=y+15;
		sprintf(p.str,"%02x %d",byte,y);
		p.len=strlen(p.str);
		EGB_sjisString(EGB_work,&p);
	}
}

int main(int ac,char *av[])
{
	if(2!=ac)
	{
		printf("Usage: ff82 EGBScreenMode\n");
		return 1;
	}

	int page=atoi(av[1]);
	if(page<1 || 18<page)
	{
		printf("Unsupported EGB page.\n");
		return 1;
	}

	EGB_init(EGB_work,EgbWorkSize);

	EGB_resolution(EGB_work,0,page);
	EGB_resolution(EGB_work,1,page);

	EGB_displayPage(EGB_work,0,3);

	// EGB_color(EGB_work,EGB_BACKGROUND_COLOR,0);
	// EGB_color(EGB_work,EGB_FOREGROUND_COLOR,15);

	EGB_writePage(EGB_work,1);
	EGB_clearScreen(EGB_work);

	EGB_writePage(EGB_work,0);
	EGB_clearScreen(EGB_work);

	EGB_writeMode(EGB_work,EGB_PSET);

	Print(0);

	unsigned int FMRDisplayPage=0x10;
	unsigned int FMRPlaneMask=0x27;
	for(;;)
	{
		int pad;
		SND_joy_in_2(0,&pad);
		if(0xC0!=(pad&0xc0))
		{
			break;
		}

		if(0x10!=(pad&0x10)) // Button A
		{
			int write=FMRDisplayPage|FMRPlaneMask;
			FMRDisplayPage^=0x10;
			IOWriteByte(0xFF82,write);
			WaitPadRelease();
		}
		if(0x20!=(pad&0x20)) // Button B
		{
			int write=FMRDisplayPage|FMRPlaneMask;
			FMRDisplayPage^=0x03;
			IOWriteByte(0xFF82,write);
			WaitPadRelease();
		}

		if(0x1!=(pad&0x1)) // Down
		{
			int write=0x40|FMRDisplayPage|FMRPlaneMask;
			FMRDisplayPage^=0x10;
			IOWriteByte(0xFF82,write);
			WaitPadRelease();
		}
		if(0x2!=(pad&0x2)) // Up
		{
			int write=0x40|FMRDisplayPage|FMRPlaneMask;
			FMRDisplayPage^=0x03;
			IOWriteByte(0xFF82,write);
			WaitPadRelease();
		}
	}

	return 0;
}
