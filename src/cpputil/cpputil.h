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
void Capitalize(std::string &s);
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

std::string Uitox(unsigned int i);
std::string Ustox(unsigned short i);
std::string Ubtox(unsigned char i);
std::string Uitoa(unsigned int i);

std::string Itox(int i);
std::string Stox(short i);
std::string Btox(char i);
std::string Itoa(int i);

void ExtendString(std::string &str,int minimumLength);

unsigned int GetDword(const unsigned char byteData[]);
unsigned int GetWord(const unsigned char byteData[]);

int GetSignedDword(const unsigned char byteData[]);
int GetSignedWord(const unsigned char byteData[]);
int GetSignedByte(const unsigned char byteData);

char BoolToChar(bool flag);
const char *BoolToNumberStr(bool flag);

};

/* } */
#endif
