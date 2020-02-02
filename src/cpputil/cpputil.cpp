#include "fstream"

#include "cpputil.h"



std::string cpputil::MakeFullPathName(std::string dirName,std::string fName)
{
	return dirName+"/"+fName;
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

std::vector <std::string> cpputil::Parser(const char str[])
{
	int state=0;
	std::string curStr;
	std::vector <std::string> argv;
	for(int i=0; 0!=str[i]; ++i)
	{
		if(0==state)
		{
			if(' '!=str[i] && '\t'!=str[i])
			{
				curStr.push_back(str[i]);
				state=1;
			}
		}
		else if(1==state)
		{
			if(' '==str[i] || '\t'==str[i] || 0==str[i+1])
			{
				if(' '!=str[i] && '\t'!=str[i])
				{
					curStr.push_back(str[i]);
				}
				argv.push_back((std::string &&)curStr);
				curStr="";
				state=0;
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

void cpputil::ExtendString(std::string &str,int minimumLength)
{
	while(str.size()<minimumLength)
	{
		str.push_back(' ');
	}
}

unsigned int cpputil::GetDword(const unsigned char byteData[])
{
	return byteData[0]+(byteData[1]<<8)+(byteData[2]<<16)+(byteData[3]<<24);
}

unsigned int cpputil::GetWord(const unsigned char byteData[])
{
	return byteData[0]+(byteData[1]<<8);
}

int cpputil::GetSignedDword(const unsigned char byteData[])
{
	long long int dword;
	dword=GetDword(byteData);
	if(0x80000000<=dword)
	{
		dword-=0x100000000;
	}
	return (int)dword;
}

int cpputil::GetSignedWord(const unsigned char byteData[])
{
	int word=GetWord(byteData);
	if(0x8000<=word)
	{
		word-=0x10000;
	}
	return word;
}

int cpputil::GetSignedByte(const unsigned char byteData)
{
	int byte=byteData;
	if(0x80<=byteData)
	{
		byte-=0x100;
	}
	return byte;
}

char cpputil::BoolToChar(bool flag)
{
	return ((true==flag) ? '1' : '0');
}

const char *cpputil::BoolToNumberStr(bool flag)
{
	return ((true==flag) ? "1" : "0");
}
