#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FMCFRB.H"



enum
{
	MODE_2D,       // 320K
	MODE_2DD,      // 640/720K
	MODE_2HD_1232K,// 1232K
};

#define MODE1_MFM_MODE          0x00
#define MODE1_FM_MODE           0x80
#define MODE1_2HD               0x00
#define MODE1_2DD               0x10
#define MODE1_2D                0x20
#define MODE1_128_BYTE_PER_SEC  0x00
#define MODE1_256_BYTE_PER_SEC  0x01
#define MODE1_512_BYTE_PER_SEC  0x02
#define MODE1_1024_BYTE_PER_SEC 0x03

int CheckDiskMediaType(unsigned int driveStatus,unsigned int mediaType)
{
	printf("Media Status:%02x\n",driveStatus);

	printf("BIOS Detected:\n");
	switch(driveStatus&0x30)
	{
	case 0x10:
		printf("2D or 2DD disk\n");
		break;
	case 0x00:
		printf("2HD disk\n");
		break;
	default:
		printf("Unsupported disk type.\n");
		return -1;
	}

	if(0==(driveStatus&0x80))
	{
		printf("MFM (double density) mode.\n");
	}
	else
	{
		printf("FM (single density) mode.\n");
	}

	if((driveStatus&0x30)==0 && mediaType!=MODE_2HD_1232K)
	{
		return -1;
	}
	if((driveStatus&0x30)==0x10 && (mediaType!=MODE_2D && mediaType!=MODE_2DD))
	{
		return -1;
	}
	return 0;
}

void SetDriveMode(unsigned int drive,unsigned int mode)
{
	drive&=0x0F;
	drive|=0x20;


	DKB_restore(drive);
	unsigned int driveStatus;
	int biosError;
	if(0!=(biosError=DKB_rdstatus(drive,&driveStatus)))
	{
		fprintf(stderr,"Cannot get the drive status.\n");
		fprintf(stderr,"  Bios Error Code 0x%02x\n",biosError);
	}
	if(0!=CheckDiskMediaType(driveStatus,mode))
	{
		fprintf(stderr,"Wrong Disk Media.\n");
	}

	switch(mode)
	{
	case MODE_2D:
		DKB_setmode(drive,MODE1_MFM_MODE|MODE1_2D|MODE1_256_BYTE_PER_SEC,0x0210);
		break;
	case MODE_2DD:
		DKB_setmode(drive,MODE1_MFM_MODE|MODE1_2DD|MODE1_512_BYTE_PER_SEC,0x0208);
		break;
	case MODE_2HD_1232K:
		DKB_setmode(drive,MODE1_MFM_MODE|MODE1_2HD|MODE1_1024_BYTE_PER_SEC,0x0208);
		break;
	}
}

int main(void)
{
	printf("Set Drive A to 2HD mode.\n");
	printf(">");
	getchar();
	SetDriveMode(0,MODE_2HD_1232K);
	printf("Set Drive A to 2DD mode.\n");
	printf(">");
	getchar();
	SetDriveMode(0,MODE_2DD);
	printf("Set Drive A to 2D mode.\n");
	printf(">");
	getchar();
	SetDriveMode(0,MODE_2D);
	printf("Set Drive A to 2HD mode.\n");
	printf(">");
	getchar();
	SetDriveMode(0,MODE_2HD_1232K);
	return 0;
}
