#include <stdio.h>
#include <vector>
#include "yssimplesound.h"


#define ALSA_PCM_NEW_HW_PARAMS_API
#include <alsa/asoundlib.h>

/*
2015/10/21  About a broken promise of ALSA asynchronous playback.

I am writing this memo so that I won't waste any more time to explore possibility of eliminating KeepPlaying function by ALSA's asynchronous-playback feature.

After numerous frustrating attempts, I conclude that there is no such thing called "asynchronous playback" in ALSA.

There is a working example pcm.c, which you can download from:
    http://www.alsa-project.org/alsa-doc/alsa-lib/_2test_2pcm_8c-example.html

In this example, asnychronous playback looks to be working.  You can add:
    printf("*\n");

at the beginning of async_callback function to confirm the function is called if you start the program with "-m async" options.

It appears to be playing audio asynchronously, but it is not.

Try commenting out sleep(1) in the async_loop function.  The call-back function async_callback won't be called unless you keep your main thread idle.

If I let my main thread do something useful, the playback stops.  
It is not even asynchronous.  Yes, async_callback is playing audio, but, asynchronous with what?  Nothing.

The term "asynchronous playback" implies that I can let the main thread do something useful while ALSA is playing an audio data concurrently.
However, in ALSA, you need to stop your main thread while ALSA's so-called asynchronous-playback is playing an audio data.
It is absolutely useless.

Better use polling-based playback.  At least that way you can let your main thread do something useful, while keeping the audio played background.

*/



class YsSoundPlayer::APISpecificData
{
public:
	class PlayingSound
	{
	public:
		SoundData *dat;
		unsigned int ptr;
		YSBOOL loop;
		YSBOOL stop;

		void Make(SoundData &dat,YSBOOL loop);
	};

	std::vector <PlayingSound> playing;

	snd_pcm_t *handle;
	snd_async_handler_t *asyncHandler;
	snd_pcm_hw_params_t *hwParam;
	snd_pcm_sw_params_t *swParam;

	unsigned int nChannel,rate,bufSize;
	snd_pcm_uframes_t nPeriod;

	const unsigned int bytePerTimeStep;
	unsigned int bufSizeInNStep;
	unsigned int bufSizeInByte;
	unsigned char *writeBuf;


	APISpecificData();
	~APISpecificData();
	void CleanUp(void);
	YSRESULT Start(void);
	YSRESULT End(void);

	static void KeepPlayingCallBack(snd_async_handler_t *handler);

	void KeepPlaying(void);

	void DiscardEnded(void);

	void PopulateWriteBuffer(unsigned int &writeBufFilledInNStep,unsigned int wavPtr,const SoundData *wavFile,YSBOOL loop,int nThSound);
	void PrintState(int errCode);
};

void YsSoundPlayer::APISpecificData::PlayingSound::Make(SoundData &dat,YSBOOL loop)
{
	this->dat=&dat;
	this->ptr=0;
	this->loop=loop;
	this->stop=YSFALSE;
}


class YsSoundPlayer::SoundData::APISpecificDataPerSoundData
{
public:
	APISpecificDataPerSoundData();
	~APISpecificDataPerSoundData();
	void CleanUp(void);
};



////////////////////////////////////////////////////////////



YsSoundPlayer::APISpecificData::APISpecificData() :
	bytePerTimeStep(2) // 2 bytes for 16-bit mono stream per time step
{
	for(auto &p : playing)
	{
		p.dat=nullptr;
		p.ptr=0;
	}

	handle=nullptr;
	hwParam=nullptr;
	swParam=nullptr;
	asyncHandler=nullptr;
	nChannel=0;
	rate=0;
	nPeriod=0;
	bufSize=0;
	writeBuf=nullptr;

	CleanUp();
}
YsSoundPlayer::APISpecificData::~APISpecificData()
{
	End();
	CleanUp();
}

void YsSoundPlayer::APISpecificData::CleanUp(void)
{
}

YSRESULT YsSoundPlayer::APISpecificData::Start(void)
{
	int res=snd_pcm_open(&handle,"default",SND_PCM_STREAM_PLAYBACK,SND_PCM_NONBLOCK);
	if(0>res)
	{
		printf("Cannot open PCM device.\n");
		handle=nullptr;
		return YSERR;
	}

	snd_pcm_hw_params_alloca(&hwParam);
	snd_pcm_hw_params_any(handle,hwParam);
	snd_pcm_hw_params_set_access(handle,hwParam,SND_PCM_ACCESS_RW_INTERLEAVED);
	snd_pcm_hw_params_set_format(handle,hwParam,SND_PCM_FORMAT_S16_LE);
	snd_pcm_hw_params_set_channels(handle,hwParam,1);

	unsigned int request=8000;//22050;
	int dir;  // What's dir?
	snd_pcm_hw_params_set_rate_near(handle,hwParam,&request,&dir);

	if(0>snd_pcm_hw_params(handle,hwParam))
	{
		printf("Cannot set hardward parameters.\n");
		return YSERR;
	}

	snd_pcm_hw_params_get_channels(hwParam,&nChannel);
	snd_pcm_hw_params_get_rate(hwParam,&rate,&dir);
	snd_pcm_hw_params_get_period_size(hwParam,&nPeriod,&dir);
	snd_pcm_hw_params_get_buffer_size(hwParam,(snd_pcm_uframes_t *)&bufSize);
	printf("%d channels, %d Hz, %d periods, %d frames buffer.\n",
		   nChannel,rate,(int)nPeriod,(int)bufSize);

	bufSizeInNStep=nPeriod*4;
	bufSizeInByte=bufSizeInNStep*bytePerTimeStep;
	if(nullptr!=writeBuf)
	{
		delete [] writeBuf;
	}
	writeBuf=new unsigned char [bufSizeInByte];
	for(int i=0; i<bufSizeInByte; ++i)
	{
		writeBuf[i]=0;
	}

	// snd_pcm_sw_params_alloca(&swParam);
	// snd_async_add_pcm_handler(&asyncHandler,handle,KeepPlayingCallBack,this);


	snd_pcm_prepare(handle);
	if(0==snd_pcm_wait(handle,1000))
	{
		printf("snd_pcm_wait timed out.\n");
		printf("There may be no sound.\n");
	}
	snd_pcm_start(handle);

	printf("YsSoundPlayer started.\n");

	return YSOK;
}
YSRESULT YsSoundPlayer::APISpecificData::End(void)
{
	if(nullptr!=hwParam)
	{
		// This gives an error => snd_pcm_hw_params_free(hwParam);
		hwParam=nullptr;
	}
	if(nullptr!=swParam)
	{
		// This gives an error => snd_pcm_sw_params_free(swParam);
		swParam=nullptr;
	}
	if(nullptr!=handle)
	{
		snd_pcm_close(handle);
		handle=nullptr;
	}
	if(nullptr!=writeBuf)
	{
		delete [] writeBuf;
		writeBuf=nullptr;
	}

	CleanUp();

	return YSOK;
}

/* static */ void YsSoundPlayer::APISpecificData::KeepPlayingCallBack(snd_async_handler_t *handler)
{
	printf("*\n");
}

void YsSoundPlayer::APISpecificData::KeepPlaying(void)
{
	if(nullptr!=handle)
	{
		const int nAvail=(unsigned int)snd_pcm_avail_update(handle);
		if(nPeriod<nAvail)
		{
			unsigned int writeBufFilledInNStep=0;
			int nThSound=0;

			for(auto &p : playing)
			{
				if(nullptr!=p.dat)
				{
					PopulateWriteBuffer(writeBufFilledInNStep,p.ptr,p.dat,p.loop,nThSound);
					++nThSound;
				}
			}

			if(0<nThSound)
			{
				int nWritten=snd_pcm_writei(handle,writeBuf,writeBufFilledInNStep);

				if(nWritten==-EAGAIN)
				{
				}
				else if(-EPIPE==nWritten || -EBADFD==nWritten)
				{
					snd_pcm_prepare(handle);
					snd_pcm_wait(handle,1);
					printf("ALSA: Recover from underrun\n");
				}
				else if(0>nWritten)
				{
					PrintState(-nWritten);
				}
				else if(0<nWritten)
				{
					for(auto &p : playing)
					{
						if(nullptr!=p.dat)
						{
							if(YSTRUE!=p.loop)
							{
								p.ptr+=nWritten;
								if(p.dat->NTimeStep()<=p.ptr)
								{
									p.dat=NULL;
									p.ptr=0;
								}
							}
							else
							{
								p.ptr+=nWritten;
								while(p.ptr>=p.dat->NTimeStep())
								{
									p.ptr-=p.dat->NTimeStep();
								}
							}
						}
					}
				}
			}
		}
		else
		{
			snd_pcm_state_t state=snd_pcm_state(handle);
			if(SND_PCM_STATE_RUNNING!=state)
			{
				PrintState(0);
				snd_pcm_prepare(handle);
				snd_pcm_wait(handle,1);
				printf("ALSA: Recover from state\n");
			}
		}

		DiscardEnded();
	}
}

void YsSoundPlayer::APISpecificData::DiscardEnded(void)
{
	for(long long int i=playing.size()-1; 0<=i; --i)
	{
		if(nullptr==playing[i].dat)
		{
			playing[i]=playing.back();
			playing.resize(playing.size()-1);
		}
	}

	// Question: Do I have to snd_pcm_drop here or not?
	// if(0==playing.size())
	// {
	// 	snd_pcm_drop(handle);
	// }
}

void YsSoundPlayer::APISpecificData::PopulateWriteBuffer(unsigned int &writeBufFilledInNStep,unsigned int wavPtr,const SoundData *wavFile,YSBOOL loop,int nThSound)
{
	auto currentFilledInNStep=writeBufFilledInNStep;
	writeBufFilledInNStep=0;
	while(writeBufFilledInNStep<bufSizeInNStep)
	{
		const unsigned char *dataPtr=wavFile->DataPointerAtTimeStep(wavPtr);
		const int ptrInByte=wavPtr*bytePerTimeStep;
		const int wavByteLeft=wavFile->SizeInByte()-ptrInByte;
		const int writeBufLeftInByte=(bufSizeInNStep-writeBufFilledInNStep)*bytePerTimeStep;
		
		int nByteToWrite;
		if(wavByteLeft<writeBufLeftInByte)
		{
			nByteToWrite=wavByteLeft;
		}
		else
		{
			nByteToWrite=writeBufLeftInByte;
		}

		for(int i=0; i<=nByteToWrite-2; i+=2)
		{
			if(0==nThSound || currentFilledInNStep<writeBufFilledInNStep+i/bytePerTimeStep)
			{
				writeBuf[writeBufFilledInNStep*bytePerTimeStep+i  ]=dataPtr[i];
				writeBuf[writeBufFilledInNStep*bytePerTimeStep+i+1]=dataPtr[i+1];
			}
			else
			{
				int v=(int)dataPtr[i]+(((int)dataPtr[i+1])<<8);
				if(32768<=v)
				{
					v-=65536;
				}
				int c=  (int)writeBuf[writeBufFilledInNStep*bytePerTimeStep+i]
				     +(((int)writeBuf[writeBufFilledInNStep*bytePerTimeStep+i+1])<<8);
				if(32768<=c)
				{
					c-=65536;
				}
				v+=c;
				if(32767<v)
				{
					v=32767;
				}
				if(v<-32768)
				{
					v=-32768;
				}
				writeBuf[writeBufFilledInNStep*bytePerTimeStep+i  ]=v&0xff;
				writeBuf[writeBufFilledInNStep*bytePerTimeStep+i+1]=((v>>8)&0xff);
			}
		}
		writeBufFilledInNStep+=nByteToWrite/bytePerTimeStep;
		wavPtr+=nByteToWrite/bytePerTimeStep;
		if(wavFile->NTimeStep()<=wavPtr)
		{
			if(YSTRUE!=loop)
			{
				break;
			}
			wavPtr=0;
		}
	}

	if(writeBufFilledInNStep<currentFilledInNStep)
	{
		writeBufFilledInNStep=currentFilledInNStep;
	}
}

void YsSoundPlayer::APISpecificData::PrintState(int errCode)
{
	if(0==errCode)
	{
	}
	else if(EBADFD==errCode)
	{
		printf("EBADFD\n");
	}
	else if(EPIPE==errCode)
	{
		printf("EPIPE\n");
	}
	else if(ESTRPIPE==errCode)
	{
		printf("ESTRPIPE\n");
	}
	else if(EAGAIN==errCode)
	{
		printf("EAGAIN\n");
	}
	else
	{
		printf("Unknown error.\n");
	}

	snd_pcm_state_t state=snd_pcm_state(handle);
	switch(state)
	{
	case SND_PCM_STATE_OPEN:
		printf("SND_PCM_STATE_OPEN\n");
		break;
	case SND_PCM_STATE_SETUP:
		printf("SND_PCM_STATE_SETUP\n");
		break;
	case SND_PCM_STATE_PREPARED:
		printf("SND_PCM_STATE_PREPARED\n");
		break;
	case SND_PCM_STATE_RUNNING:
		printf("SND_PCM_STATE_RUNNING\n");
		break;
	case SND_PCM_STATE_XRUN:
		printf("SND_PCM_STATE_XRUN\n");
		break;
	case SND_PCM_STATE_DRAINING:
		printf("SND_PCM_STATE_DRAINING\n");
		break;
	case SND_PCM_STATE_PAUSED:
		printf("SND_PCM_STATE_PAUSED\n");
		break;
	case SND_PCM_STATE_SUSPENDED:
		printf("SND_PCM_STATE_SUSPENDED\n");
		break;
	case SND_PCM_STATE_DISCONNECTED:
		printf("SND_PCM_STATE_DISCONNECTED\n");
		break;
	}
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

void YsSoundPlayer::SetVolumeAPISpecific(SoundData &dat,float vol)
{
}

void YsSoundPlayer::KeepPlayingAPISpecific(void)
{
	api->KeepPlaying();
}

YSRESULT YsSoundPlayer::PlayOneShotAPISpecific(SoundData &dat)
{
	for(auto &p : api->playing)
	{
		if(p.dat==&dat)
		{
			return YSOK;
		}
	}

	YsSoundPlayer::APISpecificData::PlayingSound p;
	p.Make(dat,YSFALSE);
	api->playing.push_back(p);

	if(nullptr!=api->handle)
	{
		snd_pcm_drop(api->handle);
		snd_pcm_prepare(api->handle);
		snd_pcm_wait(api->handle,1);
	}
	return YSOK;
}
YSRESULT YsSoundPlayer::PlayBackgroundAPISpecific(SoundData &dat)
{
	for(auto &p : api->playing)
	{
		if(p.dat==&dat)
		{
			return YSOK;
		}
	}

	YsSoundPlayer::APISpecificData::PlayingSound p;
	p.Make(dat,YSTRUE);
	api->playing.push_back(p);

	if(nullptr!=api->handle)
	{
		snd_pcm_drop(api->handle);
		snd_pcm_prepare(api->handle);
		snd_pcm_wait(api->handle,1);
	}
	return YSOK;
}

YSBOOL YsSoundPlayer::IsPlayingAPISpecific(const SoundData &dat) const
{
	for(auto &p : api->playing)
	{
		if(nullptr!=p.dat)
		{
			return YSTRUE;
		}
	}
	return YSFALSE;
}

void YsSoundPlayer::StopAPISpecific(SoundData &dat)
{
	YSBOOL stopped=YSFALSE;

	for(auto &p : api->playing)
	{
		if(&dat==p.dat)
		{
			p.ptr=0;
			p.dat=nullptr;
			stopped=YSTRUE;
		}
	}

	if(YSTRUE==stopped)
	{
		api->DiscardEnded();
		snd_pcm_drop(api->handle);
	}
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
	Resample(player.api->rate);
	ConvertToMono();
	ConvertTo16Bit();
	ConvertToSigned();
	return YSOK;
}

void YsSoundPlayer::SoundData::CleanUpAPISpecific(void)
{
	api->CleanUp();
}
