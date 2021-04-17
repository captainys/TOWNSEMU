/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include <cstring>
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
void TownsSound::PCMStartPlay(unsigned char chStartPlay)
{
}
void TownsSound::PCMStopPlay(unsigned char chStopPlay)
{
	for(unsigned int ch=0; ch<RF5C68::NUM_CHANNELS; ++ch)
	{
		if(0!=(chStopPlay&(1<<ch)))
		{
			state.rf5c68.PlayStopped(ch);
		}
	}
}

/* virtual */ void TownsSound::PowerOn(void)
{
	state.PowerOn();
}
/* virtual */ void TownsSound::Reset(void)
{
	state.Reset();
	nextFMPCMWave.clear();
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
		{
			state.ym2612.WriteRegister(0,state.addrLatch[0],data);
		}
		break;
	case TOWNSIO_SOUND_ADDRESS1://          0x4DC, // [2] pp.18,
		state.addrLatch[1]=data&0xff;
		break;
	case TOWNSIO_SOUND_DATA1://             0x4DE, // [2] pp.18,
		{
			state.ym2612.WriteRegister(3,state.addrLatch[1],data);
		}
		break;
	case TOWNSIO_SOUND_INT_REASON://        0x4E9, // [2] pp.19,
		break;
	case TOWNSIO_SOUND_PCM_INT_MASK://      0x4EA, // [2] pp.19,
		state.rf5c68.WriteIRQBankMask(data);
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
			auto startStop=state.rf5c68.WriteControl(data);
			if(0!=startStop.chStartPlay && nullptr!=outside_world)
			{
				PCMStartPlay(startStop.chStartPlay);
			}
			if(0!=startStop.chStopPlay && nullptr!=outside_world)
			{
				PCMStopPlay(startStop.chStopPlay);
			}
		}
		break;
	case TOWNSIO_SOUND_PCM_CH_ON_OFF://     0x4F8, // [2] pp.19,
		{
			auto startStop=state.rf5c68.WriteChannelOnOff(data);
			if(0!=startStop.chStartPlay && nullptr!=outside_world)
			{
				PCMStartPlay(startStop.chStartPlay);
			}
			if(0!=startStop.chStopPlay && nullptr!=outside_world)
			{
				PCMStopPlay(startStop.chStopPlay);
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
	case TOWNSIO_SOUND_ADDRESS1://          0x4DC, // [2] pp.18,
		// Supposed to be Write Only
		// Sim City 2000 is waiting for bit7 of this byte to clear.
		// If address bus bit2 is not checked, it should return the same value as 4D8.
	case TOWNSIO_SOUND_STATUS_ADDRESS0://   0x4D8, // [2] pp.18,
		data=0b01111100;
		data|=(true==state.ym2612.TimerAUp() ? 1 : 0);
		data|=(true==state.ym2612.TimerBUp() ? 2 : 0);
		break;
	case TOWNSIO_SOUND_DATA0://             0x4DA, // [2] pp.18,
		// Write Only
		break;
	case TOWNSIO_SOUND_DATA1://             0x4DE, // [2] pp.18,
		// Write Only
		break;
	case TOWNSIO_SOUND_INT_REASON://        0x4E9, // [2] pp.19,
		data=0;
		if(state.rf5c68.state.IRQ())
		{
			data|=0b1000;
		}
		if((state.ym2612.TimerAUp() || state.ym2612.TimerBUp()))
		{
			data|=0b0001;
		}
		break;
	case TOWNSIO_SOUND_PCM_INT_MASK://      0x4EA, // [2] pp.19,
		break;
	case TOWNSIO_SOUND_PCM_INT://           0x4EB, // [2] pp.19,
		data=state.rf5c68.state.IRQBank;
		state.rf5c68.state.IRQBank=0;
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



void TownsSound::SoundPolling(unsigned long long int townsTime)
{
	state.ym2612.Run(townsTime);
	bool IRQ=(state.ym2612.TimerAUp() || state.ym2612.TimerBUp()) || state.rf5c68.state.IRQ();
	townsPtr->pic.SetInterruptRequestBit(TOWNSIRQ_SOUND,IRQ);
}

std::vector <std::string> TownsSound::GetStatusText(void) const
{
	return state.ym2612.GetStatusText();
}

void TownsSound::ProcessSound(void)
{
	if((0!=state.ym2612.state.playingCh || true==state.rf5c68.IsPlaying()) && nullptr!=outside_world)
	{
		if(true==nextFMPCMWave.empty())
		{
			if(0!=state.ym2612.state.playingCh)
			{
				nextFMPCMWave=state.ym2612.MakeWaveAllChannels(FM_PCM_MILLISEC_PER_WAVE);
			}
			if(true==state.rf5c68.IsPlaying())
			{
				const unsigned int WAVE_OUT_SAMPLING_RATE=YM2612::WAVE_SAMPLING_RATE; // Align with YM2612.
				unsigned int numSamples=0;
				if(true==nextFMPCMWave.empty()) // YM2612 not playing.
				{
					numSamples=FM_PCM_MILLISEC_PER_WAVE*WAVE_OUT_SAMPLING_RATE/1000;
					nextFMPCMWave.resize(numSamples*4);
					std::memset(nextFMPCMWave.data(),0,numSamples*4);
				}
				else
				{
					numSamples=(unsigned int)(nextFMPCMWave.size()/4);
				}

				state.rf5c68.AddWaveForNumSamples(nextFMPCMWave.data(),numSamples,WAVE_OUT_SAMPLING_RATE);

				for(unsigned int chNum=0; chNum<RF5C68::NUM_CHANNELS; ++chNum)
				{
					auto &ch=state.rf5c68.state.ch[chNum];
					if(true==ch.IRQAfterThisPlayBack)
					{
						state.rf5c68.SetIRQBank(ch.IRQBank);
						ch.IRQAfterThisPlayBack=false;
					}
				}
			}
		}

		if(true!=outside_world->FMPCMChannelPlaying() && true!=nextFMPCMWave.empty())
		{
			if(true==recordFMandPCM)
			{
				FMPCMrecording.insert(FMPCMrecording.end(),nextFMPCMWave.begin(),nextFMPCMWave.end());
			}
			outside_world->FMPCMPlay(nextFMPCMWave);
			nextFMPCMWave.clear(); // It was supposed to be cleared in FMPlay.  Just in case.
			state.ym2612.CheckToneDoneAllChannels();
		}
	}

	if (townsPtr->timer.IsBuzzerPlaying()) {
		if (!outside_world->BeepChannelPlaying()) {
			auto r = townsPtr->timer.MakeBuzzerWave(BEEP_MILLISEC_PER_WAVE);
			outside_world->BeepPlay(r.first, r.second);
		}
	}
}

void TownsSound::StartRecording(void)
{
	recordFMandPCM=true;
	FMPCMrecording.clear();
}
void TownsSound::EndRecording(void)
{
	recordFMandPCM=false;
}
#include "yssimplesound.h"
void TownsSound::SaveRecording(std::string fName) const
{
	YsSoundPlayer::SoundData data;
	auto dataCopy=FMPCMrecording;
	data.CreateFromSigned16bitStereo(44100,dataCopy);
	auto wavFile=data.MakeWavByteData();
	cpputil::WriteBinaryFile(fName,wavFile.size(),wavFile.data());
}



void TownsSound::SerializeYM2612(std::vector <unsigned char> &data) const
{
	auto &ym2612=state.ym2612;

	PushBool(data,ym2612.state.LFO);
	PushUint32(data,ym2612.state.FREQCTRL);
	PushUint64(data,ym2612.state.deviceTimeInNS);
	PushUint64(data,ym2612.state.lastTickTimeInNS);
	for(auto &ch : ym2612.state.channels)
	{
		PushUint32(data,ch.F_NUM);
		PushUint32(data,ch.BLOCK);
		PushUint32(data,ch.FB);
		PushUint32(data,ch.CONNECT);
		PushUint32(data,ch.L);
		PushUint32(data,ch.R);
		PushUint32(data,ch.AMS);
		PushUint32(data,ch.PMS);
		PushUint32(data,ch.usingSlot);

		for(auto &sl : ch.slots)
		{
			PushUint32(data,sl.DT);
			PushUint32(data,sl.MULTI);
			PushUint32(data,sl.TL);
			PushUint32(data,sl.KS);
			PushUint32(data,sl.AR);
			PushUint32(data,sl.AM);
			PushUint32(data,sl.DR);
			PushUint32(data,sl.SR);
			PushUint32(data,sl.SL);
			PushUint32(data,sl.RR);
			PushUint32(data,sl.SSG_EG);

			// Cache for wave-generation >>
			PushUint64(data,sl.microsecS12);
			PushUint64(data,sl.toneDurationMicrosecS12);
			PushUint32(data,sl.phaseS12);
			PushUint32(data,sl.phaseS12Step);
			PushUint32(data,sl.env[0]);
			PushUint32(data,sl.env[1]);
			PushUint32(data,sl.env[2]);
			PushUint32(data,sl.env[3]);
			PushUint32(data,sl.env[4]);
			PushUint32(data,sl.env[5]);
			PushUint32(data,sl.envDurationCache);
			PushBool(data,sl.InReleasePhase);
			PushUint32(data,sl.ReleaseStartTime);
			PushUint32(data,sl.ReleaseEndTime);
			PushUint32(data,sl.ReleaseStartDbX100);
			PushUint32(data,sl.lastDbX100Cache);
			// Cache for wave-generation <<
		}

		PushUint32(data,ch.playState);
		PushInt32(data,ch.lastSlot0Out[0]);
		PushInt32(data,ch.lastSlot0Out[1]);
	}
	PushUint32(data,ym2612.state.F_NUM_3CH[0]);
	PushUint32(data,ym2612.state.F_NUM_3CH[1]);
	PushUint32(data,ym2612.state.F_NUM_3CH[2]);
	PushUint32(data,ym2612.state.BLOCK_3CH[0]);
	PushUint32(data,ym2612.state.BLOCK_3CH[1]);
	PushUint32(data,ym2612.state.BLOCK_3CH[2]);
	PushUint32(data,ym2612.state.F_NUM_6CH[0]);
	PushUint32(data,ym2612.state.F_NUM_6CH[1]);
	PushUint32(data,ym2612.state.F_NUM_6CH[2]);
	PushUint32(data,ym2612.state.BLOCK_6CH[0]);
	PushUint32(data,ym2612.state.BLOCK_6CH[1]);
	PushUint32(data,ym2612.state.BLOCK_6CH[2]);
	PushUcharArray(data,256,ym2612.state.regSet[0]);
	PushUcharArray(data,256,ym2612.state.regSet[1]);
	PushUint64(data,ym2612.state.timerCounter[0]);
	PushUint64(data,ym2612.state.timerCounter[1]);
	PushBool(data,ym2612.state.timerUp[0]);
	PushBool(data,ym2612.state.timerUp[1]);
	PushUint32(data,ym2612.state.playingCh);
	PushInt32(data,ym2612.state.volume);
}
void TownsSound::DeserializeYM2612(const unsigned char *&data,unsigned int version)
{
	auto &ym2612=state.ym2612;

	ym2612.state.LFO=ReadBool(data);
	ym2612.state.FREQCTRL=ReadUint32(data);
	ym2612.state.deviceTimeInNS=ReadUint64(data);
	ym2612.state.lastTickTimeInNS=ReadUint64(data);
	for(auto &ch : ym2612.state.channels)
	{
		ch.F_NUM=ReadUint32(data);
		ch.BLOCK=ReadUint32(data);
		ch.FB=ReadUint32(data);
		ch.CONNECT=ReadUint32(data);
		ch.L=ReadUint32(data);
		ch.R=ReadUint32(data);
		ch.AMS=ReadUint32(data);
		ch.PMS=ReadUint32(data);
		ch.usingSlot=ReadUint32(data);

		for(auto &sl : ch.slots)
		{
			sl.DT=ReadUint32(data);
			sl.MULTI=ReadUint32(data);
			sl.TL=ReadUint32(data);
			sl.KS=ReadUint32(data);
			sl.AR=ReadUint32(data);
			sl.AM=ReadUint32(data);
			sl.DR=ReadUint32(data);
			sl.SR=ReadUint32(data);
			sl.SL=ReadUint32(data);
			sl.RR=ReadUint32(data);
			sl.SSG_EG=ReadUint32(data);

			// Cache for wave-generation >>
			sl.microsecS12=ReadUint64(data);
			sl.toneDurationMicrosecS12=ReadUint64(data);
			sl.phaseS12=ReadUint32(data);
			sl.phaseS12Step=ReadUint32(data);
			sl.env[0]=ReadUint32(data);
			sl.env[1]=ReadUint32(data);
			sl.env[2]=ReadUint32(data);
			sl.env[3]=ReadUint32(data);
			sl.env[4]=ReadUint32(data);
			sl.env[5]=ReadUint32(data);
			sl.envDurationCache=ReadUint32(data);
			sl.InReleasePhase=ReadBool(data);
			sl.ReleaseStartTime=ReadUint32(data);
			sl.ReleaseEndTime=ReadUint32(data);
			sl.ReleaseStartDbX100=ReadUint32(data);
			sl.lastDbX100Cache=ReadUint32(data);
			// Cache for wave-generation <<
		}

		ch.playState=ReadUint32(data);
		ch.lastSlot0Out[0]=ReadInt32(data);
		ch.lastSlot0Out[1]=ReadInt32(data);
	}
	ym2612.state.F_NUM_3CH[0]=ReadUint32(data);
	ym2612.state.F_NUM_3CH[1]=ReadUint32(data);
	ym2612.state.F_NUM_3CH[2]=ReadUint32(data);
	ym2612.state.BLOCK_3CH[0]=ReadUint32(data);
	ym2612.state.BLOCK_3CH[1]=ReadUint32(data);
	ym2612.state.BLOCK_3CH[2]=ReadUint32(data);
	ym2612.state.F_NUM_6CH[0]=ReadUint32(data);
	ym2612.state.F_NUM_6CH[1]=ReadUint32(data);
	ym2612.state.F_NUM_6CH[2]=ReadUint32(data);
	ym2612.state.BLOCK_6CH[0]=ReadUint32(data);
	ym2612.state.BLOCK_6CH[1]=ReadUint32(data);
	ym2612.state.BLOCK_6CH[2]=ReadUint32(data);
	ReadUcharArray(data,256,ym2612.state.regSet[0]);
	if(1<=version)
	{
		ReadUcharArray(data,256,ym2612.state.regSet[1]);
	}
	ym2612.state.timerCounter[0]=ReadUint64(data);
	ym2612.state.timerCounter[1]=ReadUint64(data);
	ym2612.state.timerUp[0]=ReadBool(data);
	ym2612.state.timerUp[1]=ReadBool(data);
	ym2612.state.playingCh=ReadUint32(data);
	ym2612.state.volume=ReadInt32(data);
}
void TownsSound::SerializeRF5C68(std::vector <unsigned char> &data) const
{
	auto &rf5c68=state.rf5c68;

	PushUcharArray(data,rf5c68.state.waveRAM);
	for(auto &ch : rf5c68.state.ch)
	{
		PushUint16(data,ch.ENV);
		PushUint16(data,ch.PAN);
		PushUint16(data,ch.ST);
		PushUint16(data,ch.FD);
		PushUint16(data,ch.LS);

		PushUint16(data,ch.playPtr);
		PushBool(data,ch.repeatAfterThisSegment);
		PushBool(data,ch.IRQAfterThisPlayBack);
		PushUint16(data,ch.IRQBank);
	}
	PushBool(data,rf5c68.state.playing);
	PushUint16(data,rf5c68.state.Bank);
	PushUint16(data,rf5c68.state.CB);
	PushUint16(data,rf5c68.state.chOnOff);
	PushInt32(data,rf5c68.state.timeBalance);
	PushUint16(data,rf5c68.state.IRQBank);
	PushUint16(data,rf5c68.state.IRQBankMask);
	PushInt32(data,rf5c68.state.volume);
}
void TownsSound::DeserializeRF5C68(const unsigned char *&data)
{
	auto &rf5c68=state.rf5c68;

	rf5c68.state.waveRAM=ReadUcharArray(data);
	for(auto &ch : rf5c68.state.ch)
	{
		ch.ENV=ReadUint16(data);
		ch.PAN=ReadUint16(data);
		ch.ST =ReadUint16(data);
		ch.FD =ReadUint16(data);
		ch.LS =ReadUint16(data);

		ch.playPtr=ReadUint16(data);
		ch.repeatAfterThisSegment=ReadBool(data);
		ch.IRQAfterThisPlayBack=ReadBool(data);
		ch.IRQBank=ReadUint16(data);
	}
	rf5c68.state.playing=ReadBool(data);
	rf5c68.state.Bank=ReadUint16(data);
	rf5c68.state.CB=ReadUint16(data);
	rf5c68.state.chOnOff=ReadUint16(data);
	rf5c68.state.timeBalance=ReadInt32(data);
	rf5c68.state.IRQBank=ReadUint16(data);
	rf5c68.state.IRQBankMask=ReadUint16(data);
	rf5c68.state.volume=ReadInt32(data);
}



/* virtual */ uint32_t TownsSound::SerializeVersion(void) const
{
	return 1;
}
/* virtual */ void TownsSound::SpecificSerialize(std::vector <unsigned char> &data,std::string stateFName) const
{
	PushUint32(data,state.muteFlag);
	PushUint32(data,state.addrLatch[0]);
	PushUint32(data,state.addrLatch[1]);
	SerializeYM2612(data);
	SerializeRF5C68(data);
}
/* virtual */ bool TownsSound::SpecificDeserialize(const unsigned char *&data,std::string stateFName,uint32_t version)
{
	state.muteFlag=ReadUint32(data);
	state.addrLatch[0]=ReadUint32(data);
	state.addrLatch[1]=ReadUint32(data);
	DeserializeYM2612(data,version);
	DeserializeRF5C68(data);
	return true;
}
