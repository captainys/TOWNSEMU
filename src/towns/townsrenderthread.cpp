#include <iostream>
#include <thread>
#include <chrono>

#include <stdlib.h>
#include <string.h>

#include "townsrenderthread.h"
#include "render.h"
#include "outside_world.h"



TownsRenderingThread::TownsRenderingThread()
{
	command=NO_COMMAND;
	std::thread thr(&TownsRenderingThread::ThreadFunc,this);
	workerThread.swap(thr);
}
TownsRenderingThread::~TownsRenderingThread()
{
	{
		std::unique_lock <std::mutex> mainLock(mainMutex);
		command=QUIT;
	}
	cond.notify_one();
	workerThread.join();
	std::cout << "Ending Rendering Thread." << std::endl;
}

void TownsRenderingThread::ThreadFunc(void)
{
	std::unique_lock <std::mutex> mainLock(mainMutex);
	for(;;)
	{
		cond.wait(mainLock);
		if(QUIT==command)
		{
			break;
		}
		else if(RENDER==command)
		{
			rendererPtr->BuildImage(VRAMCopy,paletteCopy,chaseHQPaletteCopy);

			if(true==imageNeedsFlip)
			{
				rendererPtr->FlipUpsideDown();
			}

			{
				std::unique_lock <std::mutex> statusLock(statusMutex);
				command=NO_COMMAND;
				imageReady=true;
			}
		}
	}
}

void TownsRenderingThread::WaitIdle(void)
{
	// Being able to lock means the thread is either just started, or waiting on condition variable.
	std::unique_lock <std::mutex> mainLock(mainMutex);
}

void TownsRenderingThread::CheckRenderingTimer(FMTowns &towns,TownsRender &render)
{
	if(STATE_IDLE==state && 
	   towns.var.nextRenderingTime<=towns.state.townsTime && 
	   true!=towns.crtc.InVSYNC(towns.state.townsTime))
	{
		render.Prepare(towns.crtc);
		this->rendererPtr=&render;
		memcpy(this->VRAMCopy,towns.physMem.state.VRAM.data(),towns.crtc.GetEffectiveVRAMSize());
		this->paletteCopy=towns.crtc.GetPalette();
		this->chaseHQPaletteCopy=towns.crtc.chaseHQPalette;

		state=STATE_RENDERING;
		towns.var.nextRenderingTime=towns.state.townsTime+TOWNS_RENDERING_FREQUENCY;

		checkImageAfterThisTIme=towns.state.townsTime+3000000; // Give sub-thread some time.

		{
			std::unique_lock <std::mutex> mainLock(mainMutex);
			command=RENDER;
			imageReady=false;
		}
		cond.notify_one();
	}
}

void TownsRenderingThread::CheckImageReady(FMTowns &towns,Outside_World &world)
{
	if(STATE_RENDERING==state && checkImageAfterThisTIme<towns.state.townsTime)
	{
		bool imageReady;
		{
			std::unique_lock <std::mutex> statusLock(statusMutex);
			imageReady=this->imageReady;
		}
		if(true==imageReady)
		{
			world.Render(rendererPtr->GetImage());
			world.UpdateStatusBitmap(towns);
			state=STATE_IDLE;
		}
	}
}
