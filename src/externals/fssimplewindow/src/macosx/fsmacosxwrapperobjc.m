/* ////////////////////////////////////////////////////////////

File Name: fsmacosxwrapperobjc.m
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

#import <Cocoa/Cocoa.h>
#include <OpenGL/OpenGL.h>

#include "fssimplewindow.h"


// About Multi-Language input.
//  First attempted to use NSTextInputClient protocol.  It doesn't work like Windows IME.
//  It needs to closely work together with a text-input widget.  That means, I need to do
//  something inside FsGuiLib.  Cannot keep it low-level and transparent.
//
//  Second attempt is to make a sub-class of NSTextView as a sub-view of my OpenGL view.
//  Let the user type the letters, and when the user types Enter, send them to my character buffer,
//  ESC just cancel, BackSpace beyond what's in the text box to delete the window.
//
//  I can create a NSTextView and addSubview to my OpenGL view, but the sub-view is invisible.
//  The problem NSOpenGLView does not draw sub-views correctly.  Good job Apple!
//  
//  The sub-views of NSOpenGLView can be made visible by calling setWantsLayer:YES to the OpenGL view.
//  However, then all drawings must be done inside drawRect function.
//
//  Next attempt can be to make a NSWindow dedicated to the text input when necessary.



// NSAutoReleasePool doesn't seem to exist if Auto Reference Count is on.
// It needs to be checked by __has_feature(objc_arc)
// Can't Apple just make NSAutoReleasePool a dummy class in that case?  
#ifndef __has_feature
#define __has_feature(x) 0
#endif


void FsPollDeviceC(void);
void FsMakeCurrentC(void);
extern void FsOnPaintCallBackCpp(void);
extern void FsOnInitializeOpenGLC(void);


static bool openGLprepared=false;
static int mouseLb=0,mouseMb=0,mouseRb=0;


static int FsNormalKeyCode[256]=
{
	0,                 // 0
	0,                 // 1
	0,                 // 2
	0,                 // 3
	0,                 // 4
	0,                 // 5
	0,                 // 6
	0,                 // 7
	0,                 // 8
	FSKEY_TAB,         // 9
	0,                 // 10
	0,                 // 11
	0,                 // 12
	FSKEY_ENTER,       // 13
	0,                 // 14
	0,                 // 15
	0,                 // 16
	0,                 // 17
	0,                 // 18
	0,                 // 19
	0,                 // 20
	0,                 // 21
	0,                 // 22
	0,                 // 23
	0,                 // 24
	FSKEY_TAB,         // 25
	0,                 // 26
	FSKEY_ESC,         // 27
	0,                 // 28
	0,                 // 29
	0,                 // 30
	0,                 // 31
	FSKEY_SPACE,       // 32
	0,                 // 33
	0,                 // 34
	0,                 // 35
	0,                 // 36
	0,                 // 37
	0,                 // 38
	FSKEY_SINGLEQUOTE, // 39
	0,                 // 40
	0,                 // 41
	0,                 // 42
	FSKEY_PLUS,        // 43
	FSKEY_COMMA,       // 44
	FSKEY_MINUS,       // 45
	FSKEY_DOT,         // 46
	FSKEY_SLASH,       // 47
	FSKEY_0,           // 48
	FSKEY_1,           // 49
	FSKEY_2,           // 50
	FSKEY_3,           // 51
	FSKEY_4,           // 52
	FSKEY_5,           // 53
	FSKEY_6,           // 54
	FSKEY_7,           // 55
	FSKEY_8,           // 56
	FSKEY_9,           // 57
	FSKEY_COLON,       // 58
	FSKEY_SEMICOLON,   // 59
	0,                 // 60
	0,                 // 61
	0,                 // 62
	0,                 // 63
	FSKEY_AT,          // 64
	FSKEY_A,           // 65
	FSKEY_B,           // 66
	FSKEY_C,           // 67
	FSKEY_D,           // 68
	FSKEY_E,           // 69
	FSKEY_F,           // 70
	FSKEY_G,           // 71
	FSKEY_H,           // 72
	FSKEY_I,           // 73
	FSKEY_J,           // 74
	FSKEY_K,           // 75
	FSKEY_L,           // 76
	FSKEY_M,           // 77
	FSKEY_N,           // 78
	FSKEY_O,           // 79
	FSKEY_P,           // 80
	FSKEY_Q,           // 81
	FSKEY_R,           // 82
	FSKEY_S,           // 83
	FSKEY_T,           // 84
	FSKEY_U,           // 85
	FSKEY_V,           // 86
	FSKEY_W,           // 87
	FSKEY_X,           // 88
	FSKEY_Y,           // 89
	FSKEY_Z,           // 90
	FSKEY_LBRACKET,    // 91
	FSKEY_BACKSLASH,   // 92
	FSKEY_RBRACKET,    // 93
	0,                 // 94
	0,                 // 95
	0,                 // 96
	FSKEY_A,           // 97
	FSKEY_B,           // 98
	FSKEY_C,           // 99
	FSKEY_D,           // 100
	FSKEY_E,           // 101
	FSKEY_F,           // 102
	FSKEY_G,           // 103
	FSKEY_H,           // 104
	FSKEY_I,           // 105
	FSKEY_J,           // 106
	FSKEY_K,           // 107
	FSKEY_L,           // 108
	FSKEY_M,           // 109
	FSKEY_N,           // 110
	FSKEY_O,           // 111
	FSKEY_P,           // 112
	FSKEY_Q,           // 113
	FSKEY_R,           // 114
	FSKEY_S,           // 115
	FSKEY_T,           // 116
	FSKEY_U,           // 117
	FSKEY_V,           // 118
	FSKEY_W,           // 119
	FSKEY_X,           // 120
	FSKEY_Y,           // 121
	FSKEY_Z,           // 122
	FSKEY_LBRACKET,    // 123
	0,                 // 124
	FSKEY_RBRACKET,    // 125
	FSKEY_TILDA,       // 126
	FSKEY_BS,          // 127
	0,                 // 128
	0,                 // 129
	0,                 // 130
	0,                 // 131
	0,                 // 132
	0,                 // 133
	0,                 // 134
	0,                 // 135
	0,                 // 136
	0,                 // 137
	0,                 // 138
	0,                 // 139
	0,                 // 140
	0,                 // 141
	0,                 // 142
	0,                 // 143
	0,                 // 144
	0,                 // 145
	0,                 // 146
	0,                 // 147
	0,                 // 148
	0,                 // 149
	0,                 // 150
	0,                 // 151
	0,                 // 152
	0,                 // 153
	0,                 // 154
	0,                 // 155
	0,                 // 156
	0,                 // 157
	0,                 // 158
	0,                 // 159
	0,                 // 160
	0,                 // 161
	0,                 // 162
	0,                 // 163
	0,                 // 164
	0,                 // 165
	0,                 // 166
	0,                 // 167
	0,                 // 168
	0,                 // 169
	0,                 // 170
	0,                 // 171
	0,                 // 172
	0,                 // 173
	0,                 // 174
	0,                 // 175
	0,                 // 176
	0,                 // 177
	0,                 // 178
	0,                 // 179
	0,                 // 180
	0,                 // 181
	0,                 // 182
	0,                 // 183
	0,                 // 184
	0,                 // 185
	0,                 // 186
	0,                 // 187
	0,                 // 188
	0,                 // 189
	0,                 // 190
	0,                 // 191
	0,                 // 192
	0,                 // 193
	0,                 // 194
	0,                 // 195
	0,                 // 196
	0,                 // 197
	0,                 // 198
	0,                 // 199
	0,                 // 200
	0,                 // 201
	0,                 // 202
	0,                 // 203
	0,                 // 204
	0,                 // 205
	0,                 // 206
	0,                 // 207
	0,                 // 208
	0,                 // 209
	0,                 // 210
	0,                 // 211
	0,                 // 212
	0,                 // 213
	0,                 // 214
	0,                 // 215
	0,                 // 216
	0,                 // 217
	0,                 // 218
	0,                 // 219
	0,                 // 220
	0,                 // 221
	0,                 // 222
	0,                 // 223
	0,                 // 224
	0,                 // 225
	0,                 // 226
	0,                 // 227
	0,                 // 228
	0,                 // 229
	0,                 // 230
	0,                 // 231
	0,                 // 232
	0,                 // 233
	0,                 // 234
	0,                 // 235
	0,                 // 236
	0,                 // 237
	0,                 // 238
	0,                 // 239
	0,                 // 240
	0,                 // 241
	0,                 // 242
	0,                 // 243
	0,                 // 244
	0,                 // 245
	0,                 // 246
	0,                 // 247
	0,                 // 248
	0,                 // 249
	0,                 // 250
	0,                 // 251
	0,                 // 252
	0,                 // 253
	0,                 // 254
	0                  // 255
};

static int FsSpecialKeyCode[256]=
{
	FSKEY_UP,           // 0
	FSKEY_DOWN,         // 1
	FSKEY_LEFT,         // 2
	FSKEY_RIGHT,        // 3
	FSKEY_F1,           // 4
	FSKEY_F2,           // 5
	FSKEY_F3,           // 6
	FSKEY_F4,           // 7
	FSKEY_F5,           // 8
	FSKEY_F6,           // 9
	FSKEY_F7,           // 10
	FSKEY_F8,           // 11
	FSKEY_F9,           // 12
	FSKEY_F10,          // 13
	FSKEY_F11,          // 14
	FSKEY_F12,          // 15
	FSKEY_F1,           // 16 0x10
	FSKEY_F2,           // 17
	FSKEY_F3,           // 18
	FSKEY_F4,           // 19
	FSKEY_F5,           // 20
	FSKEY_F6,           // 21
	FSKEY_F7,           // 22
	FSKEY_F8,           // 23
	FSKEY_F9,           // 24
	FSKEY_F10,          // 25
	FSKEY_F11,          // 26
	FSKEY_F12,          // 27
	FSKEY_F1,           // 28
	FSKEY_F2,           // 29
	FSKEY_F3,           // 30
	FSKEY_F4,           // 31
	FSKEY_F5,           // 32 0x20
	FSKEY_F6,           // 33
	FSKEY_F7,           // 34
	FSKEY_F8,           // 35
	FSKEY_F9,           // 36
	FSKEY_F10,          // 37
	FSKEY_F11,          // 38
	FSKEY_INS,          // 39
	FSKEY_DEL,          // 40
	FSKEY_HOME,         // 41
	0,                  // 42 Begin
	FSKEY_END,          // 43
	FSKEY_PAGEUP,       // 44
	FSKEY_PAGEDOWN,     // 45
	FSKEY_PRINTSCRN,    // 46
	FSKEY_SCROLLLOCK,   // 47
	FSKEY_PAUSEBREAK,   // 48 0x30
	0,                  // 49
	FSKEY_PAUSEBREAK,   // 50
	0,                  // 51 Reset
	0,                  // 52 Stop
	FSKEY_CONTEXT,      // 53 Menu 0x35
	0,                  // 54 User
	0,                  // 55 System
	FSKEY_PRINTSCRN,    // 56
	0,                  // 57 Clear line
	0,                  // 58 Clear display
	0,                  // 59 Insert line
	0,                  // 60 Delete line
	FSKEY_INS,          // 61
	FSKEY_DEL,          // 62
	FSKEY_PAGEUP,       // 63
	FSKEY_PAGEDOWN,     // 64
	0,                  // 65 Select
	0,                  // 66 Execute
	0,                  // 67 Undo
	0,                  // 68 Redo
	0,                  // 69 Find
	0,                  // 70 Help
	0,                  // 71 Mode Switch
	0,                  // 72
	0,                  // 73
	0,                  // 74
	0,                  // 75
	0,                  // 76
	0,                  // 77
	0,                  // 78
	0,                  // 79
	0,                  // 80
	0,                  // 81
	0,                  // 82
	0,                  // 83
	0,                  // 84
	0,                  // 85
	0,                  // 86
	0,                  // 87
	0,                  // 88
	0,                  // 89
	0,                  // 90
	0,                  // 91
	0,                  // 92
	0,                  // 93
	0,                  // 94
	0,                  // 95
	0,                  // 96
	0,                  // 97
	0,                  // 98
	0,                  // 99
	0,                  // 100
	0,                  // 101
	0,                  // 102
	0,                  // 103
	0,                  // 104
	0,                  // 105
	0,                  // 106
	0,                  // 107
	0,                  // 108
	0,                  // 109
	0,                  // 110
	0,                  // 111
	0,                  // 112
	0,                  // 113
	0,                  // 114
	0,                  // 115
	0,                  // 116
	0,                  // 117
	0,                  // 118
	0,                  // 119
	0,                  // 120
	0,                  // 121
	0,                  // 122
	0,                  // 123
	0,                  // 124
	0,                  // 125
	0,                  // 126
	0,                  // 127
	0,                  // 128
	0,                  // 129
	0,                  // 130
	0,                  // 131
	0,                  // 132
	0,                  // 133
	0,                  // 134
	0,                  // 135
	0,                  // 136
	0,                  // 137
	0,                  // 138
	0,                  // 139
	0,                  // 140
	0,                  // 141
	0,                  // 142
	0,                  // 143
	0,                  // 144
	0,                  // 145
	0,                  // 146
	0,                  // 147
	0,                  // 148
	0,                  // 149
	0,                  // 150
	0,                  // 151
	0,                  // 152
	0,                  // 153
	0,                  // 154
	0,                  // 155
	0,                  // 156
	0,                  // 157
	0,                  // 158
	0,                  // 159
	0,                  // 160
	0,                  // 161
	0,                  // 162
	0,                  // 163
	0,                  // 164
	0,                  // 165
	0,                  // 166
	0,                  // 167
	0,                  // 168
	0,                  // 169
	0,                  // 170
	0,                  // 171
	0,                  // 172
	0,                  // 173
	0,                  // 174
	0,                  // 175
	0,                  // 176
	0,                  // 177
	0,                  // 178
	0,                  // 179
	0,                  // 180
	0,                  // 181
	0,                  // 182
	0,                  // 183
	0,                  // 184
	0,                  // 185
	0,                  // 186
	0,                  // 187
	0,                  // 188
	0,                  // 189
	0,                  // 190
	0,                  // 191
	0,                  // 192
	0,                  // 193
	0,                  // 194
	0,                  // 195
	0,                  // 196
	0,                  // 197
	0,                  // 198
	0,                  // 199
	0,                  // 200
	0,                  // 201
	0,                  // 202
	0,                  // 203
	0,                  // 204
	0,                  // 205
	0,                  // 206
	0,                  // 207
	0,                  // 208
	0,                  // 209
	0,                  // 210
	0,                  // 211
	0,                  // 212
	0,                  // 213
	0,                  // 214
	0,                  // 215
	0,                  // 216
	0,                  // 217
	0,                  // 218
	0,                  // 219
	0,                  // 220
	0,                  // 221
	0,                  // 222
	0,                  // 223
	0,                  // 224
	0,                  // 225
	0,                  // 226
	0,                  // 227
	0,                  // 228
	0,                  // 229
	0,                  // 230
	0,                  // 231
	0,                  // 232
	0,                  // 233
	0,                  // 234
	0,                  // 235
	0,                  // 236
	0,                  // 237
	0,                  // 238
	0,                  // 239
	0,                  // 240
	0,                  // 241
	0,                  // 242
	0,                  // 243
	0,                  // 244
	0,                  // 245
	0,                  // 246
	0,                  // 247
	0,                  // 248
	0,                  // 249
	0,                  // 250
	0,                  // 251
	0,                  // 252
	0,                  // 253
	0,                  // 254
	0                   // 255
};

static int YsMacUnicodeToFsKeyCode(int uni)
{
	if(0<=uni && uni<256)
	{
		return FsNormalKeyCode[uni];
	}
	else if(0xf700<=uni && uni<0xf800)
	{
		return FsSpecialKeyCode[uni-0xf700];
	}
	return 0;
}


struct FsMouseEventLog
{
	int eventType;
	int lb,mb,rb;
	int mx,my;
};

static int fsCommandKeyState=0;
static int fsKeyIsDown[FSKEY_NUM_KEYCODE]={0};

#define NKEYBUF 256
static int nKeyBufUsed=0;
static int keyBuffer[NKEYBUF];

static int nCharBufUsed=0;
static int charBuffer[NKEYBUF];

static int nMosBufUsed=0;
static struct FsMouseEventLog mosBuffer[NKEYBUF];

static int exposure=0;

static bool maximizedOrFullScreen=false;
static NSRect restoreRect={0,0,800,600};


@interface YsMacDelegate : NSObject /* < NSApplicationDelegate > */
/* Example: Fire has the same problem no explanation */
{
}
/* - (BOOL) applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication; */
@end

@interface YsOpenGLWindow : NSWindow
{
}
@end

@interface YsOpenGLView : NSOpenGLView 
{
}
- (void) drawRect: (NSRect) bounds;
@end

@interface YsTextView : NSTextView
{
}
@end



static bool isTextViewOpen=false;
static YsOpenGLWindow *ysWnd=nil;
static YsOpenGLView *ysView=nil;
static YsTextView *ysText=nil;
static NSView *masterView=nil;
static YsMacDelegate *ysDelegate=nil;



@implementation YsMacDelegate
- (BOOL) applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication
{
	return YES;
}

- (void) applicationWillBecomeActive: (NSNotification *)notification
{
    printf("Will become active\n");
}

- (void) applicationDidBecomeActive: (NSNotification *)notification
{
    printf("Did become active\n");
}

- (void) applicationWillResignActive: (NSNotification *)notification
{
    printf("Will resign active\n");
}

- (void) applicationDidResignActive: (NSNotification *)notification
{
    printf("Did resign active\n");
}
@end

@implementation YsOpenGLWindow
- (void) resizeSubViewFromMasterView
{
	NSRect rect=[masterView frame];

	if(true==isTextViewOpen)
	{
		NSRect glRect=rect;
		glRect.size.height-=24;
		glRect.origin.y+=24;
		[ysView setFrameSize:glRect.size];
		[ysView setFrameOrigin:glRect.origin];

		NSRect textRect=rect;
		textRect.size.height=24;
		[ysText setFrameSize:textRect.size];
		[ysText setFrameOrigin:textRect.origin];
	}
	else
	{
		[ysView setFrameSize:rect.size];
		[ysView setFrameOrigin:rect.origin];
	}
}

- (id) initWithContentRect: (NSRect)rect styleMask:(NSUInteger)wndStyle backing:(NSBackingStoreType)bufferingType defer:(BOOL)deferFlg
{
	self=[super initWithContentRect:rect styleMask:wndStyle backing:bufferingType defer:deferFlg];

	[[NSNotificationCenter defaultCenter] 
		addObserver:self
		selector:@selector(windowDidResize:)
		name:NSWindowDidResizeNotification
		object:self];

	[[NSNotificationCenter defaultCenter]
	  addObserver:self
	  selector:@selector(windowWillClose:)
	  name:NSWindowWillCloseNotification
	  object:self];

	[self setAcceptsMouseMovedEvents:YES];

	// Cocoa seems to move window so that it doesn't overlap icons on the bottom of the window.
	[self setFrameOrigin:rect.origin];

	printf("%s\n",__FUNCTION__);
	return self;
}

- (void) windowDidResize: (NSNotification *)notification
{
	[self resizeSubViewFromMasterView];
}

- (void) windowWillClose: (NSNotification *)notification
{
	[NSApp terminate:nil];	// This can also be exit(0);
}
@end

@implementation YsOpenGLView
-(void) drawRect: (NSRect) bounds
{
	// printf("%s\n",__FUNCTION__);
	exposure=1;
	FsOnPaintCallBackCpp();
}

-(void) prepareOpenGL
{
	printf("%s\n",__FUNCTION__);

	{
		int stencilBits;
		int depthBits;
		glGetIntegerv(GL_DEPTH_BITS,&depthBits);
		printf("Depth Bits:     %d\n",depthBits);
		glGetIntegerv(GL_STENCIL_BITS,&stencilBits);
		printf("Stencil Bits:   %d\n",stencilBits);
	}

	NSRect rect;
	rect=self.frame;
	int wid=rect.size.width;
	int hei=rect.size.height;


	glClearColor(1.0F,1.0F,1.0F,0.0F);
	glClearDepth(1.0F);
	glDisable(GL_DEPTH_TEST);

	glViewport(0,0,wid,hei);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,(float)wid-1,(float)hei-1,0,-1,1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glShadeModel(GL_FLAT);
	glPointSize(1);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glColor3ub(0,0,0);
	printf("%s %d\n",__FUNCTION__,__LINE__);

	openGLprepared=true;

	FsOnInitializeOpenGLC();
}

-(NSMenu *)menuForEvent: (NSEvent *)theEvent
{
	printf("%s\n",__FUNCTION__);
	return [NSView defaultMenu];
}

- (void) flagsChanged: (NSEvent *)theEvent
{
	unsigned int flags;
	flags=[theEvent modifierFlags];

	if(flags&NSAlphaShiftKeyMask) // Caps
	{
		if(fsKeyIsDown[FSKEY_CAPSLOCK]==0 && nKeyBufUsed<NKEYBUF)
		{
			keyBuffer[nKeyBufUsed++]=FSKEY_CAPSLOCK;
		}
		fsKeyIsDown[FSKEY_CAPSLOCK]=1;
	}
	else
	{
		fsKeyIsDown[FSKEY_CAPSLOCK]=0;
	}

	if(flags&NSShiftKeyMask)
	{
		if(fsKeyIsDown[FSKEY_SHIFT]==0 && nKeyBufUsed<NKEYBUF)
		{
			keyBuffer[nKeyBufUsed++]=FSKEY_SHIFT;
		}
		fsKeyIsDown[FSKEY_SHIFT]=1;
	}
	else
	{
		fsKeyIsDown[FSKEY_SHIFT]=0;
	}

	if(flags&NSControlKeyMask)
	{
		if(fsKeyIsDown[FSKEY_CTRL]==0 && nKeyBufUsed<NKEYBUF)
		{
			keyBuffer[nKeyBufUsed++]=FSKEY_CTRL;
		}
		fsKeyIsDown[FSKEY_CTRL]=1;
	}
	else
	{
		fsKeyIsDown[FSKEY_CTRL]=0;
	}

	fsCommandKeyState=(0!=(flags&NSCommandKeyMask) ? 1 : 0);

	if((flags&NSAlternateKeyMask) || (flags&NSCommandKeyMask))
	{
		if(fsKeyIsDown[FSKEY_ALT]==0 && nKeyBufUsed<NKEYBUF)
		{
			keyBuffer[nKeyBufUsed++]=FSKEY_ALT;
		}
		fsKeyIsDown[FSKEY_ALT]=1;
	}
	else
	{
		fsKeyIsDown[FSKEY_ALT]=0;
	}

	// Other possible key masks
	// NSNumericPadKeyMask
	// NSHelpKeyMask
	// NSFunctionKeyMask
	// NSDeviceIndependentModifierFlagsMask
}

- (BOOL) performKeyEquivalent: (NSEvent *)theEvent
{
	NSString *chrs;
	chrs=[theEvent characters];
	if([chrs length]>0)
	{
		int unicode=[chrs characterAtIndex:0];
		int keyCode=[theEvent keyCode];

		// I should be able to find a list of key codes so that I can take advantage of [theEvent keyCode]
		// However, Apple has not documented.  The definition of key codes is nowhere to bd found.
		// Good job, Apple.  
		// All I know is:
		//	Ctrl+Tab				[chrs characterAtIndex:0]==9		[theEvent keyCode]==48
		//	Ctrl+Shift+Tab			[chrs characterAtIndex:0]==25		[theEvent keyCode]==48
		//	Ctrl+Y					[chrs characterAtIndex:0]==25		[theEvent keyCode]==16
		//	Ctrl+Shift+Y			[chrs characterAtIndex:0]==25		[theEvent keyCode]==16
		// So, I need to use a number from experiment to distinguish Ctrl+Tab and Ctrl+Y

		if(9==unicode || 25==unicode)
		{
			if(nKeyBufUsed<NKEYBUF)
			{
				if(48==keyCode)
				{
					keyBuffer[nKeyBufUsed++]=FSKEY_TAB;
				}
				else if(16==keyCode)
				{
					keyBuffer[nKeyBufUsed++]=FSKEY_Y;
				}
				return YES;
			}
		}
	}

    return [super performKeyEquivalent:theEvent];
}

- (void) scrollWheel:(NSEvent *)theEvent
{
	if(0>[theEvent deltaY])
	{
		if(nKeyBufUsed<NKEYBUF)
		{
			keyBuffer[nKeyBufUsed++]=FSKEY_WHEELDOWN;
		}
	}
	else if(0<[theEvent deltaY])
	{
		if(nKeyBufUsed<NKEYBUF)
		{
			keyBuffer[nKeyBufUsed++]=FSKEY_WHEELUP;
		}
	}
}

- (void) keyDown:(NSEvent *)theEvent
{
	unsigned int flags;
	flags=[theEvent modifierFlags];

	NSString *chrs,*chrsNoMod;
	chrs=[theEvent characters];
	if(0==(flags & NSCommandKeyMask) && [chrs length]>0)
	{
		int unicode;
		unicode=[chrs characterAtIndex:0];

		if(32<=unicode && unicode<128 && nCharBufUsed<NKEYBUF)
		{
			charBuffer[nCharBufUsed++]=unicode;
		}
	}

	chrsNoMod=[theEvent charactersIgnoringModifiers];
	if([chrsNoMod length]>0)
	{
		int unicode,fskey;
		unicode=[chrsNoMod characterAtIndex:0];
		fskey=YsMacUnicodeToFsKeyCode(unicode);

		if(fskey!=0)
		{
			// 2014/07/26
			// A problem has been discovered.  When the Command key is held down and another key is pressed,
			// a keyDown event is sent, but keyUp event won't be sent.  As a result, the program thinks that
			// the key is held down even after the key is actually released.  Nice logic, Apple.
			// To prevent it from happening, fsKeyIsDown must not be set to 1 if the Command key is down.
			if(0==fsCommandKeyState)
			{
				fsKeyIsDown[fskey]=1;
			}

			if(nKeyBufUsed<NKEYBUF)
			{
				keyBuffer[nKeyBufUsed++]=fskey;
			}
		}
	}
}

- (void) keyUp:(NSEvent *)theEvent
{
	NSString *chrs,*chrsNoMod;
	chrs=[theEvent characters];
	if([chrs length]>0)
	{
		int unicode;
		unicode=[chrs characterAtIndex:0];
	}

	chrsNoMod=[theEvent charactersIgnoringModifiers];
	if([chrsNoMod length]>0)
	{
	  int unicode,fskey;
		unicode=[chrsNoMod characterAtIndex:0];
		fskey=YsMacUnicodeToFsKeyCode(unicode);

		if(fskey!=0)
		{
			fsKeyIsDown[fskey]=0;
		}
	}
}

- (void) mouseMoved:(NSEvent *)theEvent
{
	NSPoint mosPos=[self convertPoint:[theEvent locationInWindow] fromView:nil];

	if(0<nMosBufUsed &&
	   FSMOUSEEVENT_MOVE==mosBuffer[nMosBufUsed-1].eventType)
	{
	  NSRect rect;
	  rect=[self frame];

		mosBuffer[nMosBufUsed-1].mx=(int)mosPos.x;
		mosBuffer[nMosBufUsed-1].my=rect.size.height-1-(int)mosPos.y;
	}
	else if(NKEYBUF>nMosBufUsed)
	{
	  NSRect rect;
	  rect=[self frame];

		mosBuffer[nMosBufUsed].eventType=FSMOUSEEVENT_MOVE;
		mosBuffer[nMosBufUsed].mx=(int)mosPos.x;
		mosBuffer[nMosBufUsed].my=rect.size.height-1-(int)mosPos.y;
		mosBuffer[nMosBufUsed].lb=mouseLb;
		mosBuffer[nMosBufUsed].mb=mouseMb;
		mosBuffer[nMosBufUsed].rb=mouseRb;
		nMosBufUsed++;
	}
}

- (void) mouseDragged:(NSEvent *)theEvent
{
  [self mouseMoved:theEvent];
}

- (void) rightMouseDragged:(NSEvent *)theEvent
{
  [self mouseMoved:theEvent];
}

- (void) otherMouseDragged:(NSEvent *)theEvent
{
  [self mouseMoved:theEvent];
}

- (void) mouseDown:(NSEvent *)theEvent
{
	NSPoint mosPos=[self convertPoint:[theEvent locationInWindow] fromView:nil];
	mouseLb=1;	

	if(NKEYBUF>nMosBufUsed)
	{
	  NSRect rect;
	  rect=[self frame];

		mosBuffer[nMosBufUsed].eventType=FSMOUSEEVENT_LBUTTONDOWN;
		mosBuffer[nMosBufUsed].mx=(int)mosPos.x;
		mosBuffer[nMosBufUsed].my=rect.size.height-1-(int)mosPos.y;
		mosBuffer[nMosBufUsed].lb=mouseLb;
		mosBuffer[nMosBufUsed].mb=mouseMb;
		mosBuffer[nMosBufUsed].rb=mouseRb;
		nMosBufUsed++;
	}
}

- (void) mouseUp:(NSEvent *)theEvent
{
	NSPoint mosPos=[self convertPoint:[theEvent locationInWindow] fromView:nil];
	mouseLb=0;

	if(NKEYBUF>nMosBufUsed)
	{
	  NSRect rect;
	  rect=[self frame];

		mosBuffer[nMosBufUsed].eventType=FSMOUSEEVENT_LBUTTONUP;
		mosBuffer[nMosBufUsed].mx=(int)mosPos.x;
		mosBuffer[nMosBufUsed].my=rect.size.height-1-(int)mosPos.y;
		mosBuffer[nMosBufUsed].lb=mouseLb;
		mosBuffer[nMosBufUsed].mb=mouseMb;
		mosBuffer[nMosBufUsed].rb=mouseRb;
		nMosBufUsed++;
	}
}

- (void) rightMouseDown:(NSEvent *)theEvent
{
	NSPoint mosPos=[self convertPoint:[theEvent locationInWindow] fromView:nil];
	mouseRb=1;

	if(NKEYBUF>nMosBufUsed)
	{
	  NSRect rect;
	  rect=[self frame];

		mosBuffer[nMosBufUsed].eventType=FSMOUSEEVENT_RBUTTONDOWN;
		mosBuffer[nMosBufUsed].mx=(int)mosPos.x;
		mosBuffer[nMosBufUsed].my=rect.size.height-1-(int)mosPos.y;
		mosBuffer[nMosBufUsed].lb=mouseLb;
		mosBuffer[nMosBufUsed].mb=mouseMb;
		mosBuffer[nMosBufUsed].rb=mouseRb;
		nMosBufUsed++;
	}
}

- (void) rightMouseUp:(NSEvent *)theEvent
{
	NSPoint mosPos=[self convertPoint:[theEvent locationInWindow] fromView:nil];
	mouseRb=0;

	if(NKEYBUF>nMosBufUsed)
	{
	  NSRect rect;
	  rect=[self frame];

		mosBuffer[nMosBufUsed].eventType=FSMOUSEEVENT_RBUTTONUP;
		mosBuffer[nMosBufUsed].mx=(int)mosPos.x;
		mosBuffer[nMosBufUsed].my=rect.size.height-1-(int)mosPos.y;
		mosBuffer[nMosBufUsed].lb=mouseLb;
		mosBuffer[nMosBufUsed].mb=mouseMb;
		mosBuffer[nMosBufUsed].rb=mouseRb;
		nMosBufUsed++;
	}
}

- (void) otherMouseDown:(NSEvent *)theEvent
{
	NSPoint mosPos=[self convertPoint:[theEvent locationInWindow] fromView:nil];
	mouseMb=1;

	if(NKEYBUF>nMosBufUsed)
	{
	  NSRect rect;
	  rect=[self frame];

		mosBuffer[nMosBufUsed].eventType=FSMOUSEEVENT_MBUTTONDOWN;
		mosBuffer[nMosBufUsed].mx=(int)mosPos.x;
		mosBuffer[nMosBufUsed].my=rect.size.height-1-(int)mosPos.y;
		mosBuffer[nMosBufUsed].lb=mouseLb;
		mosBuffer[nMosBufUsed].mb=mouseMb;
		mosBuffer[nMosBufUsed].rb=mouseRb;
		nMosBufUsed++;
	}
}

- (void) otherMouseUp:(NSEvent *)theEvent
{
	NSPoint mosPos=[self convertPoint:[theEvent locationInWindow] fromView:nil];
	mouseMb=0;

	if(NKEYBUF>nMosBufUsed)
	{
	  NSRect rect;
	  rect=[self frame];

		mosBuffer[nMosBufUsed].eventType=FSMOUSEEVENT_MBUTTONUP;
		mosBuffer[nMosBufUsed].mx=(int)mosPos.x;
		mosBuffer[nMosBufUsed].my=rect.size.height-1-(int)mosPos.y;
		mosBuffer[nMosBufUsed].lb=mouseLb;
		mosBuffer[nMosBufUsed].mb=mouseMb;
		mosBuffer[nMosBufUsed].rb=mouseRb;
		nMosBufUsed++;
	}
}
@end

@implementation YsTextView
- (void) didChangeText
{
printf("%s %d\n",__FUNCTION__,__LINE__);
}

- (void) keyDown:(NSEvent *)theEvent
{
	// Problem:
	//   I may just want to pass the key event to ysView, if it does not start IME.
	// Question:
	//   How can I tell if the key can just be passed to ysView?
	//   There are some situations that are automatic.
	//   For example, when Command, Alt, or Control key is pressed -> pass to ysView.
	//   When IME is already active -> pass to super.

	NSRange markedRange=[self markedRange];
	if(0<markedRange.length)
	{
		printf("IME active.\n");
		[super keyDown:theEvent];
	}
	else
	{
		unsigned int flags;
		flags=[theEvent modifierFlags];

		int fskey=0;
		NSString *chrsNoMod;
		chrsNoMod=[theEvent charactersIgnoringModifiers];
		if([chrsNoMod length]>0)
		{
			int unicode;
			unicode=[chrsNoMod characterAtIndex:0];
			fskey=YsMacUnicodeToFsKeyCode(unicode);
		}

		bool keyShouldBeSentToYsView=false;
		if(0!=(flags & NSCommandKeyMask) ||
		   0!=(flags & NSAlternateKeyMask) ||
		   0!=(flags &NSControlKeyMask) ||
		   FSKEY_TAB==fskey)
		{
			keyShouldBeSentToYsView=true;
		}
		if((FSKEY_BS==fskey || FSKEY_ENTER==fskey || FSKEY_ESC==fskey ||
		    FSKEY_LEFT==fskey || FSKEY_RIGHT==fskey || FSKEY_UP==fskey || FSKEY_DOWN==fskey ||
		    FSKEY_HOME==fskey || FSKEY_END==fskey || FSKEY_DEL==fskey) &&
		   0==[[[self textStorage] mutableString] length])
		{
			keyShouldBeSentToYsView=true;
		}

		if(keyShouldBeSentToYsView)
		{
			if(fskey!=0)
			{
				if(0==fsCommandKeyState)
				{
					fsKeyIsDown[fskey]=1;
				}
				if(nKeyBufUsed<NKEYBUF)
				{
					keyBuffer[nKeyBufUsed++]=fskey;
				}
			}
		}
		else
		{
			[super keyDown:theEvent];
		}
	}

	{
		NSRange markedRange=[self markedRange];
		if(0==markedRange.length)
		{
			printf("IME inactive (or became inactive).  Flush characters.\n");
			NSMutableAttributedString *text=[self textStorage];
			printf("Len=%d\n",(int)[text length]);

			NSString *plainText=[text string];
			printf("Len=%d\n",(int)[plainText length]);

			for(int i=0; i<[plainText length]; ++i)
			{
				int chr=[plainText characterAtIndex:i];
				printf("[%d] %4x\n",i,chr);
				if(nCharBufUsed<NKEYBUF)
				{
					charBuffer[nCharBufUsed++]=chr;
				}
			}

			[[[self textStorage] mutableString] setString:@""];
		}
	}
}

- (void) flagsChanged: (NSEvent *)theEvent
{
	unsigned int flags;
	flags=[theEvent modifierFlags];

	if(flags&NSAlphaShiftKeyMask) // Caps
	{
		if(fsKeyIsDown[FSKEY_CAPSLOCK]==0 && nKeyBufUsed<NKEYBUF)
		{
			keyBuffer[nKeyBufUsed++]=FSKEY_CAPSLOCK;
		}
		fsKeyIsDown[FSKEY_CAPSLOCK]=1;
	}
	else
	{
		fsKeyIsDown[FSKEY_CAPSLOCK]=0;
	}

	if(flags&NSShiftKeyMask)
	{
		if(fsKeyIsDown[FSKEY_SHIFT]==0 && nKeyBufUsed<NKEYBUF)
		{
			keyBuffer[nKeyBufUsed++]=FSKEY_SHIFT;
		}
		fsKeyIsDown[FSKEY_SHIFT]=1;
	}
	else
	{
		fsKeyIsDown[FSKEY_SHIFT]=0;
	}

	if(flags&NSControlKeyMask)
	{
		if(fsKeyIsDown[FSKEY_CTRL]==0 && nKeyBufUsed<NKEYBUF)
		{
			keyBuffer[nKeyBufUsed++]=FSKEY_CTRL;
		}
		fsKeyIsDown[FSKEY_CTRL]=1;
	}
	else
	{
		fsKeyIsDown[FSKEY_CTRL]=0;
	}

	fsCommandKeyState=(0!=(flags&NSCommandKeyMask) ? 1 : 0);

	if((flags&NSAlternateKeyMask) || (flags&NSCommandKeyMask))
	{
		if(fsKeyIsDown[FSKEY_ALT]==0 && nKeyBufUsed<NKEYBUF)
		{
			keyBuffer[nKeyBufUsed++]=FSKEY_ALT;
		}
		fsKeyIsDown[FSKEY_ALT]=1;
	}
	else
	{
		fsKeyIsDown[FSKEY_ALT]=0;
	}

	// Other possible key masks
	// NSNumericPadKeyMask
	// NSHelpKeyMask
	// NSFunctionKeyMask
	// NSDeviceIndependentModifierFlagsMask

	[super flagsChanged:theEvent];
}
@end



void YsAddMenu(void)
{
#if !__has_feature(objc_arc)
 	NSAutoreleasePool *pool=[[NSAutoreleasePool alloc] init];
#endif

	NSMenu *mainMenu;

	mainMenu=[NSMenu alloc];
	[mainMenu initWithTitle:@"Minimum"];

	NSMenuItem *fileMenu;
	fileMenu=[[NSMenuItem alloc] initWithTitle:@"File" action:NULL keyEquivalent:[NSString string]];
	[mainMenu addItem:fileMenu];

	NSMenu *fileSubMenu;
	fileSubMenu=[[NSMenu alloc] initWithTitle:@"File"];
	[fileMenu setSubmenu:fileSubMenu];

	NSMenuItem *fileMenu_Quit;
	fileMenu_Quit=[[NSMenuItem alloc] initWithTitle:@"Quit"  action:@selector(terminate:) keyEquivalent:@"q"];
	[fileMenu_Quit setTarget:NSApp];
	[fileSubMenu addItem:fileMenu_Quit];

	[NSApp setMainMenu:mainMenu];

#if !__has_feature(objc_arc)
	[pool release];
#endif
}

void YsTestApplicationPath(void)
{
#if !__has_feature(objc_arc)
 	NSAutoreleasePool *pool=[[NSAutoreleasePool alloc] init];
#endif

	char cwd[256];
	getcwd(cwd,255);
	printf("CWD(Initial): %s\n",cwd);

	NSString *path;
	path=[[NSBundle mainBundle] resourcePath]; // 2015/11/30 Changed from bundlePath to resourcePath
	printf("BundlePath:%s\n",[path UTF8String]);

	[[NSFileManager defaultManager] changeCurrentDirectoryPath:path];

	getcwd(cwd,255);
	printf("CWD(Changed): %s\n",cwd);

#if !__has_feature(objc_arc)
	[pool release];
#endif
}



static void AddOpenGLPixelFormatAttrib(int *nAttrib,NSOpenGLPixelFormatAttribute attrib[],NSOpenGLPixelFormatAttribute toAdd)
{
	attrib[ *nAttrib   ]=toAdd;
	attrib[(*nAttrib)+1]=0;
	++(*nAttrib);
}

void FsOpenWindowC(int x0,int y0,int wid,int hei,int useDoubleBuffer,int useMultiSampleBuffer,int fullScreen,const char windowTitle[])
{
#if !__has_feature(objc_arc)

 	NSAutoreleasePool *pool=[[NSAutoreleasePool alloc] init];
#endif

	[NSApplication sharedApplication];
	// [NSBundle loadNibNamed:@"MainMenu" owner:NSApp];

	ysDelegate=[YsMacDelegate alloc];
	[ysDelegate init];
	[NSApp setDelegate: ysDelegate];

	[NSApp finishLaunching];

	NSRect contRect;
	contRect=NSMakeRect(x0,y0,wid,hei);

	maximizedOrFullScreen=false;
	restoreRect=contRect;

	NSWindowStyleMask winStyle=
	  NSTitledWindowMask|
	  NSClosableWindowMask|
	  NSMiniaturizableWindowMask|
	  NSResizableWindowMask;
	
	ysWnd=[YsOpenGLWindow alloc];
	[ysWnd
		initWithContentRect:contRect
		styleMask:winStyle
		backing:NSBackingStoreBuffered 
		defer:NO];

	if(0!=fullScreen)
	{
		[ysWnd setCollectionBehavior:NSWindowCollectionBehaviorFullScreenPrimary];
	}

	int nFormatAttrib=0;
	NSOpenGLPixelFormatAttribute formatAttrib[64]={0};

	AddOpenGLPixelFormatAttrib(&nFormatAttrib,formatAttrib,NSOpenGLPFAOpenGLProfile);
	AddOpenGLPixelFormatAttrib(&nFormatAttrib,formatAttrib,NSOpenGLProfileVersionLegacy);
	// AddOpenGLPixelFormatAttrib(&nFormatAttrib,formatAttrib,NSOpenGLProfileVersion3_2Core);

	// Deprecated? AddOpenGLPixelFormatAttrib(&nFormatAttrib,formatAttrib,NSOpenGLPFAWindow);
	AddOpenGLPixelFormatAttrib(&nFormatAttrib,formatAttrib,NSOpenGLPFADepthSize);
	AddOpenGLPixelFormatAttrib(&nFormatAttrib,formatAttrib,(NSOpenGLPixelFormatAttribute)32);
	AddOpenGLPixelFormatAttrib(&nFormatAttrib,formatAttrib,NSOpenGLPFAStencilSize);
	AddOpenGLPixelFormatAttrib(&nFormatAttrib,formatAttrib,(NSOpenGLPixelFormatAttribute)1);
	if(0!=useDoubleBuffer)
	{
		AddOpenGLPixelFormatAttrib(&nFormatAttrib,formatAttrib,NSOpenGLPFADoubleBuffer);
	}
	if(0!=useMultiSampleBuffer)
	{
		AddOpenGLPixelFormatAttrib(&nFormatAttrib,formatAttrib,NSOpenGLPFAMultisample);
		AddOpenGLPixelFormatAttrib(&nFormatAttrib,formatAttrib,NSOpenGLPFASampleBuffers);
		AddOpenGLPixelFormatAttrib(&nFormatAttrib,formatAttrib,(NSOpenGLPixelFormatAttribute)1);
		AddOpenGLPixelFormatAttrib(&nFormatAttrib,formatAttrib,NSOpenGLPFASamples);
		AddOpenGLPixelFormatAttrib(&nFormatAttrib,formatAttrib,(NSOpenGLPixelFormatAttribute)4);
	}

	NSOpenGLPixelFormat *format=[NSOpenGLPixelFormat alloc];
	[format initWithAttributes: formatAttrib];


	// 2019/11/18
	// macOSX 10.15 changed the default value of wantsBestResolutionOpenGLSurface from NO to YES.
	// Value apparently needs to be NO.

	contRect=NSMakeRect(0,0,800,600);
	masterView=[[NSView alloc] initWithFrame:contRect];
	[masterView setWantsBestResolutionOpenGLSurface:NO];
	[ysWnd setContentView:masterView];



	openGLprepared=false;
	ysView=[[YsOpenGLView alloc] initWithFrame:contRect pixelFormat:format];
	[ysView setWantsBestResolutionOpenGLSurface:NO];
	[masterView addSubview:ysView];
	[ysWnd makeFirstResponder:ysView];


	ysText=[[YsTextView alloc] initWithFrame:contRect];
	// [masterView addSubview:ysText];


	[ysWnd makeKeyAndOrderFront:nil];
	[ysWnd makeMainWindow];

	[ysWnd resizeSubViewFromMasterView];

	[NSApp activateIgnoringOtherApps:YES];

	YsAddMenu();

#if !__has_feature(objc_arc)
	[pool release];
#endif

	int i;
	for(i=0; i<FSKEY_NUM_KEYCODE; i++)
	{
		fsKeyIsDown[i]=0;
	}

	// 2015/10/04
	// OSX El Capitan creates OpenGL context asynchronously.
	// Therefore, the initialization needs to be delayed until prepareOpenGL.
	time_t t0=time(NULL);
	while(time(NULL)-t0<2)
	{
	    FsPollDeviceC();
	    if(true==openGLprepared)
		{
			printf("OpenGL context prepared.  Good to go.\n");
			break;
		}
	}

	// 2018/11/24
	// OSX Mojave intentinally nullify OpenGL context time to time.
	// You cannot load textures outside of the drawing functions, which is ridiculous.
	// Obviously Apple is trying to deliberately kill OpenGL applications.
	// To counter the hostility, I need to re-make OpenGL context current
	// from time to time.
	FsMakeCurrentC();
}

void FsResizeWindowC(int wid,int hei)
{
	NSRect viewRect=[ysView frame];
	viewRect.size.width=wid;
	viewRect.size.height=hei;

	[ysWnd setContentSize:viewRect.size];

	[ysView setFrame:viewRect];
}

void FsGetWindowSizeC(int *wid,int *hei)
{
	NSRect rect;
	rect=[ysView frame];
	*wid=rect.size.width;
	*hei=rect.size.height;
}

void FsGetWindowPositionC(int *x0,int *y0)
{
	NSRect rect;
	rect=[ysWnd frame];

	NSRect contRect=[ysWnd contentRectForFrameRect:rect];

	*x0=contRect.origin.x;
	*y0=contRect.origin.y;
}

void FsMaximizeWindowC(void)
{
	if(true!=maximizedOrFullScreen)
	{
		restoreRect=[ysWnd frame];
		maximizedOrFullScreen=true;
	}

	[[NSApplication sharedApplication] setPresentationOptions:NSApplicationPresentationDefault];

	NSScreen *screen=[ysWnd screen];
	NSRect visibleFrame=[screen visibleFrame];
	[ysWnd setFrame:visibleFrame display:TRUE];
}

void FsUnmaximizeWindowC(void)
{
	if(true==maximizedOrFullScreen)
	{
		[[NSApplication sharedApplication] setPresentationOptions:NSApplicationPresentationDefault];
		[ysWnd setFrame:restoreRect display:TRUE];
		maximizedOrFullScreen=false;
	}
}

void FsMakeFullScreenC(void)
{
	if(true!=maximizedOrFullScreen)
	{
		restoreRect=[ysWnd frame];
		maximizedOrFullScreen=true;
	}

	[[NSApplication sharedApplication] setPresentationOptions:NSApplicationPresentationDefault|NSApplicationPresentationHideMenuBar|NSApplicationPresentationHideDock];

	NSScreen *screen=[ysWnd screen];
	NSRect visibleFrame=[screen visibleFrame];
	[ysWnd setFrame:visibleFrame display:TRUE];
}

void FsPollDeviceC(void)
{
#if !__has_feature(objc_arc)
 	NSAutoreleasePool *pool=[[NSAutoreleasePool alloc] init];
#endif

	FsMakeCurrentC();

	for(;;)
	{
	#if !__has_feature(objc_arc)
	 	[pool release];
	 	pool=[[NSAutoreleasePool alloc] init];
	#endif
        
        NSEvent *event;
		event=[NSApp
			   nextEventMatchingMask:NSAnyEventMask
			   untilDate: [NSDate distantPast]
			   inMode: NSDefaultRunLoopMode
			   dequeue:YES];

		if(event!=nil)
		{
            if([event type]==NSRightMouseDown)
            {
                // printf("R mouse down event\n");
            }
            else if([event type]==NSLeftMouseDown)
            {
                // printf("L mouse down event\n");
            }

            [NSApp sendEvent:event];
			[NSApp updateWindows];
        }
		else
		{
			break;
		}
	}

	FsMakeCurrentC();

#if !__has_feature(objc_arc)
	[pool release];	
#endif
}

void FsMakeCurrentC()
{
	[[ysView openGLContext] makeCurrentContext];
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

int FsPassedTimeC(void)
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

	return ms;
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

void FsMouseC(int *lb,int *mb,int *rb,int *mx,int *my)
{
	*lb=mouseLb;
	*mb=mouseMb;
	*rb=mouseRb;

	NSPoint loc;
	loc=[NSEvent mouseLocation];
	loc=[ysWnd convertScreenToBase:loc];
	loc=[ysView convertPoint:loc fromView:nil];

	NSRect rect;
	rect=[ysView frame];
	*mx=loc.x;
	*my=rect.size.height-1-loc.y;
}

void FsSetMousePositionC(int mx,int my)
{
	if([ysWnd isKeyWindow])
	{
		NSPoint newPos;

		NSRect rect;
		rect=[ysView frame];
		my=rect.size.height-1-my;

		newPos.x=mx;
		newPos.y=my;
		newPos=[ysView convertPoint:newPos toView:nil];
		newPos=[ysWnd convertBaseToScreen:newPos];

		// Quartz uses (0,0) as top-left.  The right way.
		// Cocoa uses (0,0) as bottom-left.  The wrong way.
		// I need to convert from the wrong way to the right way.
		NSRect scrnRect=ysWnd.screen.frame;
		newPos.y=scrnRect.size.height-1-newPos.y;

		CGWarpMouseCursorPosition(newPos);
	}
}

int FsGetMouseEventC(int *lb,int *mb,int *rb,int *mx,int *my)
{
	if(0<nMosBufUsed)
	{
		const int eventType=mosBuffer[0].eventType;
		*lb=mosBuffer[0].lb;
		*mb=mosBuffer[0].mb;
		*rb=mosBuffer[0].rb;
		*mx=mosBuffer[0].mx;
		*my=mosBuffer[0].my;

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
		FsMouseC(lb,mb,rb,mx,my);
		return FSMOUSEEVENT_NONE;
	}
}

void FsSwapBufferC(void)
{
	glFlush();
	[[ysView openGLContext] flushBuffer];
}

int FsInkeyC(void)
{
	if(nKeyBufUsed>0)
	{
		int i,fskey;
		fskey=keyBuffer[0];
		nKeyBufUsed--;
		for(i=0; i<nKeyBufUsed; i++)
		{
			keyBuffer[i]=keyBuffer[i+1];
		}
		return fskey;
	}
	return 0;
}

void FsPushKeyC(int fsKeyCode)
{
	if(nKeyBufUsed<NKEYBUF)
	{
		keyBuffer[nKeyBufUsed]=fsKeyCode;
		++nKeyBufUsed;
	}
}

int FsInkeyCharC(void)
{
	if(nCharBufUsed>0)
	{
		int i,c;
		c=charBuffer[0];
		nCharBufUsed--;
		for(i=0; i<nCharBufUsed; i++)
		{
			charBuffer[i]=charBuffer[i+1];
		}
		return c;
	}
	return 0;
}

int FsKeyStateC(int fsKeyCode)
{
	if(0<=fsKeyCode && fsKeyCode<FSKEY_NUM_KEYCODE)
	{
		return fsKeyIsDown[fsKeyCode];
	}
	return 0;
}

void FsChangeToProgramDirC(void)
{
	NSString *path;
	path=[[NSBundle mainBundle] resourcePath]; // 2015/11/30 Changed from bundlePath to resourcePath
	printf("ResourcePath:%s\n",[path UTF8String]);

	[[NSFileManager defaultManager] changeCurrentDirectoryPath:path];
}

int FsCheckExposureC(void)
{
	int ret;
	ret=exposure;
	exposure=0;
	return ret;
}

void FsPushOnPaintEventC(void)
{
	[ysView setNeedsDisplay:YES];
}

void FsEnableIMEC(void)
{
	if(true!=isTextViewOpen)
	{
		isTextViewOpen=true;
		[masterView addSubview:ysText];
		[ysWnd resizeSubViewFromMasterView];
		[ysWnd makeFirstResponder:ysText];

		{
			// 2017/07/08
			// While the text view is open, BackSpace key up event is not sent to the
			// main window.  Key down event apparently is.  As a result, subsequent
			// FsClearEventQueue gets stuck unless the user presses and releases the BackSpace key
			// again.  A not-so-clean solution is force clear fsKeyIsDown whenever the view is closed.
			int i;
			for(i=0; i<FSKEY_NUM_KEYCODE; ++i)
			{
				fsKeyIsDown[i]=0;
			}
		}
	}
}

void FsDisableIMEC(void)
{
	if(true==isTextViewOpen)
	{
		isTextViewOpen=false;
		[ysText removeFromSuperview];
		[ysWnd resizeSubViewFromMasterView];
		[ysWnd makeFirstResponder:ysView];

		{
			// 2017/07/08
			// While the text view is open, BackSpace key up event is not sent to the
			// main window.  Key down event apparently is.  As a result, subsequent
			// FsClearEventQueue gets stuck unless the user presses and releases the BackSpace key
			// again.  A not-so-clean solution is force clear fsKeyIsDown whenever the view is open.
			int i;
			for(i=0; i<FSKEY_NUM_KEYCODE; ++i)
			{
				fsKeyIsDown[i]=0;
			}
		}
	}
}

/* int main(int argc, char *argv[])
{
	YsTestApplicationPath();

	YsOpenWindow();

	printf("Going into the event loop\n");

	double angle;
	angle=0.0;
	while(1)
	{
		YsPollEvent();

		DrawTriangle(angle);
		angle=angle+0.05;

		YsSleep(20);
	}

	return 0;
	} */
