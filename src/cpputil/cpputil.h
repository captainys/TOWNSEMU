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
#include <stdint.h>

namespace cpputil
{

std::string MakeFullPathName(std::string dirName,std::string fName);
bool FileExists(std::string fName);
std::string FindFileWithSearchPaths(std::string fName,const std::vector <std::string> &searchPaths);
std::vector <unsigned char> ReadBinaryFile(std::string fName);
std::vector <unsigned char> ReadBinaryFile(std::string fName,long long int start,long long int length);
bool WriteBinaryFile(const std::string &fName,unsigned long long length,const unsigned char dat[]);
bool WriteBinaryFile(const std::string &fName,unsigned long long int start,unsigned long long length,const unsigned char dat[]);
std::vector <std::string> ReadTextFile(std::string fName);
bool WriteTextFile(std::string fName,const std::vector <std::string> &text);
std::vector <std::string> Parser(const char str[]);
const std::string &Capitalize(std::string &s);
std::string Capitalize(const char s[]);
char Capitalize(char in);
std::string GetExtension(std::string fName);
std::string ChangeExtension(const char orgFName[],const char newExt[]);
std::string RemoveExtension(const char orgFName[]);
std::string GetBaseName(const std::string &path);
bool StrStartsWith(const std::string &str,const char ptn[]);
bool StrIncludes(const std::string str,char c); // std::find sucks.
const char *StrSkip(const char str[],const char ptn[]);
const char *StrSkipSpace(const char str[]);
const char *StrSkipNonSpace(const char str[]);
const char *BoolToStr(bool b);
bool StrToBool(const std::string &str);
int Xtoi(const char str[]);
int Atoi(const char str[]);
inline char FourBitToX(int i);

template <class T>
inline T Clamp(T value,T min,T max)
{
	if(value<min)
	{
		return min;
	}
	else if(max<value)
	{
		return max;
	}
	return value;
}

void SeparatePathFile(std::string &path,std::string &file,const std::string &fName);

std::string Getcwd(void);

bool IsRelativePath(std::string path);

std::string TrueName(std::string path);

std::string MakeRelativePath(std::string fName,std::string relativeToThisDir);

void SimplifyPath(std::string &path);

long long int FileSize(const std::string &fName);


char BoolToChar(bool f);

std::string Uitox(unsigned int i);
std::string Ustox(unsigned short i);
std::string Ubtox(unsigned char i);
std::string UitoaZeroPad(unsigned int i,unsigned numDigits);
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
#ifdef YS_LITTLE_ENDIAN
	return *((unsigned int *)byteData);
#else
	return byteData[0]|(byteData[1]<<8)|(byteData[2]<<16)|(byteData[3]<<24);
#endif
}

inline unsigned int GetWord(const unsigned char byteData[])
{
#ifdef YS_LITTLE_ENDIAN
	return *((unsigned short *)byteData);
#else
	return byteData[0]|(byteData[1]<<8);
#endif
}

inline int GetSignedDword(const unsigned char byteData[])
{
#if defined(YS_LITTLE_ENDIAN) && defined(YS_TWOS_COMPLEMENT)
	int32_t *signedPtr=(int32_t *)byteData;
	return *signedPtr;
#else
	long long int dword=GetDword(byteData);
	dword=(dword&0x7FFFFFFF)-(dword&0x80000000);
	return (int)dword;
#endif
}

inline int GetSignedWord(const unsigned char byteData[])
{
#if defined(YS_LITTLE_ENDIAN) && defined(YS_TWOS_COMPLEMENT)
	int16_t *signedPtr=(int16_t *)byteData;
	return *signedPtr;
#else
	int word=GetWord(byteData);
	word=(word&0x7FFF)-(word&0x8000);
	return word;
#endif
}

inline uint16_t MakeUnsignedWord(uint8_t lowByte,uint8_t highByte)
{
#if defined(YS_LITTLE_ENDIAN)
	uint16_t word;
	uint8_t *ptr=(uint8_t *)&word;
	ptr[0]=lowByte;
	ptr[1]=highByte;
	return word;
#else
	return (highByte<<8)|lowByte;
#endif
}

inline uint32_t MakeUnsignedDword(uint8_t lowByte,uint8_t midLowByte,uint8_t midHighByte,uint8_t highByte)
{
#if defined(YS_LITTLE_ENDIAN)
	uint32_t dword;
	uint8_t *ptr=(uint8_t *)&dword;
	ptr[0]=lowByte;
	ptr[1]=midLowByte;
	ptr[2]=midHighByte;
	ptr[3]=highByte;
	return dword;
#else
	return (highByte<<24)|(midHighByte<<16)|(midLowByte<<8)|lowByte;
#endif
}

inline int GetSignedByte(const unsigned char byteData)
{
#if defined(YS_LITTLE_ENDIAN) && defined(YS_TWOS_COMPLEMENT)
	char *signedPtr=(char *)&byteData;
	return *signedPtr;
#else
	int byte=byteData;
	byte=(byte&0x7F)-(byte&0x80);
	return byte;
#endif
}

inline void PutDword(unsigned char byteData[],unsigned int data)
{
#ifdef YS_LITTLE_ENDIAN
	*((unsigned int *)byteData)=data;
#else
	byteData[0]= data     &0xff;
	byteData[1]=(data>> 8)&0xff;
	byteData[2]=(data>>16)&0xff;
	byteData[3]=(data>>24)&0xff;
#endif
}

inline void PutWord(unsigned char byteData[],unsigned short data)
{
#ifdef YS_LITTLE_ENDIAN
	*((unsigned short *)byteData)=data;
#else
	byteData[0]= data     &0xff;
	byteData[1]=(data>> 8)&0xff;
#endif
}

inline void CopyWord(unsigned char dst[],const unsigned char src[])
{
	(*(uint16_t *)dst)=(*(const uint16_t *)src);
}

inline void CopyDword(unsigned char dst[],const unsigned char src[])
{
	(*(uint32_t *)dst)=(*(const uint32_t *)src);
}

inline int32_t WordToSigned32(uint16_t wd)
{
#if defined(YS_TWOS_COMPLEMENT)
	int16_t *signedPtr=(int16_t *)&wd;
	return *signedPtr;
#else
	int32_t DXAX=wd;
	DXAX=(DXAX&0x7FFF)-(DXAX&0x8000);
	return DXAX;
#endif
}

inline uint32_t WordPairToUnsigned32(uint16_t lowWord,uint16_t highWord)
{
#if defined(YS_TWOS_COMPLEMENT) && defined(YS_LITTLE_ENDIAN)
	uint32_t dw;
	uint16_t *wdPtr=(uint16_t *)&dw;
	wdPtr[0]=lowWord;
	wdPtr[1]=highWord;
	return dw;
#else
	return (highWord<<16)|lowWord;
#endif
}

inline int32_t WordPairToSigned32(uint16_t lowWord,uint16_t highWord)
{
#if defined(YS_TWOS_COMPLEMENT) && defined(YS_LITTLE_ENDIAN)
	int32_t dw;
	uint16_t *wdPtr=(uint16_t *)&dw;
	wdPtr[0]=lowWord;
	wdPtr[1]=highWord;
	return dw;
#else
	int DXAX=highWord;
	DXAX=(DXAX&0x7FFF)-(DXAX&0x8000);
	DXAX<<=16;
	DXAX|=lowWord;
	return DXAX;
#endif
}

inline uint64_t DwordPairToUnsigned64(uint32_t lowWord,uint32_t highWord)
{
#if defined(YS_TWOS_COMPLEMENT) && defined(YS_LITTLE_ENDIAN)
	uint64_t dw;
	uint32_t *dwPtr=(uint32_t *)&dw;
	dwPtr[0]=lowWord;
	dwPtr[1]=highWord;
	return dw;
#else
	uint64_t qw=highWord;
	qw<<=32;
	qw|=lowWord;
	return qw;
#endif
}

inline int64_t DwordPairToSigned64(uint32_t lowWord,uint32_t highWord)
{
#if defined(YS_TWOS_COMPLEMENT) && defined(YS_LITTLE_ENDIAN)
	int64_t qw;
	uint32_t *dwPtr=(uint32_t *)&qw;
	dwPtr[0]=lowWord;
	dwPtr[1]=highWord;
	return qw;
#else
	int64_t EDXEAX=highWord;
	EDXEAX=(EDXEAX&0x7FFFFFFF)-(EDXEAX&0x80000000);
	EDXEAX<<=32;
	EDXEAX|=lowWord;
	return EDXEAX;
#endif
}

inline int64_t DwordToSigned64(uint32_t dw)
{
#if defined(YS_TWOS_COMPLEMENT)
	int32_t *signedPtr=(int32_t *)&dw;
	return *signedPtr;
#else
	int64_t EDXEAX=dw;
	EDXEAX=(EDXEAX&0x7FFFFFFF)-(EDXEAX&0x80000000);
	return EDXEAX;
#endif
}

inline int32_t ByteToSigned32(uint8_t b)
{
#if defined(YS_TWOS_COMPLEMENT)
	int8_t *signedPtr=(int8_t *)&b;
	return *signedPtr;
#else
	int32_t AL=b;
	return (AL&0x7F)-(AL&0x80);
#endif
}

inline uint16_t LowWord(int32_t dw)
{
#ifdef YS_LITTLE_ENDIAN
	uint16_t *wdPtr=(uint16_t *)&dw;
	return wdPtr[0];
#else
	return dw&0xFFFF;
#endif
}

inline uint16_t HighWord(int32_t dw)
{
#ifdef YS_LITTLE_ENDIAN
	uint16_t *wdPtr=(uint16_t *)&dw;
	return wdPtr[1];
#else
	return (dw>>16)&0xFFFF;
#endif
}

inline uint16_t LowWord(uint32_t dw)
{
#ifdef YS_LITTLE_ENDIAN
	uint16_t *wdPtr=(uint16_t *)&dw;
	return wdPtr[0];
#else
	return dw&0xFFFF;
#endif
}

inline uint8_t LowByte(uint32_t dw)
{
#ifdef YS_LITTLE_ENDIAN
	uint8_t *wdPtr=(uint8_t *)&dw;
	return wdPtr[0];
#else
	return dw&0xFF;
#endif
}

inline uint16_t HighWord(uint32_t dw)
{
#ifdef YS_LITTLE_ENDIAN
	uint16_t *wdPtr=(uint16_t *)&dw;
	return wdPtr[1];
#else
	return (dw>>16)&0xFFFF;
#endif
}

inline uint32_t LowDword(int64_t qw)
{
#ifdef YS_LITTLE_ENDIAN
	uint32_t *dwPtr=(uint32_t *)&qw;
	return dwPtr[0];
#else
	return qw&0xffffffff;
#endif
}

inline uint32_t HighDword(int64_t qw)
{
#ifdef YS_LITTLE_ENDIAN
	uint32_t *dwPtr=(uint32_t *)&qw;
	return dwPtr[1];
#else
	return (qw>>32)&0xFFFFFFFF;
#endif
}

inline uint32_t LowDword(uint64_t qw)
{
#ifdef YS_LITTLE_ENDIAN
	uint32_t *dwPtr=(uint32_t *)&qw;
	return dwPtr[0];
#else
	return qw&0xffffffff;
#endif
}

inline uint32_t HighDword(uint64_t qw)
{
#ifdef YS_LITTLE_ENDIAN
	uint32_t *dwPtr=(uint32_t *)&qw;
	return dwPtr[1];
#else
	return (qw>>32)&0xFFFFFFFF;
#endif
}

inline uint8_t GetWordHighByte(uint16_t wd)
{
#ifdef YS_LITTLE_ENDIAN
	uint8_t *chPtr=(uint8_t *)&wd;
	return chPtr[1];
#else
	return (wd>>8)&0xFF;
#endif
}


char BoolToChar(bool flag);
const char *BoolToNumberStr(bool flag);

std::vector <std::string> MakeDump(unsigned int printAddr,long long int size,const unsigned char data[]);


template <class T>
bool Match(long long int len,const T dat1[],const T dat2[])
{
	for(long long int i=0; i<len; ++i)
	{
		if(dat1[i]!=dat2[i])
		{
			return false;
		}
	}
	return true;
}

bool WildCardCompare(std::string ptn,std::string str);
bool WildCardCompare(int lenPtn,const char ptn[],int lenStr,const char str[]);

bool Is2toN(unsigned int i);
};


#ifdef _WIN32
	#define std_unreachable __assume(0)
#elif defined(__clang__) || defined(__GNUC__)
	#define std_unreachable __builtin_unreachable()
#else
	#define std_unreachable
#endif


/* } */
#endif
