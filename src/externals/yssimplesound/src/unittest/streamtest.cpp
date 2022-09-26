#include <cstdio>
#include <vector>

#include <fssimplewindow.h>
#include <yssimplesound.h>



int main(int ac,char *av[])
{
	if(ac<2)
	{
		printf("Usage:\n");
		printf("streamtest filename.wav\n");
		return 1;
	}

	YsSoundPlayer::SoundData data;
	if(YSOK!=data.LoadWav(av[1]))
	{
		printf("Cannot open file.\n");
		return 1;
	}


	unsigned int playPtr=0,filledLen=0;
	const unsigned int pieceSteps=512,pieceLen=pieceSteps*2;
	std::vector <unsigned char> pieceByte;

	while(playPtr<data.GetNumSamplePerChannel() && filledLen<pieceSteps)
	{
		auto ch0=data.GetSignedValue16(0,playPtr);
		auto ch1=data.GetSignedValue16(1,playPtr);
		pieceByte.push_back(ch0&0xFF);
		pieceByte.push_back(ch0>>8);
		pieceByte.push_back(ch1&0xFF);
		pieceByte.push_back(ch1>>8);
		++filledLen;
		++playPtr;
	}


	YsSoundPlayer sndPlayer;
	sndPlayer.Start();

	YsSoundPlayer::SoundData piece;
	piece.CreateFromSigned16bitStereo(44100,pieceByte);

	YsSoundPlayer::Stream streamer;
	YsSoundPlayer::StreamingOption opt;
	sndPlayer.StartStreaming(streamer,opt);
	sndPlayer.AddNextStreamingSegment(streamer,piece);
	pieceByte.clear();

	while(playPtr<data.GetNumSamplePerChannel())
	{
		if(0==pieceByte.size())
		{
			filledLen=0;
			while(playPtr<data.GetNumSamplePerChannel() && filledLen<pieceSteps)
			{
				auto ch0=data.GetSignedValue16(0,playPtr);
				auto ch1=data.GetSignedValue16(1,playPtr);
				pieceByte.push_back(ch0&0xFF);
				pieceByte.push_back(ch0>>8);
				pieceByte.push_back(ch1&0xFF);
				pieceByte.push_back(ch1>>8);
				++filledLen;
				++playPtr;
			}
			printf("%d %d\n",playPtr,filledLen);
		}
		if(0!=pieceByte.size() && sndPlayer.StreamPlayerReadyToAcceptNextNumSample(streamer,filledLen))
		{
			piece.CreateFromSigned16bitStereo(44100,pieceByte);
			sndPlayer.AddNextStreamingSegment(streamer,piece);
			pieceByte.clear();
		}
	}

	sndPlayer.End();

	printf("End Test.\n");
	return 0;
}

