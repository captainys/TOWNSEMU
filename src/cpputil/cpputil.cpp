/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include <fstream>

#include "cpputil.h"



std::string cpputil::MakeFullPathName(std::string dirName,std::string fName)
{
	return dirName+"/"+fName;
}

bool cpputil::FileExists(std::string fName)
{
	std::ifstream fp(fName,std::ifstream::binary);
	if(true==fp.is_open())
	{
		fp.close();
		return true;
	}
	return false;
}

std::vector <unsigned char> cpputil::ReadBinaryFile(std::string fName)
{
	std::vector <unsigned char> dat;
	std::ifstream fp(fName,std::ifstream::binary);
	if(true==fp.is_open())
	{
		fp.seekg(0,fp.end);
		auto length=fp.tellg();
		fp.seekg(0,fp.beg);

		dat.resize(length);
		fp.read((char *)dat.data(),length);

		fp.close();
	}
	return dat;
}

std::vector <unsigned char> cpputil::ReadBinaryFile(std::string fName,long long int start,long long int length)
{
	std::vector <unsigned char> dat;
	std::ifstream fp(fName,std::ifstream::binary);
	if(true==fp.is_open())
	{
		fp.seekg(start,fp.beg);

		dat.resize(length);
		fp.read((char *)dat.data(),length);

		fp.close();
	}
	return dat;
}

bool cpputil::WriteBinaryFile(const std::string &fName,unsigned long long length,const unsigned char dat[])
{
	std::ofstream fp(fName,std::ofstream::binary);
	if(true==fp.is_open())
	{
		fp.write((char *)dat,length);
		fp.close();
		return true;
	}
	return false;
}

bool cpputil::WriteBinaryFile(const std::string &fName,unsigned long long int start,unsigned long long length,const unsigned char dat[])
{
	std::fstream fp(fName,std::ios::binary|std::ios::in|std::ios::out);
	if(true==fp.is_open())
	{
		fp.seekg(start,fp.beg);
		fp.write((char *)dat,length);
		fp.close();
		return true;
	}
	return false;
}

std::vector <std::string> cpputil::Parser(const char str[])
{
	const int STATE_OUTSIDE=0,STATE_WORD=1,STATE_DOUBLEQUOTE=2;
	const char DQ='\"';
	int state=STATE_OUTSIDE;
	std::string curStr;
	std::vector <std::string> argv;
	for(int i=0; 0!=str[i]; ++i)
	{
		if(STATE_OUTSIDE==state)
		{
			if(DQ==str[i])
			{
				curStr="";
				state=STATE_DOUBLEQUOTE;
			}
			else if(' '!=str[i] && '\t'!=str[i])
			{
				curStr.push_back(str[i]);
				state=STATE_WORD;
			}

		}
		else if(STATE_WORD==state)
		{
			if(' '==str[i] || '\t'==str[i] || 0==str[i+1])
			{
				if(' '!=str[i] && '\t'!=str[i])
				{
					curStr.push_back(str[i]);
				}
				argv.push_back((std::string &&)curStr);
				curStr="";
				state=STATE_OUTSIDE;
			}
			else
			{
				curStr.push_back(str[i]);
			}
		}
		else if(STATE_DOUBLEQUOTE==state)
		{
			if(DQ==str[i])
			{
				argv.push_back((std::string&&)curStr);
				curStr="";
				state=STATE_OUTSIDE;
			}
			else
			{
				curStr.push_back(str[i]);
			}
		}
	}

	if(0<curStr.size())
	{
		argv.push_back((std::string &&)curStr);
	}

	return argv;
}

const std::string &cpputil::Capitalize(std::string &s)
{
	for(auto &c : s)
	{
		if('a'<=c && c<='z')
		{
			c=c+'A'-'a';
		}
	}
	return s;
}

std::string cpputil::Capitalize(const char s[])
{
	return std::string(s);
}

std::string cpputil::GetExtension(const char fName[])
{
	if(nullptr==fName)
	{
		return "";
	}

	int lastDot=-1;
	for(int i=0; 0!=fName[i]; ++i)
	{
		if('.'==fName[i])
		{
			lastDot=i;
		}
	}

	if(0<=lastDot)
	{
		std::string ext(fName+lastDot);
		return ext;
	}
	return "";
}

std::string cpputil::ChangeExtension(const char orgFName[],const char newExt[])
{
	int lastDot=-1;
	for(int i=0; 0!=orgFName[i]; ++i)
	{
		if('.'==orgFName[i])
		{
			lastDot=i;
		}
	}

	std::string newFn;

	if(0>lastDot)
	{
		newFn=orgFName;
	}
	else
	{
		for(int i=0; i<lastDot; ++i)
		{
			newFn.push_back(orgFName[i]);
		}
	}

	for(int i=0; 0!=newExt[i]; ++i)
	{
		newFn.push_back(newExt[i]);
	}

	return newFn;
}

std::string cpputil::RemoveExtension(const char orgFName[])
{
	int lastDot=-1;
	for(int i=0; 0!=orgFName[i]; ++i)
	{
		if('.'==orgFName[i])
		{
			lastDot=i;
		}
	}

	std::string newFn;

	if(0>lastDot)
	{
		newFn=orgFName;
	}
	else
	{
		for(int i=0; i<lastDot; ++i)
		{
			newFn.push_back(orgFName[i]);
		}
	}

	return newFn;
}

std::string cpputil::GetBaseName(const char path[])
{
	if(nullptr==path)
	{
		return "";
	}

	int lastSlash=0;
	for(int i=0; path[i]!=0; ++i)
	{
		if(':'==path[i] || '/'==path[i] || '\\'==path[i])
		{
			lastSlash=i;
		}
	}

	std::string basename;
	for(int i=lastSlash+1; 0!=path[i]; ++i)
	{
		basename.push_back(path[i]);
	}

	return basename;
}

bool cpputil::StrStartsWith(const std::string &str,const char ptn[])
{
	int i;
	for(i=0; i<str.size() && 0!=ptn[i]; ++i)
	{
		if(str[i]!=ptn[i])
		{
			return false;
		}
	}
	if(0==ptn[i])
	{
		return true;
	}
	return false;
}

const char *cpputil::StrSkip(const char str[],const char ptn[])
{
	for(int i=0; 0!=str[i]; ++i)
	{
		if(str[i]==ptn[0])
		{
			int j;
			for(j=0; 0!=str[i+j] && 0!=ptn[j]; ++j)
			{
				if(str[i+j]!=ptn[j])
				{
					goto NEXTI;
				}
			}
			if(0==ptn[j])
			{
				return str+i+j;
			}
		}
	NEXTI:
		;
	}
	return nullptr;
}

const char *cpputil::StrSkipSpace(const char str[])
{
	int i;
	for(i=0; 0!=str[i]; ++i)
	{
		if(' '!=str[i] && '\t'!=str[i])
		{
			return str+i;
		}
	}
	return str+i;
}

const char *cpputil::StrSkipNonSpace(const char str[])
{
	int i;
	for(i=0; 0!=str[i]; ++i)
	{
		if(' '==str[i] || '\t'==str[i])
		{
			return str+i;
		}
	}
	return str+i;
}

const char *cpputil::BoolToStr(bool b)
{
	if(b==true)
	{
		return "TRUE";
	}
	else
	{
		return "FALSE";
	}
}

int cpputil::Xtoi(const char str[])
{
	int n=0;
	while(0!=str[0])
	{
		if('0'<=*str && *str<='9')
		{
			n*=16;
			n+=(*str-'0');
		}
		else if('a'<=*str && *str<='f')
		{
			n*=16;
			n+=(10+*str-'a');
		}
		else if('A'<=*str && *str<='F')
		{
			n*=16;
			n+=(10+*str-'A');
		}
		else
		{
			break;
		}
		++str;
	}
	return n;
}

int cpputil::Atoi(const char str[])
{
	if(nullptr==str)
	{
		return 0;
	}

	int sign=1;
	if('-'==str[0])
	{
		sign=-1;
		++str;
	}

	int n=0;
	if(('0'==str[0] && ('x'==str[1] || 'X'==str[1])) ||
	   ('&'==str[0] && ('h'==str[1] || 'H'==str[1])))
	{
		n=Xtoi(str+2);
	}
	else if('$'==str[0])
	{
		n=Xtoi(str+1);
	}
	else
	{
		bool hexaDecimal=false;
		for(int i=0; str[i]!=0; ++i)
		{
			if(('h'==str[i] || 'H'==str[i]) &&
			   (str[i+1]<'a' || 'z'<str[i+1]) &&
			   (str[i+1]<'A' || 'Z'<str[i+1]) &&
			   (str[i+1]<'0' || '9'<str[i+1]))
			{
				hexaDecimal=true;
				break;
			}
		}
		if(true==hexaDecimal)
		{
			n=Xtoi(str);
		}
		else
		{
			while(0!=*str)
			{
				if('0'<=*str && *str<='9')
				{
					n*=10;
					n+=(*str-'0');
				}
				else
				{
					break;
				}
				++str;
			}
		}
	}
	return n*sign;
}

inline char cpputil::FourBitToX(int i)
{
	if(0<=i && i<=9)
	{
		return '0'+i;
	}
	else
	{
		return 'A'+i-10;
	}
}

void cpputil::SeparatePathFile(std::string &path,std::string &file,const std::string &fName)
{
	long long int lastSeparator=-1;
	for(long long int i=0; 0!=fName[i]; ++i)
	{
		if('/'==fName[i] || ':'==fName[i] || '\\'==fName[i])
		{
			lastSeparator=i;
		}
	}

	path=fName;
	path.resize(lastSeparator+1);
	file=fName.data()+lastSeparator+1;
}

long long int cpputil::FileSize(const std::string &fName)
{
	std::vector <unsigned char> dat;
	std::ifstream fp(fName,std::ifstream::binary);
	if(true==fp.is_open())
	{
		fp.seekg(0,fp.end);
		auto length=fp.tellg();
		fp.close();
		return length;
	}
	return 0;
}

char BoolToChar(bool f)
{
	return (true==f ? '1' : '0');
}

std::string cpputil::Uitox(unsigned int i)
{
	std::string s;
	s.resize(8);
	for(int n=7; 0<=n; --n)
	{
		s[n]=FourBitToX(i&0x0F);
		i>>=4;
	}
	return s;
}

std::string cpputil::Ustox(unsigned short i)
{
	std::string s;
	s.resize(4);
	for(int n=3; 0<=n; --n)
	{
		s[n]=FourBitToX(i&0x0F);
		i>>=4;
	}
	return s;
}

std::string cpputil::Ubtox(unsigned char i)
{
	std::string s;
	s.resize(2);
	for(int n=1; 0<=n; --n)
	{
		s[n]=FourBitToX(i&0x0F);
		i>>=4;
	}
	return s;
}

std::string cpputil::Uitoa(unsigned int i)
{
	if(0==i)
	{
		return "0";
	}

	std::string s;
	while(0<i)
	{
		s.push_back('0'+(i%10));
		i/=10;
	}
	for(int n=0; n<s.size()/2; ++n)
	{
		std::swap(s[n],s[s.size()-1-n]);
	}
	return s;
}

std::string cpputil::Uitoa(unsigned int i,unsigned minLen)
{
	std::string s=Itoa(i);
	if(s.size()<minLen)
	{
		std::string space;
		for(auto i=s.size(); i<minLen; ++i)
		{
			space.push_back(' ');
		}
		return space+s;
	}
	return s;
}

std::string cpputil::Itox(int i)
{
	if(0<=i)
	{
		return Uitox(i);
	}
	else
	{
		std::string sgnd="-";
		return sgnd+Uitox(-i);
	}
}

std::string cpputil::Stox(short i)
{
	if(0<=i)
	{
		return Ustox(i);
	}
	else
	{
		std::string sgnd="-";
		return sgnd+Ustox(-i);
	}
}

std::string cpputil::Btox(char i)
{
	if(0<=i)
	{
		return Ubtox(i);
	}
	else
	{
		std::string sgnd="-";
		return sgnd+Ubtox(-i);
	}
}

std::string cpputil::Itoa(int i)
{
	if(0<=i)
	{
		return Uitoa(i);
	}
	else
	{
		std::string sgnd="-";
		return sgnd+Uitoa(-i);
	}
}

std::string cpputil::Itoa(int i,unsigned minLen)
{
	std::string s=Itoa(i);
	if(s.size()<minLen)
	{
		std::string space;
		for(auto i=s.size(); i<minLen; ++i)
		{
			space.push_back(' ');
		}
		return space+s;
	}
	return s;
}

void cpputil::ExtendString(std::string &str,int minimumLength)
{
	while(str.size()<minimumLength)
	{
		str.push_back(' ');
	}
}

char cpputil::BoolToChar(bool flag)
{
	return ((true==flag) ? '1' : '0');
}

const char *cpputil::BoolToNumberStr(bool flag)
{
	return ((true==flag) ? "1" : "0");
}

std::vector <std::string> cpputil::MakeDump(unsigned int printAddr,long long int length,const unsigned char data[])
{
	// Make it 32-bit addressing.  I don't think there is any point rounding the address for memory dump.
	const int addressSize=32;
	std::vector <std::string> text;

	auto lineStart=(printAddr&~0x0F);
	auto lineEnd=((printAddr+length-1)&~0x0F);

	for(auto addr0=lineStart; addr0<=lineEnd; addr0+=16)
	{
		std::string str;

		// printAddr corresponds to data[0]

		str+=cpputil::Uitox(addr0);
		for(int i=0; i<16; ++i)
		{
			auto addr=addr0+i;
			if(addr<printAddr || printAddr+length<=addr)
			{
				str+="   ";
			}
			else
			{
				str+=" "+cpputil::Ubtox(data[addr-printAddr]);
			}
		}
		str.push_back('|');
		for(int i=0; i<16; ++i)
		{
			auto addr=addr0+i;
			if(addr<printAddr || printAddr+length<=addr)
			{
				str.push_back(' ');
			}
			else
			{
				auto byte=data[addr-printAddr];
				if(byte<' ' || 0x80<=byte)
				{
					str.push_back(' ');
				}
				else
				{
					str.push_back(byte);
				}
			}
		}
		text.push_back((std::string &&)str);
	}

	return text;
}
