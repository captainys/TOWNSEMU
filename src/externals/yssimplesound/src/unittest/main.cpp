#include <cstdio>
#include <vector>
#include <string.h>

#include <fssimplewindow.h>
#include <yssimplesound.h>


const int samplePerChannel=44100;


void MakeWaveData(std::vector <unsigned char> &wave)
{
	wave.resize(samplePerChannel*4);  // 4-bytes per sample * 44.1KHz = 1 second

	short *waveData=(short *)wave.data();

	// Frequency for A4=440Hz
	// 44100Hz/440Hz=100  50 high-50 low

	for(int i=0; i<44100; ++i)
	{
		if(50<i%100)
		{
			waveData[i*2  ]=8000;
			waveData[i*2+1]=8000;
		}
		else
		{
			waveData[i*2  ]=-8000;
			waveData[i*2+1]=-8000;
		}
	}
}

void MakeSilence(std::vector <unsigned char> &wave)
{
	wave.resize(samplePerChannel*4);  // 4-bytes per sample * 44.1KHz = 1 second
	memset((char *)wave.data(),0,wave.size());
}

int main(void)
{
	std::vector <unsigned char> wave;
	MakeSilence(wave); // MakeWaveData(wave);

	YsSoundPlayer::SoundData data;
	data.CreateFromSigned16bitStereo(44100,wave);

	if(data.GetNumSamplePerChannel()!=samplePerChannel)
	{
		fprintf(stderr,"Sample count error.\n");
		return 1;
	}

	FsOpenWindow(0,0,100,100,0);
	
	YsSoundPlayer sndPlayer;
	sndPlayer.Start();
	sndPlayer.PlayOneShot(data);
	while(YSTRUE==sndPlayer.IsPlaying(data))
	{
		FsPollDevice();
		sndPlayer.KeepPlaying();

		printf("%lf\n",sndPlayer.GetCurrentPosition(data));
	}

	sndPlayer.End();

	printf("End Test.\n");
	return 0;
}

