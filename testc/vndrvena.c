#include <stdio.h>
#include "io.h"

enum
{
	TOWNSIO_VNDRV_APICHECK=       0x2F10,
	TOWNSIO_VNDRV_ENABLE=         0x2F12,
	TOWNSIO_VNDRV_COMMAND=        0x2F14,
	TOWNSIO_VNDRV_AUXCOMMAND=     0x2F18,
	ENABLE_CODE=1,
	DISABLE_CODE=0,
};

int main(void)
{
	for(unsigned int x=0; x<65536; ++x)
	{
		unsigned int y;
		IOWriteWord(TOWNSIO_VNDRV_APICHECK,x);
		y=(~IOReadWord(TOWNSIO_VNDRV_APICHECK))&0xFFFF;
		if(x!=y)
		{
			printf("VNDRV I/O Presence Check Failed.\n");
			printf("Written: %08x\n",x);
			printf("~Read:   %08x\n",y);
			return 1;
		}
	}

	{
		IOWriteWord(TOWNSIO_VNDRV_ENABLE,ENABLE_CODE);
		if(ENABLE_CODE!=IOReadWord(TOWNSIO_VNDRV_ENABLE))
		{
			printf("VNDRV Enabled Word-Read Failed.\n");
			return 1;
		}
		IOWriteWord(TOWNSIO_VNDRV_ENABLE,DISABLE_CODE);
		if(DISABLE_CODE!=IOReadWord(TOWNSIO_VNDRV_ENABLE))
		{
			printf("VNDRV Enabled Word-Read Failed.\n");
			return 1;
		}
		IOWriteByte(TOWNSIO_VNDRV_ENABLE,ENABLE_CODE);
		if(ENABLE_CODE!=IOReadByte(TOWNSIO_VNDRV_ENABLE))
		{
			printf("VNDRV Enabled Byte-Read Failed.\n");
			return 1;
		}
		IOWriteByte(TOWNSIO_VNDRV_ENABLE,DISABLE_CODE);
		if(DISABLE_CODE!=IOReadByte(TOWNSIO_VNDRV_ENABLE))
		{
			printf("VNDRV Enabled Byte-Read Failed.\n");
			return 1;
		}
	}

	IOWriteByte(TOWNSIO_VNDRV_ENABLE,ENABLE_CODE);
	printf("VNDRV Enable Check Passed.\n");

	return 0;
}
