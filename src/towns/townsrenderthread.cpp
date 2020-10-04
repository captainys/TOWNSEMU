#include "townsrenderthread.h"



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
			// Render image

			{
				std::unique_lock <std::mutex> statusLock(statusMutex);
				command=NO_COMMAND;
				imageReady=true;
			}
		}
	}
}

void TownsRenderingThread::StartThread(void)
{
	command=NO_COMMAND;
	std::thread thr(&TownsRenderingThread::ThreadFunc,this);
	workerThread.swap(thr);
}

void TownsRenderingThread::StartRendering(void)
{
	// Copy VRAM, LayerInfo

	{
		std::unique_lock <std::mutex> mainLock(mainMutex);
		command=RENDER;
	}
	cond.notify_one();
}

bool TownsRenderingThread::ImageReady(void)
{
	std::unique_lock <std::mutex> statusLock(statusMutex);
	return imageReady;
}

void TownsRenderingThread::EndThread(void)
{
	{
		std::unique_lock <std::mutex> mainLock(mainMutex);
		command=QUIT;
	}
	cond.notify_one();
	workerThread.join();
}
