/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#ifndef SOUND_IS_INCLUDED
#define SOUND_IS_INCLUDED
/* { */

#include <vector>
#include <string>

#include "vgmrecorder.h"

#include "device.h"
#include "townsdef.h"
#include "cpputil.h"
#include "ym2612.h"
#include "rf5c68.h"
#include "yssimplesound.h"
#include "outside_world.h"
#include "cdrom.h"

class TownsSound : public Device
{
private:
	class FMTownsCommon *townsPtr;
public:
	enum
	{
#if !defined(__linux__) && !defined(__linux)
		FM_PCM_MILLISEC_PER_WAVE=20, // Looks like time resolution of Wave Playback of Direct Sound is 10ms.  And, it needs to be 10 times integer.
		BEEP_MILLISEC_PER_WAVE=20,
		WAVE_STREAMING_SAFETY_BUFFER=10,
#else
		FM_PCM_MILLISEC_PER_WAVE=40, // Maybe because I am developing on VirtualBox, I am getting outrageously slow latency of 80ms (40ms*2).
		BEEP_MILLISEC_PER_WAVE=40,
		WAVE_STREAMING_SAFETY_BUFFER=10,
#endif
		MILLISEC_PER_WAVE_GENERATION=4,

		RINGBUFFER_CLEAR_TIME=2000000000,  // Run 2 seconds after last wave generation to clear the ring buffer.  1 second should be enough, but just to be absolutely sure.
	};

	virtual const char *DeviceName(void) const{return "SOUND";}

	class State
	{
	public:
		YM2612 ym2612;
		unsigned int muteFlag;
		unsigned int addrLatch[2];

		RF5C68 rf5c68;

		void PowerOn(void);
		void Reset(void);
		void ResetVariables(void);
	};
	class Variable
	{
	public:
		uint64_t nextPCMSampleReadyTime=0;
		const uint32_t PCMSamplingScale=2;
		const uint64_t PCMSamplingRate=19200;
		uint64_t PCMSamplePlayed=0,PCMSamplePointer=0,PCMSamplePointerLeftOver=0;
		YsSoundPlayer::SoundData waveToBeSentToVM;
		VGMRecorder vgmRecorder;
	};

	State state;
	Variable var;
	class Outside_World::Sound *outside_world=nullptr;
	class TownsCDROM *cdrom=nullptr;

	// Since 2023/09/17, CDDA is played from the same channel as FM and PCM.
	// Although the variables say FMPCM, it also includes CDDA.
	bool recordFMandPCM=false;
	std::vector <unsigned char> FMPCMrecording;

	uint64_t nextFMPCMWaveFilledInMillisec=0;
	uint64_t nextFMPCMWaveGenTime=0;
	uint64_t lastFMPCMWaveGenTime=0;
	std::vector <unsigned char> nextFMPCMWave;

	inline bool IsFMPlaying(void) const
	{
		return 0!=state.ym2612.state.playingCh;
	}
	inline bool IsPCMPlaying(void) const
	{
		return true==state.rf5c68.IsPlaying();
	}
	inline bool IsPCMRecording(void) const
	{
		return var.PCMSamplePlayed<var.waveToBeSentToVM.GetNumSamplePerChannel() &&
		       var.PCMSamplePlayed<var.PCMSamplePointer;
	}

	TownsSound(class FMTownsCommon *townsPtr);
	void SetOutsideWorld(class Outside_World::Sound *outside_world);
	void SetCDROMPointer(class TownsCDROM *cdrom);
	void PCMStartPlay(unsigned char chStartPlay);
	void PCMStopPlay(unsigned char chStopPlay);
	void PCMPausePlay(unsigned char chStopPlay);

	virtual void PowerOn(void);
	virtual void Reset(void);

	virtual void IOWriteByte(unsigned int ioport,unsigned int data);

	virtual unsigned int IOReadByte(unsigned int ioport);

	/*! Called from FMTownsCommon::RunFastDevicePolling.
	*/
	void SoundPolling(unsigned long long int townsTime);

	std::vector <std::string> GetStatusText(void) const;

	/*! Call this function periodically to continue sound playback.
	*/
	void ProcessSound(void);

	/*! Call this function periodically while VM is paused.
	*/
	void ProcessSilence(void);

	/*! Loads .WAV file to be sent to PCM sampling register.
	*/
	bool LoadWav(std::string wav);

	void StartRecording(void);
	void EndRecording(void);
	void SaveRecording(std::string fName) const;

	void StartVGMRecording(void);
	void EndVGMRecording(void);
	void SaveVGMRecording(std::string fName) const;

	void SerializeYM2612(std::vector <unsigned char> &data) const;
	void DeserializeYM2612(const unsigned char *&data,unsigned int version);
	void SerializeRF5C68(std::vector <unsigned char> &data) const;
	void DeserializeRF5C68(const unsigned char *&data);

	virtual uint32_t SerializeVersion(void) const;
	virtual void SpecificSerialize(std::vector <unsigned char> &data,std::string stateFName) const;
	virtual bool SpecificDeserialize(const unsigned char *&data,std::string stateFName,uint32_t version);
};

/* } */
#endif
