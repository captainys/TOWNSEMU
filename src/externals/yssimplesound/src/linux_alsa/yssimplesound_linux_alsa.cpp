#include <algorithm>
#include <stdio.h>
#include <vector>
#include <iostream>
#include "yssimplesound.h"


#define ALSA_PCM_NEW_HW_PARAMS_API
#include <alsa/asoundlib.h>



// By uncommenting the following, KeepPlaying is unnecessary if the process has only one thread.
// #define ALSA_USE_INTERVAL_TIMER


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




2023/10/17
ALSA is the most difficult and the worst sound API I have ever used.  Especially, the API requires constant
polling, which is very problematic.  This requirement substantially restricts the program structure.
And many programmers are having difficulties (search StackOverflow).

As a result, among macOS, Windows, and Linux versions of this YsSimpleSound library, only Linux version
required constant call to KeepPlaying function.

I let my students use this library for an assignment.  It was a disaster.  So many students didn't bother
calling KeepPlaying in the main loop because it was unnecessary in Windows and macOS.

But, that was before.

Finally!  Interval Timer solved the problem.  One hiccup of the interval timer was that the signal
SIGALRM could be sent to any thread.  If I don't do anything if the thread was not the audio thread,
it at least prevented crash, but could not feed the next piece of wave to ALSA in time.

The solution was to force sending SIGALRM to the audio thread to pthread_signal if another thread
ended up receiving te signal.

Still there was a possibility of reentrance, which was prevented by not set up interval timer.
Instead, just set up a one-time timer, and then set a new timer in the timer handler.

The last issue was ALSA seems to freeze randomly if I call one of snd_pcm_drop, snd_pcm_prepare, or snd_pcm_wait 
while the PCM state is PREPARED.  I was doing it when PCM state was not RUNNING.  But, once I figured,
simply not calling them when the state is PREPARED solved the problem.

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

	class PlayingStream
	{
	public:
		Stream *dat=nullptr;
		unsigned int ptr=0;

		void Make(Stream &dat);
	};

	std::vector <PlayingSound> playing;
	std::vector <PlayingStream> playingStream;

	snd_pcm_t *handle=nullptr;
	snd_async_handler_t *asyncHandler=nullptr;
	snd_pcm_hw_params_t *hwParam=nullptr;
	snd_pcm_sw_params_t *swParam=nullptr;

	unsigned int nChannel=2,rate=44100;
	snd_pcm_uframes_t pcmBufSize=0;
	snd_pcm_uframes_t nPeriod=0;

	const unsigned int bytePerSample=2;
	unsigned int bytePerTimeStep=4;
	unsigned int bufSizeInNStep=0;
	unsigned int bufSizeInByte=0;
	unsigned char *writeBuf=nullptr;

	static std::vector <YsSoundPlayer::APISpecificData *> activePlayers;

	APISpecificData();
	~APISpecificData();
	void CleanUp(void);
	YSRESULT Start(void);
	YSRESULT End(void);

	static void KeepPlayingCallBack(snd_async_handler_t *handler);

	void KeepPlaying(void);

	void DiscardEnded(void);

	/* writePtr, wavPtr is in number of samples, not number of bytes.
	   Returns true if loop==YSFALSE and the buffer moved all the way to the end.
	   writePtr is updated.

	   writePtr is pointer to the write buffer, not the input SoundData.

	   However, actual number of steps written to the hardware (or the driver's buffer) depends on the
	   subsequent ALSA function.  Therefore, actually updating the pointer should take place after doing so.
	*/
	bool PopulateWriteBuffer(unsigned int &writePtr,unsigned int wavPtr,const SoundData *wavFile,YSBOOL loop,int nThSound);

	void PrintState(int errCode);

	double GetCurrentPosition(const SoundData &dat) const;

	static void TimerInterrupt(int signum);
	static void UserInterrupt(int signum);
	static void StartTimer(void);
	static void EndTimer(void);
	static void MaskTimer(void);
	static void UnmaskTimer(void);
	class TimerMaskGuard;
};

std::vector <YsSoundPlayer::APISpecificData *> YsSoundPlayer::APISpecificData::activePlayers;

class YsSoundPlayer::Stream::APISpecificData
{
public:
	SoundData playing,standBy;
};

void YsSoundPlayer::APISpecificData::PlayingSound::Make(SoundData &dat,YSBOOL loop)
{
	this->dat=&dat;
	this->ptr=0;
	this->loop=loop;
	this->stop=YSFALSE;
}

void YsSoundPlayer::APISpecificData::PlayingStream::Make(Stream &dat)
{
	this->dat=&dat;
	this->ptr=0;
}

class YsSoundPlayer::SoundData::APISpecificDataPerSoundData
{
public:
	APISpecificDataPerSoundData();
	~APISpecificDataPerSoundData();
	void CleanUp(void);
};


#ifdef ALSA_USE_INTERVAL_TIMER

// Another attempt to un-necessitate KeepPlaying function. >>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <stdio.h>
#include <pthread.h>
#include <thread>
#include <mutex>

// https://stackoverflow.com/questions/21091000/how-to-get-thread-id-of-a-pthread-in-linux-c-program
static pthread_t audioThread;
static bool timerRunning=false;
static struct itimerval prevTimer;
static void (*prevSignalHandler)(int);
static int maskLevel=0;

void YsSoundPlayer::APISpecificData::TimerInterrupt(int signum)
{
	auto thisThread=pthread_self();
	if(thisThread!=audioThread)
	{
		// This sigprocmask is supposed to disable signal sent to other thread.
		// https://devarea.com/linux-handling-signals-in-a-multithreaded-application/
		// But, doesn't work.
		//static sigset_t sigset;
		//sigemptyset(&sigset);
		//sigaddset(&sigset,SIGALRM);
		//sigprocmask(SIG_BLOCK,&sigset,NULL);

		// This handler needs to be invoked in the audioThread, but if I
		// do it, somehow the program hangs in player->KeepPlaying()
		// It looks to crash when nothing is playing.
		pthread_kill(audioThread,SIGALRM);

		return;
	}

	if(0==maskLevel)
	{
		// Just in case.  If another threads intecepts the signal, and shot SIGALRM by pthread_kill,
		// there might be a lag until the audio thread receives the signal.
		// If the signal comes in while the timer signal is masked, do not call KeepPlaying, 
		// but instead schedule next timer.
		for(auto player : activePlayers)
		{
			player->KeepPlaying();
		}
	}

	struct itimerval timer,prevTimer;
	timer.it_value.tv_sec=0;
	timer.it_value.tv_usec=2000; // 2ms timer.
	timer.it_interval.tv_sec=0;
	timer.it_interval.tv_usec=0;
	setitimer(ITIMER_REAL,&timer,&prevTimer);
}
void YsSoundPlayer::APISpecificData::StartTimer(void)
{
	if(true!=timerRunning)
	{
		audioThread=pthread_self();

		struct itimerval timer;
		timer.it_value.tv_sec=0;
		timer.it_value.tv_usec=2000; // 2ms timer.
		timer.it_interval.tv_sec=0;
		timer.it_interval.tv_usec=0;
		// Do not repeat.  Next timer will be set in the handler.
		// This way prevents re-entrance problem.
		prevSignalHandler=signal(SIGALRM,TimerInterrupt);
		setitimer(ITIMER_REAL,&timer,&prevTimer);
		timerRunning=true;
	}
}
void YsSoundPlayer::APISpecificData::EndTimer(void)
{
	if(true==timerRunning)
	{
		struct itimerval timer;
		setitimer(ITIMER_REAL,&prevTimer,&timer);
		signal(SIGALRM,SIG_IGN);
		timerRunning=false;
		std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Give 5 chances to flush pending signals.
		signal(SIGALRM,prevSignalHandler);
	}
}
void YsSoundPlayer::APISpecificData::MaskTimer(void)
{
	if(true==timerRunning)
	{
		if(0==maskLevel)
		{
			sigset_t sigset;
			sigemptyset(&sigset);
			sigaddset(&sigset,SIGALRM);
			sigprocmask(SIG_BLOCK,&sigset,NULL);
		}
		++maskLevel;
	}
}
void YsSoundPlayer::APISpecificData::UnmaskTimer(void)
{
	if(true==timerRunning)
	{
		if(0<maskLevel)
		{
			--maskLevel;
			if(0==maskLevel)
			{
				sigset_t sigset;
				sigemptyset(&sigset);
				sigaddset(&sigset,SIGALRM);
				sigprocmask(SIG_UNBLOCK,&sigset,NULL);
			}
		}
	}
}
class YsSoundPlayer::APISpecificData::TimerMaskGuard
{
public:
	TimerMaskGuard()
	{
		MaskTimer();
	}
	~TimerMaskGuard()
	{
		UnmaskTimer();
	}
};
// Another attempt to un-necessitate KeepPlaying function. <<
#else
void YsSoundPlayer::APISpecificData::TimerInterrupt(int){}
void YsSoundPlayer::APISpecificData::UserInterrupt(int){}
void YsSoundPlayer::APISpecificData::StartTimer(void){}
void YsSoundPlayer::APISpecificData::EndTimer(void){}
void YsSoundPlayer::APISpecificData::MaskTimer(void){}
void YsSoundPlayer::APISpecificData::UnmaskTimer(void){}
class YsSoundPlayer::APISpecificData::TimerMaskGuard
{
public:
};
#endif


////////////////////////////////////////////////////////////



YsSoundPlayer::APISpecificData::APISpecificData() :
	bytePerSample(2) // 2 bytes for 16-bit mono stream per time step
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
	pcmBufSize=0;
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
	snd_pcm_hw_params_set_channels(handle,hwParam,2);

	// ? Why did I make it 8000 before ?
	// Apparently 8000Hz has some problems in play back.
	// 
	unsigned int rateRequest=44100;
	int dir;  // What's dir?
	snd_pcm_hw_params_set_rate_near(handle,hwParam,&rateRequest,&dir);

	// Make buffer size small, otherwise it's going to give me too much latency.
	// Wanted 40ms, but didn't work.
	// 2020/01/12
	//   After numerous attempts, period size of 1/25 seconds (40ms) and buffer size double of it (80ms)
	//   Works well without giving buffer underrun.
	//   It's Linux after all.  Don't expect quality audio from Linux.
	snd_pcm_uframes_t periodSizeRequest;
	periodSizeRequest=rateRequest/25;
	snd_pcm_hw_params_set_period_size(handle,hwParam,periodSizeRequest,0);

	snd_pcm_uframes_t bufferSizeRequest;
	bufferSizeRequest=periodSizeRequest*2;
	snd_pcm_hw_params_set_buffer_size_near(handle,hwParam,&bufferSizeRequest);

	if(0>snd_pcm_hw_params(handle,hwParam))
	{
		printf("Cannot set hardward parameters.\n");
		return YSERR;
	}

	snd_pcm_hw_params_get_channels(hwParam,&nChannel);
	snd_pcm_hw_params_get_rate(hwParam,&rate,&dir);
	snd_pcm_hw_params_get_period_size(hwParam,&nPeriod,&dir);
	snd_pcm_hw_params_get_buffer_size(hwParam,&pcmBufSize);
	printf("%d channels, %d Hz, %d periods, %d frames buffer.\n",
		   nChannel,rate,(int)nPeriod,(int)pcmBufSize);

	bytePerTimeStep=bytePerSample*nChannel;
	bufSizeInNStep=nPeriod*4;
	bufSizeInByte=nChannel*bufSizeInNStep*bytePerTimeStep;
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


	snd_pcm_drop(handle);
	snd_pcm_prepare(handle);
	snd_pcm_wait(handle,1);


	auto found=std::find(activePlayers.begin(),activePlayers.end(),this);
	if(activePlayers.end()==found)
	{
		activePlayers.push_back(this);
	}

	StartTimer();

	return YSOK;
}
YSRESULT YsSoundPlayer::APISpecificData::End(void)
{
	CleanUp();

	{
		TimerMaskGuard tmg;

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

		auto found=std::find(activePlayers.begin(),activePlayers.end(),this);
		if(activePlayers.end()!=found)
		{
			activePlayers.erase(found);
		}
	}
	if(0==activePlayers.size())
	{
		EndTimer();
	}

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
		snd_pcm_state_t pcmState=snd_pcm_state(handle);

		snd_pcm_status_t *pcmStatus;
		snd_pcm_status_alloca(&pcmStatus);
		snd_pcm_status(handle,pcmStatus);

		if(SND_PCM_STATE_RUNNING!=pcmState && SND_PCM_STATE_PREPARED!=pcmState)
		{
			// 2023/10/19
			// Call to the following three functions while the PCM state is PREPARED might freeze.
			// If it is PREPARED, probably I don't have to do any of the three.
			snd_pcm_drop(handle);
			snd_pcm_prepare(handle);
			snd_pcm_wait(handle,1);
		}

		const int nAvail=(unsigned int)snd_pcm_avail(handle);

		if(nPeriod<=nAvail)
		{
			unsigned int writeBufFilledInNStep=0;
			int nThSound=0;
			for(auto &p : playing)
			{
				if(nullptr!=p.dat)
				{
					unsigned int writePtr=0;
					PopulateWriteBuffer(writePtr,p.ptr,p.dat,p.loop,nThSound);
					writeBufFilledInNStep=std::max(writePtr,writeBufFilledInNStep);
					++nThSound;
				}
			}
			for(auto &p : playingStream)
			{
				// p.ptr is pointer in number of samples (not in number of bytes).
				if(nullptr!=p.dat)
				{
					unsigned int writePtr=0;
					YSBOOL loop=YSFALSE;
					if(0!=p.dat->api->playing.NTimeStep())
					{
						bool firstBufferDone=PopulateWriteBuffer(writePtr,p.ptr,&p.dat->api->playing,loop,nThSound);
						if(true==firstBufferDone)
						{
							// PopulateWriteBuffer returning true means the buffer is gone to the end.
							if(0!=p.dat->api->standBy.NTimeStep())
							{
								PopulateWriteBuffer(writePtr,0,&p.dat->api->standBy,loop,nThSound);
							}
						}
						++nThSound;
						writeBufFilledInNStep=std::max(writePtr,writeBufFilledInNStep);
					}
				}
			}

			if(0<nThSound)
			{
				int nWritten=(0<writeBufFilledInNStep ? snd_pcm_writei(handle,writeBuf,writeBufFilledInNStep) : 0);
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
					int outPlaybackRate=this->rate;
					for(auto &p : playing)
					{
						if(nullptr!=p.dat)
						{
							if(YSTRUE!=p.loop)
							{
								p.ptr+=nWritten*p.dat->PlayBackRate()/outPlaybackRate;
								if(p.dat->NTimeStep()<=p.ptr)
								{
									p.dat=NULL;
									p.ptr=0;
								}
							}
							else
							{
								p.ptr+=nWritten*p.dat->PlayBackRate()/outPlaybackRate;
								while(p.ptr>=p.dat->NTimeStep())
								{
									p.ptr-=p.dat->NTimeStep();
								}
							}
						}
					}
					for(auto &p : playingStream)
					{
						if(nullptr!=p.dat)
						{
							p.ptr+=nWritten*p.dat->api->playing.PlayBackRate()/outPlaybackRate;
							if(p.dat->api->playing.NTimeStep()<=p.ptr)
							{
								p.ptr-=p.dat->api->playing.NTimeStep();
								p.dat->api->playing.MoveFrom(p.dat->api->standBy);
								p.dat->api->standBy.CleanUp();
								if(p.dat->api->playing.NTimeStep()<=p.ptr)
								{
									p.dat->api->playing.CleanUp();
									p.ptr=0;
								}
							}
							if(0==p.dat->api->playing.NTimeStep())
							{
								p.ptr=0;
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

bool YsSoundPlayer::APISpecificData::PopulateWriteBuffer(unsigned int &writePtr,unsigned int wavPtr,const SoundData *wavFile,YSBOOL loop,int nThSound)
{
	TimerMaskGuard tmg;

	bool notLoopAndAllTheWayToEnd=false;

	int64_t numSamplesIn=wavFile->GetNumSamplePerChannel();
	int64_t numSamplesOut=bufSizeInNStep;

	int numChannelsIn=wavFile->GetNumChannel();
	int numChannelsOut=this->nChannel;

	int playbackRate=this->rate;

	int balance=0;
	int inChannel1=numChannelsIn-1;
	while(writePtr<bufSizeInNStep && wavPtr<numSamplesIn)
	{
		short chOut[2];
		chOut[0]=wavFile->GetSignedValue16(0,wavPtr);
		chOut[1]=wavFile->GetSignedValue16(inChannel1,wavPtr);

		for(int outCh=0; outCh<numChannelsOut; ++outCh)
		{
			if(0==nThSound)
			{
				writeBuf[writePtr*bytePerTimeStep+outCh*2  ]=chOut[outCh]&255;
				writeBuf[writePtr*bytePerTimeStep+outCh*2+1]=(chOut[outCh]>>8)&255;
			}
			else
			{
				int c=  (int)writeBuf[writePtr*bytePerTimeStep+outCh*2]
				     +(((int)writeBuf[writePtr*bytePerTimeStep+outCh*2+1])<<8);
				c=(c&0x7fff)-(c&0x8000);
				auto v=chOut[outCh]+c;
				if(32767<v)
				{
					v=32767;
				}
				if(v<-32768)
				{
					v=-32768;
				}
				writeBuf[writePtr*bytePerTimeStep+outCh*2  ]=v&0xff;
				writeBuf[writePtr*bytePerTimeStep+outCh*2+1]=((v>>8)&0xff);
			}
			++writePtr;

			balance-=wavFile->PlayBackRate();
			while(balance<0)
			{
				balance+=playbackRate;
				++wavPtr;
			}

			if(wavFile->NTimeStep()<=wavPtr)
			{
				if(YSTRUE!=loop)
				{
					notLoopAndAllTheWayToEnd=true;
				}
				else
				{
					wavPtr=0;
				}
			}
		}
	}
	if(0==nThSound)
	{
		for(auto ptr=writePtr; ptr<bufSizeInNStep; ++ptr)
		{
			for(int outCh=0; outCh<numChannelsOut; ++outCh)
			{
				writeBuf[ptr*bytePerTimeStep+outCh*2  ]=0;
				writeBuf[ptr*bytePerTimeStep+outCh*2+1]=0;
			}
		}
	}

	return notLoopAndAllTheWayToEnd;
}

void YsSoundPlayer::APISpecificData::PrintState(int errCode)
{
	TimerMaskGuard tmg;

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

double YsSoundPlayer::APISpecificData::GetCurrentPosition(const SoundData &dat) const
{
	TimerMaskGuard tmg;

 	for(auto &p : playing)
	{
		if(&dat==p.dat)
		{
			return (double)p.ptr/(double)dat.PlayBackRate();
		}
	}
	return 0.0;
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

void YsSoundPlayer::SetVolumeAPISpecific(SoundData &dat,float leftVol,float rightVol)
{
}

void YsSoundPlayer::KeepPlayingAPISpecific(void)
{
#ifndef ALSA_USE_INTERVAL_TIMER
	APISpecificData::TimerMaskGuard tmg;
	api->KeepPlaying();
#endif
}

YSRESULT YsSoundPlayer::PlayOneShotAPISpecific(SoundData &dat)
{
	APISpecificData::TimerMaskGuard tmg;

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

	return YSOK;
}
YSRESULT YsSoundPlayer::PlayBackgroundAPISpecific(SoundData &dat)
{
	APISpecificData::TimerMaskGuard tmg;

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

	return YSOK;
}

YSBOOL YsSoundPlayer::IsPlayingAPISpecific(const SoundData &dat) const
{
	APISpecificData::TimerMaskGuard tmg;

	for(auto &p : api->playing)
	{
		if(&dat==p.dat)
		{
			return YSTRUE;
		}
	}
	return YSFALSE;
}

double YsSoundPlayer::GetCurrentPositionAPISpecific(const SoundData &dat) const
{
	APISpecificData::TimerMaskGuard tmg;
	return api->GetCurrentPosition(dat);
}

void YsSoundPlayer::StopAPISpecific(SoundData &dat)
{
	APISpecificData::TimerMaskGuard tmg;

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

void YsSoundPlayer::PauseAPISpecific(SoundData &dat)
{
	APISpecificData::TimerMaskGuard tmg;
}

void YsSoundPlayer::ResumeAPISpecific(SoundData &dat)
{
	APISpecificData::TimerMaskGuard tmg;
}

////////////////////////////////////////////////////////////

YsSoundPlayer::SoundData::APISpecificDataPerSoundData::APISpecificDataPerSoundData()
{
	APISpecificData::TimerMaskGuard tmg;
	CleanUp();
}
YsSoundPlayer::SoundData::APISpecificDataPerSoundData::~APISpecificDataPerSoundData()
{
	APISpecificData::TimerMaskGuard tmg;
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
	APISpecificData::TimerMaskGuard tmg;
	Resample(player.api->rate);
	ConvertToMono();
	ConvertTo16Bit();
	ConvertToSigned();
	return YSOK;
}

void YsSoundPlayer::SoundData::CleanUpAPISpecific(void)
{
	APISpecificData::TimerMaskGuard tmg;
	api->CleanUp();
}

////////////////////////////////////////////////////////////

YsSoundPlayer::Stream::APISpecificData *YsSoundPlayer::Stream::CreateAPISpecificData(void)
{
	APISpecificData *api=new APISpecificData;
	return api;
}
void YsSoundPlayer::Stream::DeleteAPISpecificData(APISpecificData *api)
{
	delete api;
}

YSRESULT YsSoundPlayer::StartStreamingAPISpecific(Stream &stream,StreamingOption)
{
	APISpecificData::TimerMaskGuard tmg;
	for(auto playingStream : this->api->playingStream)
	{
		if(playingStream.dat==&stream)
		{
			return YSOK; // Already playing
		}
	}

	APISpecificData::PlayingStream newStream;
	newStream.Make(stream);
	this->api->playingStream.push_back(newStream);

	return YSOK;
}
void YsSoundPlayer::StopStreamingAPISpecific(Stream &stream)
{
	APISpecificData::TimerMaskGuard tmg;
	for(int i=0; i<this->api->playingStream.size(); ++i)
	{
		if(this->api->playingStream[i].dat==&stream)
		{
			this->api->playingStream[i]=this->api->playingStream.back();
			this->api->playingStream.pop_back();
		}
	}
}
YSBOOL YsSoundPlayer::StreamPlayerReadyToAcceptNextNumSampleAPISpecific(const Stream &stream,unsigned int) const
{
	APISpecificData::TimerMaskGuard tmg;
	if(0==stream.api->standBy.NTimeStep())
	{
		return YSTRUE;
	}
	return YSFALSE;
}
YSRESULT YsSoundPlayer::AddNextStreamingSegmentAPISpecific(Stream &stream,const SoundData &dat)
{
	APISpecificData::TimerMaskGuard tmg;
	if(0==stream.api->playing.NTimeStep())
	{
		stream.api->playing.CopyFrom(dat);
		return YSOK;
	}
	if(0==stream.api->standBy.NTimeStep())
	{
		stream.api->standBy.CopyFrom(dat);
		return YSOK;
	}
	return YSERR;
}
