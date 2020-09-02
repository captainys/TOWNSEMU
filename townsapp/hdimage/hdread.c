#include <stdio.h>
#include <stdlib.h>
#include <fmcfrb.h>



#define DATABUF_LEN 16384
static char dataBuf[DATABUF_LEN];

int main(int ac,char *av[])
{
	if(ac<5)
	{
		printf("Usage: run386 -nocrt hdread output.bin SCSIID startSector sectorCount\n");
		return 1;
	}

	unsigned int scsiID=atoi(av[2]);
	unsigned int startSector=atoi(av[3]);
	unsigned int sectorCount=atoi(av[4]);

	unsigned int devNum=0xB0|(scsiID&0x07);

	unsigned int drvMode;
	long drvNumSectors;
	unsigned int drvErr=DKB_rdstatus2(devNum,&drvMode,&drvNumSectors);
	if(0!=drvErr)
	{
		printf("Hard Drive Error: %02x\n",drvErr);
		return 0;
	}

	unsigned int sectorLength=(128<<(drvMode&3));

	printf("Number of Sectors: %d\n",drvNumSectors);
	printf("Sector Length: %d\n",sectorLength);

	{
		FILE *ofp=fopen(av[1],"wb");
		if(NULL==ofp)
		{
			printf("Cannot open output file.\n");
			return 1;
		}
		fclose(ofp);
	}

	unsigned int sectorsAtATime=DATABUF_LEN/sectorLength;
	unsigned int progressPtr=0;
	if(drvNumSectors<=startSector)
	{
		sectorCount=0;
	}
	else if(drvNumSectors<startSector+sectorCount)
	{
		sectorCount=drvNumSectors-startSector;
	}

	while(progressPtr<sectorCount)
	{
		unsigned int readCount=sectorsAtATime;
		int sectorsLeft;
		if(sectorCount<progressPtr+readCount)
		{
			readCount=sectorCount-progressPtr;
		}

		printf("Reading Sector %d (%d/%d)\r",startSector+progressPtr,progressPtr,sectorCount);
		unsigned int err=DKB_read2(devNum,startSector+progressPtr,readCount,dataBuf,&sectorsLeft);
		if(0!=err)
		{
			printf("\nError %02x between Sector %d and %d\n",startSector+progressPtr,startSector+progressPtr+readCount);
		}

		{
			FILE *ofp=fopen(av[1],"ab");
			fwrite(dataBuf,1,readCount*sectorLength,ofp);
			fclose(ofp);
		}

		progressPtr+=sectorsAtATime;
	}
	printf("Reading Sector %d (%d/%d)\n",startSector+progressPtr,sectorCount,sectorCount);
	printf("Done.\n");

	return 0;
}
