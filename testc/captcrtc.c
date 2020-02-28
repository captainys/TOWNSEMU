#include "egb.h"
#include "io.h"
#include "vmif.h"

static char EGB_work[EgbWorkSize];

void SetScreenMode(int m1,int m2);

int main(void)
{
	EGB_init(EGB_work,EgbWorkSize);

	SetScreenMode(1,1);

	SetScreenMode(2,2);

	SetScreenMode(3,3);
	SetScreenMode(3,5);
	SetScreenMode(3,10);

	SetScreenMode(4,4);
	SetScreenMode(4,6);

	SetScreenMode(5,5);
	SetScreenMode(5,3);
	SetScreenMode(5,10);

	SetScreenMode(6,4);
	SetScreenMode(6,6);

	SetScreenMode(7,7);
	SetScreenMode(7,9);

	SetScreenMode(8,8);
	SetScreenMode(8,11);

	SetScreenMode(9,7);
	SetScreenMode(9,9);

	SetScreenMode(10,3);
	SetScreenMode(10,5);
	SetScreenMode(10,10);

	SetScreenMode(11,8);
	SetScreenMode(11,11);

	SetScreenMode(12,-1);
	SetScreenMode(13,-1);
	SetScreenMode(14,-1);
	SetScreenMode(15,-1);
	SetScreenMode(16,-1);
	SetScreenMode(17,-1);
	SetScreenMode(18,-1);

	return 0;
}

void SetScreenMode(int m1,int m2)
{
	EGB_resolution(EGB_work,0,m1);
	if(0<=m2)
	{
		EGB_resolution(EGB_work,1,m2);
	}

	IOWriteByte(TOWNSIO_VM_HOST_IF_DATA,m1);
	IOWriteByte(TOWNSIO_VM_HOST_IF_DATA,m2);
	IOWriteByte(TOWNSIO_VM_HOST_IF_CMD_STATUS,TOWNS_VMIF_CMD_CAPTURE_CRTC);
}
