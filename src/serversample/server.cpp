#include <thread>
#include <mutex>
#include <string>
#include <vector>
#include <chrono>
#include <iostream>
#include "yssocket.h"


class Input
{
public:
	std::mutex mutex;
	std::vector <std::string> textBuf;

	void InputLoop(void);
};

void Input::InputLoop(void)
{
	for(;;)
	{
		std::string str;
		std::getline(std::cin,str);

		mutex.lock();
		textBuf.push_back(str);
		mutex.unlock();

		if("quit"==str || "QUIT"==str || "exit"==str || "EXIT"==str)
		{
			break;
		}
	}
}

class DumbServer : public YsSocketServer
{
public:
	DumbServer();
	virtual YSRESULT ReceivedFrom(int clientId,YSSIZE_T nBytes,unsigned char dat[]) override;
};

DumbServer::DumbServer() : YsSocketServer(12345,1) // Port 12345, Max 1 client.
{
}

/* virtual */ YSRESULT DumbServer::ReceivedFrom(int clientId,YSSIZE_T nBytes,unsigned char datIn[])
{
	char *dat=(char *)datIn;

	for(int i=0; i<nBytes; ++i)
	{
		unsigned int x=datIn[i];
		std::cout << x << " ";
	}
	std::cout << std::endl;

	bool lastCharWasCtrl=false;
	for(int i=0; i<nBytes; ++i)
	{
		if(0x0D==dat[i] || 0x0A==dat[i] || 0==dat[i])
		{
			if(true!=lastCharWasCtrl)
			{
				lastCharWasCtrl=true;
				std::cout << std::endl;
			}
		}
		else
		{
			lastCharWasCtrl=false;
			std::cout << dat[i];
		}
	}
	return YSOK;
}

int main(void)
{
	std::cout << "Type QUIT to close." << std::endl;

	Input input;
	std::thread t(&Input::InputLoop,&input);

	bool quit=false;
	DumbServer svr;
	svr.Start();
	while(true!=quit)
	{
		std::this_thread::sleep_for(std::chrono::microseconds(1));

		svr.CheckAndAcceptConnection();
		svr.CheckReceive();

		input.mutex.lock();
		for(auto str : input.textBuf)
		{
			if("quit"==str || "QUIT"==str || "exit"==str || "EXIT"==str)
			{
				quit=true;
				break;
			}
			str.push_back(0x0D);
			str.push_back(0x0A);
			for(int clientId=0; clientId<svr.GetNumClient(); ++clientId)
			{
				if(YSTRUE==svr.IsClientActive(clientId))
				{
					svr.Send(clientId,str.size(),(unsigned char *)str.data(),1);
				}
			}
		}
		input.textBuf.clear();
		input.mutex.unlock();
	}

	svr.Terminate();
	t.join();
	return 0;
}
