#include <stdio.h>
#include <stdlib.h>
#include <fmcfrb.h>



#define DATABUF_BATCH_SIZE 16384
#define DATABUF_NUM_BATCH 16
#define DATABUF_SIZE (DATABUF_BATCH_SIZE*DATABUF_NUM_BATCH)
static char dataBuf[DATABUF_SIZE];

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

	unsigned int sectorsAtATime=DATABUF_BATCH_SIZE/sectorLength;
	unsigned int progressPtr=0;
	if(drvNumSectors<=startSector)
	{
		sectorCount=0;
	}
	else if(drvNumSectors<startSector+sectorCount)
	{
		sectorCount=drvNumSectors-startSector;
	}

	unsigned int dataBufFilled=0;
	while(progressPtr<sectorCount)
	{
		unsigned int readCount=sectorsAtATime;
		int actualRead;
		if(sectorCount<progressPtr+readCount)
		{
			readCount=sectorCount-progressPtr;
		}

		printf("Reading Sector %d (%d/%d)\r",startSector+progressPtr,progressPtr,sectorCount);
		unsigned int err=DKB_read2(devNum,startSector+progressPtr,readCount,dataBuf+dataBufFilled,&actualRead);
		dataBufFilled+=(readCount*sectorLength);
		if(0!=err)
		{
			printf("\nError %02x between Sector %d and %d\n",startSector+progressPtr,startSector+progressPtr+readCount);
		}
		else if(readCount!=actualRead)
		{
			printf("\nWarning %d sectors skipped between Sector %d and %d\n",readCount-actualRead,startSector+progressPtr,startSector+progressPtr+readCount);
		}

		if(DATABUF_SIZE<=dataBufFilled)
		{
			// Why not open once and keep adding?  I'm not sure if DKB_read2 interfares with DOS file operations.
			// Just to be safe.
			FILE *ofp=fopen(av[1],"ab");
			fwrite(dataBuf,1,dataBufFilled,ofp);
			fclose(ofp);
			dataBufFilled=0;
		}

		progressPtr+=sectorsAtATime;
	}
	if(0<dataBufFilled)
	{
		FILE *ofp=fopen(av[1],"ab");
		fwrite(dataBuf,1,dataBufFilled,ofp);
		fclose(ofp);
		dataBufFilled=0;
	}

	printf("Reading Sector %d (%d/%d)\n",startSector+sectorCount,sectorCount,sectorCount);
	printf("Done.\n");

	return 0;
}
