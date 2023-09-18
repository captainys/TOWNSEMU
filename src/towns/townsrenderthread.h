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
#include "outside_world.h"



class TownsRenderingThread
{
public:
	enum
	{
		NO_COMMAND,
		RENDER,
		QUIT
	};

	enum
	{
		RENDER_TIMING_OUTSIDE_VSYNC,
		RENDER_TIMING_FIRST1MS_OF_VERTICAL,
	};

	std::mutex mainMutex,statusMutex;
	std::thread workerThread;
	std::condition_variable cond;
	bool imageNeedsFlip=false;

	unsigned int renderTiming=RENDER_TIMING_OUTSIDE_VSYNC;

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

	void CheckRenderingTimer(FMTownsCommon &towns,class Outside_World::WindowInterface &window);

	void CheckRenderingTimer(FMTownsCommon &towns,class TownsRender &render);
	void CheckImageReady(FMTownsCommon &towns,class Outside_World &world,Outside_World::WindowInterface &windowInterface);
	void WaitIdle(void);

	void DiscardRunningRenderingTask(void);
};


/* } */
#endif
