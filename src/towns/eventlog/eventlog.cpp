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
#include "townsdef.h"



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
void TownsEventLog::AddEvent(Event e)
{
	if(events.size()==0)
	{
		e.t=std::chrono::milliseconds(0);
	}
	else
	{
		e.t+=events.back().t;
	}
	events.push_back(e);
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
	t0=std::chrono::system_clock::now();
	playbackPtr=events.begin();
}

void TownsEventLog::StopPlayBack(void)
{
	mode=MODE_NONE;
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
	case EVT_KEYCODE:
		return "KEYCODE";
	case EVT_PAD0_A_DOWN:
		return "PAD0ADOWN";
	case EVT_PAD0_A_UP:
		return "PAD0AUP";
	case EVT_KEYPRESS:
		return "KEYPRESS";
	case EVT_KEYRELEASE:
		return "KEYRELEASE";
	case EVT_REPEAT:
		return "REPEAT";

	case EVT_PAD0_B_DOWN:
		return "PAD0BDOWN";
	case EVT_PAD0_B_UP:
		return "PAD0BUP";
	case EVT_PAD0_SEL_DOWN:
		return "PAD0SELDOWN";
	case EVT_PAD0_SEL_UP:
		return "PAD0SELUP";
	case EVT_PAD0_RUN_DOWN:
		return "PAD0RUNDOWN";
	case EVT_PAD0_RUN_UP:
		return "PAD0RUNUP";
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
		events.back().t=std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()-t0);
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
		events.back().t=std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()-t0);
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
		events.back().t=std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()-t0);
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
		events.back().t=std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()-t0);
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
		events.back().t=std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()-t0);
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
		events.back().t=std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()-t0);
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
		events.back().t=std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()-t0);
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
		events.back().t=std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()-t0);
		events.back().townsTime=townsTime;
		events.back().eventType=eventType;
		events.back().fName=fName;
	}
}
void TownsEventLog::LogKeyCode(long long int townsTime,unsigned char keyCode1,unsigned char keyCode2)
{
	const int eventType=EVT_KEYCODE;
	if(MODE_RECORDING==mode)
	{
		events.push_back(Event());
		events.back().t=std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()-t0);
		events.back().townsTime=townsTime;
		events.back().eventType=eventType;
		events.back().keyCode[0]=keyCode1;
		events.back().keyCode[1]=keyCode2;
	}
}

std::vector <std::string> TownsEventLog::GetText(void) const
{
	std::vector <std::string> text;
	if(0<events.size())
	{
		auto prev=events.front();
		for(auto e : events)
		{
			text.push_back("EVT ");
			text.back()+=EventTypeToString(e.eventType);

			text.push_back("DELTAT ");
			text.back()+=cpputil::Uitoa((int)(std::chrono::duration_cast <std::chrono::milliseconds>(e.t-prev.t).count()));
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
				text.back().push_back('\"');
				text.back()+=e.fName;
				text.back().push_back('\"');
				break;
			case EVT_KEYCODE:
				text.push_back("KEYCODE ");
				text.back()+=cpputil::Ubtox(e.keyCode[0]);
				text.back()+=" ";
				text.back()+=cpputil::Ubtox(e.keyCode[1]);
				break;
			case EVT_KEYPRESS:
			case EVT_KEYRELEASE:
				text.push_back("KEY ");
				text.back()+=TownsKeyCodeToStr(e.keyCode[0]);
				break;
			case EVT_REPEAT:
				if(REP_INFINITY!=e.repCountMax)
				{
					text.push_back("REPCOUNT ");
					text.back()+=cpputil::Uitoa(e.repCountMax);
				}
				break;
			}

			prev=e;
		}
	}
	return text;
}

void TownsEventLog::MakeRepeat(void)
{
	if(0<events.size() && EVT_REPEAT!=events.back().eventType)
	{
		Event rept=events.back();
		rept.t+=std::chrono::milliseconds(10);
		rept.townsTime+=PER_SECOND/100;
		rept.eventType=EVT_REPEAT;
		events.push_back(rept);
	}
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
						std::chrono::milliseconds  t;
						if(0==events.size())
						{
							t=std::chrono::milliseconds(0);
						}
						else
						{
							t=events.back().t;
						}
						events.push_back(Event());
						events.back().eventType=strToEventType[argv[1]];
						events.back().t=t; // Tentative.
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
						events.back().t=std::chrono::milliseconds(cpputil::Atoi(argv[1].c_str()));
					}
					else
					{
						std::cout << "Too few arguments" << std::endl;
						std::cout << "  " << line << std::endl;
						return false;
					}
				}
				else if("DELTAT"==argv[0])
				{
					if(2<=argv.size())
					{
						if(1<events.size())
						{
							auto iter=events.end();
							--iter;
							--iter;
							auto prev=*iter;
							events.back().t=prev.t+std::chrono::milliseconds(cpputil::Atoi(argv[1].c_str()));
						}
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
				else if("KEYCODE"==argv[0])
				{
					if(3<=argv.size())
					{
						events.back().keyCode[0]=cpputil::Xtoi(argv[1].c_str());
						events.back().keyCode[1]=cpputil::Xtoi(argv[2].c_str());
					}
					else
					{
						std::cout << "Too few arguments" << std::endl;
						std::cout << "  " << line << std::endl;
						return false;
					}
				}
				else if("KEY"==argv[0])
				{
					if(2<=argv.size())
					{
						events.back().keyCode[0]=TownsStrToKeyCode(argv[1]);
					}
				}
				else if("REPCOUNT"==argv[0])
				{
					if(2<=argv.size())
					{
						events.back().repCountMax=cpputil::Atoi(argv[1].c_str());
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
			std::chrono::time_point <std::chrono::system_clock> baseT;
			std::chrono::milliseconds dt;
			if(events.begin()==playbackPtr)
			{
				dt=playbackPtr->t;
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
			auto tPassed=std::chrono::duration_cast<std::chrono::milliseconds>(now-baseT);

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
					towns.ControlMouseInVMCoord(playbackPtr->mos.x(),playbackPtr->mos.y(),towns.state.tbiosVersion);
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
					towns.ControlMouseInVMCoord(playbackPtr->mos.x(),playbackPtr->mos.y(),towns.state.tbiosVersion);
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
					towns.ControlMouseInVMCoord(playbackPtr->mos.x(),playbackPtr->mos.y(),towns.state.tbiosVersion);
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
					towns.ControlMouseInVMCoord(playbackPtr->mos.x(),playbackPtr->mos.y(),towns.state.tbiosVersion);
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

			case EVT_PAD0_A_DOWN:
				if(dt<=tPassed)
				{
					towns.gameport.state.ports[0].button[0]=true;
					playbackPtr->tPlayed=now;
					++playbackPtr;
				}
				break;
			case EVT_PAD0_A_UP:
				if(dt<=tPassed)
				{
					towns.gameport.state.ports[0].button[0]=false;
					playbackPtr->tPlayed=now;
					++playbackPtr;
				}
				break;

			case EVT_PAD0_B_DOWN:
				if(dt<=tPassed)
				{
					towns.gameport.state.ports[0].button[1]=true;
					playbackPtr->tPlayed=now;
					++playbackPtr;
				}
				break;
			case EVT_PAD0_B_UP:
				if(dt<=tPassed)
				{
					towns.gameport.state.ports[0].button[1]=false;
					playbackPtr->tPlayed=now;
					++playbackPtr;
				}
				break;
			case EVT_PAD0_RUN_DOWN:
				if(dt<=tPassed)
				{
					towns.gameport.state.ports[0].button[2]=true;
					playbackPtr->tPlayed=now;
					++playbackPtr;
				}
				break;
			case EVT_PAD0_RUN_UP:
				if(dt<=tPassed)
				{
					towns.gameport.state.ports[0].button[2]=false;
					playbackPtr->tPlayed=now;
					++playbackPtr;
				}
				break;
			case EVT_PAD0_SEL_DOWN:
				if(dt<=tPassed)
				{
					towns.gameport.state.ports[0].button[3]=true;
					playbackPtr->tPlayed=now;
					++playbackPtr;
				}
				break;
			case EVT_PAD0_SEL_UP:
				if(dt<=tPassed)
				{
					towns.gameport.state.ports[0].button[3]=false;
					playbackPtr->tPlayed=now;
					++playbackPtr;
				}
				break;


			case EVT_FILE_OPEN:  // INT 21H AH=3DH
			case EVT_FILE_EXEC:  // INT 21H AH=4BH
				break;

			case EVT_KEYCODE:
				if(dt<=tPassed)
				{
					towns.keyboard.PushFifo(playbackPtr->keyCode[0],playbackPtr->keyCode[1]);
					playbackPtr->tPlayed=now;
					++playbackPtr;
				}
				break;

			case EVT_KEYPRESS:
				if(dt<=tPassed)
				{
					unsigned char byteData=0;
					// byteData|=(0!=FsGetKeyState(FSKEY_CTRL) ? TOWNS_KEYFLAG_CTRL : 0);
					// byteData|=(0!=FsGetKeyState(FSKEY_SHIFT) ? TOWNS_KEYFLAG_SHIFT : 0);
					byteData|=TOWNS_KEYFLAG_JIS_PRESS;

					towns.keyboard.PushFifo(byteData,playbackPtr->keyCode[0]);
					playbackPtr->tPlayed=now;
					++playbackPtr;
				}
				break;
			case EVT_KEYRELEASE:
				if(dt<=tPassed)
				{
					unsigned char byteData=0;
					// byteData|=(0!=FsGetKeyState(FSKEY_CTRL) ? TOWNS_KEYFLAG_CTRL : 0);
					// byteData|=(0!=FsGetKeyState(FSKEY_SHIFT) ? TOWNS_KEYFLAG_SHIFT : 0);
					byteData|=TOWNS_KEYFLAG_JIS_RELEASE;

					towns.keyboard.PushFifo(byteData,playbackPtr->keyCode[0]);
					playbackPtr->tPlayed=now;
					++playbackPtr;
				}
				break;
			case EVT_REPEAT:
				if(dt<=tPassed)
				{
					if(REP_INFINITY!=playbackPtr->repCountMax)
					{
						++(playbackPtr->repCount);
						if(playbackPtr->repCountMax<=playbackPtr->repCount)
						{
							playbackPtr->repCount=0;
							++playbackPtr;
							break;
						}
					}
					playbackPtr=events.begin();
					t0=now;
				}
				break;
			}
		}
	}
}
