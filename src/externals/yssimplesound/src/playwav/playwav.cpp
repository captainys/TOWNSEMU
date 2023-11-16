#include "yssimplesound.h"
#include <iostream>



int main(int ac,char *av[])
{
	if(2!=ac)
	{
		std::cout << "Usage: playwav waveFileName.wav" << std::endl;;
		return 1;
	}

	YsSoundPlayer player;
	YsSoundPlayer::SoundData data;
	if(YSOK!=data.LoadWav(av[1]))
	{
		std::cout << "Cannot load .WAV" << std::endl;;
		return 1;
	}

	player.Start();
	player.PlayOneShot(data);
	while(YSTRUE==player.IsPlaying(data))
	{
		player.KeepPlaying();
	}
	player.End();

	return 0;
}
