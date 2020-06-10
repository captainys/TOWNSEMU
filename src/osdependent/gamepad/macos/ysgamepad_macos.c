#include "../ysgamepad.h"
#include "ysgamepad_macos_objc.h"



void YsGamePadInitialize(void)
{
	YsJoyReader_MacOS_InitializeController();
}

void YsGamePadTerminate(void)
{
}

void YsGamePadWaitReady(void)
{
}

int YsGamePadGetNumDevices(void)
{
	return YsJoyReader_MacOS_NumControllers();
}

void YsGamePadRead(struct YsGamePadReading *reading,int gamePadId)
{
	YsJoyReader_MacOS_ReadController(reading,gamePadId);
}
