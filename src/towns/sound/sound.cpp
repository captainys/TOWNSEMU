/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include "townsdef.h"
#include "sound.h"
#include "towns.h"
#include "outside_world.h"


void TownsSound::State::PowerOn(void)
{
	ym2612.PowerOn();
	ResetVariables();
}
void TownsSound::State::Reset(void)
{
	ym2612.Reset();
	ResetVariables();
}
void TownsSound::State::ResetVariables(void)
{
	muteFlag=0;
	addrLatch[0]=0;
	addrLatch[1]=0;
}


////////////////////////////////////////////////////////////

TownsSound::TownsSound(class FMTowns *townsPtr) : Device(townsPtr)
{
	this->townsPtr=townsPtr;
}
void TownsSound::SetOutsideWorld(class Outside_World *outside_world)
{
	this->outside_world=outside_world;
}

/* virtual */ void TownsSound::PowerOn(void)
{
	state.PowerOn();
}
/* virtual */ void TownsSound::Reset(void)
{
	state.Reset();
}
/* virtual */ void TownsSound::IOWriteByte(unsigned int ioport,unsigned int data)
{
	switch(ioport)
	{
	case TOWNSIO_SOUND_MUTE://              0x4D5, // [2] pp.18,
		state.muteFlag=data&3;
		break;
	case TOWNSIO_SOUND_STATUS_ADDRESS0://   0x4D8, // [2] pp.18,
		state.addrLatch[0]=data&0xff;
		break;
	case TOWNSIO_SOUND_DATA0://             0x4DA, // [2] pp.18,
		state.ym2612.WriteRegister(state.addrLatch[0],data);
		break;
	case TOWNSIO_SOUND_ADDRESS1://          0x4DC, // [2] pp.18,
		state.addrLatch[1]=data&0xff;
		break;
	case TOWNSIO_SOUND_DATA1://             0x4DE, // [2] pp.18,
		state.ym2612.WriteRegister(state.addrLatch[1],data);
		break;
	case TOWNSIO_SOUND_INT_REASON://        0x4E9, // [2] pp.19,
		break;
	case TOWNSIO_SOUND_PCM_INT_MASK://      0x4EA, // [2] pp.19,
		break;
	case TOWNSIO_SOUND_PCM_INT://           0x4EB, // [2] pp.19,
		break;
	case TOWNSIO_SOUND_PCM_ENV://           0x4F0, // [2] pp.19,
		state.rf5c68.WriteENV(data);
		break;
	case TOWNSIO_SOUND_PCM_PAN://           0x4F1, // [2] pp.19,
		state.rf5c68.WritePAN(data);
		break;
	case TOWNSIO_SOUND_PCM_FDL://           0x4F2, // [2] pp.19,
		state.rf5c68.WriteFDL(data);
		break;
	case TOWNSIO_SOUND_PCM_FDH://           0x4F3, // [2] pp.19,
		state.rf5c68.WriteFDH(data);
		break;
	case TOWNSIO_SOUND_PCM_LSL://           0x4F4, // [2] pp.19,
		state.rf5c68.WriteLSL(data);
		break;
	case TOWNSIO_SOUND_PCM_LSH://           0x4F5, // [2] pp.19,
		state.rf5c68.WriteLSH(data);
		break;
	case TOWNSIO_SOUND_PCM_ST://            0x4F6, // [2] pp.19,
		state.rf5c68.WriteST(data);
		break;
	case TOWNSIO_SOUND_PCM_CTRL://          0x4F7, // [2] pp.19,
		{
			auto chStartPlay=state.rf5c68.WriteControl(data);
			if(0!=chStartPlay && nullptr!=outside_world)
			{
printf("%s %d %02x\n",__FUNCTION__,__LINE__,chStartPlay);
				for(unsigned int ch=0; ch<RF5C68::NUM_CHANNELS; ++ch)
				{
					if(0!=(chStartPlay&(1<<ch)))
					{
						outside_world->PCMPlay(state.rf5c68,ch);
					}
				}
			}
		}
		break;
	case TOWNSIO_SOUND_PCM_CH_ON_OFF://     0x4F8, // [2] pp.19,
		{
			auto chStartPlay=state.rf5c68.WriteChannelOnOff(data);
			if(0!=chStartPlay && nullptr!=outside_world)
			{
printf("%s %d %02x\n",__FUNCTION__,__LINE__,chStartPlay);
				for(unsigned int ch=0; ch<RF5C68::NUM_CHANNELS; ++ch)
				{
					if(0!=(chStartPlay&(1<<ch)))
					{
						outside_world->PCMPlay(state.rf5c68,ch);
					}
				}
			}
		}
		break;
	}
}
/* virtual */ unsigned int TownsSound::IOReadByte(unsigned int ioport)
{
	unsigned char data=0xff;
	switch(ioport)
	{
	case TOWNSIO_SOUND_MUTE://              0x4D5, // [2] pp.18,
		data=state.muteFlag;
		break;
	case TOWNSIO_SOUND_STATUS_ADDRESS0://   0x4D8, // [2] pp.18,
		data=0b01111100;
		data|=(true==state.ym2612.TimerAUp() ? 1 : 0);
		data|=(true==state.ym2612.TimerBUp() ? 2 : 0);
		break;
	case TOWNSIO_SOUND_DATA0://             0x4DA, // [2] pp.18,
		// Write Only
		break;
	case TOWNSIO_SOUND_ADDRESS1://          0x4DC, // [2] pp.18,
		// Write Only
		break;
	case TOWNSIO_SOUND_DATA1://             0x4DE, // [2] pp.18,
		// Write Only
		break;
	case TOWNSIO_SOUND_INT_REASON://        0x4E9, // [2] pp.19,
		break;
	case TOWNSIO_SOUND_PCM_INT_MASK://      0x4EA, // [2] pp.19,
		break;
	case TOWNSIO_SOUND_PCM_INT://           0x4EB, // [2] pp.19,
		data=0; // No bank firing an interrupt at this time.
		break;
	case TOWNSIO_SOUND_PCM_ENV://           0x4F0, // [2] pp.19,
		break;
	case TOWNSIO_SOUND_PCM_PAN://           0x4F1, // [2] pp.19,
		break;
	case TOWNSIO_SOUND_PCM_FDL://           0x4F2, // [2] pp.19,
		break;
	case TOWNSIO_SOUND_PCM_FDH://           0x4F3, // [2] pp.19,
		break;
	case TOWNSIO_SOUND_PCM_LSL://           0x4F4, // [2] pp.19,
		break;
	case TOWNSIO_SOUND_PCM_LSH://           0x4F5, // [2] pp.19,
		break;
	case TOWNSIO_SOUND_PCM_ST://            0x4F6, // [2] pp.19,
		break;
	case TOWNSIO_SOUND_PCM_CTRL://          0x4F7, // [2] pp.19,
		break;
	case TOWNSIO_SOUND_PCM_CH_ON_OFF://     0x4F8, // [2] pp.19,
		break;
	}
	return data;
}



/* virtual */ void TownsSound::RunScheduledTask(unsigned long long int townsTime)
{
	state.ym2612.Run(townsTime);

	bool IRQ=(state.ym2612.TimerAUp() || state.ym2612.TimerBUp());
	townsPtr->pic.SetInterruptRequestBit(TOWNSIRQ_SOUND,IRQ);
}


std::vector <std::string> TownsSound::GetStatusText(void) const
{
	return state.ym2612.GetStatusText();
}
