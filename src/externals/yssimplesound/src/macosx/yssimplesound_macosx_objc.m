#import <Cocoa/Cocoa.h>


#ifndef __has_feature
#define __has_feature(x) 0
#endif


struct YsNSSound
{
#if !__has_feature(objc_arc)
	NSSound *snd;
#else
	void *snd;
#endif
};


extern struct YsNSSound *YsSimpleSound_OSX_CreateSound(long long int size,const unsigned char wavByteData[]);
extern void YsSimpleSound_OSX_DeleteSound(struct YsNSSound *ptr);
extern void YsSimpleSound_OSX_PlayOneShot(struct YsNSSound *ptr);
extern void YsSimpleSound_OSX_PlayBackground(struct YsNSSound *ptr);
extern void YsSimpleSound_OSX_SetVolume(struct YsNSSound *ptr,float vol);
extern void YsSimpleSound_OSX_Stop(struct YsNSSound *ptr);
extern bool YsSimpleSound_OSX_IsPlaying(struct YsNSSound *ptr);


struct YsNSSound *YsSimpleSound_OSX_CreateSound(long long int size,const unsigned char wavByteData[])
{
	struct YsNSSound *snd=NULL;
	NSData *nsData=nil;
	NSSound *soundData=nil;

	snd=(struct YsNSSound *)malloc(sizeof(struct YsNSSound));
	snd->snd=nil;

	nsData=[NSData dataWithBytes:wavByteData length:size];
	soundData=[[NSSound alloc] initWithData:nsData];
#if !__has_feature(objc_arc)
	snd->snd=soundData;
#else
	snd->snd=(void *)CFBridgingRetain(soundData);
#endif

	return snd;
}

void YsSimpleSound_OSX_DeleteSound(struct YsNSSound *ptr)
{
#if !__has_feature(objc_arc)
	[ptr->snd release];
#else
	CFBridgingRelease(ptr->snd);
#endif
	free(ptr);
}

void YsSimpleSound_OSX_PlayOneShot(struct YsNSSound *ptr)
{
	if(nil!=ptr)
	{
#if !__has_feature(objc_arc)
		NSSound *snd=ptr->snd;
#else
		NSSound *snd=(__bridge NSSound *)(ptr->snd);
#endif
		[snd setLoops:NO];
		[snd play];
	}
}

void YsSimpleSound_OSX_PlayBackground(struct YsNSSound *ptr)
{
	if(nil!=ptr)
	{
#if !__has_feature(objc_arc)
		NSSound *snd=ptr->snd;
#else
		NSSound *snd=(__bridge NSSound *)(ptr->snd);
#endif
		[snd setLoops:YES];
		[snd play];
	}
}

void YsSimpleSound_OSX_SetVolume(struct YsNSSound *ptr,float vol)
{
	if(nil!=ptr)
	{
#if !__has_feature(objc_arc)
		NSSound *snd=ptr->snd;
#else
		NSSound *snd=(__bridge NSSound *)(ptr->snd);
#endif
		snd.volume=vol;
	}
}

void YsSimpleSound_OSX_Stop(struct YsNSSound *ptr)
{
	if(nil!=ptr)
	{
#if !__has_feature(objc_arc)
		NSSound *snd=ptr->snd;
#else
		NSSound *snd=(__bridge NSSound *)(ptr->snd);
#endif
		[snd stop];
	}
}

bool YsSimpleSound_OSX_IsPlaying(struct YsNSSound *ptr)
{
	if(nil!=ptr)
	{
#if !__has_feature(objc_arc)
		NSSound *snd=ptr->snd;
#else
		NSSound *snd=(__bridge NSSound *)(ptr->snd);
#endif
		if(YES==[snd isPlaying])
		{
			return true;
		}
	}
	return false;
}
