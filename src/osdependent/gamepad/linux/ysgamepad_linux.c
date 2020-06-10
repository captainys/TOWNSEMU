#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/poll.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include "../ysgamepad.h"



#define MAX_NUM_GAMEPADS 8

struct GamepadInfo
{
  unsigned int tm;
  short v;
  unsigned char tp;
  unsigned char n;
};

static const int PadButtonRead=1;
static const int PadAxisRead=2;

struct GamepadCache
{
	int fd;
	int lastAxis; // Most likely hat is the last two.
	struct YsGamePadReading reading;
};

static struct GamepadCache cache[MAX_NUM_GAMEPADS];

static void Update(struct GamepadCache *cachePtr)
{
	for(;;)
	{
		struct GamepadInfo gp;
		struct pollfd pfd;

		pfd.fd=cachePtr->fd;
		pfd.events=POLLIN|POLLPRI;
		if(poll(&pfd,1,0)<=0)
		{
			break;
		}

		if(read(cachePtr->fd,&gp,sizeof(struct GamepadInfo))==sizeof(struct GamepadInfo))
		{
			if((gp.tp&PadButtonRead) && gp.n<YSGAMEPAD_MAX_NUM_BUTTONS)
			{
				cachePtr->reading.buttons[gp.n]=gp.v;
			}
			if((gp.tp&PadAxisRead) && gp.n<YSGAMEPAD_MAX_NUM_AXES)
			{
				cachePtr->reading.axes[gp.n]=(float)gp.v/(float)32768.0f;
			}

			if(gp.tp&PadAxisRead && cachePtr->lastAxis<gp.n)
			{
				cachePtr->lastAxis=gp.n;
			}
			if((gp.tp&PadAxisRead) && gp.n==cachePtr->lastAxis-1)
			{
				if(gp.v<0)
				{
					cachePtr->reading.dirs[0].upDownLeftRight[2]=1;
					cachePtr->reading.dirs[0].upDownLeftRight[3]=0;
				}
				else if(0<gp.v)
				{
					cachePtr->reading.dirs[0].upDownLeftRight[2]=0;
					cachePtr->reading.dirs[0].upDownLeftRight[3]=1;
				}
				else
				{
					cachePtr->reading.dirs[0].upDownLeftRight[2]=0;
					cachePtr->reading.dirs[0].upDownLeftRight[3]=0;
				}
			}
			if((gp.tp&PadAxisRead) && gp.n==cachePtr->lastAxis)
			{
				if(gp.v<0)
				{
					cachePtr->reading.dirs[0].upDownLeftRight[0]=1;
					cachePtr->reading.dirs[0].upDownLeftRight[1]=0;
				}
				else if(0<gp.v)
				{
					cachePtr->reading.dirs[0].upDownLeftRight[0]=0;
					cachePtr->reading.dirs[0].upDownLeftRight[1]=1;
				}
				else
				{
					cachePtr->reading.dirs[0].upDownLeftRight[0]=0;
					cachePtr->reading.dirs[0].upDownLeftRight[1]=0;
				}
			}
		}
	}
}

void YsGamePadInitialize(void)
{
	for(int gamepadId=0; gamepadId<MAX_NUM_GAMEPADS; ++gamepadId)
	{
		char fn[256];
		sprintf(fn,"/dev/input/js%d",gamepadId);
		cache[gamepadId].fd=open(fn,O_RDONLY);

		if(cache[gamepadId].fd<0)
		{
			sprintf(fn,"/dev/js%d",gamepadId);
			cache[gamepadId].fd=open(fn,O_RDONLY);
		}

		cache[gamepadId].lastAxis=1;
		YsGamePadClear(&cache[gamepadId].reading);

		if(0<=cache[gamepadId].fd)
		{
			printf("xyz %d\n",gamepadId);
			Update(&cache[gamepadId]);
			cache[gamepadId].reading.dirs[0].upDownLeftRight[0]=0;
			cache[gamepadId].reading.dirs[0].upDownLeftRight[1]=0;
			cache[gamepadId].reading.dirs[0].upDownLeftRight[2]=0;
			cache[gamepadId].reading.dirs[0].upDownLeftRight[3]=0;
		}
	}
}

void YsGamePadTerminate(void)
{
	for(int gamepadId=0; gamepadId<MAX_NUM_GAMEPADS; ++gamepadId)
	{
		if(0<=cache[gamepadId].fd)
		{
			close(cache[gamepadId].fd);
		}
	}
}

void YsGamePadWaitReady(void)
{
}

int YsGamePadGetNumDevices(void)
{
	return 0;
}

void YsGamePadRead(struct YsGamePadReading *reading,int gamePadId)
{
	if(gamePadId<MAX_NUM_GAMEPADS)
	{
		Update(&cache[gamePadId]);
		*reading=cache[gamePadId].reading;
	}
	else
	{
		YsGamePadClear(reading);
	}
}
