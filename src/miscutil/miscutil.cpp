/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include "miscutil.h"
#include "cpputil.h"



std::vector <std::string> miscutil::MakeMemDump(const i486DX &cpu,const Memory &mem,i486DX::FarPointer ptr,unsigned int length,bool shiftJIS)
{
	// Make it 32-bit addressing.  I don't think there is any point rounding the address for memory dump.
	const int addressSize=32;
	std::vector <std::string> text;

	auto lineStart=(ptr.OFFSET&~0x0F);
	auto lineEnd=((ptr.OFFSET+length-1)&~0x0F);

	if((ptr.SEG&0xffff0000)==i486DX::FarPointer::LINEAR_ADDR)
	{
		for(auto addr0=lineStart; addr0<=lineEnd; addr0+=16)
		{
			std::string str;

			str+=cpputil::Uitox(addr0);
			for(int i=0; i<16; ++i)
			{
				auto addr=addr0+i;
				if(addr<ptr.OFFSET || ptr.OFFSET+length<=addr)
				{
					str+="   ";
				}
				else
				{
					str+=" "+cpputil::Ubtox(cpu.FetchByteByLinearAddress(mem,addr));
				}
			}
			str.push_back('|');
			for(int i=0; i<16; ++i)
			{
				auto addr=addr0+i;
				if(addr<ptr.OFFSET || ptr.OFFSET+length<=addr)
				{
					str.push_back(' ');
				}
				else
				{
					auto byte=cpu.FetchByteByLinearAddress(mem,addr);
					if(byte<' ' || (true!=shiftJIS && 0x80<=byte))
					{
						str.push_back(' ');
					}
					else
					{
						str.push_back(byte);
					}
				}
			}
			if(true==shiftJIS)
			{
				// Make sure to break first char of shift-JIS
				str.push_back(' ');
				str.push_back(' ');
			}

			text.push_back((std::string &&)str);
		}
	}
	else if((ptr.SEG&0xffff0000)==i486DX::FarPointer::PHYS_ADDR)
	{
		for(auto addr0=lineStart; addr0<=lineEnd; addr0+=16)
		{
			std::string str;

			str+=cpputil::Uitox(addr0);
			for(int i=0; i<16; ++i)
			{
				auto addr=addr0+i;
				if(addr<ptr.OFFSET || ptr.OFFSET+length<=addr)
				{
					str+="   ";
				}
				else
				{
					str+=" "+cpputil::Ubtox(mem.FetchByte(addr));
				}
			}
			str.push_back('|');
			for(int i=0; i<16; ++i)
			{
				auto addr=addr0+i;
				if(addr<ptr.OFFSET || ptr.OFFSET+length<=addr)
				{
					str.push_back(' ');
				}
				else
				{
					auto byte=mem.FetchByte(addr);
					if(byte<' ' || (true!=shiftJIS && 0x80<=byte))
					{
						str.push_back(' ');
					}
					else
					{
						str.push_back(byte);
					}
				}
			}
			if(true==shiftJIS)
			{
				// Make sure to break first char of shift-JIS
				str.push_back(' ');
				str.push_back(' ');
			}

			text.push_back((std::string &&)str);
		}
	}
	else
	{
		i486DX::SegmentRegister seg;
		std::string segTxt;
		if((ptr.SEG&0xffff0000)==i486DX::FarPointer::SEG_REGISTER)
		{
			seg=cpu.state.GetSegmentRegister(ptr.SEG&0xffff);
			if((ptr.SEG&0xff)<i486DX::REG_TOTAL_NUMBER_OF_REGISTERS)
			{
				segTxt=cpu.RegToStr[ptr.SEG&0xff];
			}
			else
			{
				segTxt="????";
			}
		}
		else
		{
			cpu.LoadSegmentRegisterQuiet(seg,ptr.SEG,mem,cpu.IsInRealMode());
			segTxt=cpputil::Ustox(ptr.SEG);
		}
		for(auto addr0=lineStart; addr0<=lineEnd; addr0+=16)
		{
			std::string str;

			str=segTxt;
			str.push_back(':');
			str+=cpputil::Uitox(addr0);
			for(int i=0; i<16; ++i)
			{
				auto addr=addr0+i;
				if(addr<ptr.OFFSET || ptr.OFFSET+length<=addr)
				{
					str+="   ";
				}
				else
				{
					str+=" "+cpputil::Ubtox(cpu.FetchByte(addressSize,seg,addr,mem));
				}
			}
			str.push_back('|');
			for(int i=0; i<16; ++i)
			{
				auto addr=addr0+i;
				if(addr<ptr.OFFSET || ptr.OFFSET+length<=addr)
				{
					str.push_back(' ');
				}
				else
				{
					auto byte=cpu.DebugFetchByte(addressSize,seg,addr,mem);
					if(byte<' ' || (true!=shiftJIS && 0x80<=byte))
					{
						str.push_back(' ');
					}
					else
					{
						str.push_back(byte);
					}
				}
			}
			if(true==shiftJIS)
			{
				// Make sure to break first char of shift-JIS
				str.push_back(' ');
				str.push_back(' ');
			}

			text.push_back((std::string &&)str);
		}
	}

	return text;
}

