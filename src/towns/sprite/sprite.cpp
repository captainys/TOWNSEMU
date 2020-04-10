/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include "towns.h"
#include "townsdef.h"
#include "cpputil.h"
#include "sprite.h"



void TownsSprite::State::PowerOn(void)
{
	addressLatch=0;
	for(auto &r : reg)
	{
		r=0;
	}
	spriteBusy=false;
}
void TownsSprite::State::Reset(void)
{
}

TownsSprite::TownsSprite(class FMTowns *townsPtr) : Device(townsPtr)
{
	this->townsPtr=townsPtr;
}

/* virtual */ void TownsSprite::PowerOn(void)
{
	state.PowerOn();
}
/* virtual */ void TownsSprite::Reset(void)
{
	state.Reset();
}
/* virtual */ void TownsSprite::IOWriteByte(unsigned int ioport,unsigned int data)
{
	switch(ioport)
	{
	// case TOWNSIO_DPMD_SPRITEBUSY_SPRITEPAGE Not assigned to TownsSprite.

	case TOWNSIO_SPRITE_ADDRESS://           0x450, // [2] pp.128
		state.addressLatch=data&7;
		break;
	case TOWNSIO_SPRITE_DATA://              0x452, // [2] pp.128
		state.reg[state.addressLatch]=data;
		break;
	}
}
/* virtual */ unsigned int TownsSprite::IOReadByte(unsigned int ioport)
{
	unsigned char data=0xff;
	switch(ioport)
	{
	// case TOWNSIO_DPMD_SPRITEBUSY_SPRITEPAGE Not assigned to TownsSprite.

	case TOWNSIO_SPRITE_ADDRESS://           0x450, // [2] pp.128
		data=state.addressLatch;
		break;
	case TOWNSIO_SPRITE_DATA://              0x452, // [2] pp.128
		if(state.addressLatch!=REG_DISPLAY_PAGE)
		{
			data=state.reg[state.addressLatch];
		}
		else
		{
			data=(state.reg[state.addressLatch]>>3); // [2] pp.130.  DP1 write -> Bit7, DP1 read -> Bit 4
		}
		break;
	}
	return data;
}

void TownsSprite::Render(unsigned char VRAMIn[],const unsigned char spriteRAM[]) const
{
	unsigned char *VRAMTop=VRAMIn+SPRITE_HALF_VRAM_SIZE*DisplayPage();
	auto xOffset=HOffset(),yOffset=VOffset();
	for(unsigned int spriteIndex=0; spriteIndex<NumSpritesToDraw(); ++spriteIndex)
	{
		auto indexPtr=spriteRAM+SPRITERAM_INDEX_OFFSET+(spriteIndex<<3);

		const unsigned short indexInfo[4]=  // I hope the optimizer recognizes it.
		{
			(unsigned short)(indexPtr[0]|(indexPtr[1]<<8)),
			(unsigned short)(indexPtr[2]|(indexPtr[3]<<8)),
			(unsigned short)(indexPtr[4]|(indexPtr[5]<<8)),
			(unsigned short)(indexPtr[6]|(indexPtr[7]<<8)),
		};

		unsigned int dstX=indexInfo[0];
		unsigned int dstY=indexInfo[1];
		const unsigned int attrib=indexInfo[2];
		const unsigned int paletteInfo=indexInfo[3];

		if(0!=(paletteInfo&PALETTE_DISP))  // Active LOW
		{
			continue;
		}

		if(0!=(attrib&ATTR_OFFS))
		{
			dstX+=xOffset;
			dstY+=yOffset;
		}
		unsigned int patternIndex=attrib&ATTR_PAT_MASK;

		const unsigned char ROT=((attrib&ATTR_ROT_MASK)>>ATTR_ROT_SHIFT);
		const int xStep=(0!=(attrib&ATTR_SUX) ? 2 : 1);
		const int yStep=(0!=(attrib&ATTR_SUY) ? 2 : 1);

		if(0!=(paletteInfo&PALETTE_CTEN))
		{
			// 16-color paletted sprite
			const unsigned int paletteIndex=paletteInfo&PALETTE_INDEX_MASK;
			auto pattenPtr=spriteRAM+(patternIndex<<7);
			auto palettePtr=spriteRAM+(paletteIndex<<5);
			auto srcPtr=spriteRAM+(patternIndex<<7);
			if(dstX<256-SPRITE_DIMENSION && dstY<256-SPRITE_DIMENSION)
			{
				auto dstPtr=VRAMTop+SPRITE_VRAM_BYTES_PER_LINE*dstY;
				for(unsigned int ptnY=0; ptnY<SPRITE_DIMENSION; ptnY+=yStep)
				{
					auto nextDstPtr=dstPtr+SPRITE_VRAM_BYTES_PER_LINE;
					for(unsigned int ptnX=0; ptnX<SPRITE_DIMENSION; ptnX+=(xStep<<1))
					{
						unsigned int xTfm,yTfm;
						Transform(xTfm,yTfm,ptnX,ptnY,ROT);

						auto src=srcPtr+SPRITE_PTN16_BYTES_PER_LINE*ptnY+(ptnX>>1);
						unsigned char pix4bit=(src[0]>>4);
						const unsigned char *col=palettePtr+(pix4bit<<1);
						if(0==(col[1]&0x80))
						{
							dstPtr[0]=col[0];
							dstPtr[1]=col[1];
						}
						dstPtr+=2;

						pix4bit=(src[0]&0x0F);
						col=palettePtr+(pix4bit<<1);
						if(0==(col[1]&0x80))
						{
							dstPtr[0]=col[0];
							dstPtr[1]=col[1];
						}
						dstPtr+=2;
					}
					dstPtr=nextDstPtr;
				}
			}
			else // Clipping not supported yet.
			{
			}
		}
		else
		{
			// 32768-color sprite
			patternIndex&=(~3);
			auto srcPtr=spriteRAM+(patternIndex<<7);
			if(dstX<256-SPRITE_DIMENSION && dstY<256-SPRITE_DIMENSION)
			{
				auto dstPtr=VRAMTop+SPRITE_VRAM_BYTES_PER_LINE*dstY;
				for(unsigned int ptnY=0; ptnY<SPRITE_DIMENSION; ptnY+=yStep)
				{
					auto nextDstPtr=dstPtr+SPRITE_VRAM_BYTES_PER_LINE;
					for(unsigned int ptnX=0; ptnX<SPRITE_DIMENSION; ptnX+=xStep)
					{
						unsigned int xTfm,yTfm;
						Transform(xTfm,yTfm,ptnX,ptnY,ROT);

						auto src=srcPtr+SPRITE_PTN32K_BYTES_PER_LINE*ptnY+(ptnX<<1);
						if(0==(src[1]&0x80))
						{
							dstPtr[0]=src[0];
							dstPtr[1]=src[1];
						}
						dstPtr+=2;
					}
					dstPtr=nextDstPtr;
				}
			}
			else // Clipping not supported yet.
			{
			}
		}
	}
}
