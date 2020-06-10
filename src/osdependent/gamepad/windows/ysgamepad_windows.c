#include <windows.h>
#include <mmsystem.h>
#include "../ysgamepad.h"

#pragma comment(lib,"winmm.lib")

#define YS_MAX_NUM_GAMEPADS 8
static int numDev=0;
static JOYCAPS joyCaps[YS_MAX_NUM_GAMEPADS];

void YsGamePadInitialize(void)
{
	for(int i=0; i<YS_MAX_NUM_GAMEPADS; ++i)
	{
		if(JOYERR_NOERROR==joyGetDevCaps(i,&joyCaps[i],sizeof(joyCaps[i])))
		{
			numDev=i+1;
		}
		else
		{
			ZeroMemory(&joyCaps[i],sizeof(joyCaps[i]));
		}
	}
}

void YsGamePadTerminate(void)
{
}

void YsGamePadWaitReady(void)
{
}

int YsGamePadGetNumDevices(void)
{
	return numDev;
}

static float ScaleAxis(float value,float vmin,float vmax)
{
	return ((value-vmin)/(vmax-vmin))*2.0f-1.0f;
}

void YsGamePadRead(struct YsGamePadReading *reading,int gamePadId)
{
	JOYINFOEX joy;
	joy.dwSize=sizeof(joy);
	joy.dwFlags=JOY_RETURNALL /*|JOY_RETURNRAWDATA */;
	if(joyGetPosEx(gamePadId,&joy)==JOYERR_NOERROR)
	{
		reading->axes[0]=ScaleAxis((float)joy.dwXpos,(float)joyCaps[gamePadId].wXmin,(float)joyCaps[gamePadId].wXmax);
		reading->axes[1]=ScaleAxis((float)joy.dwYpos,(float)joyCaps[gamePadId].wYmin,(float)joyCaps[gamePadId].wYmax);
		reading->axes[2]=ScaleAxis((float)joy.dwZpos,(float)joyCaps[gamePadId].wZmin,(float)joyCaps[gamePadId].wZmax);
		reading->axes[3]=ScaleAxis((float)joy.dwRpos,(float)joyCaps[gamePadId].wRmin,(float)joyCaps[gamePadId].wRmax);
		reading->axes[4]=ScaleAxis((float)joy.dwUpos,(float)joyCaps[gamePadId].wUmin,(float)joyCaps[gamePadId].wUmax);
		reading->axes[5]=ScaleAxis((float)joy.dwVpos,(float)joyCaps[gamePadId].wVmin,(float)joyCaps[gamePadId].wVmax);
		reading->axes[6]=0;
		reading->axes[7]=0;

		for(int i=0; i<YSGAMEPAD_MAX_NUM_BUTTONS; i++)
		{
			reading->buttons[i]=((joy.dwButtons&(1<<i))!=0 ? 1 : 0);
		}

		if(joy.dwPOV==JOY_POVCENTERED)
		{
			reading->dirs[0].upDownLeftRight[0]=0;
			reading->dirs[0].upDownLeftRight[1]=0;
			reading->dirs[0].upDownLeftRight[2]=0;
			reading->dirs[0].upDownLeftRight[3]=0;
		}
		else
		{
			if((33750<=joy.dwPOV && joy.dwPOV<35900) || (0<=joy.dwPOV && joy.dwPOV<2250))
			{
				reading->dirs[0].upDownLeftRight[0]=1;
				reading->dirs[0].upDownLeftRight[1]=0;
				reading->dirs[0].upDownLeftRight[2]=0;
				reading->dirs[0].upDownLeftRight[3]=0;
			}
			else if(2250<=joy.dwPOV && joy.dwPOV<6750)
			{
				reading->dirs[0].upDownLeftRight[0]=1;
				reading->dirs[0].upDownLeftRight[1]=0;
				reading->dirs[0].upDownLeftRight[2]=0;
				reading->dirs[0].upDownLeftRight[3]=1;
			}
			else if(6750<=joy.dwPOV && joy.dwPOV<11250)
			{
				reading->dirs[0].upDownLeftRight[0]=0;
				reading->dirs[0].upDownLeftRight[1]=0;
				reading->dirs[0].upDownLeftRight[2]=0;
				reading->dirs[0].upDownLeftRight[3]=1;
			}
			else if(11250<=joy.dwPOV && joy.dwPOV<15750)
			{
				reading->dirs[0].upDownLeftRight[0]=0;
				reading->dirs[0].upDownLeftRight[1]=1;
				reading->dirs[0].upDownLeftRight[2]=0;
				reading->dirs[0].upDownLeftRight[3]=1;
			}
			else if(15750<=joy.dwPOV && joy.dwPOV<20250)
			{
				reading->dirs[0].upDownLeftRight[0]=0;
				reading->dirs[0].upDownLeftRight[1]=1;
				reading->dirs[0].upDownLeftRight[2]=0;
				reading->dirs[0].upDownLeftRight[3]=0;
			}
			else if(20250<=joy.dwPOV && joy.dwPOV<24750)
			{
				reading->dirs[0].upDownLeftRight[0]=0;
				reading->dirs[0].upDownLeftRight[1]=1;
				reading->dirs[0].upDownLeftRight[2]=1;
				reading->dirs[0].upDownLeftRight[3]=0;
			}
			else if(24750<=joy.dwPOV && joy.dwPOV<29250)
			{
				reading->dirs[0].upDownLeftRight[0]=0;
				reading->dirs[0].upDownLeftRight[1]=0;
				reading->dirs[0].upDownLeftRight[2]=1;
				reading->dirs[0].upDownLeftRight[3]=0;
			}
			else if(29250<=joy.dwPOV && joy.dwPOV<33750)
			{
				reading->dirs[0].upDownLeftRight[0]=1;
				reading->dirs[0].upDownLeftRight[1]=0;
				reading->dirs[0].upDownLeftRight[2]=1;
				reading->dirs[0].upDownLeftRight[3]=0;
			}
		}
		for(int i=1; i<YSGAMEPAD_MAX_NUM_DIRS; ++i)
		{
			reading->dirs[i].upDownLeftRight[0]=0;
			reading->dirs[i].upDownLeftRight[1]=0;
			reading->dirs[i].upDownLeftRight[2]=0;
			reading->dirs[i].upDownLeftRight[3]=0;
		}
	}
	else
	{
		YsGamePadClear(reading);
	}
}
