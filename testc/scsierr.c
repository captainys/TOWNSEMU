#include <stdio.h>
#include <stdlib.h>

/*
This program is to test if SCSI controller of FM TOWNS has time-out after SEL bit is set.
The program sampled 2 seconds to see if the status byte changed.

The conclusion was no.  Tested on real FM TOWNS II MX.  Also, after this test program, 
TOWNS cannot access SCSI hard drive.

Therefore, once SCSI controller is set to selection phase, it needs to be set to the command phase,
or the SCSI controller is unresponsive.

Implication is Windows 3.1 behavior on INT 93H AX=93B0H must not write C6H to I/O 0C32H.

If I wait until BUSY and then cleared SEL bit, the status transitioned:

    08H->18H->9AH

    08H  BUSY
    18H  Data, BUSY
    9AH  REQ, Data, BUSY, INT

It doesn't flip PERR flag.
*/

extern void TEST_SCSI_ERR(int *numStatus,unsigned char status[1024*1024],unsigned int time[1024*1024]);

int main(void)
{
	int i,numStatus;

	unsigned char *status;
	unsigned int *time;
	
	status=malloc(sizeof(unsigned char)*1024*1024);
	time=malloc(sizeof(unsigned int)*1024*1024);

	printf("Testing\n");
	TEST_SCSI_ERR(&numStatus,status,time);
	printf("%d\n",numStatus);
	for(i=0; i<numStatus; ++i)
	{
		printf("%02xH %d\n",status[i],time[i]);
	}
	return 0;
}
