/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#ifndef FSSIMPLEWINDOW_CONNECTION_IS_INCLUDED
#define FSSIMPLEWINDOW_CONNECTION_IS_INCLUDED
/* { */

#include "outside_world.h"
#include "yssimplesound.h"
#include "ysgamepad.h"
#include <vector>

class FsSimpleWindowConnection : public Outside_World
{
public:
	#define PAUSE_KEY_CODE FSKEY_SCROLLLOCK

	bool gamePadInitialized=false;
	unsigned int *FSKEYtoTownsKEY=nullptr;
	unsigned int *FSKEYState=nullptr;
	FsSimpleWindowConnection();
	~FsSimpleWindowConnection();

	std::vector <struct YsGamePadReading> gamePads,prevGamePads;

	// For mouse emulation by pad digital axes.
	int mouseDX=0,mouseDY=0;

	int winWid=640,winHei=480;
	unsigned int sinceLastResize=0;


	virtual std::vector <std::string> MakeKeyMappingText(void) const;
	virtual void LoadKeyMappingFromText(const std::vector <std::string> &text);

	virtual void Start(void);
	virtual void Stop(void);
	virtual void DevicePolling(class FMTowns &towns);
	void PollGamePads(void);
	virtual void UpdateStatusBitmap(class FMTowns &towns);
	virtual void Render(const TownsRender::Image &img,const class FMTowns &towns);
	virtual bool ImageNeedsFlip(void);

	virtual void SetKeyboardLayout(unsigned int layout);


	YsSoundPlayer soundPlayer;
	YsSoundPlayer::SoundData cddaChannel;
	unsigned long long cddaStartHSG;
	virtual void CDDAPlay(const DiscImage &discImg,DiscImage::MinSecFrm from,DiscImage::MinSecFrm to,bool repeat,unsigned int leftLevel,unsigned int rightLevel);
	virtual void CDDAStop(void);
	virtual void CDDAPause(void);
	virtual void CDDAResume(void);
	virtual bool CDDAIsPlaying(void);
	virtual DiscImage::MinSecFrm CDDACurrentPosition(void);


	YsSoundPlayer::SoundData PCMChannel;
	virtual void PCMPlay(std::vector <unsigned char > &wave);
	virtual void PCMPlayStop(void);
	virtual bool PCMChannelPlaying(void);


	YsSoundPlayer::SoundData FMChannel;
	virtual void FMPlay(std::vector <unsigned char> &wave);
	virtual void FMPlayStop(void);
	virtual bool FMChannelPlaying(void);

	YsSoundPlayer::SoundData BeepChannel;
	virtual void BeepPlay(int samplingRate, std::vector<unsigned char> &wave);
	virtual void BeepPlayStop();
	virtual bool BeepChannelPlaying() const;
};

/* } */
#endif
