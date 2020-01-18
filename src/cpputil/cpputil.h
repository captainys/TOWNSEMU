#ifndef CPPUTIL_IS_INCLUDED
#define CPPUTIL_IS_INCLUDED
/* { */

#include <vector>
#include <string>

namespace cpputil
{

std::string MakeFullPathName(std::string dirName,std::string fName);
std::vector <unsigned char> ReadBinaryFile(std::string fName);
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

};

/* } */
#endif
