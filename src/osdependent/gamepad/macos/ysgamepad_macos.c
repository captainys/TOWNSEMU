#include "../ysgamepad.h"
#include "ysgamepad_macos_objc.h"



void YsGamePadInitialize(void)
{
}

void YsGamePadTerminate(void)
{
}

void YsGamePadWaitReady(void)
{
}

int YsGamePadGetNumDevices(void)
{
	return 0;
}

void YsGamePadRead(struct YsGamePadReading *reading,int gamePadId)
{
	gamePadId;
	YsGamePadClear(reading);
}
