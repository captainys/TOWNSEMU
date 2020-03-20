/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */


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



void TownsEventLog::LogMouseStart(long long int townsTime)
{
	events.push_back(Event());
	events.back().t=std::chrono::system_clock::now();
	events.back().townsTime=townsTime;
	events.back().eventType=EVT_TBIOS_MOS_START;
}
void TownsEventLog::LogMouseEnd(long long int townsTime)
{
	events.push_back(Event());
	events.back().t=std::chrono::system_clock::now();
	events.back().townsTime=townsTime;
	events.back().eventType=EVT_TBIOS_MOS_END;
}
void TownsEventLog::LogLeftButtonDown(long long int townsTime,int mx,int my)
{
	events.push_back(Event());
	events.back().t=std::chrono::system_clock::now();
	events.back().townsTime=townsTime;
	events.back().eventType=EVT_LBUTTONDOWN;
	events.back().mos[0]=mx;
	events.back().mos[1]=my;
}
void TownsEventLog::LogLeftButtonUp(long long int townsTime,int mx,int my)
{
	events.push_back(Event());
	events.back().t=std::chrono::system_clock::now();
	events.back().townsTime=townsTime;
	events.back().eventType=EVT_LBUTTONUP;
	events.back().mos[0]=mx;
	events.back().mos[1]=my;
}
void TownsEventLog::LogRightButtonDown(long long int townsTime,int mx,int my)
{
	events.push_back(Event());
	events.back().t=std::chrono::system_clock::now();
	events.back().townsTime=townsTime;
	events.back().eventType=EVT_RBUTTONDOWN;
	events.back().mos[0]=mx;
	events.back().mos[1]=my;
}
void TownsEventLog::LogRightButtonUp(long long int townsTime,int mx,int my)
{
	events.push_back(Event());
	events.back().t=std::chrono::system_clock::now();
	events.back().townsTime=townsTime;
	events.back().eventType=EVT_RBUTTONDOWN;
	events.back().mos[0]=mx;
	events.back().mos[1]=my;
}
void TownsEventLog::LogFileOpen(long long int townsTime,std::string fName)
{
	events.push_back(Event());
	events.back().t=std::chrono::system_clock::now();
	events.back().townsTime=townsTime;
	events.back().eventType=EVT_FILE_OPEN;
	events.back().fName=fName;
}
void TownsEventLog::LogFileExec(long long int townsTime,std::string fName)
{
	events.push_back(Event());
	events.back().t=std::chrono::system_clock::now();
	events.back().townsTime=townsTime;
	events.back().eventType=EVT_FILE_EXEC;
	events.back().fName=fName;
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
