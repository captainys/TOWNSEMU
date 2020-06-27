#include <iostream>
#include <windows.h>
#include "subproc.h"

// Based on:
// https://docs.microsoft.com/en-us/windows/win32/procthread/creating-a-child-process-with-redirected-input-and-output



#pragma comment(lib,"user32.lib")
#pragma comment(lib,"kernel32.lib")



Subprocess::Subprocess()
{
	ZeroMemory(&procInfo,sizeof(procInfo));
}
Subprocess::~Subprocess()
{
	CleanUp();
}
void Subprocess::CleanUp(void)
{
	if(nullptr!=subprocStdoutW)
	{
		CloseHandle(subprocStdoutW);
	}
	if(nullptr!=subprocStdoutR)
	{
		CloseHandle(subprocStdoutR);
	}
	if(nullptr!=subprocStdinW)
	{
		CloseHandle(subprocStdinW);
	}
	if(nullptr!=subprocStdinR)
	{
		CloseHandle(subprocStdinR);
	}

	if(nullptr!=procInfo.hProcess)
	{
		CloseHandle(procInfo.hProcess);
	}
	if(nullptr!=procInfo.hThread)
	{
		CloseHandle(procInfo.hThread);
	}

	subprocStdoutR=nullptr;
	subprocStdoutW=nullptr;
	subprocStdinR=nullptr;
	subprocStdinW=nullptr;
	ZeroMemory(&procInfo,sizeof(procInfo));

	errMsg="";
}
bool Subprocess::StartProc(const std::vector <std::string> &argv,bool usePipe)
{
	SECURITY_ATTRIBUTES secAttr;
	ZeroMemory(&secAttr,sizeof(secAttr));
	secAttr.nLength=sizeof(secAttr);
	secAttr.bInheritHandle=TRUE;
	secAttr.lpSecurityDescriptor=nullptr;

	if(0==CreatePipe(&subprocStdoutR,&subprocStdoutW,&secAttr,0) ||
	   0==SetHandleInformation(subprocStdoutR,HANDLE_FLAG_INHERIT,0) ||
	   0==CreatePipe(&subprocStdinR,&subprocStdinW,&secAttr,0) ||
	   0==SetHandleInformation(subprocStdinW,HANDLE_FLAG_INHERIT,0))
	{
		CleanUp();
		errMsg="Cannot create PIPE.";
		return false;
	}



	std::string cmdline;
	for(auto arg : argv)
	{
		if(0<cmdline.size())
		{
			cmdline.push_back(' ');
		}
		cmdline.push_back('\"');
		cmdline+=arg;
		cmdline.push_back('\"');
	}

	STARTUPINFO startInfo;
	ZeroMemory(&procInfo,sizeof(procInfo));
	ZeroMemory(&startInfo,sizeof(startInfo));
	startInfo.cb=sizeof(startInfo);
	startInfo.hStdError=subprocStdoutW;
	startInfo.hStdOutput=subprocStdoutW;
	startInfo.hStdInput=subprocStdinR;
	if(true==usePipe)
	{
		startInfo.dwFlags|=STARTF_USESTDHANDLES;
	}

	if(0==CreateProcess(nullptr,(char *)cmdline.c_str(),nullptr,nullptr,TRUE,0,nullptr,nullptr,&startInfo,&procInfo))
	{
		CleanUp();
		errMsg="Cannot create process.";
		return false;
	}

	// I don't quite understand the logic yet, but the MSDN example tells that these two handles need to be closed,
	// or the parent process cannot tell if the child process ended.
	CloseHandle(subprocStdoutW);
	subprocStdoutW=nullptr;
	CloseHandle(subprocStdinR);
	subprocStdinR=nullptr;

	errMsg="";
	return true;
}

bool Subprocess::SubprocEnded(void) const
{
	DWORD exitCode;
	return (nullptr==procInfo.hProcess || 0==GetExitCodeProcess(procInfo.hProcess,&exitCode) || STILL_ACTIVE!=exitCode);
}

bool Subprocess::SubprocRunning(void) const
{
	return !SubprocEnded();
}

void Subprocess::TerminateSubprocess(void)
{
	if(nullptr!=procInfo.hProcess)
	{
		TerminateProcess(procInfo.hProcess,0);
	}
	CleanUp();
}

bool Subprocess::Send(const std::string &str)
{
	if(nullptr!=subprocStdinW)
	{
		// Experiment indicates that it must not send '\0'.
		// Sub-process that receives '\0' will not get any more data.
		DWORD writeSize;
		return (0!=WriteFile(subprocStdinW,(char *)str.c_str(),str.size(),&writeSize,nullptr));
	}
	return false;
}
bool Subprocess::Receive(std::string &str)
{
	bool received=false;
	str="";
	if(nullptr!=subprocStdoutR)
	{
		const int buflen=256;
		for(;;)
		{
			DWORD nAvailable=0;
			if(0==PeekNamedPipe(subprocStdoutR,nullptr,0,nullptr,&nAvailable,nullptr) || 0==nAvailable)
			{
				break;
			}

			DWORD nReceived=0;
			char readBuf[buflen+1]={0};
			if(0!=ReadFile(subprocStdoutR,readBuf,buflen,&nReceived,nullptr) && 0<nReceived)
			{
				readBuf[nReceived]=0;  // Be absolutely sure.
				str+=readBuf;
				received=true;
			}
			else
			{
				break;
			}
		}
	}
	return received;
}
