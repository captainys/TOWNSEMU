#include <stdio.h>
#include "yssimplesound.h"



class YsSoundPlayer::APISpecificData
{
public:
	APISpecificData();
	~APISpecificData();
	void CleanUp(void);
	void Start(void);
	void End(void);
};

class YsSoundPlayer::SoundData::APISpecificDataPerSoundData
{
public:
	APISpecificDataPerSoundData();
	~APISpecificDataPerSoundData();
	void CleanUp(void);
};



////////////////////////////////////////////////////////////



YsSoundPlayer::APISpecificData::APISpecificData()
{
	CleanUp();
}
YsSoundPlayer::APISpecificData::~APISpecificData()
{
	CleanUp();
}

void YsSoundPlayer::APISpecificData::CleanUp(void)
{
}

////////////////////////////////////////////////////////////

YsSoundPlayer::APISpecificData *YsSoundPlayer::CreateAPISpecificData(void)
{
	return new APISpecificData;
}
void YsSoundPlayer::DeleteAPISpecificData(APISpecificData *ptr)
{
	delete ptr;
}

YSRESULT YsSoundPlayer::StartAPISpecific(void)
{
	return YSOK;
}
YSRESULT YsSoundPlayer::EndAPISpecific(void)
{
	return YSOK;
}

void YsSoundPlayer::SetVolumeAPISpecific(SoundData &,float,float)
{
}

YSRESULT YsSoundPlayer::PlayOneShotAPISpecific(SoundData &dat)
{
	return YSOK;
}

YSRESULT YsSoundPlayer::PlayBackgroundAPISpecific(SoundData &dat)
{
	return YSOK;
}

void YsSoundPlayer::StopAPISpecific(SoundData &dat)
{
}

void YsSoundPlayer::PauseAPISpecific(SoundData &dat)
{
}
void YsSoundPlayer::ResumeAPISpecific(SoundData &dat)
{
}

void YsSoundPlayer::KeepPlayingAPISpecific(void)
{
}
YSBOOL YsSoundPlayer::IsPlayingAPISpecific(const SoundData &) const
{
	return YSFALSE;
}
double YsSoundPlayer::GetCurrentPositionAPISpecific(const SoundData &) const
{
	return 0.0;
}

////////////////////////////////////////////////////////////

YsSoundPlayer::SoundData::APISpecificDataPerSoundData::APISpecificDataPerSoundData()
{
	CleanUp();
}
YsSoundPlayer::SoundData::APISpecificDataPerSoundData::~APISpecificDataPerSoundData()
{
	CleanUp();
}
void YsSoundPlayer::SoundData::APISpecificDataPerSoundData::CleanUp(void)
{
}

////////////////////////////////////////////////////////////

YsSoundPlayer::SoundData::APISpecificDataPerSoundData *YsSoundPlayer::SoundData::CreateAPISpecificData(void)
{
	return new APISpecificDataPerSoundData;
}
void YsSoundPlayer::SoundData::DeleteAPISpecificData(APISpecificDataPerSoundData *ptr)
{
	delete ptr;
}

YSRESULT YsSoundPlayer::SoundData::PreparePlay(YsSoundPlayer &player)
{
	return YSOK;
}

void YsSoundPlayer::SoundData::CleanUpAPISpecific(void)
{
	api->CleanUp();
}

//////////////////////////////////////////////////////////////

YsSoundPlayer::Stream::APISpecificData *YsSoundPlayer::Stream::CreateAPISpecificData(void)
{
	return nullptr;
}
void YsSoundPlayer::Stream::DeleteAPISpecificData(APISpecificData *)
{
}

YSRESULT YsSoundPlayer::StartStreamingAPISpecific(Stream &,StreamingOption)
{
	return YSOK;
}
void YsSoundPlayer::StopStreamingAPISpecific(Stream &)
{
}
YSBOOL YsSoundPlayer::StreamPlayerReadyToAcceptNextNumSampleAPISpecific(const Stream &streamPlayer,unsigned int numSamples) const
{
	return YSFALSE;
}
YSRESULT YsSoundPlayer::AddNextStreamingSegmentAPISpecific(Stream &streamPlayer,const SoundData &dat)
{
	return YSOK;
}