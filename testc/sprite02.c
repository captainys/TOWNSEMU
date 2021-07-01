/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include "egb.h"
#include "spr.h"
#include "snd.h"
#include "conio.h"

static char EGB_work[EgbWorkSize];

void SetScreenMode(int m1,int m2);


#define CLI _inline(0xfa)
#define STI _inline(0xfb)
// Can be _ASM("cli");


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
#define EGB_OPAQUE 9
#define EGB_MASKSET 13
#define EGB_MASKRESET 14
#define EGB_MASKNOT 15

#define SAMPLES_PER_ROW 640
#define NUM_ROWS 4
#define MAX_SAMPLES (SAMPLES_PER_ROW*NUM_ROWS)
int SPD0[MAX_SAMPLES];
int VSYNC[MAX_SAMPLES];
short lineBuf[MAX_SAMPLES*4+1];

//extern int _inp(unsigned int);
//extern unsigned _inpw(unsigned int);
//extern int _outp(unsigned int ,int );
//extern unsigned _outpw(unsigned int ,unsigned int );


// Result (sampled by MX)
// VSYNC duration 18us
// Sprite busy starts at the beginning of VSYNC
// Up to 290 sprites can be transferred in one CRT refresh cycle.
// If 289 sprites, sprite is almost always busy.


int main(void)
{
	int nSprite=10;

	EGB_init(EGB_work,EgbWorkSize);

	EGB_resolution(EGB_work,0,3);
	EGB_resolution(EGB_work,1,5);

	EGB_writePage(EGB_work,1);
	EGB_clearScreen(EGB_work);
	EGB_color(EGB_work,EGB_FOREGROUND_COLOR,32767);
	EGB_color(EGB_work,EGB_BACKGROUND_COLOR,0);

	EGB_writePage(EGB_work,0);
	EGB_clearScreen(EGB_work);
	EGB_color(EGB_work,EGB_FOREGROUND_COLOR,15);
	EGB_color(EGB_work,EGB_BACKGROUND_COLOR,0);

	EGB_writeMode(EGB_work,EGB_PSET);

	SPR_init();
	SPR_display(1,nSprite);

	int timeInterval=26; // 26us=0.26ms (Roughly 640 samples per CRTC refresh)
	int prevPad=0xFF;
	for(;;)
	{
		int pad;
		SND_joy_in_2(0,&pad);
		if(0xC0!=(pad&0xc0)) // Start/Select
		{
			break;
		}

		if(0==(pad&0x10))
		{
			nSprite++;
			SPR_display(1,nSprite);
		}
		if(0==(pad&0x20) && 10<nSprite)
		{
			nSprite--;
			SPR_display(1,nSprite);
		}
		if(0==(pad&0x01))
		{
			nSprite+=10;
			SPR_display(1,nSprite);
		}
		if(0==(pad&0x02))
		{
			nSprite-=10;
			if(nSprite<10)
			{
				nSprite=10;
			}
			SPR_display(1,nSprite);
		}
		if(0==(pad&0x04))
		{
			timeInterval++;
		}
		if(0==(pad&0x08))
		{
			if(1<timeInterval)
			{
				timeInterval--;
			}
		}
		

		prevPad=pad;

		CLI;
		// Wait for Sprite Ready
		while(_inp(0x44C)&0x02)
		{
		}
		// Wait for VSYNC clear
		while(0!=(_inp(0xFDA0)&0x01))
		{
		}
		// Wait for VSYNC start
		while(0==(_inp(0xFDA0)&0x01))
		{
		}

		// Sample
		int nSamples=0;
		int freeRun=_inpw(0x0026);
		int timeBalance=timeInterval;
		while(nSamples<MAX_SAMPLES)
		{
			SPD0[nSamples]=(_inp(0x44C)&0x02)/2;
			VSYNC[nSamples]=_inp(0xFDA0)&0x01;
			++nSamples;

			while(0<timeBalance)
			{
				int now=_inpw(0x0026);
				int deltaT=(now-freeRun)&0xFFFF;
				timeBalance-=deltaT;
				freeRun=now;
			}
			timeBalance+=timeInterval;
		}

		STI;

		for(int row=0; row<NUM_ROWS; ++row)
		{
			lineBuf[0]=SAMPLES_PER_ROW*2;
			char str[256];
			int x,x0,y0,nSPD0;
			nSPD0=0;
			x0=row*SAMPLES_PER_ROW;
			y0=row*120;
			for(x=0; x<SAMPLES_PER_ROW; ++x)
			{
				lineBuf[1+x*4  ]=x;
				lineBuf[1+x*4+1]=y0+50;
				lineBuf[1+x*4+2]=x;
				lineBuf[1+x*4+3]=y0+50-SPD0[x0+x]*50;
				nSPD0+=SPD0[x0+x];
			}
			EGB_color(EGB_work,EGB_FOREGROUND_COLOR,12);
			EGB_unConnect(EGB_work,lineBuf);
			for(x=0; x<SAMPLES_PER_ROW; ++x)
			{
				lineBuf[1+x*4  ]=x;
				lineBuf[1+x*4+1]=y0;
				lineBuf[1+x*4+2]=x;
				lineBuf[1+x*4+3]=y0+50-SPD0[x0+x]*50;
			}
			EGB_color(EGB_work,EGB_FOREGROUND_COLOR,1);
			EGB_unConnect(EGB_work,lineBuf);

			(*(short *)(str+0))=512;
			(*(short *)(str+2))=y0+25;
			(*(short *)(str+4))=6;
			sprintf(str+6,"%3d%3d",nSPD0,timeInterval);
			EGB_color(EGB_work,EGB_FOREGROUND_COLOR,15);
			EGB_sjisString(EGB_work,str);


			int nVSYNCSet=0;
			for(x=0; x<SAMPLES_PER_ROW; ++x)
			{
				lineBuf[1+x*4  ]=x;
				lineBuf[1+x*4+1]=y0+100;
				lineBuf[1+x*4+2]=x;
				lineBuf[1+x*4+3]=y0+100-VSYNC[x0+x]*50;
				nVSYNCSet+=VSYNC[x0+x];
			}
			EGB_color(EGB_work,EGB_FOREGROUND_COLOR,15);
			EGB_unConnect(EGB_work,lineBuf);
			for(x=0; x<SAMPLES_PER_ROW; ++x)
			{
				lineBuf[1+x*4  ]=x;
				lineBuf[1+x*4+1]=y0+50;
				lineBuf[1+x*4+2]=x;
				lineBuf[1+x*4+3]=y0+100-VSYNC[x0+x]*50;
			}
			EGB_color(EGB_work,EGB_FOREGROUND_COLOR,1);
			EGB_unConnect(EGB_work,lineBuf);

			sprintf(str+6,"%3d",nSprite);
			(*(short *)(str+0))=16;
			(*(short *)(str+2))=y0+25;
			(*(short *)(str+4))=3;
			EGB_color(EGB_work,EGB_FOREGROUND_COLOR,15);
			EGB_sjisString(EGB_work,str);

			(*(short *)(str+0))=512;
			(*(short *)(str+2))=y0+75;
			(*(short *)(str+4))=6;
			sprintf(str+6,"%3d%3d",nVSYNCSet,timeInterval);
			EGB_color(EGB_work,EGB_FOREGROUND_COLOR,15);
			EGB_sjisString(EGB_work,str);
		}
	}

	SPR_display(0,0);

	return 0;
}


