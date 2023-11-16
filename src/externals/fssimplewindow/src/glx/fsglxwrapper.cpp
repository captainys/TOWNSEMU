/* ////////////////////////////////////////////////////////////

File Name: fsglxwrapper.cpp
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

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <errno.h>

#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/poll.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/keysymdef.h>
#include <X11/Xatom.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>


#include "fssimplewindow.h"

#define FS_NUM_XK 65536

extern void FsXCreateKeyMapping(void);
extern int FsXKeySymToFsInkey(int keysym);
extern int FsXKeySymToFsGetKeyState(int keysym);
extern char FsXKeySymToChar(int keysym);
extern int FsXFskeyToKeySym(int fskey);

class FsMouseEventLog
{
public:
	int eventType;
	int lb,mb,rb;
	int mx,my;
	unsigned int shift,ctrl;
};

#define NKEYBUF 256
static int nKeyBufUsed=0;
static int keyBuffer[NKEYBUF];
static int nCharBufUsed=0;
static int charBuffer[NKEYBUF];
static int nMosBufUsed=0;
static FsMouseEventLog mosBuffer[NKEYBUF];



static Display *ysXDsp;
static Window ysXWnd;
static Colormap ysXCMap;
static XVisualInfo *ysXVis;
static const int ysXEventMask=(KeyPressMask|KeyReleaseMask|ButtonPressMask|ButtonReleaseMask|PointerMotionMask|ExposureMask|StructureNotifyMask);

static GLXContext ysGlRC;
static int ysGlxCfgSingle[]={GLX_RGBA,GLX_DEPTH_SIZE,16,None};
static int ysGlxCfgDouble[]={GLX_DOUBLEBUFFER,GLX_RGBA,GLX_DEPTH_SIZE,16,None};

static int ysXWid,ysXHei,ysXlupX,ysXlupY;



static int fsKeyPress[FSKEY_NUM_KEYCODE];
static int exposure=0;
static int lastKnownLb=0,lastKnownMb=0,lastKnownRb=0;



// For FsGui library tunnel >>
static long long int clipBoardContentLength=0;
static char *clipBoardContent=NULL;
static long long int pastedContentLength=0;
static char *pastedContent=NULL;
static void ProcessSelectionRequest(XEvent &evt);
// For FsGui library tunnel <<


static void ForceMoveWindow(Display *dsp,Window &wnd,int x,int y);

void FsOpenWindow(const FsOpenWindowOption &opt)
{
	int x0=opt.x0;
	int y0=opt.y0;
	int wid=opt.wid;
	int hei=opt.hei;
	int useDoubleBuffer=(int)opt.useDoubleBuffer;
	// int useMultiSampleBuffer=(int)opt.useMultiSampleBuffer;
	const char *title=(NULL!=opt.windowTitle ? opt.windowTitle : "Main Window");

	int n;
	char **m,*def;
	XSetWindowAttributes swa;
	Font font;

	int lupX,lupY,sizX,sizY;
	lupX=x0;
	lupY=y0;
	sizX=wid;
	sizY=hei;

	FsXCreateKeyMapping();
	for(n=0; n<FSKEY_NUM_KEYCODE; n++)
	{
		fsKeyPress[n]=0;
	}

	// Apparently XInitThread is requierd even if only one thread is accessing X-Window system, unless that thread is the main thread.
	// Weird.
	XInitThreads();
	ysXDsp=XOpenDisplay(NULL);

	if(ysXDsp!=NULL)
	{
		printf("Opened display.\n");
		if(glXQueryExtension(ysXDsp,NULL,NULL)!=0)
		{
			printf("Acquired GLX extension.\n");

			int tryAlternativeSingleBuffer=0;
			if(useDoubleBuffer!=0)
			{
				ysXVis=glXChooseVisual(ysXDsp,DefaultScreen(ysXDsp),ysGlxCfgDouble);
			}
			else
			{
				ysXVis=glXChooseVisual(ysXDsp,DefaultScreen(ysXDsp),ysGlxCfgSingle);
				if(NULL==ysXVis)
				{
					ysXVis=glXChooseVisual(ysXDsp,DefaultScreen(ysXDsp),ysGlxCfgDouble);
					tryAlternativeSingleBuffer=1;
				}
			}

			printf("Chose visual.\n");

			if(ysXVis!=NULL)
			{
				ysXCMap=XCreateColormap(ysXDsp,RootWindow(ysXDsp,ysXVis->screen),ysXVis->visual,AllocNone);
				printf("Created colormap.\n");

				ysGlRC=glXCreateContext(ysXDsp,ysXVis,None,GL_TRUE);
				if(ysGlRC!=NULL)
				{
					printf("Created OpenGL context.\n");

					swa.colormap=ysXCMap;
					swa.border_pixel=0;
					swa.event_mask=ysXEventMask;

					// Memo: lupX and lupY given to XCreateWindow will be ignored.
					//       Window must be moved to the desired position by XMoveWindow after XMapWindow.
					ysXWnd=XCreateWindow(ysXDsp,RootWindow(ysXDsp,ysXVis->screen),
							  lupX,lupY,sizX,sizY,
					                  1,
							  ysXVis->depth,
					                  InputOutput,
							  ysXVis->visual,
					                  CWEventMask|CWBorderPixel|CWColormap,&swa);

					printf("Created Window.\n");

					ysXWid=sizX;
					ysXHei=sizY;
					ysXlupX=lupX;
					ysXlupY=lupY;

					XStoreName(ysXDsp,ysXWnd,title);


// Should I use XSetWMProperties? titlebar problem.
					XWMHints wmHints;
					wmHints.flags=0;
					wmHints.initial_state=NormalState;
					XSetWMHints(ysXDsp,ysXWnd,&wmHints);


					XSetIconName(ysXDsp,ysXWnd,title);
					XMapWindow(ysXDsp,ysXWnd);

					// Memo: XCreateWindow probably ignore lupX and lupY.  Window must be moved here.
					ForceMoveWindow(ysXDsp,ysXWnd,lupX,lupY);
					// ForceMoveWindow may have failed to place the window, but let's at least reset lupX and lupY.
					ysXlupX=lupX;
					ysXlupY=lupY;

					printf("Zzz...\n");
					sleep(1);
					printf("Slept one second.\n");

					if(opt.sizeOpt==FsOpenWindowOption::FULLSCREEN)
					{
						FsMakeFullScreen();
					}
					else if(opt.sizeOpt==FsOpenWindowOption::MAXIMIZE_WINDOW)
					{
						FsMaximizeWindow();
					}

					/* printf("Wait Expose Event\n");
					XEvent ev;
					while(XCheckTypedEvent(ysXDsp,Expose,&ev)!=True)
					  {
					    printf("Waiting for create notify\n");
					    sleep(1);
					  }
					printf("Window=%d\n",ev.xexpose.window);
					printf("Window Created\n"); */

					glXMakeCurrent(ysXDsp,ysXWnd,ysGlRC);

					// These lines are needed, or window will not appear >>
				    glClearColor(1.0F,1.0F,1.0F,0.0F);
					glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
					glFlush();
					// glXSwapBuffers(ysXDsp,ysXWnd);
					// These lines are needed, or window will not appear <<

					glEnable(GL_DEPTH_TEST);
					glDepthFunc(GL_LEQUAL);
					glShadeModel(GL_SMOOTH);

					GLfloat dif[]={0.8F,0.8F,0.8F,1.0F};
					GLfloat amb[]={0.4F,0.4F,0.4F,1.0F};
					GLfloat spc[]={0.9F,0.9F,0.9F,1.0F};
					GLfloat shininess[]={50.0,50.0,50.0,0.0};

					glEnable(GL_LIGHTING);
					glEnable(GL_LIGHT0);
					glLightfv(GL_LIGHT0,GL_DIFFUSE,dif);
					glLightfv(GL_LIGHT0,GL_SPECULAR,spc);
					glMaterialfv(GL_FRONT|GL_BACK,GL_SHININESS,shininess);

					glLightModelfv(GL_LIGHT_MODEL_AMBIENT,amb);
					glEnable(GL_COLOR_MATERIAL);
					glEnable(GL_NORMALIZE);

					if(0!=tryAlternativeSingleBuffer)
					{
						glDrawBuffer(GL_FRONT);
					}

				    glClearColor(1.0F,1.0F,1.0F,0.0F);
				    glClearDepth(1.0F);
					glDisable(GL_DEPTH_TEST);

					glViewport(0,0,sizX,sizY);

				    glMatrixMode(GL_PROJECTION);
					glLoadIdentity();
					glOrtho(0,(float)sizX-1,(float)sizY-1,0,-1,1);

					glMatrixMode(GL_MODELVIEW);
					glLoadIdentity();

					glShadeModel(GL_FLAT);
					glPointSize(1);
					glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
					glColor3ub(0,0,0);

					if(NULL!=fsOpenGLInitializationCallBack)
					{
						(*fsOpenGLInitializationCallBack)(fsOpenGLInitializationCallBackParam);
					}

					if(NULL!=fsAfterWindowCreationCallBack)
					{
						(*fsAfterWindowCreationCallBack)(fsAfterWindowCreationCallBackParam);
					}
				}
				else
				{
					fprintf(stderr,"Cannot create OpenGL context.\n");
					exit(1);
				}
			}
			else
			{
				fprintf(stderr,"Double buffer not supported?\n");
				exit(1);
			}
		}
		else
		{
			fprintf(stderr,"This system doesn't support OpenGL.\n");
			exit(1);
		}
	}
	else
	{
		fprintf(stderr,"Cannot Open Display.\n");
		exit(1);
	}

	return;
}

void FsResizeWindow(int newWid,int newHei)
{
	XResizeWindow(ysXDsp,ysXWnd,newWid,newHei);
	XFlush(ysXDsp);
}

int FsCheckWindowOpen(void)
{
	if(ysXWnd!=NULL)
	{
		return 1;
	}
	return 0;
}

static void ForceMoveWindow(Display *dsp,Window &wnd,int goalX,int goalY)
{
	// This function tries to address the inability of X-Window system to place a window at a precise location.
	// However, the window may still be moved to a random location after this function.
	// There seems to be no remedy.

	auto timeOut=time(NULL); 

	int tryX=goalX;
	int tryY=goalY;

	int tryingX=tryX+1;
	int tryingY=tryY+1;

	// Wait until the second border
	while(time(NULL)==timeOut)
	{
	}

//	timeOut=time(NULL);
//	while(time(NULL)==timeOut)
//	{
//		XEvent ev;
//		XCheckTypedWindowEvent(dsp,wnd,ConfigureNotify,&ev);
//	}

	timeOut=time(NULL)+1;
	while(time(NULL)<timeOut) // Until the window is really located at lupX, lupY
	{
		if(tryingX!=tryX || tryingY!=tryY)
		{
			XMoveWindow(dsp,wnd,tryX,tryY);
			tryingX=tryX;
			tryingY=tryY;
		}

		XEvent ev;
		if(XCheckTypedWindowEvent(dsp,wnd,ConfigureNotify,&ev)==True)
		{
			const int actualX=ev.xconfigure.x;
			const int actualY=ev.xconfigure.y;

			printf("%d %d %d %d\n",actualX,actualY,goalX,goalY);

			const int dx=goalX-actualX;
			const int dy=goalY-actualY;

			if(-1<=dx && dx<=1 && -1<=dy && dy<=1)
			{
				break;
			}

			printf("dx %d dy %d\n",dx,dy);
			tryX+=dx;
			tryY+=dy;

			if(0<dx)
			{
				++tryX;
			}
			else if(0>dx)
			{
				--tryX;
			}

			if(0<dy)
			{
				++tryY;
			}
			else if(0>dy)
			{
				--tryY;
			}
		}
	}
}

void FsGetWindowSize(int &wid,int &hei)
{
	//wid=ysXWid;
	//hei=ysXHei;
	Window root_return;
	int x_return,y_return;
	unsigned int width_return,height_return;
	unsigned int border_width_return,depth_return;
	XGetGeometry(ysXDsp,ysXWnd,&root_return,&x_return,&y_return,&width_return,&height_return,&border_width_return,&depth_return);
	wid=width_return;
	hei=height_return;
}

void FsGetWindowPosition(int &x0,int &y0)
{
	x0=ysXlupX;
	y0=ysXlupY;
}

void FsSetWindowTitle(const char windowTitle[])
{
	printf("Sorry. %s not supported on this platform yet\n",__FUNCTION__);
}

void FsPollDevice(void)
{
	if(NULL==ysXWnd)
	{
		return;
	}

	int i,fsKey,fsKey2;
	char chr;
	KeySym ks;
	XEvent ev;

	fsKey=FSKEY_NULL;

	while(XCheckWindowEvent(ysXDsp,ysXWnd,KeyPressMask|KeyReleaseMask,&ev)==True)
	{
		int i;
		KeySym *keySymMap;
		int keysyms_per_keycode_return;
		keySymMap=XGetKeyboardMapping(ysXDsp,ev.xkey.keycode,1,&keysyms_per_keycode_return);
		//	 	printf("NumKeySym=%d\n",keysyms_per_keycode_return);
		//	printf("%s %s\n",XKeysymToString(keySymMap[0]),XKeysymToString(keySymMap[1]));
		//	printf("%d %d %d %d %d %d %d %d\n",
		//	 (ev.xkey.state&LockMask)!=0,
		//	 (ev.xkey.state&ShiftMask)!=0,
		//	 (ev.xkey.state&ControlMask)!=0,
		//	 (ev.xkey.state&Mod1Mask)!=0,  // Alt
		//	 (ev.xkey.state&Mod2Mask)!=0,  // Num Lock
		//	 (ev.xkey.state&Mod3Mask)!=0,
		//	 (ev.xkey.state&Mod4Mask)!=0,  // Windows key
		//	 (ev.xkey.state&Mod5Mask)!=0);

		if(0!=(ev.xkey.state&ControlMask) || 0!=(ev.xkey.state&Mod1Mask))
		  {
		    chr=0;
		  }
		  else if((ev.xkey.state&LockMask)==0 && (ev.xkey.state&ShiftMask)==0)
		{
		    chr=FsXKeySymToChar(keySymMap[0]); // mapXKtoChar[keySymMap[0]];
		}
		else if((ev.xkey.state&LockMask)==0 && (ev.xkey.state&ShiftMask)!=0)
		{
		    chr=FsXKeySymToChar(keySymMap[1]); // mapXKtoChar[keySymMap[1]];
		}
		else if((ev.xkey.state&ShiftMask)==0 && (ev.xkey.state&LockMask)!=0)
		{
			chr=FsXKeySymToChar(keySymMap[0]); // mapXKtoChar[keySymMap[0]];
			if('a'<=chr && chr<='z')
			{
				chr=chr+('A'-'a');
			}
		}
		else if((ev.xkey.state&ShiftMask)!=0 && (ev.xkey.state&LockMask)!=0)
		{
			chr=FsXKeySymToChar(keySymMap[1]); // mapXKtoChar[keySymMap[1]];
			if('a'<=chr && chr<='z')
			{
				chr=chr+('A'-'a');
			}
		}

		// Memo:
		// XK code is so badly designed.  XK_KP_Divide, XK_KP_Multiply,
		// XK_KP_Subtract, XK_KP_Add, should not be altered to
		// XK_XF86_Next_VMode or like that.  Other XK_KP_ code
		// can be altered by Mod2Mask.


		// following keys should be flipped based on Num Lock mask.  Apparently mod2mask is num lock by standard.
		// XK_KP_Space
		// XK_KP_Tab
		// XK_KP_Enter
		// XK_KP_F1
		// XK_KP_F2
		// XK_KP_F3
		// XK_KP_F4
		// XK_KP_Home
		// XK_KP_Left
		// XK_KP_Up
		// XK_KP_Right
		// XK_KP_Down
		// XK_KP_Prior
		// XK_KP_Page_Up
		// XK_KP_Next
		// XK_KP_Page_Down
		// XK_KP_End
		// XK_KP_Begin
		// XK_KP_Insert
		// XK_KP_Delete
		// XK_KP_Equal
		// XK_KP_Multiply
		// XK_KP_Add
		// XK_KP_Separator
		// XK_KP_Subtract
		// XK_KP_Decimal
		// XK_KP_Divide

		// XK_KP_0
		// XK_KP_1
		// XK_KP_2
		// XK_KP_3
		// XK_KP_4
		// XK_KP_5
		// XK_KP_6
		// XK_KP_7
		// XK_KP_8
		// XK_KP_9

		ks=XKeycodeToKeysym(ysXDsp,ev.xkey.keycode,0);
		if(XK_a<=ks && ks<=XK_z)
		{
			ks=ks+XK_A-XK_a;
		}

		if(0<=ks && ks<FS_NUM_XK)
		{
			fsKey=FsXKeySymToFsInkey(ks); // mapXKtoFSKEY[ks];
			fsKey2=FsXKeySymToFsGetKeyState(ks);

			// 2005/03/29 >>
			if(fsKey==0)
			{
				KeyCode kcode;
				kcode=XKeysymToKeycode(ysXDsp,ks);
				if(kcode!=0)
				{
					ks=XKeycodeToKeysym(ysXDsp,kcode,0);
					if(XK_a<=ks && ks<=XK_z)
					{
						ks=ks+XK_A-XK_a;
					}

					if(0<=ks && ks<FS_NUM_XK)
					{
						fsKey=FsXKeySymToFsInkey(ks); // mapXKtoFSKEY[ks];
						fsKey2=FsXKeySymToFsGetKeyState(ks);
					}
				}
			}
			// 2005/03/29 <<

			if(ev.type==KeyPress && fsKey!=0)
			{
				fsKeyPress[fsKey]=1;
				if(FSKEY_NULL!=fsKey2)
				{
					fsKeyPress[fsKey2]=1;
				}
				if(ev.xkey.window==ysXWnd) // 2005/04/08
				{
					if(nKeyBufUsed<NKEYBUF)
					{
						keyBuffer[nKeyBufUsed++]=fsKey;
					}
					if(chr!=0 && nCharBufUsed<NKEYBUF)
					{
						charBuffer[nCharBufUsed++]=chr;
					}
				}
			}
			else
			{
				fsKeyPress[fsKey]=0;
				if(FSKEY_NULL!=fsKey2)
				{
					fsKeyPress[fsKey2]=0;
				}
			}
		}
	}

	while(XCheckWindowEvent(ysXDsp,ysXWnd,ButtonPressMask|ButtonReleaseMask|PointerMotionMask,&ev)==True)
	{
		if(ButtonPress==ev.type || ButtonRelease==ev.type)
		{
			fsKey=FSKEY_NULL;
			if(ev.xbutton.button==Button4)
			{
				fsKey=FSKEY_WHEELUP;
			}
			else if(ev.xbutton.button==Button5)
			{
				fsKey=FSKEY_WHEELDOWN;
			}

			if(FSKEY_NULL!=fsKey)
			{
				if(ev.type==ButtonPress)
				{
					fsKeyPress[fsKey]=1;
					if(ev.xbutton.window==ysXWnd)
					{
						if(nKeyBufUsed<NKEYBUF)
						{
							keyBuffer[nKeyBufUsed++]=fsKey;
						}
					}

				}
				else if(ev.type==ButtonRelease)
				{
					fsKeyPress[fsKey]=0;
				}
			}
			else if(NKEYBUF>nMosBufUsed)
			{
				mosBuffer[nMosBufUsed].eventType=FSMOUSEEVENT_NONE;
				if(ev.type==ButtonPress)
				{
					switch(ev.xbutton.button)
					{
					case Button1:
						mosBuffer[nMosBufUsed].eventType=FSMOUSEEVENT_LBUTTONDOWN;
						lastKnownLb=1;
						break;
					case Button2:
						mosBuffer[nMosBufUsed].eventType=FSMOUSEEVENT_MBUTTONDOWN;
						lastKnownMb=1;
						break;
					case Button3:
						mosBuffer[nMosBufUsed].eventType=FSMOUSEEVENT_RBUTTONDOWN;
						lastKnownRb=1;
						break;
					}
				}
				else if(ev.type==ButtonRelease)
				{
					switch(ev.xbutton.button)
					{
					case Button1:
						mosBuffer[nMosBufUsed].eventType=FSMOUSEEVENT_LBUTTONUP;
						lastKnownLb=0;
						break;
					case Button2:
						mosBuffer[nMosBufUsed].eventType=FSMOUSEEVENT_MBUTTONUP;
						lastKnownMb=0;
						break;
					case Button3:
						mosBuffer[nMosBufUsed].eventType=FSMOUSEEVENT_RBUTTONUP;
						lastKnownRb=0;
						break;
					}
				}

				mosBuffer[nMosBufUsed].mx=ev.xbutton.x;
				mosBuffer[nMosBufUsed].my=ev.xbutton.y;
				// Turned out these button states are highly unreliable.
				// It may come with (state&Button1Mask)==0 on ButtonPress event of Button 1.
				// Confirmed this problem in VirtualBox.  This silly flaw may not occur in 
				// real environment.
				// mosBuffer[nMosBufUsed].lb=(0!=(ev.xbutton.state & Button1Mask));
				// mosBuffer[nMosBufUsed].mb=(0!=(ev.xbutton.state & Button2Mask));
				// mosBuffer[nMosBufUsed].rb=(0!=(ev.xbutton.state & Button3Mask));
				mosBuffer[nMosBufUsed].lb=lastKnownLb;
				mosBuffer[nMosBufUsed].mb=lastKnownMb;
				mosBuffer[nMosBufUsed].rb=lastKnownRb;
				mosBuffer[nMosBufUsed].shift=(0!=(ev.xbutton.state & ShiftMask));
				mosBuffer[nMosBufUsed].ctrl=(0!=(ev.xbutton.state & ControlMask));

				nMosBufUsed++;
			}
		}
		else if(ev.type==MotionNotify)
		{
			int mx=ev.xbutton.x;
			int my=ev.xbutton.y;
			int lb=lastKnownLb; // XButtonEvent.state turns out to be highly unreliable  (0!=(ev.xbutton.state & Button1Mask));
			int mb=lastKnownMb; // (0!=(ev.xbutton.state & Button2Mask));
			int rb=lastKnownRb; // (0!=(ev.xbutton.state & Button3Mask));
			int shift=(0!=(ev.xbutton.state & ShiftMask));
			int ctrl=(0!=(ev.xbutton.state & ControlMask));

			if(0<nMosBufUsed &&
			   mosBuffer[nMosBufUsed-1].eventType==FSMOUSEEVENT_MOVE &&
			   mosBuffer[nMosBufUsed-1].lb==lb &&
			   mosBuffer[nMosBufUsed-1].mb==mb &&
			   mosBuffer[nMosBufUsed-1].rb==rb &&
			   mosBuffer[nMosBufUsed-1].shift==shift &&
			   mosBuffer[nMosBufUsed-1].ctrl==ctrl)
			{
				mosBuffer[nMosBufUsed-1].mx=mx;
				mosBuffer[nMosBufUsed-1].my=my;
			}

			if(NKEYBUF>nMosBufUsed)
			{
				mosBuffer[nMosBufUsed].mx=mx;
				mosBuffer[nMosBufUsed].my=my;
				mosBuffer[nMosBufUsed].lb=lb;
				mosBuffer[nMosBufUsed].mb=mb;
				mosBuffer[nMosBufUsed].rb=rb;
				mosBuffer[nMosBufUsed].shift=shift;
				mosBuffer[nMosBufUsed].ctrl=ctrl;

				mosBuffer[nMosBufUsed].eventType=FSMOUSEEVENT_MOVE;

				nMosBufUsed++;
			}
		}
	}

	if(XCheckTypedWindowEvent(ysXDsp,ysXWnd,ConfigureNotify,&ev)==True)
	{
		ysXWid=ev.xconfigure.width;
		ysXHei=ev.xconfigure.height;
		ysXlupX=ev.xconfigure.x;
		ysXlupY=ev.xconfigure.y;
	}

	if(XCheckWindowEvent(ysXDsp,ysXWnd,ExposureMask,&ev)==True)
	{
		exposure=1;
		if(NULL!=fsOnPaintCallback)
		{
			(*fsOnPaintCallback)(fsOnPaintCallbackParam);
		}
	}

	// Clipboard Tunnel for FsGuiLib >>
	if(True==XCheckTypedWindowEvent(ysXDsp,ysXWnd,SelectionRequest,&ev) ||
	   True==XCheckTypedWindowEvent(ysXDsp,ysXWnd,SelectionClear,&ev))
	{
		if(ev.type==SelectionRequest)
		{
			ProcessSelectionRequest(ev);
		}
		else if(ev.type==SelectionClear)
		{
			printf("Lost selection ownership.\n");
		}
	}
	// Clipboard Tunnel for FsGuiLib <<

	if(XCheckTypedWindowEvent(ysXDsp,ysXWnd,DestroyNotify,&ev)==True)
	{
		exit(1);
	}

	return;
}

void FsPushOnPaintEvent(void)
{
	XExposeEvent evt;
	evt.type=Expose;
	evt.serial=0;
	evt.send_event=true;
	evt.display=ysXDsp;
	evt.window=ysXWnd;
	evt.x=0;
	evt.y=0;
	evt.width=ysXWid;
	evt.height=ysXHei;
	evt.count=0;
	XSendEvent(ysXDsp,ysXWnd,true,ExposureMask,(XEvent *)&evt);
}

void FsCloseWindow(void)
{
	if(NULL!=clipBoardContent)
	{
		delete [] clipBoardContent;
	}
	if(NULL!=pastedContent)
	{
		delete [] pastedContent;
	}
	XCloseDisplay(ysXDsp);
}

static void FsChangeWindowState(int setOrRemove,const char propertyName[])
{
	Atom _NET_WM_STATE=XInternAtom(ysXDsp,"_NET_WM_STATE",False);
	Atom _NET_WM_STATE_ADD=XInternAtom(ysXDsp,"_NET_WM_STATE_ADD",False); // Somehow doesn't work
	Atom _NET_WM_STATE_REMOVE=XInternAtom(ysXDsp,"_NET_WM_STATE_REMOVE",False); // Somehow doesn't work
	Atom _NET_WM_STATE_Property=XInternAtom(ysXDsp,propertyName,False);

	// From https://pyra-handheld.com/boards/threads/x11-fullscreen-howto.70443/
	XEvent evt;
	evt.xclient.type=ClientMessage;
	evt.xclient.serial=0;
	evt.xclient.send_event=True;
	evt.xclient.window=ysXWnd;
	evt.xclient.message_type=_NET_WM_STATE;
	evt.xclient.format=32;
	evt.xclient.data.l[0]=setOrRemove;
	evt.xclient.data.l[1]=_NET_WM_STATE_Property;
	evt.xclient.data.l[2]=0;
	evt.xclient.data.l[3]=0;
	evt.xclient.data.l[4]=0;
	XSendEvent(ysXDsp,RootWindow(ysXDsp,ysXVis->screen),False,SubstructureRedirectMask|SubstructureNotifyMask,&evt);
}

void FsMaximizeWindow(void)
{
	FsChangeWindowState(0,"_NET_WM_STATE_FULLSCREEN");
	FsChangeWindowState(1,"_NET_WM_STATE_MAXIMIZED_HORZ");
	FsChangeWindowState(1,"_NET_WM_STATE_MAXIMIZED_VERT");
}

void FsUnmaximizeWindow(void)
{
	FsChangeWindowState(0,"_NET_WM_STATE_MAXIMIZED_HORZ");
	FsChangeWindowState(0,"_NET_WM_STATE_MAXIMIZED_VERT");
	FsChangeWindowState(0,"_NET_WM_STATE_FULLSCREEN");
}
void FsMakeFullScreen(void)
{
	FsChangeWindowState(0,"_NET_WM_STATE_MAXIMIZED_HORZ");
	FsChangeWindowState(0,"_NET_WM_STATE_MAXIMIZED_VERT");
	FsChangeWindowState(1,"_NET_WM_STATE_FULLSCREEN");
}

void FsSleep(int ms)
{
	if(ms>0)
	{
		fd_set set;
		struct timeval wait;
		wait.tv_sec=ms/1000;
		wait.tv_usec=(ms%1000)*1000;
		FD_ZERO(&set);
		select(0,&set,NULL,NULL,&wait);
	}
}

long long int FsPassedTime(void)
{
	static long long int lastTick;
	long long int tick;

	static int first=1;
	if(1==first)
	{
		lastTick=FsSubSecondTimer();
		first=0;
	}

	tick=FsSubSecondTimer();
	long long passed=tick-lastTick;
	lastTick=tick;

	return passed;
}

long long int FsSubSecondTimer(void)
{
	timeval tm;
	gettimeofday(&tm,NULL);
	long long int sec=tm.tv_sec;
	long long int usec=tm.tv_usec;

	long long int clk=sec*(long long int)1000+usec/(long long int)1000;


	static long long int lastValue=0;
	static long long int base=0;

	if(clk<lastValue) // Underflow.  It's tomorrow now.
	{
		base+=1000*3600*24;
	}
	lastValue=clk;

	static int first=1;
	static long long int t0=0;
	if(1==first)
	{
		t0=base+clk;
		first=0;
	}

	return base+clk-t0;
}

void FsGetMouseState(int &lb,int &mb,int &rb,int &mx,int &my)
{
	Window r,c;
	int xInRoot,yInRoot;
	unsigned int mask;

	XQueryPointer(ysXDsp,ysXWnd,&r,&c,&xInRoot,&yInRoot,&mx,&my,&mask);
	
	/* These masks are seriouly unreliable.  It could still report zero after ButtonPress event 
	   is issued.  Therefore, it causes inconsistency and unusable.  Flaw confirmed in VirtualBox.
	lb=((mask & Button1Mask) ? 1 : 0);
	mb=((mask & Button2Mask) ? 1 : 0);
	rb=((mask & Button3Mask) ? 1 : 0); */

	lb=lastKnownLb;
	mb=lastKnownMb;
	rb=lastKnownRb;
}

void FsSetMousePosition(int mx,int my)
{
	// This should move the mouse cursor to the given location.
	// However, it doesn't in VirtualBox.
	// This function may no longer implemented.
	XWarpPointer(ysXDsp,ysXWnd,ysXWnd,0,0,ysXWid,ysXHei,mx,my);
}

int FsGetMouseEvent(int &lb,int &mb,int &rb,int &mx,int &my)
{
	if(0<nMosBufUsed)
	{
		int eventType=mosBuffer[0].eventType;
		mx=mosBuffer[0].mx;
		my=mosBuffer[0].my;
		lb=mosBuffer[0].lb;
		mb=mosBuffer[0].mb;
		rb=mosBuffer[0].rb;

		int i;
		for(i=0; i<nMosBufUsed-1; i++)
		{
			mosBuffer[i]=mosBuffer[i+1];
		}
		nMosBufUsed--;

		return eventType;
	}
	else
	{
		FsGetMouseState(lb,mb,rb,mx,my);
		return FSMOUSEEVENT_NONE;
	}
}

void FsSwapBuffers(void)
{
	glFlush();
	glXSwapBuffers(ysXDsp,ysXWnd);
}

int FsInkey(void)
{
	if(nKeyBufUsed>0)
	{
		int i,keyCode;
		keyCode=keyBuffer[0];
		nKeyBufUsed--;
		for(i=0; i<nKeyBufUsed; i++)
		{
			keyBuffer[i]=keyBuffer[i+1];
		}
		return keyCode;
	}
	return 0;
}

int FsInkeyChar(void)
{
	if(nCharBufUsed>0)
	{
		int i,asciiCode;
		asciiCode=charBuffer[0];
		nCharBufUsed--;
		for(i=0; i<nCharBufUsed; i++)
		{
			charBuffer[i]=charBuffer[i+1];
		}
		return asciiCode;
	}
	return 0;
}

int FsGetKeyState(int fsKeyCode)
{
	if(0<fsKeyCode && fsKeyCode<FSKEY_NUM_KEYCODE)
	{
		return fsKeyPress[fsKeyCode];
	}
	return 0;
}

int FsCheckWindowExposure(void)
{
	int ret;
	ret=exposure;
	exposure=0;
	return ret;
}



////////////////////////////////////////////////////////////
// Clipboard support
static void ProcessSelectionRequest(XEvent &evt)
{
	XEvent reply;

	Atom targetsAtom=XInternAtom(ysXDsp,"TARGETS",0);

	reply.xselection.type=SelectionNotify;
	reply.xselection.serial=evt.xselectionrequest.serial;
	reply.xselection.send_event=True;
	reply.xselection.display=ysXDsp;
	reply.xselection.requestor=evt.xselectionrequest.requestor;
	reply.xselection.selection=evt.xselectionrequest.selection;
	reply.xselection.target=evt.xselectionrequest.target;
	reply.xselection.property=evt.xselectionrequest.property; // None for nothing.
	reply.xselection.time=evt.xselectionrequest.time; // None for nothing.

	if(evt.xselectionrequest.target==targetsAtom)
	{
		Atom dataType[]={XA_STRING,targetsAtom};
		XChangeProperty(ysXDsp,evt.xselectionrequest.requestor,evt.xselectionrequest.property,XA_ATOM,32,PropModeReplace,(unsigned char *)dataType,2);
		printf("Answered supported data type.\n");
	}
	else if(evt.xselectionrequest.target==XA_STRING) // Since it declares it only accepts XA_TARGETS and XA_STRING, target should be a XA_STRING
	{
		XChangeProperty(ysXDsp,evt.xselectionrequest.requestor,evt.xselectionrequest.property,evt.xselectionrequest.target,8,PropModeReplace,(unsigned char *)clipBoardContent,clipBoardContentLength);
	}
	else // Just in case.
	{
		// Apparently, in this case, the requestor is supposed to try a different target type.
		reply.xselection.property=None;
	}

	XSendEvent(ysXDsp,evt.xselectionrequest.requestor,True,NoEventMask,&reply);
}

void FsX11GetClipBoardString(long long int &returnLength,char *&returnStr)
{
	if(NULL!=pastedContent)
	{
		delete [] pastedContent;
	}
	pastedContent=NULL;
	pastedContentLength=0;

	returnLength=0;
	returnStr=NULL;

	Atom clipboardAtom=XInternAtom(ysXDsp,"CLIPBOARD",0);
	Atom dataReceiverAtom=XInternAtom(ysXDsp,"GET_DATA",0); // For receiving

	Atom type_return;
	int format_return;
	long unsigned int nitems_return;
	long unsigned int bytes_after_return;
	unsigned char *bufPtr=NULL;

	XConvertSelection(ysXDsp,clipboardAtom,XA_STRING,dataReceiverAtom,ysXWnd,CurrentTime);

	// See experiment/XWindow/cutbuf/selfcopypaste.cpp
	//   If this program owns a clipboard, and also inquire the clipboard contents,
	//   this program needs to manage both SelectionRequest and SelectionNotify to
	//   return the clipboard content.
	for(;;)
	{
		XEvent evt;
		if(True==XCheckTypedWindowEvent(ysXDsp,ysXWnd,SelectionRequest,&evt) ||
		   True==XCheckTypedWindowEvent(ysXDsp,ysXWnd,SelectionClear,&evt) ||
		   True==XCheckTypedWindowEvent(ysXDsp,ysXWnd,SelectionNotify,&evt))
		{
			if(evt.type==SelectionRequest)
			{
				ProcessSelectionRequest(evt);
			}
			else if(evt.type==SelectionClear)
			{
				printf("Lost selection ownership.\n");
			}
			else if(evt.type==SelectionNotify)
			{
				if(None!=evt.xselection.property)
				{
					Atom type_return;
					int format_return;
					long unsigned int nitems_return;
					long unsigned int bytes_after_return;
					unsigned char *bufPtr=NULL;

					XGetWindowProperty(ysXDsp,ysXWnd,evt.xselection.property,0,256,False,XA_STRING,&type_return,&format_return,&nitems_return,&bytes_after_return,&bufPtr);

					printf("Type %d\n",(int)type_return);
					printf("%d bytes\n",(int)nitems_return);
					printf("%d remain\n",(int)bytes_after_return);
					printf("%s\n",bufPtr);

					pastedContentLength=nitems_return;
					pastedContent=new char [nitems_return];
					for(int idx=0; idx<nitems_return; ++idx)
					{
						pastedContent[idx]=(char)bufPtr[idx];
					}

					XFree(bufPtr);
				}
				else
				{
					printf("No selection.\n");
				}
				break;
			}
			else
			{
				printf("Other event.\n");
			}
		}
	}

	returnLength=pastedContentLength;
	returnStr=pastedContent;
}

void FsX11SetClipBoardString(long long int length,const char str[])
{
	if(NULL!=clipBoardContent)
	{
		delete [] clipBoardContent;
		clipBoardContent=NULL;
		clipBoardContentLength=0;
	}

	clipBoardContentLength=length;
	clipBoardContent=new char [length];
	for(int i=0; i<length; ++i)
	{
		clipBoardContent[i]=str[i];
	}

	Atom clipboardAtom=XInternAtom(ysXDsp,"CLIPBOARD",0);
	XSetSelectionOwner(ysXDsp,clipboardAtom,ysXWnd,CurrentTime);
	XSetSelectionOwner(ysXDsp,XA_PRIMARY,ysXWnd,CurrentTime);
}

void FsChangeToProgramDir(void)
{
	char buf[4096];
	auto len=readlink("/proc/self/exe",buf,4095);
	if(len<4095)
	{
		buf[len]=0;
		for(len=strlen(buf); 0<len; --len) // Probably it is safe to say for(len=len; 0<len; --len)
		{
			if(buf[len]=='/')
			{
				buf[len]=0;
				break;
			}
		}

		if(0!=chdir(buf))
		{
			switch(errno)
			{
			case EACCES:
				printf("EACCES\n");
				break;
			case EFAULT:
				printf("EFAULT\n");
				break;
			case EIO:
				printf("EIO\n");
				break;
			case ELOOP:
				printf("ELOOP\n");
				break;
			case ENAMETOOLONG:
				printf("ENAMETOOLONG\n");
				break;
			case ENOENT:
				printf("ENOENT\n");
				break;
			case ENOMEM:
				printf("ENOMEM\n");
				break;
			case ENOTDIR:
				printf("ENOTDIR\n");
				break;
			case EBADF:
				printf("EBADF\n");
				break;
			}
		}

		// 2016/1/1 chdir ignored?
		getcwd(buf,4095);
		printf("Changed to %s\n",buf);
	}
	else
	{
		printf("Current process file name too long.\n");
	}
}

void FsPushKey(int fskey)
{
	if(nKeyBufUsed<NKEYBUF)
	{
		keyBuffer[nKeyBufUsed++]=fskey;
	}
}

void FsPushChar(int c)
{
	if(nCharBufUsed<NKEYBUF)
	{
		charBuffer[nCharBufUsed++]=c;
	}
}

int FsGetNumCurrentTouch(void)
{
	return 0;
}

const FsVec2i *FsGetCurrentTouch(void)
{
	return nullptr;
}

////////////////////////////////////////////////////////////

// How can I enable IME in Linux?

int FsEnableIME(void)
{
	return 0;
}

void FsDisableIME(void)
{
}

////////////////////////////////////////////////////////////

int FsIsNativeTextInputAvailable(void)
{
	return 0;
}

int FsOpenNativeTextInput(int x1,int y1,int wid,int hei)
{
	return 0;
}

void FsCloseNativeTextInput(void)
{
}

void FsSetNativeTextInputText(const wchar_t [])
{
}

int FsGetNativeTextInputTextLength(void)
{
	return 0;
}

void FsGetNativeTextInputText(wchar_t str[],int bufLen)
{
	if(0<bufLen)
	{
		str[0]=0;
	}
}

int FsGetNativeTextInputEvent(void)
{
	return FSNATIVETEXTEVENT_NONE;
}

void FsShowMouseCursor(int showFlag)
{
}
int FsIsMouseCursorVisible(void)
{
	return 1;
}
