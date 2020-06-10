#include "ysgamepad.h"



void YsGamePadClear(struct YsGamePadReading *reading)
{
	for(int i=0; i<YSGAMEPAD_MAX_NUM_BUTTONS; ++i)
	{
		reading->buttons[i]=0;
	}
	for(int i=0; i<YSGAMEPAD_MAX_NUM_DIRS; ++i)
	{
		reading->dirs[i].upDownLeftRight[0]=0;
		reading->dirs[i].upDownLeftRight[1]=0;
		reading->dirs[i].upDownLeftRight[2]=0;
		reading->dirs[i].upDownLeftRight[3]=0;
	}
	for(int i=0; i<YSGAMEPAD_MAX_NUM_AXES; ++i)
	{
		reading->axes[i]=0.0f;
	}
}

void YsGamdPadTranslateAnalogToDigital(struct YsGamePadDirectionButton *dir,float x,float y)
{
	if(0.5<y)
	{
		dir->upDownLeftRight[0]=0;
		dir->upDownLeftRight[1]=1;
	}
	else if(y<-0.5)
	{
		dir->upDownLeftRight[0]=1;
		dir->upDownLeftRight[1]=0;
	}
	else
	{
		dir->upDownLeftRight[0]=0;
		dir->upDownLeftRight[1]=0;
	}

	if(0.5<x)
	{
		dir->upDownLeftRight[2]=0;
		dir->upDownLeftRight[3]=1;
	}
	else if(x<-0.5)
	{
		dir->upDownLeftRight[2]=1;
		dir->upDownLeftRight[3]=0;
	}
	else
	{
		dir->upDownLeftRight[2]=0;
		dir->upDownLeftRight[3]=0;
	}
}

