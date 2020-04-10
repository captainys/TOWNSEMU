/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include <iostream>
#include <vector>
#include <string>

#include "cpputil.h"
#include "discimg.h"
#include "yssimplesound.h"



int main(int argc,char *argv[])
{
	if(argc!=2)
	{
		printf("Usage: BINCUE2WAV filename.cue\n");
		return 1;
	}

	DiscImage img;
	if(DiscImage::ERROR_NOERROR==img.Open(argv[1]))
	{
		int trkNum=1;
		for(auto t : img.GetTracks())
		{
			if(DiscImage::TRACK_AUDIO==t.trackType)
			{
				std::cout << "Track " << trkNum << std::endl;

				auto wave=img.GetWave(t.start,t.end);
				if(0<wave.size())
				{
					YsSoundPlayer::SoundData data;
					data.CreateFromSigned16bitStereo(44100,wave);
					auto wavFormat=data.MakeWavByteData();

					std::string fName;
					fName="TRACK";
					std::string number=cpputil::Itoa(trkNum);
					if(1==number.size())
					{
						fName.push_back('0');
					}
					fName+=number;
					fName+=".wav";

					cpputil::WriteBinaryFile(fName,wavFormat.size(),wavFormat.data());
				}
			}
			++trkNum;
		}
		return 0;
	}
	else
	{
		fprintf(stderr,"Cannot open a disc image.\n");
		return 1;
	}
}
