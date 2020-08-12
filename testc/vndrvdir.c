#include <stdio.h>
#include "io.h"

enum
{
	TOWNSIO_VNDRV_APICHECK=       0x2F10,
	TOWNSIO_VNDRV_ENABLE=         0x2F12,
	TOWNSIO_VNDRV_COMMAND=        0x2F14,
	TOWNSIO_VNDRV_AUXCOMMAND=     0x2F18,

	TOWNS_VNDRV_AUXCMD_DEBUGBREAK=0x00,
	TOWNS_VNDRV_AUXCMD_PRINTCSTR= 0x09,
	TOWNS_VNDRV_AUXCMD_MEMDUMP=   0x0A,

	ENABLE_CODE=1,
	DISABLE_CODE=0,
};

extern unsigned int VNDRV_NUM_DRIVES(void);
extern unsigned int VNDRV_FINDFIRST(char *buf,unsigned int drvNum,const char subDir[]);
extern unsigned int VNDRV_FINDNEXT(char *buf,unsigned int drvNum,const char subDir[]);

int main(void)
{
	int numDrv=0,first=1;
	IOWriteWord(TOWNSIO_VNDRV_ENABLE,ENABLE_CODE);

	numDrv=VNDRV_NUM_DRIVES();
	printf("%d shared directories.\n",numDrv);

	for(;;)
	{
		int err;
		char buf[64];
		if(0!=first)
		{
			err=VNDRV_FINDFIRST(buf,0,"");
			first=0;
		}
		else
		{
			err=VNDRV_FINDNEXT(buf,0,"");
		}
		if(0!=err)
		{
			printf("ErrCode=%04x\n",err);
			break;
		}
		printf("%s  %d\n",buf+9,*((int *)(buf+5)));
	}

	return 0;
}
