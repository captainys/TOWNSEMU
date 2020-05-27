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

class FsSimpleWindowConnection : public Outside_World
{
public:
	unsigned int *FSKEYtoTownsKEY=nullptr;
	unsigned int *FSKEYState=nullptr;
	FsSimpleWindowConnection();
	~FsSimpleWindowConnection();

	virtual void Start(void);
	virtual void Stop(void);
	virtual void DevicePolling(class FMTowns &towns);
	virtual void UpdateStatusBitmap(class FMTowns &towns);
	virtual void Render(const TownsRender::Image &img);

	virtual void SetKeyboardLayout(unsigned int layout);


	YsSoundPlayer soundPlayer;
	YsSoundPlayer::SoundData cddaChannel;
	unsigned long long cddaStartHSG;
	virtual void CDDAPlay(const DiscImage &discImg,DiscImage::MinSecFrm from,DiscImage::MinSecFrm to);
	virtual void CDDAStop(void);
	virtual void CDDAPause(void);
	virtual void CDDAResume(void);
	virtual bool CDDAIsPlaying(void);
	virtual DiscImage::MinSecFrm CDDACurrentPosition(void);


	YsSoundPlayer::SoundData PCMChannel[RF5C68::NUM_CHANNELS];
	virtual void PCMPlay(RF5C68 &pcm,unsigned int ch);
	virtual void PCMPlayStop(int ch);
	virtual bool PCMChannelPlaying(int ch);
	virtual double PCMCurrentPosition(int ch);


	YsSoundPlayer::SoundData FMChannel[YM2612::NUM_CHANNELS];
	virtual void FMPlay(unsigned int ch,std::vector <unsigned char> &wave);
	virtual void FMPlayStop(int ch);
	virtual bool FMChannelPlaying(int ch);
};

/* } */
#endif
