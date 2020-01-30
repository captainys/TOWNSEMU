/* ////////////////////////////////////////////////////////////

File Name: fssimplewindowios_objc.m
Copyright (c) 2017 Soji Yamakawa.  All rights reserved.
http://www.ysflight.com

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS 
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE 
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

//////////////////////////////////////////////////////////// */

#include "../fssimplewindow.h"

#import <UIKit/UIKit.h>
#import <UIKit/UIView.h>

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>


static UIView *viewCache=NULL;

void FsIOSReportUIViewPointer(UIView *view)
{
	viewCache=view;
}

unsigned int FsIOSCreateDepthBuffer(int backingWidth,int backingHeight)
{
	GLuint depthRenderbuffer;
	
	glGenRenderbuffers(1,&depthRenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER,depthRenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, backingWidth, backingHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
	
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        NSLog(@"Failed to make complete framebuffer object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
        return 0;
    }

	return depthRenderbuffer;
}

void FsIOSDeleteDepthBuffer(unsigned int depthBuffer)
{
	if (depthBuffer)
	{
		GLuint bufferIdArray[1]={(GLuint)depthBuffer};
		glDeleteRenderbuffers(1,bufferIdArray);
	}
}



void FsChangeToProgramDirC(void)
{
	NSString *path;
	path=[[NSBundle mainBundle] resourcePath]; // 2016/04/16 Changed from bundlePath to resourcePath
	printf("ResourcePath:%s\n",[path UTF8String]);
	
	[[NSFileManager defaultManager] changeCurrentDirectoryPath:path];
}

void FsSleepC(int ms)
{
	if(ms>0)
	{
		double sec;
		sec=(double)ms/1000.0;
		[NSThread sleepForTimeInterval:sec];
	}
}

long long int FsSubSecondTimerC(void)
{
	static int first=1;
	static NSTimeInterval t0=0.0;
	if(0!=first)
	{
		t0=[[NSDate date] timeIntervalSince1970];
		first=0;
	}


	long long int ms;

#if !__has_feature(objc_arc)
 	NSAutoreleasePool *pool=[[NSAutoreleasePool alloc] init];
#endif

	NSTimeInterval now;
	now=[[NSDate date] timeIntervalSince1970];

	NSTimeInterval passed;
	passed=now-t0;
	ms=(long long int)(1000.0*passed);

#if !__has_feature(objc_arc)
	[pool release];	
#endif

	return ms;
}

long long int FsPassedTimeC(void)
{
	int ms;
	
#if !__has_feature(objc_arc)
 	NSAutoreleasePool *pool=[[NSAutoreleasePool alloc] init];
#endif
	
	static NSTimeInterval last=0.0;
	NSTimeInterval now;
	
	now=[[NSDate date] timeIntervalSince1970];
	
	NSTimeInterval passed;
	passed=now-last;
	ms=(int)(1000.0*passed);
	
	if(ms<0)
	{
		ms=1;
	}
	last=now;
	
#if !__has_feature(objc_arc)
	[pool release];	
#endif

	static bool first=true;
	if(true==first)
	{
		first=false;
		ms=1;
	}

	return ms;
}

void FsSwapBuffersC(void)
{
	if(NULL!=viewCache)
	{
		// [[viewCache view] presentFramebuffer];
	}
}

#define FS_MAX_NUM_TOUCH 16
static int nTouchCache=0;
static int touchCache[FS_MAX_NUM_TOUCH];

void FsIOSReportCurrentTouch(int nTouch,const int touchCoord[])
{
	int i;
	nTouchCache=0;
	for(i=0; i<FS_MAX_NUM_TOUCH && i<nTouch; ++i)
	{
		touchCache[i*2  ]=touchCoord[i*2];
		touchCache[i*2+1]=touchCoord[i*2+1];
		++nTouchCache;
	}
}

int FsGetNumCurrentTouchC(void)
{
	return nTouchCache;
}

const int *FsGetCurrentTouchC(void)
{
	return touchCache;
}

