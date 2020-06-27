#include "subproc.h"

Subprocess::Subprocess()
{
	procId=-1;
}

Subprocess::~Subprocess()
{
	CleanUp();
}

void Subprocess::CleanUp(void)
{
	// How can I force terminate the process?
}

bool Subprocess::StartProc(const std::vector <std::string> &argv,bool usePipe=true)
{
	procId=fork();
	if(procId<0)
	{
		printf("Fork failed.\n");
		return false;
	}
	else if(0<procId)
	{
		// Parent process
		close(parToChd[0]);
		close(chdToPar[1]);
	}
	else
	{
		// Child process
		close(parToChd[1]);
		close(chdToPar[0]);

		dup2(parToChd[0],STDIN_FILENO);
		dup2(chdToPar[1],STDOUT_FILENO);
		dup2(chdToPar[2],STDERR_FILENO);
		close(parToChd[0]);
		close(chdToPar[2]);
		####system("./child");
	}
}

bool Subprocess::SubprocRunning(void) const
{
	int s;
	pid_t res=waitpid(procId,&s,WNOHANG);
	if(0==res)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Subprocess::SubprocEnded(void) const
{
	return !SubprocRunning();
}

void Subprocess::TerminateSubprocess(void)
{
	if(0<procId)
	{
		kill(procId,SIGKILL);
	}
	prodId=-1;
}


bool Subprocess::Send(const std::string &str)
{
	if('\n'!=str.back())
	{
		std::string copy=str;
		copy.push_back('\n');
		write(parToChd[1],copy.data(),copy.size());
	}
	else
	{
		write(parToChd[1],str.data(),str.size());
	}
}

bool Subprocess::Receive(std::string &str)
{
	bool received=false;
	std::string str;

	for(;;)
	{
		struct pollfd pfd;
		pfd.fd=listeningSocket;
		pfd.events=POLLIN;
		pfd.revents=0;
		if(poll(&pfd,1,1)>=1)
		{
			const int buflen=256;
			char readBuf[buflen+1];
			auto s=read(chdToPar[0],readBuf,buflen);
			readBuf[s]=0; // Just in case.
			str+=readBuf;
			received=true;
		}
		else
		{
			break;
		}
	}

	if(0<str.size() && '\n'==str.back())
	{
		str.pop_back();
	}

	return received;
}
