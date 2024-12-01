/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */


#ifndef MIDI_IS_INCLUDED
#define MIDI_IS_INCLUDED
/* { */

#include <stdint.h>
#include "device.h"
#include "townsdef.h"
#include "i8251.h"
#include "i8253.h"
#include "outside_world.h"
#include "midi_interface.h"


class TownsMIDI : public Device
{
private:
	class FMTownsCommon *townsPtr;
public:
	enum
	{
		MAX_NUM_MIDI_CARDS=5,
		TIMER_INTERVAL=1000000000/500000, // 500KHz? 480MHz?
	};

	bool midiMonitor=false;

	class MIDICard;

	class i8251Client : public i8251::Client
	{
	public:
		MIDICard *owner=nullptr;
		int portNo=0;

		bool TxRDY(void) override{return true;}
		void Tx(unsigned char data) override;
		void SetStopBits(unsigned char stopBits) override {}
		void SetParity(bool enabled,bool evenParity) override {}
		void SetDataLength(unsigned char bitsPerData) override {}
		void SetBaudRate(unsigned int baudRate) override {}
		void Command(bool RTS,bool DTR,bool BREAK) override {}
		bool RxRDY(void) override {return false;}
		unsigned char Rx(void) override {return 0;}
		unsigned char PeekRx(void) const override {return 0;};
		bool DSR(void) override {return true;}
	};
	class MIDICard
	{
	public:
		TownsMIDI *owner=nullptr;

		class MIDIPort
		{
		public:
			i8251 usart;

			MIDI_Interface *midiItfc=nullptr;
			unsigned char midiMessageFilled=0,midiMessageLen=0;
			bool midiSysExflag=false;
			unsigned char midiMessage[12];
			i8251Client interface;  // Fixed in the constructor.  Not saved in the state.
		};

		bool enabled=false;
		int portBase=0;  // Fixed in the constructor.  Not saved in the state.
		MIDIPort ports[2];
		unsigned int fifoReg=0;
		unsigned int fifoDat=0;

		MIDICard()
		{
			for(int i=0; i<2; ++i)
			{
				ports[i].midiItfc=MIDI_Interface::Create();
				ports[i].usart.clientPtr=&ports[i].interface;
				ports[i].interface.portNo=i;
				ports[i].interface.owner=this;
			}
			ForceTxEmpty();
		}
		~MIDICard()
		{
			for(auto &p : ports)
			{
				MIDI_Interface::Delete(p.midiItfc);
				p.midiItfc=nullptr;
			}
		}
		void ForceTxEmpty(void)
		{
			for(auto &p : ports)
			{
				p.usart.state.TxRDY=true;
				p.usart.state.TxEMPTY=true;
			}
		}
		void ByteSentFromVM(int port,unsigned char data);

		void IOWriteByte(unsigned int ioport,unsigned int data,uint64_t townsTime);
		unsigned int IOReadByte(unsigned int ioport,uint64_t townsTime);
	};

	class State
	{
	public:
		i8253 timer;
		MIDICard cards[MAX_NUM_MIDI_CARDS];
		unsigned int INTMaskSend=0,INTMaskReceive=0;
		unsigned int writeINTOccured=~0,readINTOccured=~0; // Looks like active low.

		uint64_t lastTimerTickTime=0;
		unsigned int timerINTMask=0,timerINTOccured=0;
	};
	State state;
	class Variable
	{
	};
	Variable var;

	virtual const char *DeviceName(void) const{return "MIDI";}

	TownsMIDI(class FMTownsCommon *townsPtr);

	void EnableCards(int nCards);
	void Stop(void);

	inline void TimerPolling(uint64_t townsTime)
	{
		if(0==state.lastTimerTickTime)
		{
			state.lastTimerTickTime=townsTime;
		}
		else if(state.lastTimerTickTime+TIMER_INTERVAL<=townsTime)
		{
			TimerPollingInternal(townsTime);
		}
	}

	void TimerPollingInternal(uint64_t townsTime);

	void UpdateInterruptRequestSerial(void);
	void UpdateInterruptRequestTimer(void);
	void UpdateSchedule(void);

	void PowerOn(void) override;
	void Reset(void) override;

	void IOWriteByte(unsigned int ioport,unsigned int data) override;
	unsigned int IOReadByte(unsigned int ioport) override;

	void RunScheduledTask(unsigned long long int townsTime) override;

	uint32_t SerializeVersion(void) const override;
	void SpecificSerialize(std::vector <unsigned char> &data,std::string stateFName) const override;
	bool SpecificDeserialize(const unsigned char *&data,std::string stateFName,uint32_t version) override;
};



// MB89371A data sheet
// http://act-ele.c.ooco.jp/jisaku/rsmoni/MB89371A.pdf
//
// midiOutShortMsg sample
// http://www13.plala.or.jp/kymats/study/MULTIMEDIA/midiOutShortMsg.html
//
// MIDI messages
// https://www.g200kg.com/jp/docs/tech/midi.html



// Sample Partial Code from BCC
// void TownsMIDI::IOWriteByte(unsigned int ioport,unsigned int data)
// {
// 	case TOWNSIO_MIDI_CARD1_DATREG1: // 0x0E50, MIDI card(MT-402 or 403) No.1 Out port 1 datReg1 (from Linux source)
// 		if (state.data_byte == false)
// 		{
// 			int data_check;
// 
// 			data_check = data & 0xf0;
// 			state.MIDI_data_bytes = 0;
// 			state.MIDI_states = data & 0xff;
// 			state.MIDI_data[0] = 0;
// 			state.MIDI_data[1] = 0;
// 			state.data_byte = true;
// 
// 			if (data_check == 0xf0)
// 			{
// 				switch (data)
// 				{
// 				case 0xf0:
// 				case 0xf1:
// 				case 0xf3:
// 					state.MIDI_data_length = 1;
// 					break;
// 				case 0xf2:
// 					state.MIDI_data_length = 2;
// 					break;
// 				default:
// 					state.MIDI_data_length = 0;
// 					//outside_world->MIDIMessage(MIDI_states, 0, 0);
// 					midiOutShortMsg(hMidiOut, MIDIMSG(data, 0x0, 0, 0));
// 					state.data_byte = false;
// 					break;
// 				}
// 			}
// 			else
// 			{
// 				if (data_check == 0xc0 || data_check == 0xd0)
// 				{
// 					state.MIDI_data_length = 1;
// 				}
// 				else
// 				{
// 					state.MIDI_data_length = 2;
// 				}
// 			}
// 		}
// 		else
// 		{
// 			state.MIDI_data[state.MIDI_data_bytes++] = data;
// 			if (state.MIDI_data_bytes == state.MIDI_data_length)
// 			{
// 				#define MIDIMSG(status,channel,data1,data2) ( (DWORD)((status<<4) | channel | (data1<<8) | (data2<<16)) )
// 				midiOutShortMsg(hMidiOut, MIDIMSG((state.MIDI_states >> 4), (state.MIDI_states & 0xf), state.MIDI_data[0], state.MIDI_data[1]));
// 				state.data_byte = false;
// 			}
// 		}
// 		if (state.int_mask_send & 0x1)
// 		{
// 			townsPtr->pic.SetInterruptRequestBit(TOWNSIRQ_MIDI, true); // TOWNSIRQ_MIDI = 4
// 		}
// 		break;
// 	case TOWNSIO_MIDI_MIDI_INT_MASK_SEND: // 0x0E70 //MIDI SEND interrupt MASK
// 		state.int_mask_send = data & 0xff;
// 		break;
// 	case TOWNSIO_MIDI_MIDI_INT_MASK_RECEIVE:// 0x0E71 //MIDI RECEIVE interrupt MASK
// 		state.int_mask_receive = data & 0xff;
// 		break;
// }
// 
// unsigned int TownsMIDI::IOReadByte(unsigned int ioport)
// {
// 	case TOWNSIO_MIDI_CARD1_CMDREG1: // 0x0E51, MIDI card(MT-402 or 403) No.1 cmdReg1 (Linux source)
// 		return 4;
// 		break;
// 	case TOWNSIO_MIDI_MIDI_INT_MASK_SEND: // 0x0E70
// 		return state.int_mask_send;
// 		break;
// 	case TOWNSIO_MIDI_MIDI_INT_MASK_RECEIVE: // 0x0E71
// 		return state.int_mask_receive;
// 		break;
// }

/* } */
#endif
