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

void cpputil::Capitalize(std::string &s)
{
	for(auto &c : s)
	{
		if('a'<=c && c<='z')
		{
			c=c+'A'-'a';
		}
	}
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
