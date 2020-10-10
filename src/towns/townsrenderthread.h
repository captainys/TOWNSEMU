#ifndef TOWNSRENDERTHREAD_IS_INCLUDED
#define TOWNSRENDERTHREAD_IS_INCLUDED
/* { */

#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <cstdint>

#include "crtc.h"
#include "render.h"
#include "towns.h"
#include "townsdef.h"



class TownsRenderingThread
{
public:
	enum
	{
		NO_COMMAND,
		RENDER,
		QUIT
	};

	std::mutex mainMutex,statusMutex;
	std::thread workerThread;
	std::condition_variable cond;
	bool imageNeedsFlip=false;

	TownsRender *rendererPtr;
	unsigned char VRAMCopy[TOWNS_VRAM_SIZE];
	TownsCRTC::AnalogPalette paletteCopy;
	TownsCRTC::ChaseHQPalette chaseHQPaletteCopy;

	bool imageReady=false;
	unsigned int command=NO_COMMAND;

	enum
	{
		STATE_IDLE,
		STATE_RENDERING,
	};
	unsigned int state=STATE_IDLE;
	int64_t checkImageAfterThisTIme;

private:
	void ThreadFunc(void);

public:
	TownsRenderingThread();
	~TownsRenderingThread();

	void CheckRenderingTimer(FMTowns &towns,class TownsRender &render);
	void CheckImageReady(FMTowns &towns,class Outside_World &world);
	void WaitIdle(void);
};


/* } */
#endif
