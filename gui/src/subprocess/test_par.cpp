#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include "subproc.h"



int main(void)
{
	Subprocess subproc;

	std::vector <std::string> argv;
	argv.push_back("./test_chd");
#ifdef _WIN32
	argv.back()+=".exe";
#endif

	if(true!=subproc.StartProc(argv))
	{
		std::cout << "Cannot start a child process." << std::endl;
		return 1;
	}

	subproc.Send("ABC");
	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	std::string abc;
	subproc.Receive(abc);
	std::cout << "RECV:" << abc << std::endl;

	subproc.Send("UVW");
	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	std::string uvw;
	subproc.Receive(uvw);
	std::cout << "RECV:" <<  uvw << std::endl;

	subproc.Send("qqq");

	subproc.TerminateSubprocess();
	std::cout << "End" << std::endl;

	return 0;
}
