/* ////////////////////////////////////////////////////////////

File Name: fssimplewindowios.h
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

#ifndef FSSIMPLEWINDOW_IOS_H_IS_INCLUDED
#define FSSIMPLEWINDOW_IOS_H_IS_INCLUDED

#ifdef __cplusplus
extern "C" {   
#endif

	/* For communication between XCode framework and FsSimpleWindow framework >> */
	void FsIOSReportWindowSize(int wid,int hei);
	unsigned int FsIOSCreateDepthBuffer(int backingWidth,int backingHeight);
	void FsIOSDeleteDepthBuffer(unsigned int depthBuffer);
	void FsIOSReportCurrentTouch(int nTouch,const int touchCoord[]);
	/* For communication between XCode framework and FsSimpleWindow framework << */
	
	
	void FsChangeToProgramDirC(void);
	void FsSleepC(int ms);
	long long int FsSubSecondTimerC(void);
	long long int FsPassedTimeC(void);
	void FsSwapBuffersC(void);
	int FsGetNumCurrentTouchC(void);
	const int *FsGetCurrentTouchC(void);

#ifdef __cplusplus
}
#endif


#ifdef __OBJC__
void FsIOSReportUIViewPointer(UIView *view);
#endif


#endif


