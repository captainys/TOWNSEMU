/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include "egb.h"
#include "fgs.h"
#include "snd.h"
#include "io.h"

static char EGB_work[EgbWorkSize];
static char FGS_work[FGSWorkSize];

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


#define IO_HIGHRES_ADDR_LATCH 0x472
#define IO_HIGHRES_DATA 0x474


void WriteWordToHighResCRTC(unsigned int reg,unsigned int data)
{
	_outw(IO_HIGHRES_ADDR_LATCH,reg);
	_outw(IO_HIGHRES_DATA,data);
}

void TestFullColor(void)
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

	EGB_init(EGB_work,EgbWorkSize);

	EGB_resolution(EGB_work,0,22);

	EGB_writePage(EGB_work,0);
	EGB_color(EGB_work,EGB_BACKGROUND_COLOR,0);
	EGB_clearScreen(EGB_work);
	EGB_displayStart(EGB_work,2,1,1);
	EGB_displayStart(EGB_work,3,640,480);

	EGB_writeMode(EGB_work,EGB_PSET);

	EGB_color(EGB_work,EGB_FOREGROUND_COLOR,0xFF0000);
	EGB_connect(EGB_work,seihou);

	EGB_color(EGB_work,EGB_FOREGROUND_COLOR,0x0000FF);
	EGB_connect(EGB_work,hishigata);

	EGB_color(EGB_work,EGB_FOREGROUND_COLOR,0x00FF00);
	EGB_unConnect(EGB_work,taikaku);

	EGB_color(EGB_work,EGB_FOREGROUND_COLOR,0xFFFFFF);
	EGB_writeMode(EGB_work,EGB_OPAGUE);

	//	FGS_init2(FGS_work,FGSWorkSize);
	//
	//	int res0,res1;
	//	FGS_getResolution(&res0,&res1);


	unsigned int reg011A=0x06;
	WriteWordToHighResCRTC(0x11A,reg011A);

	unsigned int priorityPage=0;
	unsigned int displayPage=3;
	for(;;)
	{
		int pad;
		SND_joy_in_2(0,&pad);
		if(0xC0!=(pad&0xC0))
		{
			break;
		}

		if(0==(pad&0x10)) // A button priority page
		{
			reg011A^=0x20;
			WriteWordToHighResCRTC(0x11A,reg011A);
		}
		if(0==(pad&0x20)) // A button priority page
		{
			reg011A^=0x02;
			WriteWordToHighResCRTC(0x11A,reg011A);
		}

		struct param
		{
			short x,y,len;
			char str[256];
		} p;
		p.x=32;
		p.y=32;
		sprintf(p.str,"%04x",reg011A);
		p.len=strlen(p.str);
		EGB_sjisString(EGB_work,&p);

		while(255!=(pad&255))
		{
			SND_joy_in_2(0,&pad);
		}
	}
}


int main(void)
{
	TestFullColor();
	return 0;
}
