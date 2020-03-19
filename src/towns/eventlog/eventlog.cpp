/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */


#include "eventlog.h"



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
void TownsEventLog::Begin(void)
{
	CleanUp();
	t0=std::chrono::system_clock::now();
}



void TownsEventLog::LogMouseStart(long long int townsTime)
{
}
void TownsEventLog::LogMouseEnd(long long int townsTime)
{
}
void TownsEventLog::LogLeftButtonDown(long long int townsTime,int mx,int my)
{
}
void TownsEventLog::LogLeftButtonUp(long long int townsTime,int mx,int my)
{
}
void TownsEventLog::LogRightButtonDown(long long int townsTime,int mx,int my)
{
}
void TownsEventLog::LogRightButtonUp(long long int townsTime,int mx,int my)
{
}
void TownsEventLog::LogFileOpen(long long int townsTime,std::string fName)
{
}
void TownsEventLog::LogFileExec(long long int townsTime,std::string fName)
{
}
