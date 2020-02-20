#ifndef CPPUTIL_IS_INCLUDED
#define CPPUTIL_IS_INCLUDED
/* { */

#include <vector>
#include <string>

namespace cpputil
{

std::string MakeFullPathName(std::string dirName,std::string fName);
std::vector <unsigned char> ReadBinaryFile(std::string fName);
bool WriteBinaryFile(const std::string &fName,unsigned long long length,const unsigned char dat[]);
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

char BoolToChar(bool f);

std::string Uitox(unsigned int i);
std::string Ustox(unsigned short i);
std::string Ubtox(unsigned char i);
std::string Uitoa(unsigned int i);

std::string Itox(int i);
std::string Stox(short i);
std::string Btox(char i);
std::string Itoa(int i);

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
