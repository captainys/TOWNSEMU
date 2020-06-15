/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */


#include <iostream>
#include <fstream>
#include <map>

#include "eventlog.h"
#include "cpputil.h"



TownsEventLog::TownsEventLog()
{
}
TownsEventLog::~TownsEventLog()
{
}
void TownsEventLog::CleanUp(void)
{
	events.clear();
}
void TownsEventLog::BeginRecording(long long int townsTime)
{
	CleanUp();
	t0=std::chrono::system_clock::now();
	townsTime0=townsTime;
	mode=MODE_RECORDING;
}
void TownsEventLog::BeginPlayback(void)
{
	mode=MODE_PLAYBACK;
	playbackPtr=events.begin();
}
/* static */ std::string TownsEventLog::EventTypeToString(int evtType)
{
	switch(evtType)
	{
	case EVT_LBUTTONDOWN:
		return "LBUTTONDOWN";
	case EVT_LBUTTONUP:
		return "LBUTTONUP";
	case EVT_RBUTTONDOWN:
		return "RBUTTONDOWN";
	case EVT_RBUTTONUP:
		return "RBUTTONUP";
	case EVT_TBIOS_MOS_START:
		return "TBIOS_MOS_START";
	case EVT_TBIOS_MOS_END:
		return "TBIOS_MOS_END";
	case EVT_FILE_OPEN:  // INT 21H AH=3DH
		return "FILE_OPEN";
	case EVT_FILE_EXEC:  // INT 21H AH=4BH
		return "FILE_EXEC";
	};
	return "?";
}


void TownsEventLog::SkipPlaybackFileEvent(void)
{
	while(events.end()!=playbackPtr &&
	      (EVT_FILE_OPEN==playbackPtr->eventType ||
	       EVT_FILE_EXEC==playbackPtr->eventType))
	{
		std::cout << "Skipped " << EventTypeToString(playbackPtr->eventType) << ":" << playbackPtr->fName << std::endl;
		if(events.begin()==playbackPtr)
		{
			playbackPtr->tPlayed=std::chrono::system_clock::now();
		}
		else
		{
			auto prevPtr=playbackPtr;
			--prevPtr;
			auto dt=(playbackPtr->t-prevPtr->t);
			playbackPtr->tPlayed=prevPtr->tPlayed+dt;
		}
		++playbackPtr;
	}
}

void TownsEventLog::LogMouseStart(long long int townsTime)
{
	const int eventType=EVT_TBIOS_MOS_START;
	if(MODE_RECORDING==mode)
	{
		events.push_back(Event());
		events.back().t=std::chrono::system_clock::now();
		events.back().townsTime=townsTime;
		events.back().eventType=eventType;
	}
	else if(MODE_PLAYBACK==mode)
	{
		received_MOS_start=true;
	}
}
void TownsEventLog::LogMouseEnd(long long int townsTime)
{
	const int eventType=EVT_TBIOS_MOS_END;
	if(MODE_RECORDING==mode)
	{
		events.push_back(Event());
		events.back().t=std::chrono::system_clock::now();
		events.back().townsTime=townsTime;
		events.back().eventType=eventType;
	}
	else if(MODE_PLAYBACK==mode)
	{
		received_MOS_end=true;
	}
}
void TownsEventLog::LogLeftButtonDown(long long int townsTime,int mx,int my)
{
	const int eventType=EVT_LBUTTONDOWN;
	if(MODE_RECORDING==mode)
	{
		events.push_back(Event());
		events.back().t=std::chrono::system_clock::now();
		events.back().townsTime=townsTime;
		events.back().eventType=eventType;
		events.back().mos[0]=mx;
		events.back().mos[1]=my;
	}
}
void TownsEventLog::LogLeftButtonUp(long long int townsTime,int mx,int my)
{
	const int eventType=EVT_LBUTTONUP;
	if(MODE_RECORDING==mode)
	{
		events.push_back(Event());
		events.back().t=std::chrono::system_clock::now();
		events.back().townsTime=townsTime;
		events.back().eventType=eventType;
		events.back().mos[0]=mx;
		events.back().mos[1]=my;
	}
}
void TownsEventLog::LogRightButtonDown(long long int townsTime,int mx,int my)
{
	const int eventType=EVT_RBUTTONDOWN;
	if(MODE_RECORDING==mode)
	{
		events.push_back(Event());
		events.back().t=std::chrono::system_clock::now();
		events.back().townsTime=townsTime;
		events.back().eventType=eventType;
		events.back().mos[0]=mx;
		events.back().mos[1]=my;
	}
}
void TownsEventLog::LogRightButtonUp(long long int townsTime,int mx,int my)
{
	const int eventType=EVT_RBUTTONDOWN;
	if(MODE_RECORDING==mode)
	{
		events.push_back(Event());
		events.back().t=std::chrono::system_clock::now();
		events.back().townsTime=townsTime;
		events.back().eventType=eventType;
		events.back().mos[0]=mx;
		events.back().mos[1]=my;
	}
}
void TownsEventLog::LogFileOpen(long long int townsTime,std::string fName)
{
	const int eventType=EVT_FILE_OPEN;
	if(MODE_RECORDING==mode)
	{
		events.push_back(Event());
		events.back().t=std::chrono::system_clock::now();
		events.back().townsTime=townsTime;
		events.back().eventType=eventType;
		events.back().fName=fName;
	}
}
void TownsEventLog::LogFileExec(long long int townsTime,std::string fName)
{
	const int eventType=EVT_FILE_EXEC;
	if(MODE_RECORDING==mode)
	{
		events.push_back(Event());
		events.back().t=std::chrono::system_clock::now();
		events.back().townsTime=townsTime;
		events.back().eventType=eventType;
		events.back().fName=fName;
	}
}

std::vector <std::string> TownsEventLog::GetText(void) const
{
	std::vector <std::string> text;
	for(auto e : events)
	{
		text.push_back("EVT ");
		text.back()+=EventTypeToString(e.eventType);

		text.push_back("T ");
		text.back()+=cpputil::Uitoa((int)(std::chrono::duration_cast <std::chrono::milliseconds>(e.t-t0).count()));
		text.back()+="ms";

		text.push_back("TWT ");
		text.back()+=cpputil::Uitoa((int)((e.townsTime-townsTime0)/1000000));
		text.back()+="ms";

		switch(e.eventType)
		{
		case EVT_LBUTTONDOWN:
		case EVT_LBUTTONUP:
		case EVT_RBUTTONDOWN:
		case EVT_RBUTTONUP:
			text.push_back("MOS ");
			text.back()+=cpputil::Uitoa(e.mos.x());
			text.back()+=" ";
			text.back()+=cpputil::Uitoa(e.mos.y());
			break;
		case EVT_TBIOS_MOS_START:
		case EVT_TBIOS_MOS_END:
			break;
		case EVT_FILE_OPEN:  // INT 21H AH=3DH
		case EVT_FILE_EXEC:  // INT 21H AH=4BH
			text.push_back("FNAME ");
			text.back()+=e.fName;
			break;
		}
	}
	return text;
}

bool TownsEventLog::SaveEventLog(std::string fName) const
{
	std::ofstream ofp(fName);
	if(ofp.is_open())
	{
		for(auto str : GetText())
		{
			ofp << str << std::endl;
		}
		ofp.close();
		return true;
	}
	return false;
}
bool TownsEventLog::LoadEventLog(std::string fName)
{
	std::ifstream ifp(fName);
	if(ifp.is_open())
	{
		std::map <std::string,int> strToEventType;
		for(int i=0; i<NUMBER_OF_EVENT_TYPES; ++i)
		{
			strToEventType[EventTypeToString(i)]=i;
		}

		CleanUp();
		t0=std::chrono::system_clock::now();

		while(true!=ifp.eof())
		{
			std::string line;
			std::getline(ifp,line);

			std::vector <std::string> argv=cpputil::Parser(line.c_str());
			if(0<argv.size())
			{
				if("EVT"==argv[0])
				{
					if(2<=argv.size())
					{
						events.push_back(Event());
						events.back().eventType=strToEventType[argv[1]];
					}
					else
					{
						std::cout << "Too few arguments" << std::endl;
						std::cout << "  " << line << std::endl;
						return false;
					}
				}
				else if("T"==argv[0])
				{
					if(2<=argv.size())
					{
						events.back().t=t0+std::chrono::milliseconds(cpputil::Atoi(argv[1].c_str()));
					}
					else
					{
						std::cout << "Too few arguments" << std::endl;
						std::cout << "  " << line << std::endl;
						return false;
					}
				}
				else if("TWT"==argv[0])
				{
					if(2<=argv.size())
					{
						events.back().townsTime=cpputil::Atoi(argv[1].c_str());
						events.back().townsTime*=1000000;
					}
					else
					{
						std::cout << "Too few arguments" << std::endl;
						std::cout << "  " << line << std::endl;
						return false;
					}
				}
				else if("FNAME"==argv[0])
				{
					if(2<=argv.size())
					{
						events.back().fName=argv[1];
					}
					else
					{
						std::cout << "Too few arguments" << std::endl;
						std::cout << "  " << line << std::endl;
						return false;
					}
				}
				else if("MOS"==argv[0])
				{
					if(3<=argv.size())
					{
						events.back().mos[0]=cpputil::Atoi(argv[1].c_str());
						events.back().mos[1]=cpputil::Atoi(argv[2].c_str());
					}
					else
					{
						std::cout << "Too few arguments" << std::endl;
						std::cout << "  " << line << std::endl;
						return false;
					}
				}
			}
		}

		ifp.close();
		return true;
	}
	return false;
}



#include "towns.h"



void TownsEventLog::Playback(class FMTowns &towns)
{
	if(MODE_PLAYBACK==mode)
	{
		if(true==dontWaitFileEventInPlayback)
		{
			SkipPlaybackFileEvent();
		}

		if(events.end()==playbackPtr)
		{
			mode=MODE_NONE;
		}
		else
		{
			decltype(playbackPtr->tPlayed) baseT;
			decltype(playbackPtr->t-t0) dt;
			if(events.begin()==playbackPtr)
			{
				dt=playbackPtr->t-t0;
				baseT=t0;
			}
			else
			{
				auto prev=playbackPtr;
				--prev;
				dt=playbackPtr->t-prev->t;
				baseT=prev->tPlayed;
			}
			auto now=std::chrono::system_clock::now();
			auto tPassed=now-baseT;

			switch(playbackPtr->eventType)
			{
			case EVT_TBIOS_MOS_START:
				if(true==received_MOS_start)
				{
					received_MOS_start=false;
					playbackPtr->tPlayed=now;
					++playbackPtr;
				}
				break;
			case EVT_TBIOS_MOS_END:
				if(true==received_MOS_end)
				{
					received_MOS_end=false;
					playbackPtr->tPlayed=now;
					++playbackPtr;
				}
				break;
			case EVT_LBUTTONDOWN:
				if(dt/2<tPassed)
				{
					towns.ControlMouse(playbackPtr->mos.x(),playbackPtr->mos.y(),towns.state.tbiosVersion);
					int mx,my;
					towns.GetMouseCoordinate(mx,my,towns.state.tbiosVersion);
					if(mx==playbackPtr->mos.x() && my==playbackPtr->mos.y() && dt<=tPassed)
					{
						std::cout << "EVT_LBUTTONDOWN" << std::endl;
						towns.SetMouseButtonState(true,false);
						playbackPtr->tPlayed=now;
						++playbackPtr;
					}
				}
				break;
			case EVT_LBUTTONUP:
				if(dt/2<tPassed)
				{
					towns.ControlMouse(playbackPtr->mos.x(),playbackPtr->mos.y(),towns.state.tbiosVersion);
					int mx,my;
					towns.GetMouseCoordinate(mx,my,towns.state.tbiosVersion);
					if(mx==playbackPtr->mos.x() && my==playbackPtr->mos.y() && dt<=tPassed)
					{
						std::cout << "EVT_LBUTTONUP" << std::endl;
						towns.SetMouseButtonState(false,false);
						playbackPtr->tPlayed=now;
						++playbackPtr;
					}
				}
				break;
			case EVT_RBUTTONDOWN:
				if(dt/2<tPassed)
				{
					towns.ControlMouse(playbackPtr->mos.x(),playbackPtr->mos.y(),towns.state.tbiosVersion);
					int mx,my;
					towns.GetMouseCoordinate(mx,my,towns.state.tbiosVersion);
					if(mx==playbackPtr->mos.x() && my==playbackPtr->mos.y() && dt<=tPassed)
					{
						std::cout << "EVT_RBUTTONDOWN" << std::endl;
						towns.SetMouseButtonState(false,true);
						playbackPtr->tPlayed=now;
						++playbackPtr;
					}
				}
				break;
			case EVT_RBUTTONUP:
				if(dt/2<tPassed)
				{
					towns.ControlMouse(playbackPtr->mos.x(),playbackPtr->mos.y(),towns.state.tbiosVersion);
					int mx,my;
					towns.GetMouseCoordinate(mx,my,towns.state.tbiosVersion);
					if(mx==playbackPtr->mos.x() && my==playbackPtr->mos.y() && dt<=tPassed)
					{
						std::cout << "EVT_RBUTTONUP" << std::endl;
						towns.SetMouseButtonState(false,false);
						playbackPtr->tPlayed=now;
						++playbackPtr;
					}
				}
				break;

			case EVT_FILE_OPEN:  // INT 21H AH=3DH
			case EVT_FILE_EXEC:  // INT 21H AH=4BH
				break;
			}
		}
	}
}
