#include <stdio.h>

#include <yssimplesound.h>
#include <fssimplewindow.h>
#include "wavedata.h"
#include "prop9.h"
#include "gun.h"


int main(void)
{
	FsOpenWindow(16,16,800,600,1);

	// Due to Direct Sound stupidity, sndPlayer must be started after the main window is created.
	// Direct Sound is essentially a text-book example of failed design.
	// MacOSX and Linux does not have this limitation.
	YsSoundPlayer sndPlayer;
	sndPlayer.Start();

	YsSoundPlayer::SoundData wave;
	if(YSOK==wave.LoadWavFromMemory(sizeof_WaveData,WaveData))
	{
		printf("Loaded\n");
	}
	else
	{
		printf("Load Error\n");
	}

	YsSoundPlayer::SoundData prop;
	if(YSOK==prop.LoadWavFromMemory(sizeof_prop9,prop9))
	{
		printf("Loaded\n");
	}
	else
	{
		printf("Load Error\n");
	}

	YsSoundPlayer::SoundData g;
	if(YSOK==g.LoadWavFromMemory(sizeof_gun,gun))
	{
		printf("Loaded\n");
	}
	else
	{
		printf("Load Error\n");
	}


	sndPlayer.PlayBackground(prop);


	for(;;)
	{
		FsPollDevice();
		auto key=FsInkey();

		if(FSKEY_ESC==key)
		{
			break;
		}

		if(FsGetKeyState(FSKEY_G))
		{
			sndPlayer.PlayBackground(g);
		}
		else
		{
			sndPlayer.Stop(g);
		}

		if(FSKEY_SPACE==key)
		{
			sndPlayer.PlayOneShot(wave);
		}
		if(FSKEY_A==key)
		{
			sndPlayer.Stop(wave);
			sndPlayer.PlayOneShot(wave);
		}
		if(FSKEY_S==key)
		{
			sndPlayer.Stop(wave);
		}
		if(FSKEY_W==key)
		{
			auto byteData=wave.MakeWavByteData();
			FILE *fp=fopen("test.wav","w");
			if(nullptr!=fp)
			{
				fwrite(byteData.data(),1,byteData.size(),fp);
				fclose(fp);
				printf("Wrote test.wav\n");
			}
		}

		sndPlayer.KeepPlaying();

		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		FsSwapBuffers();
		FsSleep(25);
	}

	wave.CleanUp();
	sndPlayer.End();
	return 0;
}

