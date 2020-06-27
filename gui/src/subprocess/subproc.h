#ifndef SUBPROC_IS_INCLUDED
#define SUBPROC_IS_INCLUDED
/* { */

#include <string>
#include <vector>

#ifdef _WIN32
	#include <windows.h>
#endif

class Subprocess
{
private:
#ifdef _WIN32
	HANDLE subprocStdoutR=nullptr;
	HANDLE subprocStdoutW=nullptr;
	HANDLE subprocStdinR=nullptr;
	HANDLE subprocStdinW=nullptr;

	PROCESS_INFORMATION procInfo;
#else
#endif

public:
	std::string errMsg;

	Subprocess();
	~Subprocess();
	void CleanUp(void);
	bool StartProc(const std::vector <std::string> &argv,bool usePipe=true);
	bool SubprocRunning(void) const;
	bool SubprocEnded(void) const;
	void TerminateSubprocess(void);

	bool Send(const std::string &str);
	bool Receive(std::string &str);
};


/* } */
#endif
