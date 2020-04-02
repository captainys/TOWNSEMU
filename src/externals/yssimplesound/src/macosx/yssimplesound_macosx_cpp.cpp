#include <stdio.h>
#include "yssimplesound.h"


struct YsNSSound;

extern "C" struct YsNSSound *YsSimpleSound_OSX_CreateSound(long long int size,const unsigned char wavByteData[]);
extern "C" void YsSimpleSound_OSX_DeleteSound(struct YsNSSound *ptr);
extern "C" void YsSimpleSound_OSX_PlayOneShot(struct YsNSSound *ptr);
extern "C" void YsSimpleSound_OSX_PlayBackground(struct YsNSSound *ptr);
extern "C" void YsSimpleSound_OSX_SetVolume(struct YsNSSound *ptr,float vol);
extern "C" void YsSimpleSound_OSX_Stop(struct YsNSSound *ptr);
extern "C" bool YsSimpleSound_OSX_IsPlaying(struct YsNSSound *ptr);



class YsSoundPlayer::APISpecificData
{
public:
	APISpecificData();
	~APISpecificData();
	void CleanUp(void);
	YSRESULT Start(void);
	YSRESULT End(void);
};

class YsSoundPlayer::SoundData::APISpecificDataPerSoundData
{
public:
	APISpecificDataPerSoundData();
	~APISpecificDataPerSoundData();
	void CleanUp(void);

	YsNSSound *sndPtr;
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

YSRESULT YsSoundPlayer::APISpecificData::Start(void)
{
	return YSOK;
}
YSRESULT YsSoundPlayer::APISpecificData::End(void)
{
	CleanUp();
	return YSOK;
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
	return api->Start();
}
YSRESULT YsSoundPlayer::EndAPISpecific(void)
{
	return api->End();
}

void YsSoundPlayer::KeepPlayingAPISpecific(void)
{
}

YSRESULT YsSoundPlayer::PlayOneShotAPISpecific(SoundData &dat)
{
	if(nullptr!=dat.api->sndPtr)
	{
		YsSimpleSound_OSX_SetVolume(dat.api->sndPtr,dat.playBackVolume);
		YsSimpleSound_OSX_PlayOneShot(dat.api->sndPtr);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsSoundPlayer::PlayBackgroundAPISpecific(SoundData &dat)
{
	if(nullptr!=dat.api->sndPtr)
	{
		YsSimpleSound_OSX_SetVolume(dat.api->sndPtr,dat.playBackVolume);
		YsSimpleSound_OSX_PlayBackground(dat.api->sndPtr);
		return YSOK;
	}
	return YSERR;
}

YSBOOL YsSoundPlayer::IsPlayingAPISpecific(const SoundData &dat) const
{
	if(nullptr!=dat.api->sndPtr && YsSimpleSound_OSX_IsPlaying(dat.api->sndPtr))
	{
		return YSTRUE;
	}
	return YSFALSE;
}

void YsSoundPlayer::StopAPISpecific(SoundData &dat)
{
	if(nullptr!=dat.api->sndPtr)
	{
		YsSimpleSound_OSX_Stop(dat.api->sndPtr);
	}
}

void YsSoundPlayer::SetVolumeAPISpecific(SoundData &dat,float vol)
{
	if(nullptr!=dat.api->sndPtr)
	{
		YsSimpleSound_OSX_SetVolume(dat.api->sndPtr,vol);
	}
}

////////////////////////////////////////////////////////////

YsSoundPlayer::SoundData::APISpecificDataPerSoundData::APISpecificDataPerSoundData()
{
	sndPtr=nullptr;
	CleanUp();
}
YsSoundPlayer::SoundData::APISpecificDataPerSoundData::~APISpecificDataPerSoundData()
{
	CleanUp();
}
void YsSoundPlayer::SoundData::APISpecificDataPerSoundData::CleanUp(void)
{
	if(nullptr!=sndPtr)
	{
		YsSimpleSound_OSX_DeleteSound(sndPtr);
		sndPtr=nullptr;
	}
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
	if(nullptr==api->sndPtr)
	{
		auto byteData=MakeWavByteData();
		api->sndPtr=YsSimpleSound_OSX_CreateSound(byteData.size(),byteData.data());
		if(nullptr!=api->sndPtr)
		{
			return YSOK;
		}
	}
	return YSERR;
}

void YsSoundPlayer::SoundData::CleanUpAPISpecific(void)
{
	api->CleanUp();
}
