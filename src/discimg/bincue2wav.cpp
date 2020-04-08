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
