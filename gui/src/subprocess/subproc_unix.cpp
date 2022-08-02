#include <signal.h>
#include <sys/wait.h>
#include <poll.h>
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

bool Subprocess::StartProc(const std::vector <std::string> &argv,bool usePipe)
{
	if(true==usePipe)
	{
		if(-1==pipe(parToChd) || -1==pipe(chdToPar))
		{
			printf("Failed to create pipes.\n");
			return false;
		}

		// parToChd[0]  Reading end of parent to child.  Must be open in child, closed in parent.
		// parToChd[1]  Writing end of parent to child.  Must be open in parent, closed in child.
		// chdToPar[0]  Reading end of child to parent.  Must be open in parent, closed in child.
		// chdToPar[1]  Writing end of child to parent.  Must be open in child, closed in parent.

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
			return true;
		}
		else
		{
			// Child process
			close(parToChd[1]);
			close(chdToPar[0]);

			dup2(parToChd[0],STDIN_FILENO);
			dup2(chdToPar[1],STDOUT_FILENO);
			dup2(chdToPar[1],STDERR_FILENO);
			close(parToChd[0]);
			close(chdToPar[1]);

			std::vector <char *> execargv;
			for(auto &arg : argv)
			{
				execargv.push_back((char *)arg.c_str());
			}
			execargv.push_back(nullptr);
			execv(argv[0].c_str(),execargv.data());
			std::cout << "Sub process ended." << std::endl;
			return false;
		}
	}
	else
	{
		procId=fork();
		if(procId<0)
		{
			printf("Fork failed.\n");
			return false;
		}
		else if(0<procId)
		{
		}
		else
		{
			// Child process
			std::vector <char *> execargv;
			for(auto &arg : argv)
			{
				execargv.push_back((char *)arg.c_str());
			}
			execargv.push_back(nullptr);
			execv(argv[0].c_str(),execargv.data());
			std::cout << "Sub process ended." << std::endl;
			return false;
		}
	}
	return true;
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
	procId=-1;
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
	return true;
}

bool Subprocess::Receive(std::string &str)
{
	bool received=false;

	str="";
	for(;;)
	{
		struct pollfd pfd;
		pfd.fd=chdToPar[0];
		pfd.events=POLLIN;
		pfd.revents=0;
		if(poll(&pfd,1,1)>=1)
		{
			const int buflen=256;
			char readBuf[buflen+1];
			auto s=read(chdToPar[0],readBuf,buflen);
			if(0<s)
			{
				readBuf[s]=0; // Just in case.
				str+=readBuf;
				received=true;
			}
			else
			{
				break;
			}
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
