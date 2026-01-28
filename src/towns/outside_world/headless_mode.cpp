/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */



#include <chrono>
#include "headless_mode.h"


Headless_Mode::Headless_Mode()
{
	SetKeyboardMode(TOWNS_KEYBOARD_MODE_DIRECT);
	SetKeyboardLayout(KEYBOARD_LAYOUT_US);
}

Headless_Mode::~Headless_Mode()
{
}


std::string Headless_Mode::GetProgramResourceDirectory(void) const
{
	return ".";
}


void Headless_Mode::Start(void)
{
}

void Headless_Mode::Stop(void)
{
}

void Headless_Mode::DevicePolling(class FMTownsCommon &towns)
{
}


bool Headless_Mode::ImageNeedsFlip(void)
{
	return false;
}

void Headless_Mode::SetKeyboardLayout(unsigned int layout)
{
}



////////////////////////////////////////////////////////////
// Window
////////////////////////////////////////////////////////////

void Headless_Mode::NoWindowConnection::Start(void)
{
}

void Headless_Mode::NoWindowConnection::Stop(void)
{
}

/*! Called from the Window thread.
*/
void Headless_Mode::NoWindowConnection::Interval(void)
{
	BaseInterval();
	{
		std::lock_guard <std::mutex> lock(deviceStateLock);
		winThr.VMClosed=shared.VMClosedFromVMThread; // At least this update is needed.
	}
}

/*! Called from the Window thread.
      VM thread may access scaling, dx, dy, and lowerRightIcon, which therefore must be locked.
*/
void Headless_Mode::NoWindowConnection::Render(bool swapBuffers)
{
}

void Headless_Mode::NoWindowConnection::UpdateImage(TownsRender::ImageCopy &img)
{
}

void Headless_Mode::NoWindowConnection::Communicate(Outside_World *)
{
}



Outside_World::WindowInterface *Headless_Mode::CreateWindowInterface(void) const
{
	return new NoWindowConnection;
}
void Headless_Mode::DeleteWindowInterface(WindowInterface *itfc) const
{
	delete itfc;
}


////////////////////////////////////////////////////////////
//Sound
////////////////////////////////////////////////////////////

void Headless_Mode::NoSoundConnection::Start(void)
{
}

void Headless_Mode::NoSoundConnection::Stop(void)
{
}


void Headless_Mode::NoSoundConnection::Polling(void)
{
}


/*! Left level and right level can be 0 to 256.  Value above 256 will be rounded to 256.
*/
Headless_Mode::NoSoundConnection::NoSoundConnection()
{
	FMPCMReadyTime=std::chrono::high_resolution_clock::now();
	BeepReadyTime=std::chrono::high_resolution_clock::now();
}

void Headless_Mode::NoSoundConnection::CDDAPlay(const DiscImage &discImg,DiscImage::MinSecFrm from,DiscImage::MinSecFrm to,bool repeat,unsigned int,unsigned int)
{
}

void Headless_Mode::NoSoundConnection::CDDASetVolume(float leftVol,float rightVol)
{
}

void Headless_Mode::NoSoundConnection::CDDAStop(void)
{
}

void Headless_Mode::NoSoundConnection::CDDAPause(void)
{
}

void Headless_Mode::NoSoundConnection::CDDAResume(void)
{
}

bool Headless_Mode::NoSoundConnection::CDDAIsPlaying(void)
{
	return false;
}

DiscImage::MinSecFrm Headless_Mode::NoSoundConnection::CDDACurrentPosition(void)
{
	DiscImage::MinSecFrm msf;
	msf.FromHSG(0);
	return msf;
}


void Headless_Mode::NoSoundConnection::FMPCMPlay(std::vector <unsigned char > &wave)
{
	auto num_samples=wave.size()/4;
	auto microsec=10000*num_samples/441; // =1000000*num_samples/44100;
	FMPCMReadyTime=std::chrono::high_resolution_clock::now()+std::chrono::microseconds(microsec);
}

void Headless_Mode::NoSoundConnection::FMPCMPlayStop(void)
{
}

bool Headless_Mode::NoSoundConnection::FMPCMChannelPlaying(void)
{
	return std::chrono::high_resolution_clock::now()<FMPCMReadyTime;
}


void Headless_Mode::NoSoundConnection::BeepPlay(int samplingRate, std::vector<unsigned char>& wave)
{
	auto num_samples=wave.size()/4;
	auto microsec=10000*num_samples/441; // =1000000*num_samples/44100;
	BeepReadyTime=std::chrono::high_resolution_clock::now()+std::chrono::microseconds(microsec);
}

void Headless_Mode::NoSoundConnection::BeepPlayStop()
{
}

bool Headless_Mode::NoSoundConnection::BeepChannelPlaying() const
{
	return std::chrono::high_resolution_clock::now()<BeepReadyTime;
}



Outside_World::Sound *Headless_Mode::CreateSound(void) const
{
	return new NoSoundConnection;
}
void Headless_Mode::DeleteSound(Sound *itfc) const
{
	delete itfc;
}
