/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#ifndef HEALESS_MODE_H_IS_INCLUDED

#include "outside_world.h"
#include <vector>
#include <thread>
#include <mutex>


class Headless_Mode : public Outside_World
{
public:
	Headless_Mode();
	~Headless_Mode();

	std::string GetProgramResourceDirectory(void) const override;

	void Start(void) override;
	void Stop(void) override;
	void DevicePolling(class FMTownsCommon &towns) override;

	bool ImageNeedsFlip(void) override;
	void SetKeyboardLayout(unsigned int layout) override;

	class NoWindowConnection : public WindowInterface
	{
	public:
		void Start(void) override;
		void Stop(void) override;
		/*! Called from the Window thread.
		*/
		void Interval(void) override;
		/*! Called from the Window thread.
		      VM thread may access scaling, dx, dy, and lowerRightIcon, which therefore must be locked.
		*/
		void Render(bool swapBuffers) override;
		void UpdateImage(TownsRender::ImageCopy &img) override;
		void Communicate(Outside_World *) override;
	};

	WindowInterface *CreateWindowInterface(void) const override;
	void DeleteWindowInterface(WindowInterface *) const override;

	class NoSoundConnection : public Sound
	{
	public:
		std::chrono::time_point<std::chrono::high_resolution_clock> FMPCMReadyTime,BeepReadyTime;

		NoSoundConnection();

		void Start(void) override;
		void Stop(void) override;

		void Polling(void) override;

		/*! Left level and right level can be 0 to 256.  Value above 256 will be rounded to 256.
		*/
		void CDDAPlay(const DiscImage &discImg,DiscImage::MinSecFrm from,DiscImage::MinSecFrm to,bool repeat,unsigned int,unsigned int) override;
		void CDDASetVolume(float leftVol,float rightVol) override;
		void CDDAStop(void) override;
		void CDDAPause(void) override;
		void CDDAResume(void) override;
		bool CDDAIsPlaying(void) override;
		DiscImage::MinSecFrm CDDACurrentPosition(void) override;

	public:
		void FMPCMPlay(std::vector <unsigned char > &wave) override;
		void FMPCMPlayStop(void) override;
		bool FMPCMChannelPlaying(void) override;

	public:
		void BeepPlay(int samplingRate, std::vector<unsigned char>& wave) override;
		void BeepPlayStop() override;
		bool BeepChannelPlaying() const override;
	};
	Sound *CreateSound(void) const override;
	void DeleteSound(Sound *) const override;
};

#endif
