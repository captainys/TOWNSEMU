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

extern void VNDRV_PRINT(const char *c_str);
extern void VNDRV_DUMP(const unsigned char *data,unsigned int length);

int main(void)
{
	unsigned char data[256];
	for(unsigned int i=0; i<256; ++i)
	{
		data[i]=i;
	}
	IOWriteByte(TOWNSIO_VNDRV_ENABLE,ENABLE_CODE);
	VNDRV_PRINT("AUX Command Test");
	VNDRV_DUMP(data,sizeof(data));
	return 0;
}
