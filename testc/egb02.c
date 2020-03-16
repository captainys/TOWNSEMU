/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include "egb.h"
#include "io.h"

static char EGB_work[EgbWorkSize];

// Test 16-color mode

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

void Test320x240_32K(void)
{
	static short seihou[]=
	{
		5,
		 20, 20,
		300, 20,
		300,220,
		 20,220,
		 20, 20
	};
	static short hishigata[]=
	{
		5,
		 160,  0,
		 320,120,
		 160,240,
		   0,120,
		 160,  0,
	};
	static short taikaku[]=
	{
		4,
		   0,  0,
		 320,240,
		 320,  0,
		   0,240,
	};

	EGB_resolution(EGB_work,0,10);
	EGB_resolution(EGB_work,1,10);

	EGB_writePage(EGB_work,0);
	EGB_color(EGB_work,EGB_BACKGROUND_COLOR,0x8000);
	EGB_clearScreen(EGB_work);
	EGB_displayStart(EGB_work,2,2,2);
	EGB_displayStart(EGB_work,3,320,240);

	EGB_writePage(EGB_work,1);
	EGB_color(EGB_work,EGB_BACKGROUND_COLOR,0x8000);
	EGB_clearScreen(EGB_work);
	EGB_displayStart(EGB_work,2,2,2);
	EGB_displayStart(EGB_work,3,320,240);


	EGB_writePage(EGB_work,1);

	EGB_color(EGB_work,EGB_FOREGROUND_COLOR,0x7FFF);
	EGB_writeMode(EGB_work,EGB_PSET);

	EGB_connect(EGB_work,seihou);
	EGB_connect(EGB_work,hishigata);

	EGB_writePage(EGB_work,0);

	EGB_color(EGB_work,EGB_FOREGROUND_COLOR,0x001F);
	EGB_writeMode(EGB_work,EGB_PSET);

	EGB_unConnect(EGB_work,taikaku);


	{
		int y;
		EGB_writePage(EGB_work,1);
		for(y=0; y<=256; y+=8)
		{
			EGB_displayStart(EGB_work,1,0,y);
		}

		EGB_writePage(EGB_work,0);
		for(y=0; y<=256; y+=8)
		{
			EGB_displayStart(EGB_work,1,0,y);
		}
	}
	{
		int x;
		EGB_writePage(EGB_work,1);
		for(x=0; x<=512; x+=16)
		{
			EGB_displayStart(EGB_work,1,x,0);
		}

		EGB_writePage(EGB_work,0);
		for(x=0; x<=512; x+=16)
		{
			EGB_displayStart(EGB_work,1,x,0);
		}
	}
}
void Test640x480_256(void)
{
	static short seihou[]=
	{
		5,
		 20, 20,
		620, 20,
		620,460,
		 20,460,
		 20, 20
	};
	static short hishigata[]=
	{
		5,
		 320,  0,
		 640,240,
		 320,480,
		   0,240,
		 320,  0,
	};
	static short taikaku[]=
	{
		4,
		   0,  0,
		 640,480,
		 640,  0,
		   0,480,
	};

	EGB_resolution(EGB_work,0,12);

	EGB_writePage(EGB_work,0);

	EGB_color(EGB_work,EGB_BACKGROUND_COLOR,0);
	EGB_clearScreen(EGB_work);
	EGB_displayStart(EGB_work,2,1,1);
	EGB_displayStart(EGB_work,3,640,480);


	EGB_color(EGB_work,EGB_FOREGROUND_COLOR,255);
	EGB_writeMode(EGB_work,EGB_PSET);

	EGB_connect(EGB_work,seihou);
	EGB_connect(EGB_work,hishigata);


	EGB_color(EGB_work,EGB_FOREGROUND_COLOR,0x001F);
	EGB_writeMode(EGB_work,EGB_PSET);

	EGB_unConnect(EGB_work,taikaku);

	{
		int y;
		for(y=0; y<=512; y+=8)
		{
			EGB_displayStart(EGB_work,1,0,y);
		}
	}
	{
		int x;
		for(x=0; x<=1024; x+=16)
		{
			EGB_displayStart(EGB_work,1,x,0);
		}
	}
}
void Test640x480_16(void)
{
	static short seihou[]=
	{
		5,
		 20, 20,
		620, 20,
		620,460,
		 20,460,
		 20, 20
	};
	static short hishigata[]=
	{
		5,
		 320,  0,
		 640,240,
		 320,480,
		   0,240,
		 320,  0,
	};
	static short taikaku[]=
	{
		4,
		   0,  0,
		 640,480,
		 640,  0,
		   0,480,
	};

	EGB_resolution(EGB_work,0,3);
	EGB_resolution(EGB_work,1,3);

	EGB_writePage(EGB_work,0);
	EGB_color(EGB_work,EGB_BACKGROUND_COLOR,0);
	EGB_clearScreen(EGB_work);
	EGB_displayStart(EGB_work,2,1,1);
	EGB_displayStart(EGB_work,3,640,480);


	EGB_writePage(EGB_work,1);
	EGB_color(EGB_work,EGB_BACKGROUND_COLOR,0);
	EGB_clearScreen(EGB_work);
	EGB_displayStart(EGB_work,2,2,2);
	EGB_displayStart(EGB_work,3,640,480);


	EGB_writePage(EGB_work,0);
	EGB_color(EGB_work,EGB_FOREGROUND_COLOR,15);
	EGB_writeMode(EGB_work,EGB_PSET);

	EGB_connect(EGB_work,seihou);
	EGB_connect(EGB_work,hishigata);

	EGB_writePage(EGB_work,1);
	EGB_color(EGB_work,EGB_FOREGROUND_COLOR,9);
	EGB_writeMode(EGB_work,EGB_PSET);

	EGB_unConnect(EGB_work,taikaku);


	{
		int y;
		for(y=0; y<=512; y+=8)
		{
			EGB_displayStart(EGB_work,1,0,y);
		}
	}
	{
		int x;
		for(x=0; x<=1024; x+=16)
		{
			EGB_displayStart(EGB_work,1,x,0);
		}
	}
}
int main(void)
{
	EGB_init(EGB_work,EgbWorkSize);
	Test640x480_16();
	Test640x480_256();
	Test320x240_32K();
	return 0;
}
