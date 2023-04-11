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



std::vector <std::string> miscutil::MakeMemDump(const i486DXCommon &cpu,const Memory &mem,i486DXCommon::FarPointer ptr,unsigned int length,bool shiftJIS)
{
	// Make it 32-bit addressing.  I don't think there is any point rounding the address for memory dump.
	const int addressSize=32;
	std::vector <std::string> text;

	auto lineStart=(ptr.OFFSET&~0x0F);
	auto lineEnd=((ptr.OFFSET+length-1)&~0x0F);

	if((ptr.SEG&0xffff0000)==i486DXCommon::FarPointer::LINEAR_ADDR)
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
					str+=" "+cpputil::Ubtox(cpu.DebugFetchByteByLinearAddress(mem,addr));
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
					auto byte=cpu.DebugFetchByteByLinearAddress(mem,addr);
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
	else if((ptr.SEG&0xffff0000)==i486DXCommon::FarPointer::PHYS_ADDR)
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
		i486DXCommon::SegmentRegister seg;
		std::string segTxt;
		if((ptr.SEG&0xffff0000)==i486DXCommon::FarPointer::SEG_REGISTER)
		{
			seg=cpu.state.GetSegmentRegister(ptr.SEG&0xffff);
			if((ptr.SEG&0xff)<i486DXCommon::REG_TOTAL_NUMBER_OF_REGISTERS)
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
			cpu.DebugLoadSegmentRegister(seg,ptr.SEG,mem,cpu.IsInRealMode());
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
					str+=" "+cpputil::Ubtox(cpu.DebugFetchByte(addressSize,seg,addr,mem));
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

std::vector <std::string> miscutil::MakeMemDump2(const i486DXCommon &cpu,const Memory &mem,i486DXCommon::FarPointer ptr,int wid,int hei,int skip,bool includeASCII,bool shiftJIS)
{
	std::vector <std::string> text;

	for(int y=0; y<hei; ++y)
	{
		unsigned int addr0=ptr.OFFSET+y*wid;
		std::string addrTxt;

		if((ptr.SEG&0xffff0000)==i486DXCommon::FarPointer::LINEAR_ADDR)
		{
			addrTxt=cpputil::Uitox(addr0);
		}
		else if((ptr.SEG&0xffff0000)==i486DXCommon::FarPointer::PHYS_ADDR)
		{
			addrTxt=cpputil::Uitox(addr0);
		}
		else
		{
			i486DXCommon::SegmentRegister seg;
			std::string segTxt;
			if((ptr.SEG&0xffff0000)==i486DXCommon::FarPointer::SEG_REGISTER)
			{
				seg=cpu.state.GetSegmentRegister(ptr.SEG&0xffff);
				if((ptr.SEG&0xff)<i486DXCommon::REG_TOTAL_NUMBER_OF_REGISTERS)
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
				cpu.DebugLoadSegmentRegister(seg,ptr.SEG,mem,cpu.IsInRealMode());
				segTxt=cpputil::Ustox(ptr.SEG);
			}

			addrTxt=segTxt;
			addrTxt.push_back(':');
			addrTxt+=cpputil::Uitox(addr0);
		}

		std::string str,ascii;
		for(int x=0; x<wid; x+=skip)
		{
			unsigned int offset=y*wid+x;

			auto byte=GetByte(cpu,mem,ptr,offset);
			str+=" "+cpputil::Ubtox(byte);

			if(true==includeASCII)
			{
				if(byte<' ' || (true!=shiftJIS && 0x80<=byte))
				{
					ascii.push_back(' ');
				}
				else
				{
					ascii.push_back(byte);
				}
			}
		}
		if(0!=ascii.size())
		{
			str.push_back('|');
			str+=ascii;
			if(true==shiftJIS)
			{
				// Make sure to break first char of shift-JIS
				ascii.push_back(' ');
				ascii.push_back(' ');
			}
		}
		text.push_back(addrTxt+str);
	}

	return text;
}

unsigned char miscutil::GetByte(const i486DXCommon &cpu,const Memory &mem,i486DXCommon::FarPointer ptr,uint32_t offset)
{
	if((ptr.SEG&0xffff0000)==i486DXCommon::FarPointer::LINEAR_ADDR)
	{
		return cpu.DebugFetchByteByLinearAddress(mem,ptr.OFFSET+offset);
	}
	else if((ptr.SEG&0xffff0000)==i486DXCommon::FarPointer::PHYS_ADDR)
	{
		return mem.FetchByte(ptr.OFFSET+offset);
	}
	else
	{
		i486DXCommon::SegmentRegister seg;
		std::string segTxt;
		if((ptr.SEG&0xffff0000)==i486DXCommon::FarPointer::SEG_REGISTER)
		{
			seg=cpu.state.GetSegmentRegister(ptr.SEG&0xffff);
		}
		else
		{
			cpu.DebugLoadSegmentRegister(seg,ptr.SEG,mem,cpu.IsInRealMode());
		}
		return cpu.DebugFetchByte(32,seg,ptr.OFFSET+offset,mem);
	}
}

