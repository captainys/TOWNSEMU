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

void YsSoundPlayer::SetVolumeAPISpecific(SoundData &,float)
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

void YsSoundPlayer::KeepPlayingAPISpecific(void)
{
}
YSBOOL YsSoundPlayer::IsPlayingAPISpecific(const SoundData &) const
{
	return YSFALSE;
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
