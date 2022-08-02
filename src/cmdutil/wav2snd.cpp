#include "yssimplesound.h"
#include <stdio.h>
#include <stdlib.h>
#include <vector>

// cl wav2snd.cpp -I..\..\..\..\public\src\yssimplesound\src ..\..\..\..\public\src\yssimplesound\src\yssimplesound.cpp ..\..\..\..\public\src\yssimplesound\src\nownd\yssimplesound_nownd.cpp -EHsc -W3



struct SndHeader
{
	char label[8];
	int soundId;  // Random number.  Why is it?
	int nByte;
	int loopPtr;
	int loopLen;
	short hertz;
	short noteCorrection;
	unsigned char baseNote;
	unsigned char reserve1;
	short reserve2;
};

void InitHeader(SndHeader &header)
{
	for(auto &c : header.label)
	{
		c=0;
	}
	header.soundId=12345;
	header.nByte=0;
	header.loopPtr=0;
	header.loopLen=0;
	header.hertz=19200;
	header.noteCorrection=0;
	header.baseNote=0x3C; // "O4C"
	header.reserve1=0;
	header.reserve2=0;
}



int main(int ac,char *av[])
{
	YsSoundPlayer::SoundData wav;

	int maxSampleRate=16000;  // Towns can go up to 19200, but only 64KB PCM memory.

	if(ac<3)
	{
		printf("WAV to FM-TOWNS SND converter.\n");
		printf("wav2snd wave.wav sound.snd\n");
		printf("  OR\n");
		printf("wav2snd wave.wav sound.snd maxSampleRate\n");
		printf("Default maxSampleRate=16000\n");
		printf("Hardware Max is 19200\n");
		printf("But, FM-TOWNS' PCM memory is only 64K.\n");
		return 1;
	}

	if(4<=ac)
	{
		maxSampleRate=atoi(av[3]);
		printf("Max Sample Rate=%d\n",maxSampleRate);
	}

	if(YSOK!=wav.LoadWav(av[1]))
	{
		printf("ERROR! Cannot load %s!\n",av[1]);
		return 0;
	}

	if(maxSampleRate<wav.PlayBackRate())
	{
		wav.Resample(maxSampleRate);
	}

	SndHeader header;
	InitHeader(header);

	{
		char *fn=av[1];
		while(0!=*fn)
		{
			++fn;
		}
		while(av[1]<fn && *fn!='\\' && *fn!='/')
		{
			--fn;
		}
		if(*fn=='\\' || *fn=='/')
		{
			++fn;
		}
		for(int i=0; i<8 && 0!=fn[i] && '.'!=fn[i]; ++i)
		{
			header.label[i]=fn[i];
		}
	}

	std::vector <unsigned char> dat;
	for(long long int i=0; i<wav.GetNumSamplePerChannel(); ++i)
	{
		int value=0;
		for(int c=0; c<wav.GetNumChannel(); ++c)
		{
			value+=wav.GetSignedValue16(c,i);
		}
		value/=wav.GetNumChannel();

		// If I remember right, FM-TOWNS SND is like:
		//   bit7   sign
		//   bit0-6 absolute value.

		unsigned char value8;
		if(0<=value)
		{
			value>>=8;
			if(127<value)
			{
				value=127;
			}
			value8=(unsigned char)value;
		}
		else
		{
			value=((-value)>>8);
			if(127<value)
			{
				value=127;
			}
			value8=0x80+(unsigned char)value;
		}
		dat.push_back(value8);
	}

	header.nByte=(int)dat.size();
	header.hertz=wav.PlayBackRate()*0x62/1000; // CL21R.DOC L:14180

	FILE *ofp=fopen(av[2],"wb");
	if(nullptr!=ofp)
	{
		fwrite(&header,1,sizeof(header),ofp);
		fwrite(dat.data(),1,dat.size(),ofp);
		fclose(ofp);
		printf("Wrote %s\n",av[2]);
		return 0;
	}
	else
	{
		printf("ERROR! Could not write to %s\n",av[2]);
		return 1;
	}
}
