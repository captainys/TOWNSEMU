#include <stdio.h>
#include "fssimplewindow.h"



int main(void)
{
	FsOpenWindow(100,100,640,480,0);

	int phase=0;
	int waitCount=0;
	int windowSize[]={640,480,200,200,400,400,320,240,640,400};
	while(phase+1<sizeof(windowSize)/sizeof(windowSize[0]))
	{
		FsPollDevice();
		if(0==waitCount)
		{
			int wid,hei;
			FsGetWindowSize(wid,hei);
			if(wid!=windowSize[phase] || hei!=windowSize[phase+1])
			{
				printf("Error! Returned %d %d\n",wid,hei);
				printf("       Expected %d %d\n",windowSize[phase],windowSize[phase+1]);
				return 1;
			}
			phase+=2;
			if(phase+1<sizeof(windowSize)/sizeof(windowSize[0]))
			{
				printf("%d\n",phase);
				FsResizeWindow(windowSize[phase],windowSize[phase+1]);
				waitCount=10;
			}
		}
		FsSleep(10);
		--waitCount;
	}
	printf("Passed.\n");
	return 0;
}
