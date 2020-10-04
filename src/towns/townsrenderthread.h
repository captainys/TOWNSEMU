#ifndef TOWNSRENDERTHREAD_IS_INCLUDED
#define TOWNSRENDERTHREAD_IS_INCLUDED
/* { */

#include <thread>
#include <mutex>
#include <condition_variable>



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

	bool imageReady=false;
	unsigned int command=NO_COMMAND;

private:
	void ThreadFunc(void);

public:
	void StartThread(void);

	void StartRendering(void);
	bool ImageReady(void);

	void EndThread(void);
};


/* } */
#endif
