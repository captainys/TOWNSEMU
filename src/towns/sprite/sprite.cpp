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
#include "physmem.h"



void TownsSprite::State::PowerOn(void)
{
	Reset();
}
void TownsSprite::State::Reset(void)
{
	addressLatch=0;
	for(auto &r : reg)
	{
		r=0;
	}
	spriteBusy=false;
	screenModeAcceptsSprite=false;
	callbackType = CALLBACK_NONE;
	page = 0;
}

TownsSprite::TownsSprite(class FMTownsCommon *townsPtr,TownsPhysicalMemory *physMemPtr) : Device(townsPtr)
{
	this->townsPtr=townsPtr;
	this->physMemPtr=physMemPtr;
}

unsigned int TownsSprite::NumSpritesActuallyDrawn(void) const
{
	auto *spriteRAM=townsPtr->physMem.state.spriteRAM;

	unsigned int nDraw=0;
	auto xOffset=HOffset(),yOffset=VOffset();
	for(unsigned int spriteIndex=FirstSpriteIndex(); spriteIndex<MAX_NUM_SPRITE_INDEX; ++spriteIndex)
	{
		auto indexPtr=spriteRAM+SPRITERAM_INDEX_OFFSET+(spriteIndex<<3);

		const unsigned short indexInfo[4]=  // I hope the optimizer recognizes it.
		{
			(unsigned short)(indexPtr[0]|(indexPtr[1]<<8)),
			(unsigned short)(indexPtr[2]|(indexPtr[3]<<8)),
			(unsigned short)(indexPtr[4]|(indexPtr[5]<<8)),
			(unsigned short)(indexPtr[6]|(indexPtr[7]<<8)),
		};

		unsigned int dstX=indexInfo[0]&511;
		unsigned int dstY=indexInfo[1]&511;
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

		if(dstX<256-SPRITE_DIMENSION && 2<=dstY && dstY<256-SPRITE_DIMENSION)
		{
			++nDraw;
		}
	}
	return nDraw;
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
		switch (state.addressLatch) {
		case REG_CONTROL0:
			state.reg[state.addressLatch] = data;
			break;
		case REG_CONTROL1:
			{
				auto prevSPEN=SPEN();
				auto prevBUSY=state.spriteBusy;

				state.reg[state.addressLatch] = data & 0x83;

				if (data & 0x80 && state.callbackType == CALLBACK_NONE) {
					auto nextVSync = townsPtr->crtc.NextVSYNCRisingEdge(townsPtr->state.townsTime);
					townsPtr->ScheduleDeviceCallBack(*this, nextVSync);
					state.callbackType = CALLBACK_VSYNC;
				}

				// For Shadow of the Beasts >>
				// Shadow of the Beasts turns off SPEN in the middle of sprite busy.
				// If it happens, sprite needs to be immediately rendered, or will never be rendered.
				if(true==prevSPEN && true!=SPEN() && true==prevBUSY)
				{
					Render(physMemPtr->state.VRAM + 0x40000,
						physMemPtr->state.spriteRAM);
				}
				// For Shadow of the Beasts <<
			}
			break;
		case REG_HORIZONTAL_OFFSET0:
		case REG_VERTICAL_OFFSET0:
			state.reg[state.addressLatch] = data;
			break;
		case REG_HORIZONTAL_OFFSET1:
		case REG_VERTICAL_OFFSET1:
			state.reg[state.addressLatch] = data & 0x01;
			break;
		case REG_DISPLAY_PAGE:
			state.reg[state.addressLatch] = data & 0x88; // DP0 saved on MA.
			break;
		case REG_DUMMY:
			state.reg[state.addressLatch] = 0;
			break;
		}
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
	unsigned char *VRAMTop=VRAMIn + SPRITE_HALF_VRAM_SIZE * PAGE();

	// [2] pp.368 (Sprite BIOS AH=00H) tells, the top 2-lines of the VRAM page are VRAM-clear data.
	//     So, apparently it is possible to clear the sprite page with non-0x8000 values.
	for(unsigned int offset=SPRITE_VRAM_BYTES_PER_LINE*2; offset<0x20000; offset+=2)
	{
		VRAMTop[offset  ]=VRAMTop[ offset   &(SPRITE_VRAM_BYTES_PER_LINE*2-1)];
		VRAMTop[offset+1]=VRAMTop[(offset+1)&(SPRITE_VRAM_BYTES_PER_LINE*2-1)];
	}

	auto xOffset=HOffset(),yOffset=VOffset();
	for(unsigned int spriteIndex=state.firstSpriteIndexCapture; spriteIndex<MAX_NUM_SPRITE_INDEX; ++spriteIndex)
	{
		auto indexPtr=spriteRAM+SPRITERAM_INDEX_OFFSET+(spriteIndex<<3);

		const unsigned short indexInfo[4]=  // I hope the optimizer recognizes it.
		{
			(unsigned short)(indexPtr[0]|(indexPtr[1]<<8)),
			(unsigned short)(indexPtr[2]|(indexPtr[3]<<8)),
			(unsigned short)(indexPtr[4]|(indexPtr[5]<<8)),
			(unsigned short)(indexPtr[6]|(indexPtr[7]<<8)),
		};

		unsigned int dstX=indexInfo[0]&511;
		unsigned int dstY=indexInfo[1]&511;
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

		const unsigned char rot = ((attrib & ATTR_ROT_MASK) >> ATTR_ROT_SHIFT);
		const bool sux = (0 != (attrib & ATTR_SUX));
		const bool suy = (0 != (attrib & ATTR_SUY));

		uint8_t spys = (paletteInfo & PALETTE_SPYS) ? 0x80 : 0;

		// pattern coordinate to drawing coordiante
		auto transform = [rot, sux, suy](int& dx, int& dy, int px, int py)
		{
			switch (rot)
			{
			default:
			case 0:
				// No transformation
				dx = px;
				dy = py;
				break;
			case 1:
				dx = px;
				dy = 15 - py;
				break;
			case 2:
				dx = 15 - px;
				dy = py;
				break;
			case 3:
				dx = 15 - px;
				dy = 15 - py;
				break;
			case 4:
				dx = py;
				dy = px;
				break;
			case 5:
				dx = py;
				dy = 15 - px;
				break;
			case 6:
				dx = 15 - py;
				dy = px;
				break;
			case 7:
				dx = 15 - py;
				dy = 15 - px;
				break;
			}
			if (sux) dx >>= 1;
			if (suy) dy >>= 1;
		};

		if (0 != (paletteInfo & PALETTE_CTEN))
		{
			// 16-color paletted sprite
			const unsigned int paletteIndex = paletteInfo & PALETTE_INDEX_MASK;
			auto palettePtr = spriteRAM + (paletteIndex << 5);
			auto srcPtr = spriteRAM + (patternIndex << 7);

			for (int patY = 0; patY < SPRITE_DIMENSION; ++patY) {
				for (int patX = 0; patX < SPRITE_DIMENSION; ++patX) {
					int dx, dy;
					transform(dx, dy, patX, patY);

					unsigned int sx = (dstX + dx) & 0x1ff;
					unsigned int sy = (dstY + dy) & 0x1ff;

					if (sx < 256 && 2 <= sy && sy < 256) {
						auto src = srcPtr + SPRITE_PTN16_BYTES_PER_LINE * patY + (patX >> 1);
						auto dst = VRAMTop + SPRITE_VRAM_BYTES_PER_LINE * sy + 2 * sx;

						auto pix4bit = (src[0] >> ((patX & 1) ? 4 : 0)) & 0x0f;
						if (pix4bit) {  // [2] pp.371 Sprite BIOS.  4bit all zero means through.
							auto col = palettePtr + (pix4bit << 1);
							dst[0] = col[0];
							dst[1] = (col[1] & 0x7f) | spys;
						}
					}
				}
			}
		}
		else
		{
			// 32768-color sprite
			patternIndex &= (~3);
			auto srcPtr = spriteRAM + (patternIndex << 7);

			for (int patY = 0; patY < SPRITE_DIMENSION; ++patY) {
				for (int patX = 0; patX < SPRITE_DIMENSION; ++patX) {
					int dx, dy;
					transform(dx, dy, patX, patY);

					unsigned int sx = (dstX + dx) & 0x1ff;
					unsigned int sy = (dstY + dy) & 0x1ff;

					if (sx < 256 && 2 <= sy && sy < 256) {
						auto src = srcPtr + SPRITE_PTN32K_BYTES_PER_LINE * patY + 2 * patX;
						auto dst = VRAMTop + SPRITE_VRAM_BYTES_PER_LINE * sy + 2 * sx;

						if ((src[1] & 0x80) == 0) {
							dst[0] = src[0];
							dst[1] = src[1] | spys;
						}
					}
				}
			}
		}
	}
}

void TownsSprite::RunScheduledTask(unsigned long long int townsTime)
{
	if (state.callbackType == CALLBACK_VSYNC) {
		if (SPEN()) {
			state.page = 1 - state.page;
			state.spriteBusy = true;
			state.firstSpriteIndexCapture=FirstSpriteIndex();
			// To correctly render Samurai Spirits.  See sprite.h

			auto finishTime = townsTime + SPRITE_SCREEN_CLEAR_TIME + (uint64_t)state.transferTime * NumSpritesToDraw();

			townsPtr->ScheduleDeviceCallBack(*this, finishTime);
			state.callbackType = CALLBACK_FINISH;
		} else {
			state.callbackType = CALLBACK_NONE;
		}
	} else if (state.callbackType == CALLBACK_FINISH) {
		state.spriteBusy = false;

		if (SPEN()) {
			state.firstSpriteIndexCapture=FirstSpriteIndex();
			// Re-capture to prevent sprite-flicker in Death Blade.
			// Pre-captured firstSpriteIndex is needed only if SPEN is cleared in the middle of
			// sprite-busy cycle.

			Render(physMemPtr->state.VRAM + 0x40000,
				physMemPtr->state.spriteRAM);

			auto nextVSync = townsPtr->crtc.NextVSYNCRisingEdge(townsTime);
			townsPtr->ScheduleDeviceCallBack(*this, nextVSync);
			state.callbackType = CALLBACK_VSYNC;
		} else {
			state.callbackType = CALLBACK_NONE;
		}
	}
}

std::vector <std::string> TownsSprite::GetStatusText(const unsigned char spriteRAM[]) const
{
	std::vector <std::string> text;

	for(unsigned int spriteIndex=FirstSpriteIndex(); spriteIndex<MAX_NUM_SPRITE_INDEX; ++spriteIndex)
	{
		auto oneSprite=GetStatusTextOneSprite(spriteRAM,spriteIndex);
		text.insert(text.end(),oneSprite.begin(),oneSprite.end());
	}


	text.push_back("");
	text.back()="BUSY:";
	text.back()+=(true==state.spriteBusy ? "1" : "0");
	text.back()+=" SPEN:";
	text.back()+=(0!=(state.reg[REG_CONTROL1]&0x80) ? "1" : "0");
	text.back()+=" Screen Mode Accepts Sprite:";
	text.back()+=(true==state.screenModeAcceptsSprite ? "1" : "0");
	text.back()+=" #DRAW:";
	text.back()+=cpputil::Itoa(NumSpritesToDraw());
	text.back()+=" 1stINDEX:";
	text.back()+=cpputil::Itoa(FirstSpriteIndex());

	text.push_back("");
	text.back()="HOFFSET:";
	text.back()+=cpputil::Itoa(HOffset());
	text.back()+=" VOFFSET:";
	text.back()+=cpputil::Itoa(VOffset());
	text.back()+=" DP1:";
	text.back()+=cpputil::Itoa(DP1());

	text.push_back("");
	text.back()="#ActuallyDrawn:";
	text.back()+=cpputil::Itoa(NumSpritesActuallyDrawn());

	text.push_back("");
	text.back()="Transfer Time:";
	text.back()+=cpputil::Itoa(state.transferTime);

	return text;
}

std::vector <std::string> TownsSprite::GetStatusTextOneSprite(const unsigned char spriteRAM[],int spriteIndex) const
{
	std::vector <std::string> text;

	text.push_back("");
	text.back()+="#";
	text.back()+=cpputil::Itoa(spriteIndex);
	while(text.back().size()<8)
	{
		text.back().push_back(' ');
	}

	auto indexPtr=spriteRAM+SPRITERAM_INDEX_OFFSET+(spriteIndex<<3);
	unsigned int physAddr=TOWNSADDR_SPRITERAM_BASE+SPRITERAM_INDEX_OFFSET+(spriteIndex<<3);

	const unsigned short indexInfo[4]=  // I hope the optimizer recognizes it.
	{
		(unsigned short)(indexPtr[0]|(indexPtr[1]<<8)),
		(unsigned short)(indexPtr[2]|(indexPtr[3]<<8)),
		(unsigned short)(indexPtr[4]|(indexPtr[5]<<8)),
		(unsigned short)(indexPtr[6]|(indexPtr[7]<<8)),
	};

	text.back()+="(";
	text.back()+=cpputil::Itoa(indexInfo[0]&511); // X
	text.back()+=",";
	text.back()+=cpputil::Itoa(indexInfo[1]&511); // Y
	text.back()+=")";
	while(text.back().size()<20)
	{
		text.back().push_back(' ');
	}

	text.back()+="PTN:";
	text.back()+=cpputil::Itoa(indexInfo[2]&1023);
	text.back()+=" ROT:";
	text.back()+=cpputil::Itoa((indexInfo[2]>>12)&7);
	text.back()+=" OFS:";
	text.back()+=cpputil::Itoa((indexInfo[2]>>15)&1);

	text.back()+=" CTEN:";
	text.back()+=cpputil::Itoa((indexInfo[3]>>15)&1);
	text.back()+=" HIDE:";
	text.back()+=cpputil::Itoa((indexInfo[3]>>13)&1);
	text.back()+=" PLT:";
	text.back()+=cpputil::Itoa(indexInfo[3]&4095);

	text.back()+=" PHYSADDR:";
	text.back()+=cpputil::Uitox(physAddr);

	return text;
}

std::vector <std::string> TownsSprite::GetStatusTextSpriteAt(const unsigned char spriteRAM[],int x,int y) const
{
	std::vector <std::string> text;

	for(unsigned int spriteIndex=FirstSpriteIndex(); spriteIndex<MAX_NUM_SPRITE_INDEX; ++spriteIndex)
	{
		auto indexPtr=spriteRAM+SPRITERAM_INDEX_OFFSET+(spriteIndex<<3);

		const unsigned short indexInfo[4]=  // I hope the optimizer recognizes it.
		{
			(unsigned short)(indexPtr[0]|(indexPtr[1]<<8)),
			(unsigned short)(indexPtr[2]|(indexPtr[3]<<8)),
			(unsigned short)(indexPtr[4]|(indexPtr[5]<<8)),
			(unsigned short)(indexPtr[6]|(indexPtr[7]<<8)),
		};

		auto x0=indexInfo[0]&511; // X
		auto y0=indexInfo[1]&511; // Y

		const unsigned int attrib=indexInfo[2];

		if(0!=(attrib&ATTR_OFFS))
		{
			x0+=HOffset();
			y0+=VOffset();
		}

		int wid=16/(0!=(attrib&ATTR_SUX) ? 2 : 1);
		int hei=16/(0!=(attrib&ATTR_SUY) ? 2 : 1);

		if(x0<=x && x<x0+wid && y0<=y && y<y0+hei)
		{
			auto oneSprite=GetStatusTextOneSprite(spriteRAM,spriteIndex);
			text.insert(text.end(),oneSprite.begin(),oneSprite.end());
		}
	}

	return text;
}

std::vector <unsigned int> TownsSprite::GetPalette(unsigned int palIdx,const unsigned char spriteRAM[]) const
{
	auto palettePtr=spriteRAM+(palIdx<<5);

	std::vector <unsigned int> palVal;
	palVal.resize(16);
	for(int i=0; i<SPRITE_PALETTE_NUM_COLORS; ++i)
	{
		palVal[i] = palettePtr[i * 2] | (palettePtr[i * 2 + 1]<<8);
	}
	return palVal;
}
std::vector <std::string> TownsSprite::GetPaletteText(unsigned int palIdx,const unsigned char spriteRAM[]) const
{
	std::vector <std::string> text;
	auto palVal=GetPalette(palIdx,spriteRAM);
	for(int i=0; i<SPRITE_PALETTE_NUM_COLORS; ++i)
	{
		if(0==i%16)
		{
			text.push_back("");
		}
		else
		{
			text.back().push_back(' ');
		}
		text.back()+=cpputil::Ustox(palVal[i]);
	}
	return text;
}
std::vector <std::string> TownsSprite::GetPattern4BitText(unsigned int ptnIdx,const unsigned char spriteRAM[]) const
{
	std::vector <std::string> text;
	auto srcPtr=spriteRAM+(ptnIdx<<7);
	for(int i=0; i<SPRITE_DIMENSION*SPRITE_DIMENSION; ++i)
	{
		if(0==i%16)
		{
			text.push_back("");
		}
		auto col=srcPtr[i/2];
		auto shift=4*(i&1);
		col=((col>>shift)&0x0F);
		if(col<10)
		{
			text.back().push_back('0'+col);
		}
		else
		{
			text.back().push_back('A'-10+col);
		}
	}
	return text;
}
std::vector <std::string> TownsSprite::GetPattern16BitText(unsigned int ptnIdx,const unsigned char spriteRAM[]) const
{
	std::vector <std::string> text;
	auto srcPtr=spriteRAM+(ptnIdx<<7);
	for(int i=0; i<SPRITE_DIMENSION*SPRITE_DIMENSION; ++i)
	{
		if(0==i%16)
		{
			text.push_back("");
		}
		else
		{
			text.back().push_back(' ');
		}
		auto col=(srcPtr[i*2]|(srcPtr[i*2+1]<<8));
		text.back()+=cpputil::Ustox(col);
	}
	return text;
}

/* virtual */ uint32_t TownsSprite::SerializeVersion(void) const
{
	// Version 2: Sprite Transfer Time
	// Version 3: firstSpriteIndexCapture
	return 3;
}
/* virtual */ void TownsSprite::SpecificSerialize(std::vector <unsigned char> &data,std::string) const
{
	PushUint16(data,state.addressLatch);
	for(auto r : state.reg)
	{
		PushUint16(data,r);
	}
	PushBool(data,state.spriteBusy);
	PushBool(data,state.screenModeAcceptsSprite);

	// Version 1 and later >>
	PushUint16(data, state.callbackType);
	PushUint16(data, state.page);

	// Vesion 2 and later >>
	PushUint32(data,state.transferTime);

	// Version 3 and later >>
	PushUint16(data,state.firstSpriteIndexCapture);
}
/* virtual */ bool TownsSprite::SpecificDeserialize(const unsigned char *&data,std::string,uint32_t version)
{
	state.transferTime=SPRITE_ONE_TRANSFER_TIME_FASTMODE;

	state.addressLatch=ReadUint16(data);
	for(auto &r : state.reg)
	{
		r=ReadUint16(data);
	}
	state.spriteBusy=ReadBool(data);
	state.screenModeAcceptsSprite=ReadBool(data);
	if(0==version)
	{
		if(true==SPEN())
		{
			state.callbackType=(true==state.spriteBusy ? CALLBACK_FINISH : CALLBACK_VSYNC);
		}
		else
		{
			state.callbackType=CALLBACK_NONE;
		}
		state.page=0; // Should catch up in the next.
	}
	if(1<=version)
	{
		state.callbackType = ReadUint16(data);
		state.page = ReadUint16(data);
	}
	if(2<=version)
	{
		state.transferTime=ReadUint32(data);
	}
	if(3<=version)
	{
		state.firstSpriteIndexCapture=ReadUint16(data);
	}
	else
	{
		state.firstSpriteIndexCapture=FirstSpriteIndex();
	}
	return true;
}
