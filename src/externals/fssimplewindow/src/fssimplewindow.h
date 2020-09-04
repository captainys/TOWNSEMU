/* ////////////////////////////////////////////////////////////

File Name: fssimplewindow.h
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

#ifndef FSSIMPLEWINDOW_IS_INCLUDED
#define FSSIMPLEWINDOW_IS_INCLUDED
/* { */

/*! \file
*/

#ifndef FS_MOUSEEVENT_TYPE_IS_DEFINED
#define FS_MOUSEEVENT_TYPE_IS_DEFINED
/*! Enumeration for mouse events.
 */
enum
{
	FSMOUSEEVENT_NONE,
	FSMOUSEEVENT_LBUTTONDOWN,
	FSMOUSEEVENT_LBUTTONUP,
	FSMOUSEEVENT_MBUTTONDOWN,
	FSMOUSEEVENT_MBUTTONUP,
	FSMOUSEEVENT_RBUTTONDOWN,
	FSMOUSEEVENT_RBUTTONUP,
	FSMOUSEEVENT_MOVE
};
#endif


#ifndef FS_NATIVETEXTEVENT_TYPE_IS_DEFINED
#define FS_NATIVETEXTEVENT_TYPE_IS_DEFINED
enum
{
	FSNATIVETEXTEVENT_NONE,
	FSNATIVETEXTEVENT_CLOSED,
	FSNATIVETEXTEVENT_CHANGED
};
#endif


#ifndef FS_KEYCODE_TYPE_IS_DEFINED
#define FS_KEYCODE_TYPE_IS_DEFINED
/*! Enumeration for key codes.
 */
enum
{
	FSKEY_NULL,
	FSKEY_SPACE,
	FSKEY_0,
	FSKEY_1,
	FSKEY_2,
	FSKEY_3,
	FSKEY_4,
	FSKEY_5,
	FSKEY_6,
	FSKEY_7,
	FSKEY_8,
	FSKEY_9,
	FSKEY_A,
	FSKEY_B,
	FSKEY_C,
	FSKEY_D,
	FSKEY_E,
	FSKEY_F,
	FSKEY_G,
	FSKEY_H,
	FSKEY_I,
	FSKEY_J,
	FSKEY_K,
	FSKEY_L,
	FSKEY_M,
	FSKEY_N,
	FSKEY_O,
	FSKEY_P,
	FSKEY_Q,
	FSKEY_R,
	FSKEY_S,
	FSKEY_T,
	FSKEY_U,
	FSKEY_V,
	FSKEY_W,
	FSKEY_X,
	FSKEY_Y,
	FSKEY_Z,
	FSKEY_ESC,
	FSKEY_F1,
	FSKEY_F2,
	FSKEY_F3,
	FSKEY_F4,
	FSKEY_F5,
	FSKEY_F6,
	FSKEY_F7,
	FSKEY_F8,
	FSKEY_F9,
	FSKEY_F10,
	FSKEY_F11,
	FSKEY_F12,
	FSKEY_PRINTSCRN,
	FSKEY_CAPSLOCK,
	FSKEY_SCROLLLOCK,
	FSKEY_PAUSEBREAK,
	FSKEY_BS,
	FSKEY_TAB,
	FSKEY_ENTER,
	FSKEY_SHIFT,
	FSKEY_CTRL,
	FSKEY_ALT,
	FSKEY_INS,
	FSKEY_DEL,
	FSKEY_HOME,
	FSKEY_END,
	FSKEY_PAGEUP,
	FSKEY_PAGEDOWN,
	FSKEY_UP,
	FSKEY_DOWN,
	FSKEY_LEFT,
	FSKEY_RIGHT,
	FSKEY_NUMLOCK,
	FSKEY_TILDA,
	FSKEY_MINUS,
	FSKEY_PLUS,
	FSKEY_LBRACKET,
	FSKEY_RBRACKET,
	FSKEY_BACKSLASH,
	FSKEY_SEMICOLON,
	FSKEY_SINGLEQUOTE,
	FSKEY_COMMA,
	FSKEY_DOT,
	FSKEY_SLASH,
	FSKEY_TEN0,
	FSKEY_TEN1,
	FSKEY_TEN2,
	FSKEY_TEN3,
	FSKEY_TEN4,
	FSKEY_TEN5,
	FSKEY_TEN6,
	FSKEY_TEN7,
	FSKEY_TEN8,
	FSKEY_TEN9,
	FSKEY_TENDOT,
	FSKEY_TENSLASH,
	FSKEY_TENSTAR,
	FSKEY_TENMINUS,
	FSKEY_TENPLUS,
	FSKEY_TENENTER,
	FSKEY_WHEELUP,
	FSKEY_WHEELDOWN,
	FSKEY_CONTEXT,

	// Japanese keyboard
	FSKEY_CONVERT,
	FSKEY_NONCONVERT,
	FSKEY_KANA,       // Japanese JIS Keyboard Only => Win32 VK_KANA
	FSKEY_RO,         // Japanese JIS Keyboard Only => Win32 VK_OEM_102
	FSKEY_ZENKAKU,    // Japanese JIS Keyboard Only => Full Pitch/Half Pitch

	// The following key codes won't be returned by FsInkey()
	// These may return non zero for FsGetKeyState
	FSKEY_LEFT_CTRL,
	FSKEY_RIGHT_CTRL,
	FSKEY_LEFT_SHIFT,
	FSKEY_RIGHT_SHIFT,
	FSKEY_LEFT_ALT,
	FSKEY_RIGHT_ALT,

FSKEY_NUM_KEYCODE
};

// Japanese JIS Keyboard @ is mapped to Tilda of US keyboard.
#define FSKEY_AT FSKEY_TILDA

// Japanese JIS Keyboard : is mapped to ' of US keyboard.
#define FSKEY_COLON FSKEY_SINGLEQUOTE

#endif



#ifndef FSSIMPLEWINDOW_MACRO_ONLY
	#if !defined(_WIN32) && !defined(GL_GLEXT_PROTOTYPES)
		#define GL_GLEXT_PROTOTYPES
	#endif

	#ifndef FSSIMPLEWINDOW_DONT_INCLUDE_OPENGL_HEADERS
		#ifdef _WIN32
			#ifndef WIN32_LEAN_AND_MEAN
				// Prevent inclusion of winsock.h
				#define WIN32_LEAN_AND_MEAN
				#include <windows.h>
				#undef WIN32_LEAN_AND_MEAN
			#else
				#include <windows.h>
			#endif
			#include <GL/gl.h>
			#include <GL/glu.h>
		#elif ((defined(WINAPI_FAMILY) && WINAPI_FAMILY==WINAPI_FAMILY_APP)) || defined(__ANDROID__)
			#include <GLES2/gl2.h>
			#include <GLES2/gl2ext.h>
			#define YS_GL_ES2
			typedef double GLdouble;
		#elif !defined(__APPLE__)
			#include <GL/gl.h>
			#include <GL/glu.h>
			#include <GL/glext.h>
		#else
		    #include <TargetConditionals.h>
		    #if TARGET_OS_EMBEDDED!=0 || TARGET_OS_IPHONE!=0 || TARGET_OS_IPHONE_SIMULATOR!=0
				#include <OpenGLES/ES2/gl.h>
				#include <OpenGLES/ES2/glext.h>
				typedef double GLdouble;
			#else
				#include <OpenGL/gl.h>
				#include <OpenGL/glu.h>
				#include <OpenGL/glext.h>
			#endif
		#endif
	#endif // << #ifndef FSSIMPLEWINDOW_DONT_INCLUDE_OPENGL_HEADERS



#ifdef __cplusplus
// This needs to be included from Objective-C code for mouse-event enums.
// C++ specific declaration must be enclosed by #ifdef __cplucplus and #endif

class FsSimpleWindowInternal;
const FsSimpleWindowInternal *FsGetSimpleWindowInternal(void);

extern void *fsCloseWindowCallBackParam;
extern bool (*fsCloseWindowCallBack)(void *param);

extern void *fsOpenGLContextCreationCallBackParam;
extern bool (*fsOpenGLContextCreationCallBack)(void *param);

extern void *fsAfterWindowCreationCallBackParam;
extern void (*fsAfterWindowCreationCallBack)(void *param);

extern void *fsOpenGLInitializationCallBackParam;
extern bool (*fsOpenGLInitializationCallBack)(void *param);

extern void *fsSwapBuffersHookParam;
extern bool (*fsSwapBuffersHook)(void *param);

extern void *fsOnPaintCallbackParam;
extern void (*fsOnPaintCallback)(void *param);

extern void *fsOnResizeCallBackParam;
extern void (*fsOnResizeCallBack)(void *param,int wid,int hei);

extern void (*fsIntervalCallBack)(void *);
extern void *fsIntervalCallBackParameter;

extern void (*fsPollDeviceHook)(void *);
extern void *fsPollDeviceHookParam;



// Memo: 2013/11/09 Clang is not capable of understanding default function parameter.
//                  Why the hell Apple switched to such an incomplete compiler?

class FsOpenWindowOption
{
public:
	enum SIZE_OPTION
	{
		NORMAL_WINDOW,
		MAXIMIZE_WINDOW,
		FULLSCREEN
	};

	int x0,y0,wid,hei;
	const char *windowTitle;

	bool useDoubleBuffer;
	bool useMultiSampleBuffer;
	SIZE_OPTION sizeOpt;

	FsOpenWindowOption();
};

class FsVec2i
{
public:
	int v[2];
	inline int x(void) const
	{
		return v[0];
	}
	inline int y(void) const
	{
		return v[1];
	}
};



void FsOpenWindow(int x0,int y0,int wid,int hei,int useDoubleBuffer);
void FsOpenWindow(int x0,int y0,int wid,int hei,int useDoubleBuffer,const char windowTitle[]);

/*! Opes the main window according to the given options. 
    Currently, useMultiSampleBuffer is respected only in MacOSX. */
void FsOpenWindow(const FsOpenWindowOption &openWindowOption);

/*! This function returns non zero if the window is currently open. */
int FsCheckWindowOpen(void);


/*!
*/
void FsResizeWindow(int newWid,int newHei);


void FsCloseWindow(void);
void FsMaximizeWindow(void);
void FsUnmaximizeWindow(void);
void FsMakeFullScreen(void);

/*!  FsRegisterIntervalCallBack has little meaning unless the program uses system-modal dialog boxes such as file dialog.
     Since the system takes over application main loop when such a system-modal dialog is visible, the main window will appear to be blank.
     If an interval function is registered (and the program is written in such a way that a single interval function drives the application), the program can draw main window image from inside the interval function. */
void FsRegisterIntervalCallBack(void (*callback)(void *),void *param);

/*! This function registers a function that is called just before OpenGL context is created and initialized.
    If the OpenGL context (or can be any other 3D graphics engine) is created and initialized in the 
    call-back function, the function must return true.  Then, the OpenGL context initialization in the
    FsSimpleWindow framework will be skipped. */
void FsRegisterBeforeOpenGLContextCreationCallBack(bool (*callback)(void *),void *param);

/*! This function registers a call-back function for initialization of OpenGL.
    It will be called after OpenGL context is created and ready to use.

    After-Window creation call back may be called before this function, or can be the other order.

    The program must not assume which function is called first.
*/
void FsRegisterOpenGLInitializationCallBack(bool (*callback)(void *),void *param);

/*! This function registers a function that is called after the window is created, and the graphics
    context is ready to go. */
void FsRegisterAfterWindowCreationCallBack(void (*callback)(void *),void *param);

/*! This function registers a function that is called when the user tries to close the window from the
    close button.

    The call-back function must return false if the window must not be closed.  This function will not be called
    if the window is closed from FsCloseWindow, or by just lapsing to the end of main function. */
void FsRegisterCloseWindowCallBack(bool (*callback)(void *),void *param);

/*! This function registers a function that is called just before FsSwapBuffers exposes the back buffer to the user.
    If the back buffer is exposed inside the call-back, this function must return true. */
void FsRegisterSwapBuffersCallBack(bool (*callback)(void *),void *param);

/*! This function registers a OnPaint call back function.  This function will be called when 
    paint event (such as WM_PAINT) is sent to the window outside of the main loop. */
void FsRegisterOnPaintCallBack(void (*callback)(void *),void *param);

/*! This function pushes a OnPaint (for example WM_PAINT in Win32) event in the system's event queue so that OnPaint call back is called next time the event is processed. 
Not necessary for purely polling-based program, however, for the systems that does not allow polling-based programming, such as iOS, may require to use this function. */
void FsPushOnPaintEvent(void);

/*! This function registers a function that is called when window is resized. */
void FsRegisterWindowResizeCallBack(void (*callback)(void *,int wid,int hei),void *param);

/*! This function registers a function that is called when FsPollDevice is called. */
void FsRegisterPollDeviceCallBack(void (*callback)(void *),void *param);



void FsGetWindowSize(int &wid,int &hei);
void FsGetWindowPosition(int &x0,int &y0);
void FsSetWindowTitle(const char windowTitle[]);
void FsPollDevice(void);
void FsSleep(int ms);
long long int FsPassedTime(void);
long long int FsSubSecondTimer(void);
void FsGetMouseState(int &lb,int &mb,int &rb,int &mx,int &my);
int FsGetMouseEvent(int &lb,int &mb,int &rb,int &mx,int &my);

/*! Move mouse cursor relative to the window.
    May not work in Linux.
*/
void FsSetMousePosition(int mx,int my);
void FsSwapBuffers(void);
int FsInkey(void);
int FsInkeyChar(void);

/*! This function pushes the key code to the key-stroke buffer and makes a virtual key-press event. */
void FsPushKey(int fskey);

/*! This function pushes the character code to the keyboard-generated character buffer and makes a virtual event. */
void FsPushChar(int c);


int FsGetKeyState(int fsKeyCode);
int FsCheckKeyHeldDown(void);
/*! This function waits (blocks) until all the event queues are flushed and keys and mouse buttons are released. 
    In the past, this function was blocking forever when a key or a button is stuck.
    That was obvious hardware problem, but to the user it appears that the program is in trouble.  */
int FsCheckWindowExposure(void);

void FsClearEventQueue(void);

void FsChangeToProgramDir(void); // Mainly for Mac OS X

const char *FsKeyCodeToString(int keyCode);

int FsStringToKeyCode(const char str[]);

/*! Enables IME for non-English language input.
    Returns non-zero if successful.  
    Returns zero if it cannot enable IME.
*/
int FsEnableIME(void);

/*! Disable IME enabled by FsEnableIME function. */
void FsDisableIME(void);


// These functions are experimental. >>
int FsGetNumCurrentTouch(void);
const FsVec2i *FsGetCurrentTouch(void);
// These functions are experimental. <<


// These functions are even more experimental. >>
int FsIsNativeTextInputAvailable(void);
int FsOpenNativeTextInput(int x1,int y1,int wid,int hei);
void FsCloseNativeTextInput(void);
void FsSetNativeTextInputText(const wchar_t str[]);
int FsGetNativeTextInputTextLength(void);
void FsGetNativeTextInputText(wchar_t str[],int bufLen);
int FsGetNativeTextInputEvent(void);
// These functions are even more experimental. <<


#endif // << #ifdef __cplusplus

#endif // << #ifndef FSSIMPLEWINDOW_MACRO_ONLY



// 2018/10/18
// I utterly was shocked min and max were defined somewhere.  Presumably windows.h
#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif



// 2018/10/18  abs and _abs have always been a problem.
#ifdef abs
#undef abs
#endif

#ifdef _abs
#undef _abs
#endif



/* } */
#endif
