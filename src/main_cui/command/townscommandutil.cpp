/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include <iostream>

#include "townscommandutil.h"
#include "townslineparser.h"
#include "cpputil.h"



i486DXCommon::FarPointer cmdutil::MakeFarPointer(const std::string &str,const i486DXCommon &cpu)
{
	i486DXCommon::FarPointer ptr;
	ptr.MakeFromString(str);

	// To allow description like EAX+EDI*2+10, discard offset part from MakeFromString and re-calculate.
	const char *offsetPtr=nullptr;
	for(int i=0; i<str.size(); ++i)
	{
		if(':'==str[i])
		{
			offsetPtr=str.c_str()+i+1;
			break;
		}
	}
	if(nullptr==offsetPtr)
	{
		offsetPtr=str.c_str();
	}
	TownsLineParserHexadecimal parser(&cpu);
	if(true==parser.Analyze(offsetPtr))
	{
		ptr.OFFSET=parser.Evaluate();
	}
	else
	{
		std::cout << "Error in offset description." << std::endl;
		ptr.OFFSET=0;
	}

	return ptr;
}

void cmdutil::PrintPrompt(void)
{
	std::cout << ">";
}
