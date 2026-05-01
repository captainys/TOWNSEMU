/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include "i486.h"
#include "i486debug.h"



#include "cpputil.h"
#include "memaccess.h"
#include "townsdef.h"
#include "crtc.h"
#include "towns.h"



////////////////////////////////////////////////////////////


TownsFMRVRAMAccess::TownsFMRVRAMAccess()
{
	breakOnFMRVRAMWrite=false;
	breakOnFMRVRAMRead=false;
	breakOnCVRAMWrite=false;
	breakOnCVRAMRead=false;
}

unsigned int TownsFMRVRAMAccess::FetchByte(const TownsPhysicalMemory *physMemPtr,unsigned int physAddr) const
{
	if((TOWNS_MEMIO_1_LOW<=physAddr && physAddr<=TOWNS_MEMIO_1_HIGH) ||
	   (TOWNS_MEMIO_2_LOW<=physAddr && physAddr<=TOWNS_MEMIO_2_HIGH))
	{
		switch(physAddr)
		{
		case TOWNSMEMIO_MIX://                0x000CFF80, // Called Dummy [2] pp.22, pp.158
			break;
		case TOWNSMEMIO_FMR_GVRAMMASK://      0x000CFF81, // [2] pp.22,pp.159
			return physMemPtr->state.FMRVRAMMask;
		case TOWNSMEMIO_FMR_GVRAMDISPMODE://  0x000CFF82, // [2] pp.22,pp.158
			break;
		case TOWNSMEMIO_FMR_GVRAMPAGESEL://   0x000CFF83, // [2] pp.22,pp.159
			if(0==physMemPtr->state.FMRVRAMWriteOffset)
			{
				return 0;
			}
			else
			{
				return 0x10;
			}
			break;

		case TOWNSMEMIO_FIRQ://               0x000CFF84, // [2] pp.22,pp.95 Always zero in FM TOWNS
			return 0;

		case TOWNSMEMIO_FMR_HSYNC_VSYNC://    0x000CFF86, // [2] pp.22,pp.160
			{
				bool VSYNC=crtcPtr->InVSYNC(townsPtr->state.townsTime);
				bool HSYNC=crtcPtr->InHSYNC(townsPtr->state.townsTime);
				unsigned char data=0x10;  // Bit4 is always 1.
				data|=(VSYNC ? 4 : 0);
				data|=(HSYNC ? 0x80 : 0);
				return data;
			}
			break;

		case TOWNSMEMIO_KANJI_JISCODE_HIGH:// 0x000CFF94,
			return 0x80;  // 
		case TOWNSMEMIO_KANJI_JISCODE_LOW://  0x000CFF95,
			break;
		case TOWNSMEMIO_KANJI_PTN_HIGH://     0x000CFF96,
			{
				auto ROMCode=(physMemPtr->state.kanjiROMAccess.FontROMCode())&8191;
				return physMemPtr->fontRom[32*ROMCode+physMemPtr->state.kanjiROMAccess.row*2];
			}
			break;
		case TOWNSMEMIO_KANJI_PTN_LOW ://     0x000CFF97,
			if(true==physMemPtr->takeJISCodeLog && 0==physMemPtr->state.kanjiROMAccess.row)
			{
				physMemPtr->JISCodeLog.push_back(physMemPtr->state.kanjiROMAccess.JISCodeHigh);
				physMemPtr->JISCodeLog.push_back(physMemPtr->state.kanjiROMAccess.JISCodeLow);
			}
			{
				auto ROMCode=physMemPtr->state.kanjiROMAccess.FontROMCode()&8191;
				auto byteData=physMemPtr->fontRom[32*ROMCode+physMemPtr->state.kanjiROMAccess.row*2+1];
				physMemPtr->state.kanjiROMAccess.row=(physMemPtr->state.kanjiROMAccess.row+1)&0x0F;
				return byteData;
			}
			break;
		case TOWNSMEMIO_BUZZER_CONTROL://     0x000CFF98
			townsPtr->timer.ControlBuzzerByMemoryIO(true);
			break;
		}
	}
	else if(TOWNSADDR_FMR_VRAM_BASE<=physAddr && physAddr<TOWNSADDR_FMR_VRAM_END) /// FMR VRAM Plane Access
	{
		const auto FMRAddr=physAddr-TOWNSADDR_FMR_VRAM_BASE;
		const auto VRAMAddr=(FMRAddr<<2)+physMemPtr->state.FMRVRAMWriteOffset;
		auto shift=(physMemPtr->state.FMRVRAMMask>>6)&3;
		unsigned char andPtnHigh=(0x10<<shift);
		unsigned char andPtnLow=(1<<shift);
		unsigned char orPtnHigh=0x40;
		unsigned char orPtnLow=0x80;
		unsigned char data=0;
		for(int i=0; i<4; ++i)
		{
			if(0!=(physMemPtr->state.VRAM[VRAMAddr+i]&andPtnHigh))
			{
				data|=orPtnHigh;
			}
			if(0!=(physMemPtr->state.VRAM[VRAMAddr+i]&andPtnLow))
			{
				data|=orPtnLow;
			}
			orPtnHigh>>=2;
			orPtnLow>>=2;
		}
		if(true==breakOnFMRVRAMRead &&
		   nullptr!=cpuPtr &&
		   nullptr!=cpuPtr->debuggerPtr)
		{
			cpuPtr->debuggerPtr->ExternalBreak("FMRVRAM Read "+cpputil::Uitox(physAddr));
		}
		return data;
	}
	else if(TOWNSADDR_FMR_CVRAM_BASE<=physAddr && physAddr<TOWNSADDR_FMR_CVRAM_END) /// FMR I/OCVRAM Access
	{
		if(true==breakOnCVRAMRead &&
		   nullptr!=cpuPtr &&
		   nullptr!=cpuPtr->debuggerPtr)
		{
			cpuPtr->debuggerPtr->ExternalBreak("CVRAM Read "+cpputil::Uitox(physAddr));
		}
		return physMemPtr->state.spriteRAM[physAddr-TOWNSADDR_FMR_VRAM_END];
	}
	else
	{
		if(true==physMemPtr->state.ANKFont)
		{
			if(TOWNSADDR_FMR_ANK8_BASE<=physAddr && physAddr<TOWNSADDR_FMR_ANK8_END)
			{
				return physMemPtr->fontRom[TOWNSADDR_FMR_ANK8_MAP+physAddr-TOWNSADDR_FMR_ANK8_BASE];
			}
			else if(TOWNSADDR_FMR_ANK16_BASE<=physAddr && physAddr<TOWNSADDR_FMR_ANK16_END)
			{
				return physMemPtr->fontRom[TOWNSADDR_FMR_ANK16_MAP+physAddr-TOWNSADDR_FMR_ANK16_BASE];
			}
		}
		else if(TOWNSADDR_FMR_CVRAM_BASE<=physAddr && physAddr<TOWNSADDR_FMR_KVRAM_END)
		{
			return physMemPtr->state.spriteRAM[physAddr-TOWNSADDR_FMR_CVRAM_BASE];
		}
	}
	return 0xff;
}
unsigned int TownsFMRVRAMAccess::FetchWord(const TownsPhysicalMemory *physMemPtr,unsigned int physAddr) const
{
	if ((TOWNS_MEMIO_1_LOW<=physAddr&&physAddr<=TOWNS_MEMIO_1_HIGH) ||
		(TOWNS_MEMIO_2_LOW<=physAddr&&physAddr<=TOWNS_MEMIO_2_HIGH))
	{
		switch (physAddr)
		{
		case TOWNSMEMIO_KANJI_PTN_HIGH://     0x000CFF96,
		{
			if (true == physMemPtr->takeJISCodeLog && 0 == physMemPtr->state.kanjiROMAccess.row)
			{
				physMemPtr->JISCodeLog.push_back(physMemPtr->state.kanjiROMAccess.JISCodeHigh);
				physMemPtr->JISCodeLog.push_back(physMemPtr->state.kanjiROMAccess.JISCodeLow);
			}
			{
				auto ROMCode=physMemPtr->state.kanjiROMAccess.FontROMCode()&8191;
				auto highData=physMemPtr->fontRom[32*ROMCode+physMemPtr->state.kanjiROMAccess.row*2];
				auto lowData=physMemPtr->fontRom[32*ROMCode+physMemPtr->state.kanjiROMAccess.row*2+1];
				physMemPtr->state.kanjiROMAccess.row=(physMemPtr->state.kanjiROMAccess.row+1)&0x0F;
				return (lowData<<8)|highData;
			}
			break;
		}
		}
	}

	return FetchByte(physMemPtr,physAddr)|
	      (FetchByte(physMemPtr,physAddr+1)<<8);
}
unsigned int TownsFMRVRAMAccess::FetchDword(const TownsPhysicalMemory *physMemPtr,unsigned int physAddr) const
{
	return FetchByte(physMemPtr,physAddr)|
	      (FetchByte(physMemPtr,physAddr+1)<<8)|
	      (FetchByte(physMemPtr,physAddr+2)<<16)|
	      (FetchByte(physMemPtr,physAddr+3)<<24);
}
void TownsFMRVRAMAccess::StoreByte(TownsPhysicalMemory *physMemPtr,unsigned int physAddr,unsigned char data)
{
	const auto FMRAddr=physAddr-TOWNSADDR_FMR_VRAM_BASE;
	if(FMRAddr<TOWNSADDR_FMR_VRAM_END-TOWNSADDR_FMR_VRAM_BASE)
	{
		// Assume screen mode 1 and 2.
		//   Logical Resolution 640x819
		//   Visible Resolution 640x400 or 640x200
		// FMRVram 8pixels per byte, 640pixels=80bytes.
		// TownsVRAM 2pixels per byte, 640pixels=320bytes.
		// Just multiply 4 to get TownsVRAM address.
		const auto VRAMAddr=(FMRAddr<<2)+physMemPtr->state.FMRVRAMWriteOffset;

		unsigned char maskLow=(physMemPtr->state.FMRVRAMMask&0x0F);
		auto maskHigh=(maskLow<<4);
		auto andPtn=~(maskLow|maskHigh);

		unsigned char bitTestHigh=0x40;
		unsigned char bitTestLow=0x80;
		for(int i=0; i<4; ++i)
		{
			physMemPtr->state.VRAM[VRAMAddr+i]&=andPtn;
			if(0!=(data&bitTestHigh))
			{
				physMemPtr->state.VRAM[VRAMAddr+i]|=maskHigh;
			}
			if(0!=(data&bitTestLow))
			{
				physMemPtr->state.VRAM[VRAMAddr+i]|=maskLow;
			}
			bitTestHigh>>=2;
			bitTestLow>>=2;
		}
		if(true==breakOnFMRVRAMWrite &&
		   nullptr!=cpuPtr &&
		   nullptr!=cpuPtr->debuggerPtr)
		{
			cpuPtr->debuggerPtr->ExternalBreak("FMRVRAM Write "+cpputil::Uitox(physAddr));
		}
	}
	else if((TOWNS_MEMIO_1_LOW<=physAddr && physAddr<=TOWNS_MEMIO_1_HIGH) ||
	        (TOWNS_MEMIO_2_LOW<=physAddr && physAddr<=TOWNS_MEMIO_2_HIGH))
	{
		switch(physAddr)
		{
		case TOWNSMEMIO_MIX://                0x000CFF80, // Called Dummy [2] pp.22, pp.158
			break;
		case TOWNSMEMIO_FMR_GVRAMMASK://      0x000CFF81, // [2] pp.22,pp.159
			physMemPtr->state.FMRVRAMMask=data;
			break;
		case TOWNSMEMIO_FMR_GVRAMDISPMODE://  0x000CFF82, // [2] pp.22,pp.158
			crtcPtr->MEMIOWriteFMRVRAMDisplayMode(data);
			break;
		case TOWNSMEMIO_FMR_GVRAMPAGESEL://   0x000CFF83, // [2] pp.22,pp.159
			// Looks like I was interpreting the definition of FM-R Graphics VRAM 'PAGE' wrong.
			// I thought PAGEs mapps to LAYERs, but after reading Artane.'s FM TOWNS emulator source,
			// FM-R Graphics VRAM PAGE0 maps to upper half or VRAM LAYER0, and PAGE1 lower half.
			// It makes sense because [2] pp. 155 Section 4.9.2 tells that FMR50 compatible mode
			// uses screen layer0 for graphics and layer1 for text.  So, 'page' and 'layer' are
			// different.
			// Credit to Artane.!  Thanks!
			physMemPtr->state.FMRVRAMWriteOffset=(0!=(data&0x10) ? TOWNS_FMRMODE_VRAM_OFFSET : 0);
			break;

		case TOWNSMEMIO_FIRQ://               0x000CFF84, // [2] pp.22,pp.95 Always zero in FM TOWNS
			break; // No write access

		case TOWNSMEMIO_FMR_HSYNC_VSYNC://    0x000CFF86, // [2] pp.22,pp.160
			break;

		case TOWNSMEMIO_KANJI_JISCODE_HIGH:// 0x000CFF94,
			physMemPtr->state.kanjiROMAccess.JISCodeHigh=data&0x7F;
			break;
		case TOWNSMEMIO_KANJI_JISCODE_LOW://  0x000CFF95,
			physMemPtr->state.kanjiROMAccess.JISCodeLow=data;
			physMemPtr->state.kanjiROMAccess.row=0;
			break;
		case TOWNSMEMIO_KANJI_PTN_HIGH://     0x000CFF96,
			// Write access enabled? [2] pp.95
			break;
		case TOWNSMEMIO_KANJI_PTN_LOW://      0x000CFF97,
			// Write access enabled? [2] pp.95
			break;
		case TOWNSMEMIO_BUZZER_CONTROL://     0x000CFF98
			townsPtr->timer.ControlBuzzerByMemoryIO(false);
			break;
		case TOWNSMEMIO_KVRAM_OR_ANKFONT: //  0x000CFF99
			physMemPtr->state.ANKFont=(0!=(data&1));
			break;
		}
	}
	else if(TOWNSADDR_FMR_CVRAM_BASE<=physAddr && physAddr<TOWNSADDR_FMR_KVRAM_END) // Except I/O.
	{
		if(true==breakOnCVRAMWrite &&
		   nullptr!=cpuPtr &&
		   nullptr!=cpuPtr->debuggerPtr)
		{
			cpuPtr->debuggerPtr->ExternalBreak("CVRAM Write "+cpputil::Uitox(physAddr));
		}
		physMemPtr->state.spriteRAM[physAddr-TOWNSADDR_FMR_CVRAM_BASE]=data;
		physMemPtr->state.TVRAMWrite=true;
	}
}
void TownsFMRVRAMAccess::StoreWord(TownsPhysicalMemory *physMemPtr,unsigned int physAddr,unsigned int data)
{
	StoreByte(physMemPtr,physAddr,data);
	StoreByte(physMemPtr,physAddr+1,data>>8);
}
void TownsFMRVRAMAccess::StoreDword(TownsPhysicalMemory *physMemPtr,unsigned int physAddr,unsigned int data)
{
	StoreByte(physMemPtr,physAddr,data);
	StoreByte(physMemPtr,physAddr+1,data>>8);
	StoreByte(physMemPtr,physAddr+2,data>>16);
	StoreByte(physMemPtr,physAddr+3,data>>24);
}

////////////////////////////////////////////////////////////

#include "rf5c68.h"
TownsWaveRAMAccess::TownsWaveRAMAccess(class FMTownsCommon *townsPtr,class RF5C68 *pcmPtr,class VGMRecorder *vgmRecPtr)
{
	this->townsPtr=townsPtr;
	this->pcmPtr=pcmPtr;
	this->vgmRecorderPtr=vgmRecPtr;
}

unsigned int TownsWaveRAMAccess::FetchByte(unsigned int physAddr) const
{
	return pcmPtr->ReadWaveRAM(physAddr&TOWNSADDR_WAVERAM_WINDOW_AND);
}

unsigned int TownsWaveRAMAccess::FetchWord(unsigned int physAddr) const
{
	// In real towns, it's byte access only.
	return FetchByte(physAddr)|(FetchByte(physAddr+1)<<8);
}

unsigned int TownsWaveRAMAccess::FetchDword(unsigned int physAddr) const
{
	// In real towns, it's byte access only.
	return FetchByte(physAddr)|
	      (FetchByte(physAddr+1)<<8)|
	      (FetchByte(physAddr+2)<<16)|
	      (FetchByte(physAddr+3)<<24);
}

void TownsWaveRAMAccess::StoreByte(unsigned int physAddr,unsigned char data)
{
	pcmPtr->WriteWaveRAM(physAddr&TOWNSADDR_WAVERAM_WINDOW_AND,data);
	if(true==vgmRecorderPtr->enabled)
	{
		vgmRecorderPtr->WritePCMMemory(townsPtr->state.townsTime,VGMRecorder::MEM_RF5C68,physAddr&TOWNSADDR_WAVERAM_WINDOW_AND,data);
	}
}

void TownsWaveRAMAccess::StoreWord(unsigned int physAddr,unsigned int data)
{
	StoreByte(physAddr,data);
	StoreByte(physAddr+1,data>>8);
}

void TownsWaveRAMAccess::StoreDword(unsigned int physAddr,unsigned int data)
{
	StoreByte(physAddr,data);
	StoreByte(physAddr+1,data>>8);
	StoreByte(physAddr+2,data>>16);
	StoreByte(physAddr+3,data>>24);
}
