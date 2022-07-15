#import <Cocoa/Cocoa.h>
#import <AVFoundation/AVFoundation.h>



#define PLAYBACK_RATE 44100
#define PLAYBACK_CHANNELS 2

struct YsAVAudioEngine
{
#if !__has_feature(objc_arc)
    AVAudioEngine *enginePtr;
    AVAudioMixerNode *mixerNodePtr;
    AVAudioPlayerNode *primaryPlayerNodePtr;
    AVAudioFormat *primaryAudioFormatPtr;
#else
	void *enginePtr;
	void *mixerNodePtr;
	void *primaryPlayerNodePtr;
	void *primaryAudioFormatPtr;
#endif
};

extern struct YsAVAudioEngine *YsSimpleSound_OSX_CreateAudioEngine(void);
extern void YsSimpleSound_OSX_DeleteAudioEngine(struct YsAVAudioEngine *engine);


struct YsAVAudioEngine *YsSimpleSound_OSX_CreateAudioEngine(void)
{
	struct YsAVAudioEngine *engineInfoPtr=(struct YsAVAudioEngine *)malloc(sizeof(struct YsAVAudioEngine));

    AVAudioEngine *enginePtr=[[AVAudioEngine alloc] init];
    AVAudioMixerNode *mixerNodePtr=[enginePtr mainMixerNode];
    AVAudioPlayerNode *primaryPlayerNodePtr=[[AVAudioPlayerNode alloc] init];
	AVAudioFormat *primaryAudioFormatPtr=[[AVAudioFormat alloc] initStandardFormatWithSampleRate:PLAYBACK_RATE channels:PLAYBACK_CHANNELS];

    [enginePtr attachNode:primaryPlayerNodePtr];
    [enginePtr connect:primaryPlayerNodePtr to:mixerNodePtr format:primaryAudioFormatPtr];

    NSError *err=nil;
    [enginePtr startAndReturnError:&err];
    if(nil!=err)
    {
        printf("Error %ld\n",[err code]);
        printf("%s\n",[[err localizedDescription] UTF8String]);
    }

#if !__has_feature(objc_arc)
    engineInfoPtr->enginePtr=enginePtr;
    engineInfoPtr->mixerNodePtr=mixerNodePtr;
	engineInfoPtr->primaryPlayerNodePtr=primaryPlayerNodePtr;
	engineInfoPtr->primaryAudioFormatPtr=primaryAudioFormatPtr;
#else
	engineInfoPtr->enginePtr=(void *)CFBridgingRetain(enginePtr);
	engineInfoPtr->mixerNodePtr=(void *)CFBridgingRetain(mixerNodePtr);
	engineInfoPtr->primaryPlayerNodePtr=(void *)CFBridgingRetain(primaryPlayerNodePtr);
	engineInfoPtr->primaryAudioFormatPtr=(void *)CFBridgingRetain(primaryAudioFormatPtr);
#endif

	return engineInfoPtr;
}

void YsSimpleSound_OSX_DeleteAudioEngine(struct YsAVAudioEngine *engineInfoPtr)
{
    AVAudioEngine *enginePtr=nil;
    AVAudioMixerNode *mixerNodePtr=nil;
    AVAudioPlayerNode *primaryPlayerNodePtr=nil;

#if !__has_feature(objc_arc)
    enginePtr=engineInfoPtr->enginePtr;
    mixerNodePtr=engineInfoPtr->mixerNodePtr;
	primaryPlayerNodePtr=engineInfoPtr->primaryPlayerNodePtr;
#else
	enginePtr=(__bridge AVAudioEngine *)engineInfoPtr->enginePtr;
	mixerNodePtr=(__bridge AVAudioMixerNode *)engineInfoPtr->mixerNodePtr;
	primaryPlayerNodePtr=(__bridge AVAudioPlayerNode *)engineInfoPtr->primaryPlayerNodePtr;
#endif

	[enginePtr detachNode:primaryPlayerNodePtr];

#if !__has_feature(objc_arc)
	[engineInfoPtr->enginePtr release];
	[engineInfoPtr->primaryPlayerNodePtr release];
	[engineInfoPtr->primaryAudioFormatPtr release];
#else
	CFBridgingRelease(engineInfoPtr->enginePtr);
	CFBridgingRelease(engineInfoPtr->primaryPlayerNodePtr);
	CFBridgingRelease(engineInfoPtr->primaryAudioFormatPtr);
#endif

	free(engineInfoPtr);
}

struct YsAVSound
{
	int bufferCount;
	uint64_t samplingRate;

#if !__has_feature(objc_arc)
    AVAudioEngine *enginePtr;

    AVAudioPlayerNode *playerNodePtr;
    AVAudioPCMBuffer *PCMBufferPtr;
    AVAudioFormat *audioFormatPtr;
#else
	void *enginePtr;

	void *playerNodePtr;
	void *PCMBufferPtr;
	void *audioFormatPtr;
#endif
};


extern struct YsAVSound *YsSimpleSound_OSX_CreateSound(struct YsAVAudioEngine *engineInfoPtr,long long int sizeInBytes,const unsigned char wavByteData[],unsigned int samplingRate,unsigned int numChannels);
extern void YsSimpleSound_OSX_DeleteSound(struct YsAVSound *ptr);
extern void YsSimpleSound_OSX_PlayOneShot(struct YsAVAudioEngine *engineInfoPtr,struct YsAVSound *ptr);
extern void YsSimpleSound_OSX_PlayBackground(struct YsAVAudioEngine *engineInfoPtr,struct YsAVSound *ptr);
extern void YsSimpleSound_OSX_SetVolume(struct YsAVAudioEngine *engineInfoPtr,struct YsAVSound *ptr,float vol);
extern void YsSimpleSound_OSX_Stop(struct YsAVAudioEngine *engineInfoPtr,struct YsAVSound *ptr);
extern void YsSimpleSound_OSX_Pause(struct YsAVAudioEngine *engineInfoPtr,struct YsAVSound *ptr);
extern void YsSimpleSound_OSX_Resume(struct YsAVAudioEngine *engineInfoPtr,struct YsAVSound *ptr);
extern bool YsSimpleSound_OSX_IsPlaying(struct YsAVAudioEngine *engineInfoPtr,struct YsAVSound *ptr);
extern double YsSimpleSound_OSX_GetCurrentPosition(struct YsAVAudioEngine *engineInfoPtr,struct YsAVSound *ptr);


struct YsAVSound *YsSimpleSound_OSX_CreateSound(struct YsAVAudioEngine *engineInfoPtr,long long int sizeInBytes,const unsigned char wavByteData[],unsigned int samplingRate,unsigned int numChannels)
{
	int64_t numSamplesIn=(sizeInBytes/2)/numChannels;
	int64_t numSamplesOut=numSamplesIn;
	numSamplesOut*=PLAYBACK_RATE;
	numSamplesOut/=samplingRate;


    /* According to https://developer.apple.com/documentation/avfoundation/avaudioformat/1390416-initstandardformatwithsamplerate?language=objc
       the returned format always uses AVAudioPCMFormatFloat32.
    */
    AVAudioFormat *audioFormatPtr=[[AVAudioFormat alloc] initStandardFormatWithSampleRate:PLAYBACK_RATE channels:PLAYBACK_CHANNELS];

    AVAudioPCMBuffer *PCMBufferPtr=[[AVAudioPCMBuffer alloc] initWithPCMFormat:audioFormatPtr frameCapacity:numSamplesOut];
    [PCMBufferPtr setFrameLength:numSamplesOut];

    int stride=[PCMBufferPtr stride];
    for(int ch=0; ch<[audioFormatPtr channelCount]; ++ch)
    {
		const unsigned char *channelSrcPtr=wavByteData+2*ch*(numChannels-1);

		int64_t balance=0;
		for(int i=0; i<[PCMBufferPtr frameLength]; ++i)
		{
			int data=(channelSrcPtr[1]<<8)|channelSrcPtr[0];
			data=(data&0x7FFF)-(data&0x8000);
			[PCMBufferPtr floatChannelData][ch][i*stride]=(float)data/32768.0f;;
			balance-=samplingRate;
			while(balance<0)
			{
				balance+=PLAYBACK_RATE;
				channelSrcPtr+=2*numChannels;
			}
		}
    }

    AVAudioPlayerNode *playerNodePtr=[[AVAudioPlayerNode alloc] init];

#if !__has_feature(objc_arc)
    AVAudioEngine *enginePtr=engineInfoPtr->enginePtr;
    AVAudioMixerNode *mixerNodePtr=engineInfoPtr->mixerNodePtr;
#else
	AVAudioEngine *enginePtr=(__bridge AVAudioEngine *)engineInfoPtr->enginePtr;
	AVAudioMixerNode *mixerNodePtr=(__bridge AVAudioMixerNode *)engineInfoPtr->mixerNodePtr;
#endif

    [enginePtr attachNode:playerNodePtr];
    [enginePtr connect:playerNodePtr to:mixerNodePtr format:audioFormatPtr];


	struct YsAVSound *snd=NULL;
	snd=(struct YsAVSound *)malloc(sizeof(struct YsAVSound));
	snd->playerNodePtr=nil;
	snd->PCMBufferPtr=nil;
	snd->audioFormatPtr=nil;
	snd->bufferCount=0;
	snd->samplingRate=samplingRate;

#if !__has_feature(objc_arc)
	snd->enginePtr=enginePtr;
	snd->playerNodePtr=playerNodePtr;
	snd->PCMBufferPtr=PCMBufferPtr;
	snd->audioFormatPtr=audioFormatPtr;
#else
	snd->enginePtr=(void*)CFBridgingRetain(enginePtr);
	snd->playerNodePtr=(void*)CFBridgingRetain(playerNodePtr);
	snd->PCMBufferPtr=(void*)CFBridgingRetain(PCMBufferPtr);
	snd->audioFormatPtr=(void*)CFBridgingRetain(audioFormatPtr);
#endif

	return snd;
}

void YsSimpleSound_OSX_DeleteSound(struct YsAVSound *ptr)
{
#if !__has_feature(objc_arc)
    AVAudioEngine *enginePtr=ptr->enginePtr;
	AVAudioPlayerNode *playerNodePtr=ptr->playerNodePtr;
#else
	AVAudioEngine *enginePtr=(__bridge AVAudioEngine *)ptr->enginePtr;
	AVAudioPlayerNode *playerNodePtr=(__bridge AVAudioPlayerNode *)ptr->playerNodePtr;
#endif

	[enginePtr detachNode:playerNodePtr];

#if !__has_feature(objc_arc)
	[ptr->playerNodePtr release];
	[ptr->PCMBufferPtr release];
	[ptr->audioFormatPtr release];
#else
	CFBridgingRelease(ptr->playerNodePtr);
	CFBridgingRelease(ptr->PCMBufferPtr);
	CFBridgingRelease(ptr->audioFormatPtr);
#endif
	free(ptr);
}

/* For future reference.  Lambda in Objective-C.  Another failure of Objective-C.
void (^YsAVPlayerCompletion)()=^()
{
	printf("Objective-C go to hell.\n");
};
See also http://fuckingblocksyntax.com/
*/

void YsSimpleSound_OSX_PlayOneShot(struct YsAVAudioEngine *engineInfoPtr,struct YsAVSound *ptr)
{
	if(nil!=ptr)
	{
#if !__has_feature(objc_arc)
	    AVAudioEngine *enginePtr=engineInfoPtr->enginePtr;
	    AVAudioMixerNode *mixerNodePtr=engineInfoPtr->mixerNodePtr;
		AVAudioPlayerNode *playerNodePtr=ptr->playerNodePtr;
		AVAudioPCMBuffer *PCMBufferPtr=ptr->PCMBufferPtr;
#else
		AVAudioEngine *enginePtr=(__bridge AVAudioEngine *)engineInfoPtr->enginePtr;
		AVAudioMixerNode *mixerNodePtr=(__bridge AVAudioMixerNode *)engineInfoPtr->mixerNodePtr;
		AVAudioPlayerNode *playerNodePtr=(__bridge AVAudioPlayerNode *)ptr->playerNodePtr;
		AVAudioPCMBuffer *PCMBufferPtr=(__bridge AVAudioPCMBuffer *)ptr->PCMBufferPtr;
#endif
		if(0<[PCMBufferPtr frameLength])
		{
			[playerNodePtr play];
			/* The following line won't give an error, but not useful.
			[playerNodePtr scheduleBuffer:PCMBufferPtr completionHandler:YsAVPlayerCompletion()]; 
			*/

			++ptr->bufferCount;

			__block struct YsAVSound *soundCopy=ptr;
			[playerNodePtr scheduleBuffer:PCMBufferPtr completionHandler:^{
				// How can I write a captured variable correctly?
				if(0<soundCopy->bufferCount)  // Apparently stop method also invokes this completionHandler.
				{
					--soundCopy->bufferCount;
				}
			}];
		}
	}
}

void YsSimpleSound_OSX_PlayBackground(struct YsAVAudioEngine *engineInfoPtr,struct YsAVSound *ptr)
{
	if(nil!=ptr)
	{
#if !__has_feature(objc_arc)
	    AVAudioEngine *enginePtr=engineInfoPtr->enginePtr;
	    AVAudioMixerNode *mixerNodePtr=engineInfoPtr->mixerNodePtr;
		AVAudioPlayerNode *playerNodePtr=ptr->playerNodePtr;
		AVAudioPCMBuffer *PCMBufferPtr=ptr->PCMBufferPtr;
#else
		AVAudioEngine *enginePtr=(__bridge AVAudioEngine *)engineInfoPtr->enginePtr;
		AVAudioMixerNode *mixerNodePtr=(__bridge AVAudioMixerNode *)engineInfoPtr->mixerNodePtr;
		AVAudioPlayerNode *playerNodePtr=(__bridge AVAudioPlayerNode *)ptr->playerNodePtr;
		AVAudioPCMBuffer *PCMBufferPtr=(__bridge AVAudioPCMBuffer *)ptr->PCMBufferPtr;
#endif

		if(0<[PCMBufferPtr frameLength])
		{
			[playerNodePtr play];
			++ptr->bufferCount;
			[playerNodePtr scheduleBuffer:PCMBufferPtr atTime:nil options:AVAudioPlayerNodeBufferLoops completionHandler:^{
				/* How can I write a captured variable correctly? ->

				Surprise!  Surprise!
				According to:
					https://www.mikeash.com/pyblog/friday-qa-2011-06-03-objective-c-blocks-vs-c0x-lambdas-fight.html
				automatic reference counting does not apply to the captured variables.
				Sounds like I cannot stop the player simply by [playerNodePtr stop];
				*/
			}];
		}
	}
}

void YsSimpleSound_OSX_SetVolume(struct YsAVAudioEngine *engineInfoPtr,struct YsAVSound *ptr,float vol)
{
	if(nil!=ptr)
	{
	}
}

void YsSimpleSound_OSX_Stop(struct YsAVAudioEngine *engineInfoPtr,struct YsAVSound *ptr)
{
	if(nil!=ptr)
	{
#if !__has_feature(objc_arc)
	    AVAudioEngine *enginePtr=engineInfoPtr->enginePtr;
	    AVAudioMixerNode *mixerNodePtr=engineInfoPtr->mixerNodePtr;
		__block AVAudioPlayerNode *playerNodePtr=ptr->playerNodePtr;
		AVAudioPCMBuffer *PCMBufferPtr=ptr->PCMBufferPtr;
#else
		AVAudioEngine *enginePtr=(__bridge AVAudioEngine *)engineInfoPtr->enginePtr;
		AVAudioMixerNode *mixerNodePtr=(__bridge AVAudioMixerNode *)engineInfoPtr->mixerNodePtr;
		__block AVAudioPlayerNode *playerNodePtr=(__bridge AVAudioPlayerNode *)ptr->playerNodePtr;
		AVAudioPCMBuffer *PCMBufferPtr=(__bridge AVAudioPCMBuffer *)ptr->PCMBufferPtr;
#endif

		[playerNodePtr stop];
		ptr->bufferCount=0;
	}
}

void YsSimpleSound_OSX_Pause(struct YsAVAudioEngine *engineInfoPtr,struct YsAVSound *ptr)
{
}

void YsSimpleSound_OSX_Resume(struct YsAVAudioEngine *engineInfoPtr,struct YsAVSound *ptr)
{
}

bool YsSimpleSound_OSX_IsPlaying(struct YsAVAudioEngine *engineInfoPtr,struct YsAVSound *ptr)
{
	if(nil!=ptr)
	{
#if !__has_feature(objc_arc)
		AVAudioPlayerNode *playerNodePtr=ptr->playerNodePtr;
#else
		AVAudioPlayerNode *playerNodePtr=(__bridge AVAudioPlayerNode *)ptr->playerNodePtr;
#endif
		// if(YES==[playerNodePtr isPlaying])  <- It didn't help.
		if(0<ptr->bufferCount)
		{
			return true;
		}
	}
	return false;
}

double YsSimpleSound_OSX_GetCurrentPosition(struct YsAVAudioEngine *engineInfoPtr,struct YsAVSound *ptr)
{
	if(nil!=ptr)
	{
#if !__has_feature(objc_arc)
	    AVAudioEngine *enginePtr=engineInfoPtr->enginePtr;
	    AVAudioMixerNode *mixerNodePtr=engineInfoPtr->mixerNodePtr;
		__block AVAudioPlayerNode *playerNodePtr=ptr->playerNodePtr;
		AVAudioPCMBuffer *PCMBufferPtr=ptr->PCMBufferPtr;
#else
		AVAudioEngine *enginePtr=(__bridge AVAudioEngine *)engineInfoPtr->enginePtr;
		AVAudioMixerNode *mixerNodePtr=(__bridge AVAudioMixerNode *)engineInfoPtr->mixerNodePtr;
		__block AVAudioPlayerNode *playerNodePtr=(__bridge AVAudioPlayerNode *)ptr->playerNodePtr;
		AVAudioPCMBuffer *PCMBufferPtr=(__bridge AVAudioPCMBuffer *)ptr->PCMBufferPtr;
#endif

		AVAudioTime *t=[playerNodePtr playerTimeForNodeTime:[playerNodePtr lastRenderTime]];
		uint64_t samplePos=[t sampleTime];

		return (double)samplePos/(double)ptr->samplingRate;
	}
	return 0.0;
}



struct YsAVAudioStreamPlayer
{
	int playingBuffer; // 0 or 1.  1-playingBuffer is stand-by.
	int numBuffersFilled;

#if !__has_feature(objc_arc)
    AVAudioEngine *enginePtr;

    AVAudioPlayerNode *playerNodePtr;
    AVAudioPCMBuffer *PCMBufferPtr[2];
#else
	void *enginePtr;

	void *playerNodePtr;
	void *PCMBufferPtr[2];
#endif
};

extern struct YsAVAudioStreamPlayer *YsSimpleSound_OSX_CreateStreamPlayer(struct YsAVAudioEngine *engineInfoPtr);
extern void YsSimpleSound_OSX_DeleteStreamPlayer(struct YsAVAudioStreamPlayer *streamPlayer);

extern int YsSimpleSound_OSX_StartStreaming(struct YsAVAudioEngine *engineInfoPtr,struct YsAVAudioStreamPlayer *streamPlayer);
extern void YsSimpleSound_OSX_StopStreaming(struct YsAVAudioEngine *engineInfoPtr,struct YsAVAudioStreamPlayer *streamPlayer);
extern int YsSimpleSound_OSX_StreamPlayerReadyToAcceptNextSegment(struct YsAVAudioEngine *engineInfoPtr,struct YsAVAudioStreamPlayer *streamPlayer);
extern int YsSimpleSound_OSX_AddNextStreamingSegment(struct YsAVAudioEngine *engineInfoPtr,struct YsAVAudioStreamPlayer *streamPlayer,long long int sizeInBytes,const unsigned char wavByteData[],unsigned int samplingRate,unsigned int numChannels);


struct YsAVAudioStreamPlayer *YsSimpleSound_OSX_CreateStreamPlayer(struct YsAVAudioEngine *engineInfoPtr)
{
    AVAudioEngine *enginePtr=nil;
    AVAudioMixerNode *mixerNodePtr=nil;
    AVAudioFormat *audioFormatPtr=nil;

#if !__has_feature(objc_arc)
    enginePtr=engineInfoPtr->enginePtr;
    mixerNodePtr=engineInfoPtr->mixerNodePtr;
	audioFormatPtr=engineInfoPtr->primaryAudioFormatPtr;
#else
	enginePtr=(__bridge AVAudioEngine *)engineInfoPtr->enginePtr;
	mixerNodePtr=(__bridge AVAudioMixerNode *)engineInfoPtr->mixerNodePtr;
	audioFormatPtr=(__bridge AVAudioFormat *)engineInfoPtr->primaryAudioFormatPtr;
#endif

    AVAudioPlayerNode *playerNodePtr=[[AVAudioPlayerNode alloc] init];
    [enginePtr attachNode:playerNodePtr];
    [enginePtr connect:playerNodePtr to:mixerNodePtr format:audioFormatPtr];

	struct YsAVAudioStreamPlayer *streamPlayer=(struct YsAVAudioStreamPlayer *)malloc(sizeof(struct YsAVAudioStreamPlayer));

	streamPlayer->playingBuffer=0;
	streamPlayer->numBuffersFilled=0;
	streamPlayer->PCMBufferPtr[0]=nil;
	streamPlayer->PCMBufferPtr[1]=nil;

#if !__has_feature(objc_arc)
	streamPlayer->enginePtr=enginePtr;
	streamPlayer->playerNodePtr=playerNodePtr;
#else
	streamPlayer->enginePtr=(void*)CFBridgingRetain(enginePtr);
	streamPlayer->playerNodePtr=(void*)CFBridgingRetain(playerNodePtr);
#endif

	return streamPlayer;
}
void YsSimpleSound_OSX_DeleteStreamPlayer(struct YsAVAudioStreamPlayer *streamPlayer)
{
#if !__has_feature(objc_arc)
	[streamPlayer->playerNodePtr release];
	for(int i=0; i<2; ++i)
	{
		if(nil!=streamPlayer->PCMBufferPtr[i])
		{
			[streamPlayer->PCMBufferPtr[i] release];
		}
	}
#else
	CFBridgingRelease(streamPlayer->playerNodePtr);
	for(int i=0; i<2; ++i)
	{
		if(nil!=streamPlayer->PCMBufferPtr[i])
		{
			CFBridgingRelease(streamPlayer->PCMBufferPtr[i]);
		}
	}
#endif
}

int YsSimpleSound_OSX_StartStreaming(struct YsAVAudioEngine *engineInfoPtr,struct YsAVAudioStreamPlayer *streamPlayer)
{
	if(nil!=streamPlayer)
	{
#if !__has_feature(objc_arc)
	    AVAudioEngine *enginePtr=engineInfoPtr->enginePtr;
	    AVAudioMixerNode *mixerNodePtr=engineInfoPtr->mixerNodePtr;
		AVAudioFormat *audioFormat=engineInfoPtr->primaryAudioFormatPtr;
		AVAudioPlayerNode *playerNodePtr=streamPlayer->playerNodePtr;
#else
		AVAudioEngine *enginePtr=(__bridge AVAudioEngine *)engineInfoPtr->enginePtr;
		AVAudioMixerNode *mixerNodePtr=(__bridge AVAudioMixerNode *)engineInfoPtr->mixerNodePtr;
		AVAudioFormat *audioFormat=(__bridge AVAudioFormat *)engineInfoPtr->primaryAudioFormatPtr;
		AVAudioPlayerNode *playerNodePtr=(__bridge AVAudioPlayerNode *)streamPlayer->playerNodePtr;
#endif
		[playerNodePtr play];
		return 1;
	}
	return 0;
}
void YsSimpleSound_OSX_StopStreaming(struct YsAVAudioEngine *engineInfoPtr,struct YsAVAudioStreamPlayer *streamPlayer)
{
	if(nil!=streamPlayer)
	{
#if !__has_feature(objc_arc)
	    AVAudioEngine *enginePtr=engineInfoPtr->enginePtr;
	    AVAudioMixerNode *mixerNodePtr=engineInfoPtr->mixerNodePtr;
		AVAudioFormat *audioFormat=engineInfoPtr->primaryAudioFormatPtr;
		AVAudioPlayerNode *playerNodePtr=streamPlayer->playerNodePtr;
#else
		AVAudioEngine *enginePtr=(__bridge AVAudioEngine *)engineInfoPtr->enginePtr;
		AVAudioMixerNode *mixerNodePtr=(__bridge AVAudioMixerNode *)engineInfoPtr->mixerNodePtr;
		AVAudioFormat *audioFormat=(__bridge AVAudioFormat *)engineInfoPtr->primaryAudioFormatPtr;
		AVAudioPlayerNode *playerNodePtr=(__bridge AVAudioPlayerNode *)streamPlayer->playerNodePtr;
#endif
		[playerNodePtr stop];
		streamPlayer->numBuffersFilled=0;
	}
}
int YsSimpleSound_OSX_StreamPlayerReadyToAcceptNextSegment(struct YsAVAudioEngine *engineInfoPtr,struct YsAVAudioStreamPlayer *streamPlayer)
{
	if(streamPlayer->numBuffersFilled<2)
	{
		return 1;
	}
	return 0;
}
int YsSimpleSound_OSX_AddNextStreamingSegment(struct YsAVAudioEngine *engineInfoPtr,struct YsAVAudioStreamPlayer *streamPlayer,long long int sizeInBytes,const unsigned char wavByteData[],unsigned int samplingRate,unsigned int numChannels)
{
	if(nil!=streamPlayer && streamPlayer->numBuffersFilled<2)
	{
#if !__has_feature(objc_arc)
	    AVAudioEngine *enginePtr=engineInfoPtr->enginePtr;
	    AVAudioMixerNode *mixerNodePtr=engineInfoPtr->mixerNodePtr;
		AVAudioFormat *audioFormat=engineInfoPtr->primaryAudioFormatPtr;
		AVAudioPlayerNode *playerNodePtr=streamPlayer->playerNodePtr;

		[streamPlayer->PCMBufferPtr[1-streamPlayer->playingBuffer] release];
		streamPlayer->PCMBufferPtr[1-streamPlayer->playingBuffer]=nil;
#else
		AVAudioEngine *enginePtr=(__bridge AVAudioEngine *)engineInfoPtr->enginePtr;
		AVAudioMixerNode *mixerNodePtr=(__bridge AVAudioMixerNode *)engineInfoPtr->mixerNodePtr;
		AVAudioFormat *audioFormat=(__bridge AVAudioFormat *)engineInfoPtr->primaryAudioFormatPtr;
		AVAudioPlayerNode *playerNodePtr=(__bridge AVAudioPlayerNode *)streamPlayer->playerNodePtr;

		CFBridgingRelease(streamPlayer->PCMBufferPtr[1-streamPlayer->playingBuffer]);
		streamPlayer->PCMBufferPtr[1-streamPlayer->playingBuffer]=nil;
#endif

		// Hate to have duplicate code here.  But I'm not sure how ARC works if I make it a function.
		// That's a failed concept.  Objective-C should be killed.  But, Swift is evil as well.
		int64_t numSamplesIn=(sizeInBytes/2)/numChannels;
		int64_t numSamplesOut=numSamplesIn;
		numSamplesOut*=PLAYBACK_RATE;
		numSamplesOut/=samplingRate;

		if(0==numSamplesOut)
		{
			return 0;
		}

		AVAudioPCMBuffer *PCMBufferPtr=[[AVAudioPCMBuffer alloc] initWithPCMFormat:audioFormat frameCapacity:numSamplesOut];
		[PCMBufferPtr setFrameLength:numSamplesOut];

		int stride=[PCMBufferPtr stride];
		for(int ch=0; ch<[audioFormat channelCount]; ++ch)
		{
			const unsigned char *channelSrcPtr=wavByteData+2*ch*(numChannels-1);

			int64_t balance=0;
			for(int i=0; i<[PCMBufferPtr frameLength]; ++i)
			{
				int data=(channelSrcPtr[1]<<8)|channelSrcPtr[0];
				data=(data&0x7FFF)-(data&0x8000);
				[PCMBufferPtr floatChannelData][ch][i*stride]=(float)data/32768.0f;;
				balance-=samplingRate;
				while(balance<0)
				{
					balance+=PLAYBACK_RATE;
					channelSrcPtr+=2*numChannels;
				}
			}
		}

#if !__has_feature(objc_arc)
		streamPlayer->PCMBufferPtr[1-streamPlayer->playingBuffer]=PCMBufferPtr;
#else
		streamPlayer->PCMBufferPtr[1-streamPlayer->playingBuffer]=(void*)CFBridgingRetain(PCMBufferPtr);
#endif
		streamPlayer->playingBuffer=1-streamPlayer->playingBuffer;

		++streamPlayer->numBuffersFilled;

		__block struct YsAVAudioStreamPlayer *streamCopy=streamPlayer;
	    [playerNodePtr scheduleBuffer:PCMBufferPtr completionHandler:^{
			// How can I write a captured variable correctly?
			if(0<streamCopy->numBuffersFilled)  // Apparently stop method also invokes this completionHandler.
			{
				--streamCopy->numBuffersFilled;
			}
		}];
	}
	return 0;
}
