/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include "cpputil.h"
#include "keyboard.h"
#include "townsdef.h"
#include "pic.h"
#include "towns.h"


void TownsKeyboard::State::Reset(void)
{
	IRQEnabled=false;
	KBINT=false;
	bootKeyCombSequenceCounter=0;
}
TownsKeyboard::TownsKeyboard(FMTowns *townsPtr,TownsPIC *picPtr) : Device(townsPtr)
{
	this->townsPtr=townsPtr;
	this->picPtr=picPtr;

	state.Reset();
	state.bootKeyComb=BOOT_KEYCOMB_NONE;
	nFifoFilled=0;
}

void TownsKeyboard::SetAutoType(std::string str)
{
	while(nFifoFilled+1<FIFO_BUF_LEN && 0<str.size())
	{
		unsigned char byteData[2];
		if(0<TranslateChar(byteData,str[0]))
		{
			PushFifo(byteData[0]|TOWNS_KEYFLAG_PRESS  ,byteData[1]);
			PushFifo(byteData[0]|TOWNS_KEYFLAG_RELEASE,byteData[1]);
		}
		str.erase(str.begin());
	}
	autoType=str;
}

void TownsKeyboard::PushFifo(unsigned char code1,unsigned char code2)
{
	if(TownsEventLog::MODE_RECORDING==townsPtr->eventLog.mode)
	{
		townsPtr->eventLog.LogKeyCode(townsPtr->state.townsTime,code1,code2);
	}
	if(true==debugBreakOnReturnKey && code2==TOWNS_JISKEY_RETURN)
	{
		this->townsPtr->debugger.ExternalBreak("Break on Enter Key");
	}
	if(nFifoFilled+1<FIFO_BUF_LEN)
	{
		fifoBuf[nFifoFilled  ]=code1;
		fifoBuf[nFifoFilled+1]=code2;
		nFifoFilled+=2;
	}
	state.KBINT=true;
	if(true==state.IRQEnabled)
	{
		picPtr->SetInterruptRequestBit(TOWNSIRQ_KEYBOARD,true);
	}
}

void TownsKeyboard::BootSequenceStarted(void)
{
	state.bootKeyComb=BOOT_KEYCOMB_NONE;
	state.bootKeyCombSequenceCounter=0;
}
void TownsKeyboard::SetBootKeyCombination(unsigned int keyComb)
{
	if(BOOT_KEYCOMB_PAD_A!=keyComb &&
	   BOOT_KEYCOMB_PAD_B!=keyComb)
	{
		state.bootKeyComb=keyComb;
		state.bootKeyCombSequenceCounter=0;
	}
}

/* virtual */ void TownsKeyboard::IOWriteByte(unsigned int ioport,unsigned int data)
{
	switch(ioport)
	{
	case TOWNSIO_KEYBOARD_DATA://       0x600, // [2] pp.234
		// FM-OASYS and Keyboard BIOS writes A1, A2 to data register.
		// Seems to have the same effect as reset command to the command register. (Tested on TOWNS II MX with JIS Keyboard)
		if(0xA1==data || 0xA2==data)
		{
			// Looks like Writing A1 or A2 to keyboard data will return B0 7F E8 25, but not reset the keyboard controller.
			// If it resets, F-29 Retaliator won't start (probably because keyboard INT is disabled in state.Reset()).
			// state.Reset();
			nFifoFilled=0;
			PushFifo(0xB0,0x7F);
			PushFifo(0xE8,0x25);
		}
		break;
	case TOWNSIO_KEYBOARD_STATUS_CMD:// 0x602, // [2] pp.231
		//Observation from FM Towns II MX with JIS Keyboard (9/11/2020)
		//Command Write(I/O 0602H)  ->  Read (I/O 0600H)
		//     A1                       B0 7F E8 25
		//     A1 A1                    B0 7F
		//     A0 A1                    B0 7F E8 25
		//     A0                       B0 7F E8 25 00 00 00 ......

		// [2] FM Towns Technical Data Book pp.232 Table I-7-3 lists command A1 as reset.
		// But, SYSROM sends command A0 then A1.  Command A0 is not listed in the table.
		// SYSROM sends command A0 then A1, and expects return A0,7F from the keyboard.
		// Keyboard BIOS sends command A1 then A1, and not expecting any return.
		// Probably command A0 is hard reset, and A1 is soft reset, and the keyboard
		// returns A0,7F after hard reset?

		// Rewrite based on the observation
		if(0xA1==state.lastCmd && 0xA1==data)
		{
			state.Reset();
			nFifoFilled=0;
			PushFifo(0xB0,0x7F);
		}
		else if(0xA0==state.lastCmd && 0xA1==data)
		{
			state.Reset();
			nFifoFilled=0;
			PushFifo(0xB0,0x7F);
			PushFifo(0xE8,0x25);
		}
		else if(0xA0==data)
		{
			state.Reset();
			nFifoFilled=0;
			PushFifo(0xB0,0x7F);
			PushFifo(0xE8,0x25);
		}
		else if(0xA1==data)
		{
			state.Reset();
			nFifoFilled=0;
			PushFifo(0xB0,0x7F);
		}
		state.lastCmd=data;
		break;
	case TOWNSIO_KEYBOARD_IRQ://        0x604, // [2] pp.236
		state.IRQEnabled=(0!=(data&1));
		if(true==state.IRQEnabled && 0<nFifoFilled)
		{
			picPtr->SetInterruptRequestBit(TOWNSIRQ_KEYBOARD,true);
		}
		break;
	}
}
/* virtual */ unsigned int TownsKeyboard::IOReadByte(unsigned int ioport)
{
	switch(ioport)
	{
	case TOWNSIO_KEYBOARD_DATA://       0x600, // [2] pp.234
		picPtr->SetInterruptRequestBit(TOWNSIRQ_KEYBOARD,false);
		state.KBINT=false;
		if(BOOT_KEYCOMB_NONE==state.bootKeyComb)
		{
			if(0<nFifoFilled)
			{
				auto ret=fifoBuf[0];
				for(int i=0; i<nFifoFilled-1; ++i)
				{
					fifoBuf[i]=fifoBuf[i+1];
				}
				--nFifoFilled;

				while(nFifoFilled+1<FIFO_BUF_LEN && 0<autoType.size())
				{
					unsigned char byteData[2];
					if(0<TranslateChar(byteData,autoType[0]))
					{
						PushFifo(byteData[0]|TOWNS_KEYFLAG_PRESS  ,byteData[1]);
						PushFifo(byteData[0]|TOWNS_KEYFLAG_RELEASE,byteData[1]);
					}
					autoType.erase(autoType.begin());
				}

				if(0<nFifoFilled && true==state.IRQEnabled)
				{
					townsPtr->ScheduleDeviceCallBack(*this,townsPtr->state.townsTime+KEY_REPEAT_INTERVAL);
				}
				return ret;
			}
		}
		else
		{
			auto highLow=state.bootKeyCombSequenceCounter&1;
			auto num=(state.bootKeyCombSequenceCounter>>1);
			++state.bootKeyCombSequenceCounter;
			if(0==highLow)
			{
				return (num<6 ? 0xA0 : 0xF0); // The longest one: 7F 'D' 'E' 'B' 'U' 'G'
			}
			if(0==num)
			{
				return 0x7F;
			}
			else
			{
				switch(state.bootKeyComb)
				{
				case BOOT_KEYCOMB_CD:
					{
						const unsigned char code[2]={0x2C,0x20};
						return code[num&1];
					}
					break;
				case BOOT_KEYCOMB_F0:
					{
						const unsigned char code[2]={0x21,0x0B};
						return code[num&1];
					}
					break;
				case BOOT_KEYCOMB_F1:
					{
						const unsigned char code[2]={0x21,0x02};
						return code[num&1];
					}
					break;
				case BOOT_KEYCOMB_F2:
					{
						const unsigned char code[2]={0x21,0x03};
						return code[num&1];
					}
					break;
				case BOOT_KEYCOMB_F3:
					{
						const unsigned char code[2]={0x21,0x04};
						return code[num&1];
					}
					break;
				case BOOT_KEYCOMB_H0:
					{
						const unsigned char code[2]={0x23,0x0B};
						return code[num&1];
					}
					break;
				case BOOT_KEYCOMB_H1:
					{
						const unsigned char code[2]={0x23,0x02};
						return code[num&1];
					}
					break;
				case BOOT_KEYCOMB_H2:
					{
						const unsigned char code[2]={0x23,0x03};
						return code[num&1];
					}
					break;
				case BOOT_KEYCOMB_H3:
					{
						const unsigned char code[2]={0x23,0x04};
						return code[num&1];
					}
					break;
				case BOOT_KEYCOMB_H4:
					{
						const unsigned char code[2]={0x23,0x05};
						return code[num&1];
					}
					break;
				case BOOT_KEYCOMB_ICM:
					{
						const unsigned char code[3]={0x18,0x2C,0x30};
						return code[num%3];
					}
					break;
				case BOOT_KEYCOMB_DEBUG:
					{
						const unsigned char code[5]={0x20,0x13,0x2E,0x17,0x22};
						return code[num%5];
					}
					break;
				}
			}
		}
		return 0;

	case TOWNSIO_KEYBOARD_STATUS_CMD:// 0x602, // [2] pp.231
		if(0<nFifoFilled || BOOT_KEYCOMB_NONE!=state.bootKeyComb)
		{
			return 1; // Data (Keyboard -> CPU) Ready.
		}
		return 0;  // IBF=0 Data Empty. Always Ready.
	case TOWNSIO_KEYBOARD_IRQ://        0x604, // [2] pp.236
		return (true==state.KBINT ? 1 : 0);
	}
	return 0xff;
}
/* virtual */ void TownsKeyboard::Reset(void)
{
	state.Reset();
}

/* virtual */ void TownsKeyboard::RunScheduledTask(unsigned long long int townsTime)
{
	// 9600bps=600words per seconds.
	if(0<nFifoFilled && true==state.IRQEnabled)
	{
		state.KBINT=true;
		picPtr->SetInterruptRequestBit(TOWNSIRQ_KEYBOARD,true);
	}
}

bool TownsKeyboard::InFifoBuffer(unsigned int code) const
{
	for(int i=0; i<nFifoFilled; ++i)
	{
		if(fifoBuf[i]==code)
		{
			return true;
		}
	}
	return false;
}



/* virtual */ uint32_t TownsKeyboard::SerializeVersion(void) const
{
	return 0;
}
/* virtual */ void TownsKeyboard::SpecificSerialize(std::vector <unsigned char> &data,std::string stateFName) const
{
	PushBool(data,state.IRQEnabled);
	PushBool(data,state.KBINT);

	PushUint32(data,state.bootKeyComb);
	PushUint32(data,state.bootKeyCombSequenceCounter);

	PushUint32(data,state.lastCmd);
}
/* virtual */ bool TownsKeyboard::SpecificDeserialize(const unsigned char *&data,std::string stateFName,uint32_t version)
{
	state.IRQEnabled=ReadBool(data);
	state.KBINT=ReadBool(data);

	state.bootKeyComb=ReadUint32(data);
	state.bootKeyCombSequenceCounter=ReadUint32(data);

	state.lastCmd=ReadUint32(data);

	return 0;
}
