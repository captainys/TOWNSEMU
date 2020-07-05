#include <stdio.h>
#include "../io.h"

#define CDCMD_READTOC               	0x05
#define CMDFLAG_STATUS_REQUEST 			0x20
#define CMDFLAG_IRQ            			0x40

#define IO_CDROM_MASTER_CTRL_STATUS		0x4C0
#define CDROM_W_CLEAR_SIRQ				0x80
#define CDROM_W_CLEAR_DEI				0x40
#define CDROM_W_CTRL_ENABLE_SIRQ		0x02
#define CDROM_W_CTRL_ENABLE_DEI			0x01
#define CDROM_R_SRQ						0x02

#define IO_CDROM_COMMAND_STATUS			0x4C2
#define CDROM_R_SIRQ					0x80

#define IO_CDROM_PARAMETER_DATA			0x4C4



#define READTOCSTATE_SKIPPING_NO_ERROR	0
#define READTOCSTATE_SKIPPING_CONST		1
#define READTOCSTATE_NUM_TRACKS			2
#define READTOCSTATE_REST				3



int main(void)
{
	unsigned int readTocState=READTOCSTATE_SKIPPING_NO_ERROR;
	unsigned int wait0x16=0,countDown=0;

	// Mask IRQ, Clear existing SIRQ and DEI
	IOWriteByte(IO_CDROM_MASTER_CTRL_STATUS,CDROM_W_CLEAR_SIRQ|CDROM_W_CTRL_ENABLE_DEI);

	// Write Read-TOC command
	IOWriteByte(IO_CDROM_PARAMETER_DATA,0);
	IOWriteByte(IO_CDROM_PARAMETER_DATA,0);
	IOWriteByte(IO_CDROM_PARAMETER_DATA,0);
	IOWriteByte(IO_CDROM_PARAMETER_DATA,0);
	IOWriteByte(IO_CDROM_PARAMETER_DATA,0);
	IOWriteByte(IO_CDROM_PARAMETER_DATA,0);
	IOWriteByte(IO_CDROM_PARAMETER_DATA,0);
	IOWriteByte(IO_CDROM_PARAMETER_DATA,0);
	IOWriteByte(IO_CDROM_COMMAND_STATUS,CDCMD_READTOC|CMDFLAG_STATUS_REQUEST);

	for(;;)
	{
		if(0!=(IOReadByte(IO_CDROM_MASTER_CTRL_STATUS)&CDROM_R_SRQ))
		{
			unsigned char statusBytes[4];
			statusBytes[0]=IOReadByte(IO_CDROM_COMMAND_STATUS);
			statusBytes[1]=IOReadByte(IO_CDROM_COMMAND_STATUS);
			statusBytes[2]=IOReadByte(IO_CDROM_COMMAND_STATUS);
			statusBytes[3]=IOReadByte(IO_CDROM_COMMAND_STATUS);

			printf("%02x %02x %02x %02x\n",
				statusBytes[0],
				statusBytes[1],
				statusBytes[2],
				statusBytes[3]);

			if(0!=wait0x16)
			{
				if(0x16==statusBytes[0])
				{
					wait0x16=0;
					continue;
				}
				else
				{
					printf("0x16 expected (Received 0x%02x).\n",statusBytes[0]);
					return 0;
				}
			}

			switch(readTocState)
			{
			case READTOCSTATE_SKIPPING_NO_ERROR:
				if(0!=statusBytes[0])
				{
					printf("Read TOC Error.\n");
					return 0;
				}
				else
				{
					readTocState=READTOCSTATE_SKIPPING_CONST;
					wait0x16=1;
				}
				break;
			case READTOCSTATE_SKIPPING_CONST:
				if(0x17!=statusBytes[0])
				{
					printf("0x17 expected (Received 0x%02x).\n",statusBytes[0]);
					return 0;
				}
				else
				{
					readTocState=READTOCSTATE_NUM_TRACKS;
					wait0x16=1;
				}
				break;
			case READTOCSTATE_NUM_TRACKS:
				if(0x17!=statusBytes[0])
				{
					printf("0x17 expected (Received 0x%02x).\n",statusBytes[0]);
					return 0;
				}
				else
				{
					wait0x16=1;
					readTocState=READTOCSTATE_REST;
					countDown=(statusBytes[1]>>4)*10+(statusBytes[1]&0x0F);
				}
				break;
			case READTOCSTATE_REST:
				if(0x17!=statusBytes[0])
				{
					printf("0x17 expected (Received 0x%02x).\n",statusBytes[0]);
					return 0;
				}
				else
				{
					wait0x16=1;
					--countDown;
					if(0==countDown)
					{
						goto EXIT;
					}
				}
				break;
			}
		}
	}
EXIT:
	return 0;
}
