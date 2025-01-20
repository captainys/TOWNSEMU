// Read TOC by CDC I/O by CaptainYS
// The purpose is to take samples from real FM TOWNS hardware to finally
// demystify what information should be passed to Tsugaru from .CUE and .MDS.
// I should have done this earlier.   2025/01/08

#include <stdio.h>
#include <conio.h>

#define IO_CDC_MASTER_CONTROL	0x4C0
#define IO_CDC_MASTER_STATUS	0x4C0
#define IO_CDC_COMMAND			0x4C2
#define IO_CDC_STATUS			0x4C2
#define IO_CDC_PARAM			0x4C4
#define IO_CDC_TFR_CONTROL		0x4C6

#define CDCCMD_TOCREAD			0x05
#define CDCCMD_IRQ				0x40
#define CDCCMD_STATUSREQ		0x20

#define CLI _inline(0xfa)
#define STI _inline(0xfb)

void WaitCDCReady(void)
{
	while(0==(_inb(IO_CDC_MASTER_STATUS)&1))
	{
	}
}

void PushZeroParameters(void)
{
	_outb(IO_CDC_PARAM,0);
	_outb(IO_CDC_PARAM,0);
	_outb(IO_CDC_PARAM,0);
	_outb(IO_CDC_PARAM,0);
	_outb(IO_CDC_PARAM,0);
	_outb(IO_CDC_PARAM,0);
	_outb(IO_CDC_PARAM,0);
	_outb(IO_CDC_PARAM,0);
}

unsigned int BCD2BIN(unsigned char bcd)
{
	return (bcd>>4)*10+(bcd&0x0f);
}

void ReadStatus(unsigned char sta[4])
{
	while(0==(_inb(IO_CDC_MASTER_STATUS)&2))
	{
	}
	sta[0]=_inb(IO_CDC_STATUS);
	sta[1]=_inb(IO_CDC_STATUS);
	sta[2]=_inb(IO_CDC_STATUS);
	sta[3]=_inb(IO_CDC_STATUS);
}

int ReadTOC(unsigned int *len,unsigned char data[])
{
	unsigned int err=0;
	unsigned char *dataPtr=data;
	unsigned int firstTrack=0,lastTrack=0;
	unsigned int dataType=0;

	*len=0;

	CLI;

	WaitCDCReady();
	PushZeroParameters();

	_outb(IO_CDC_COMMAND,CDCCMD_TOCREAD|CDCCMD_STATUSREQ|CDCCMD_IRQ);

	for(;;)
	{
		ReadStatus(dataPtr);
		if(0x16==dataPtr[0])
		{
			dataType=dataPtr[2];
			dataPtr+=4;
		}
		else if(0x17==dataPtr[0])
		{
			if(0xA0==dataType)
			{
				firstTrack=BCD2BIN(dataPtr[1]);
				dataPtr+=4;
			}
			else if(0xA1==dataType)
			{
				lastTrack=BCD2BIN(dataPtr[1]);
				dataPtr+=4;
			}
			else if(0xA2==dataType)
			{
				dataPtr+=4;
			}
			else
			{
				unsigned int thisTrack=BCD2BIN(dataType);
				dataPtr+=4;
				if(thisTrack==lastTrack)
				{
					*len=dataPtr-data;
					break;
				}
			}
		}
		else if(0x21==dataPtr[0])
		{
			err=dataPtr[1];
			printf("Hard Error.\n");
			goto ERREND;
		}
		else if(0==dataPtr[0])
		{
			if(9==dataPtr[1])
			{
				err=dataPtr[1];
				printf("Drive Not Ready.\n");
				goto ERREND;
			}
			if(8==dataPtr[1])
			{
				err=dataPtr[1];
				printf("Media Changed.\n");
				goto ERREND;
			}
		}
		else
		{
			err=~0;
			printf("Unknown Error.\n");
			goto ERREND;
		}
	}

ERREND:
	STI;
	return err;
}

unsigned char data[4096];

int main(int ac,char *av[])
{
	if(ac<2)
	{
		printf("Usage: Run386 ReadTOC out.bin\n");
		return 1;
	}
	else
	{
		unsigned int len=0;
		unsigned int err=ReadTOC(&len,data);

		if(0==err)
		{
			FILE *fp=fopen(av[1],"wb");
			if(NULL!=fp)
			{
				int i,j;

				fwrite(data,1,len,fp);
				fclose(fp);

				for(i=0; i<len; i+=8)
				{
					for(j=0; j<8; ++j)
					{
						printf(" %02x",data[i+j]);
					}
					printf("\n");
				}
			}
			else
			{
				printf("Cannot save to file.\n");
				return 0;
			}
		}
		else
		{
			printf("Error %08x\n",err);
		}
	}
	return 0;
}
