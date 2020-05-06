/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#ifndef CPPUTIL_IS_INCLUDED
#define CPPUTIL_IS_INCLUDED
/* { */

#include <vector>
#include <string>

namespace cpputil
{

std::string MakeFullPathName(std::string dirName,std::string fName);
bool FileExists(std::string fName);
std::vector <unsigned char> ReadBinaryFile(std::string fName);
std::vector <unsigned char> ReadBinaryFile(std::string fName,long long int start,long long int length);
bool WriteBinaryFile(const std::string &fName,unsigned long long length,const unsigned char dat[]);
bool WriteBinaryFile(const std::string &fName,unsigned long long int start,unsigned long long length,const unsigned char dat[]);
std::vector <std::string> Parser(const char str[]);
const std::string &Capitalize(std::string &s);
std::string Capitalize(const char s[]);
std::string GetExtension(const char fName[]);
std::string ChangeExtension(const char orgFName[],const char newExt[]);
std::string RemoveExtension(const char orgFName[]);
std::string GetBaseName(const char path[]);
bool StrStartsWith(const std::string &str,const char ptn[]);
const char *StrSkip(const char str[],const char ptn[]);
const char *StrSkipSpace(const char str[]);
const char *StrSkipNonSpace(const char str[]);
const char *BoolToStr(bool b);
int Xtoi(const char str[]);
int Atoi(const char str[]);
inline char FourBitToX(int i);

void SeparatePathFile(std::string &path,std::string &file,const std::string &fName);

long long int FileSize(const std::string &fName);


char BoolToChar(bool f);

std::string Uitox(unsigned int i);
std::string Ustox(unsigned short i);
std::string Ubtox(unsigned char i);
std::string Uitoa(unsigned int i);
std::string Uitoa(unsigned int i,unsigned minLen);

std::string Itox(int i);
std::string Stox(short i);
std::string Btox(char i);
std::string Itoa(int i);
std::string Itoa(int i,unsigned minLen);

void ExtendString(std::string &str,int minimumLength);

inline unsigned int GetDword(const unsigned char byteData[])
{
	return byteData[0]|(byteData[1]<<8)|(byteData[2]<<16)|(byteData[3]<<24);
}

inline unsigned int GetWord(const unsigned char byteData[])
{
	return byteData[0]|(byteData[1]<<8);
}

inline int GetSignedDword(const unsigned char byteData[])
{
	long long int dword=GetDword(byteData);
	if(0x80000000<=dword)
	{
		dword-=0x100000000;
	}
	return (int)dword;
}


inline int GetSignedWord(const unsigned char byteData[])
{
	int word=GetWord(byteData);
	if(0x8000<=word)
	{
		word-=0x10000;
	}
	return word;
}

inline int GetSignedByte(const unsigned char byteData){
	int byte=byteData;
	if(0x80<=byteData)
	{
		byte-=0x100;
	}
	return byte;
}



char BoolToChar(bool flag);
const char *BoolToNumberStr(bool flag);

std::vector <std::string> MakeDump(unsigned int printAddr,long long int size,const unsigned char data[]);

};

/* } */
#endif
