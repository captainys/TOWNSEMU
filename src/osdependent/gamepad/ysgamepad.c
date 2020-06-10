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
