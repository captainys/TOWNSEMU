#include <fstream>
#include <iostream>
#include <chrono>

#include "fssimplewindow.h"

#include "ym2612.h"
#include "yssimplesound.h"
#include "vgm.h"

const int num_channels=6;
const int time_precision_millisec=10;
const int playBackRate=44100;

YsSoundPlayer soundPlayer;
YsSoundPlayer::SoundData FMChannel[num_channels];

bool Play(const char fName[])
{
	std::vector <unsigned char> dat;
	std::ifstream fp(fName,std::ifstream::binary);
	if(true==fp.is_open())
	{
		fp.seekg(0,fp.end);
		auto length=fp.tellg();
		fp.seekg(0,fp.beg);

		dat.resize(length);
		fp.read((char *)dat.data(),length);

		fp.close();
	}

	std::cout << "LENGTH:" << dat.size() << std::endl;

	auto waitUntil=std::chrono::high_resolution_clock::now();

	FsPollDevice();
	while(FSKEY_NULL==FsInkey())
	{
		FsPollDevice();
	}

	YM2612 ym2612;
	ym2612.PowerOn();
	size_t playPtr=256; // For now just ignore the header.
	while(playPtr<dat.size())
	{
		FsPollDevice();

		if(0!=ym2612.state.playingCh)
		{
			for(int chNum=0; chNum<num_channels; ++chNum)
			{
				if(0!=(ym2612.state.playingCh&(1<<chNum)) &&
				   YSTRUE!=soundPlayer.IsPlaying(FMChannel[chNum]))
				{
					auto wave=ym2612.MakeWave(chNum,time_precision_millisec);
					FMChannel[chNum].CreateFromSigned16bitStereo(playBackRate,wave);
					soundPlayer.PlayOneShot(FMChannel[chNum]);
					ym2612.CheckToneDone(chNum);
				}
			}
		}

		if(std::chrono::high_resolution_clock::now()<waitUntil)
		{
			continue;
		}

		switch(dat[playPtr])
		{
		case VGMCMD_YM2612_PORT0: //0x52,
		case VGMCMD_YM2612_PORT1: //0x53,
			if(playPtr+2<dat.size())
			{
				auto channelBase=(dat[playPtr]&1)*3;
				auto keyOnCh=ym2612.WriteRegister(channelBase,dat[playPtr+1],dat[playPtr+2],0);
				if(0<=keyOnCh && keyOnCh<num_channels)
				{
					auto wave=ym2612.MakeWave(keyOnCh,time_precision_millisec);
					FMChannel[keyOnCh].CreateFromSigned16bitStereo(playBackRate,wave);
					soundPlayer.PlayOneShot(FMChannel[keyOnCh]);
				}
				playPtr+=3;
			}
			else
			{
				std::cout << "Missing parameter: " << (unsigned int)dat[playPtr] << std::endl;
				return false;
			}
			break;
		case VGMCMD_YM2612_PORT0_WAIT0: //0x70,
		case VGMCMD_YM2612_PORT0_WAIT1: //0x71,
		case VGMCMD_YM2612_PORT0_WAIT2: //0x72,
		case VGMCMD_YM2612_PORT0_WAIT3: //0x73,
		case VGMCMD_YM2612_PORT0_WAIT4: //0x74,
		case VGMCMD_YM2612_PORT0_WAIT5: //0x75,
		case VGMCMD_YM2612_PORT0_WAIT6: //0x76,
		case VGMCMD_YM2612_PORT0_WAIT7: //0x77,
		case VGMCMD_YM2612_PORT0_WAIT8: //0x78,
		case VGMCMD_YM2612_PORT0_WAIT9: //0x79,
		case VGMCMD_YM2612_PORT0_WAITA: //0x7A,
		case VGMCMD_YM2612_PORT0_WAITB: //0x7B,
		case VGMCMD_YM2612_PORT0_WAITC: //0x7C,
		case VGMCMD_YM2612_PORT0_WAITD: //0x7D,
		case VGMCMD_YM2612_PORT0_WAITE: //0x7E,
		case VGMCMD_YM2612_PORT0_WAITF: //0x7F,
			{
				unsigned int microsec=(dat[playPtr]&0x0F)+1;
				microsec*=1000000;
				microsec/=44100;
				waitUntil=std::chrono::high_resolution_clock::now()+std::chrono::microseconds(microsec);
				++playPtr;
			}
			break;

		case VGMCMD_YM2612_PORT0_2A_WAIT0: //0x80,
		case VGMCMD_YM2612_PORT0_2A_WAIT1: //0x81,
		case VGMCMD_YM2612_PORT0_2A_WAIT2: //0x82,
		case VGMCMD_YM2612_PORT0_2A_WAIT3: //0x83,
		case VGMCMD_YM2612_PORT0_2A_WAIT4: //0x84,
		case VGMCMD_YM2612_PORT0_2A_WAIT5: //0x85,
		case VGMCMD_YM2612_PORT0_2A_WAIT6: //0x86,
		case VGMCMD_YM2612_PORT0_2A_WAIT7: //0x87,
		case VGMCMD_YM2612_PORT0_2A_WAIT8: //0x88,
		case VGMCMD_YM2612_PORT0_2A_WAIT9: //0x89,
		case VGMCMD_YM2612_PORT0_2A_WAITA: //0x8A,
		case VGMCMD_YM2612_PORT0_2A_WAITB: //0x8B,
		case VGMCMD_YM2612_PORT0_2A_WAITC: //0x8C,
		case VGMCMD_YM2612_PORT0_2A_WAITD: //0x8D,
		case VGMCMD_YM2612_PORT0_2A_WAITE: //0x8E,
		case VGMCMD_YM2612_PORT0_2A_WAITF: //0x8F,
			std::cout << "Not implemented yet:" << (unsigned int)dat[playPtr] << std::endl;
			return 0;
			break;
		case VGMCMD_WAIT_N_SAMPLES: //0x61,
			if(playPtr+2<dat.size())
			{
				unsigned long long int microsec;
				microsec=(dat[playPtr+1]|(dat[playPtr+2]<<8));
				microsec*=1000000;
				microsec/=44100;
				waitUntil=std::chrono::high_resolution_clock::now()+std::chrono::microseconds(microsec);
				playPtr+=3;
			}
			else
			{
				std::cout << "Missing parameter: " << (unsigned int)dat[playPtr] << std::endl;
				return false;
			}
			break;
		case VGMCMD_WAIT_735_SAMPLES: //0x62,
			waitUntil=std::chrono::high_resolution_clock::now()+std::chrono::microseconds(1000000/60);
			++playPtr;
			break;
		case VGMCMD_WAIT_882_SAMPLES: //0x63,
			waitUntil=std::chrono::high_resolution_clock::now()+std::chrono::microseconds(1000000/50);
			++playPtr;
			break;
		case VGMCMD_END_OF_DATA: //0x66
			return true;
		default:
			std::cout << "Command not implemented. " << (unsigned int)dat[playPtr] << std::endl;
			return false;
		}
	}
	return true;
}

int main(int ac,char *av[])
{
	if(ac<2)
	{
		std::cout << "Usage: vgmplayer filename.vgm" << std::endl;
		return 1;
	}

	FsOpenWindow(0,0,256,256,0);

	soundPlayer.Start();
	for(int i=1; i<ac; ++i)
	{
		if(true!=Play(av[i]))
		{
			std::cout << "Error while playing." << std::endl;
			soundPlayer.End();
			return 1;
		}
	}
	soundPlayer.End();
	return 0;
}
