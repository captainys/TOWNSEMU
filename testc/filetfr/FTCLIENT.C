#include <stdio.h>
#include <string.h>

#include "../vmif.h"


/* Usage:
(1) In the host, use VM2HOST or HOST2VM commands, or use -VM2HOST or -HOST2VM options to schedule file transfer(s)
(2) In the VM, start this program.
(3) The program will terminate when the tranfer is over.
*/

extern void VMFILE_READ_BATCH(unsigned char buf[128*1024]);
extern void VMFILE_WRITE_BATCH(unsigned char buf[128*1024]);

#define SPRITE_RAM_SIZE 128*1024
#define MAX_FILE_NAME 260

unsigned char buf[SPRITE_RAM_SIZE];
char fName[MAX_FILE_NAME];

// HOST_TO_VM
// 1st Batch
//   [0]     TOWNS_VMIF_TFR_HOST_TO_VM
//   [1..3]  Unused(Zero)
//   [4..7]  File Size
//   [8..63] Unused(Zero)
//   [64..]  File name in VM
// 2nd Batch and on
//   File Contents
void HOST_TO_VM(void)
{
	printf("HOST TO VM\n");

	unsigned int sz=0;
	sz=buf[4]|(buf[5]<<8)|(buf[6]<<16)|buf[7]<<24;
	printf("File Size:%u\n",sz);

	strncpy(fName,(char *)buf+64,MAX_FILE_NAME);
	printf("File Name:%s\n",fName);

	FILE *ofp=fopen(fName,"wb");

	while(0<sz)
	{
		VMFILE_READ_BATCH(buf);

		unsigned int batchSize;
		if(SPRITE_RAM_SIZE<sz)
		{
			batchSize=SPRITE_RAM_SIZE;
		}
		else
		{
			batchSize=sz;
		}

		if(NULL!=ofp)
		{
			fwrite(buf,1,batchSize,ofp);
		}

		sz-=batchSize;
	}

	if(NULL!=ofp)
	{
		fclose(ofp);
	}
}

// VM_TO_HOST
// Command format
//   [0]     TOWNS_VMIF_TFR_HOST_TO_VM
//   [1..3]  Unused(Zero)
//   [4..7]  Unused(Zero)
//   [8..63] Unused(Zero)
//   [64..]  File name in VM
// Reply to host
//   [0]     0:Continue Transmission    1:Last Transmission    2:Error
//   [1..3]  Unused(Zero)
//   [4..7]  Size in the buffer
//   [8..]   Data
// Last transmission would be spriteRAM[0]==1, size_in_buffer=0.
void VM_TO_HOST(void)
{
	printf("VM TO HOST\n");

	unsigned int sz=0;
	sz=buf[4]|(buf[5]<<8)|(buf[6]<<16)|buf[7]<<24;
	printf("File Size:%u\n",sz);

	strncpy(fName,(char *)buf+64,MAX_FILE_NAME);
	printf("File Name:%s\n",fName);

	FILE *ifp=fopen(fName,"rb");
	if(NULL==ifp)
	{
		printf("Error reading file.\n");
		buf[0]=2;
		buf[1]=0;
		buf[2]=0;
		buf[3]=0;
		buf[4]=0;
		buf[5]=0;
		buf[6]=0;
		buf[7]=0;
		VMFILE_WRITE_BATCH(buf);
		return;
	}

	for(;;)
	{
		unsigned int readSize=fread(buf+8,1,SPRITE_RAM_SIZE-8,ifp);

		buf[1]=0;
		buf[2]=0;
		buf[3]=0;
		*((unsigned int *)(buf+4))=readSize;
		if(0==readSize)
		{
			buf[0]=1;
			VMFILE_WRITE_BATCH(buf);
			break;
		}
		else
		{
			buf[0]=0;
			VMFILE_WRITE_BATCH(buf);
		}
	}

	if(NULL!=ifp)
	{
		fclose(ifp);
	}
}

int main(void)
{
	printf("FM Towns Emulator TSUGARU\n");
	printf("VM-HOST File Transfer Client\n");

	int terminate=0;
	while(0==terminate)
	{
		VMFILE_READ_BATCH(buf);
		switch(buf[0])
		{
		case TOWNS_VMIF_TFR_END:
			terminate=1;
			break;
		default:
			terminate=1;
			printf("Unknown Command: %02x\n",buf[0]);
			break;
		case TOWNS_VMIF_TFR_HOST_TO_VM:
			HOST_TO_VM();
			break;
		case TOWNS_VMIF_TFR_VM_TO_HOST:
			VM_TO_HOST();
			break;
		}
	}
	return 0;
}

