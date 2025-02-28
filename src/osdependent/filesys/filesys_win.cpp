#include <iostream>
#include <windows.h>
#include <direct.h>
#include "filesys.h"
#include "cpputil.h"



class FileSys::FindContext
{
public:
	HANDLE hFind=INVALID_HANDLE_VALUE;
	std::string subPath;

	static void DirEntFromFd(DirectoryEntry &ent,WIN32_FIND_DATAA &fd);
	static std::string MakeShortPath(std::string dir,std::string name);
	static void ConvertDirEntToSJIS(DirectoryEntry &ent,std::string hostPath);

	static bool IsSJISFirstKanjiByte(unsigned char c);
	static bool SJISNameFitsWithin8dot3(std::string sjis);

	~FindContext();
	void Close(void);
};

FileSys::FindContext::~FindContext()
{
	Close();
}
void FileSys::FindContext::Close(void)
{
	if(INVALID_HANDLE_VALUE!=hFind)
	{
		::FindClose(hFind);
		hFind=INVALID_HANDLE_VALUE;
	}
}


/* static */ void FileSys::FindContext::DirEntFromFd(DirectoryEntry &ent,WIN32_FIND_DATAA &fd)
{
	FILETIME localFileTime;
	FileTimeToLocalFileTime(&fd.ftLastWriteTime,&localFileTime);
	SYSTEMTIME sysTime;
	FileTimeToSystemTime(&localFileTime,&sysTime);

	ent.fName=fd.cFileName;
	ent.year=sysTime.wYear;
	ent.month=sysTime.wMonth;
	ent.day=sysTime.wDay;
	ent.hours=sysTime.wHour;
	ent.minutes=sysTime.wMinute;
	ent.seconds=sysTime.wSecond;
	if(0==(FILE_ATTRIBUTE_DIRECTORY&fd.dwFileAttributes))
	{
		ent.attr&=~ATTR_DIR;
		ent.length=((unsigned long long)fd.nFileSizeHigh<<32)+fd.nFileSizeLow;
	}
	else
	{
		ent.attr|=ATTR_DIR;
		ent.length=0;
	}
}

/* static */ std::string FileSys::FindContext::MakeShortPath(std::string dir,std::string name)
{
	std::string longPath=cpputil::MakeFullPathName(dir,name);

	std::vector <char> shortPath;
	shortPath.resize(longPath.size()+1);
	if(0==GetShortPathNameA(longPath.data(),shortPath.data(),shortPath.size()))
	{
		return "";
	}

	cpputil::SeparatePathFile(dir,name,shortPath.data());
	return name;
}

/*!
   path needs to be in the host encoding.
*/
/* static */ void FileSys::FindContext::ConvertDirEntToSJIS(DirectoryEntry &ent,std::string hostPath)
{
	auto sjis=ToSJISEncoding(ent.fName);
	if(true==SJISNameFitsWithin8dot3(sjis))
	{
		ent.fName=sjis;
	}
	else
	{
		auto shortPath=MakeShortPath(hostPath,ent.fName);
		if(""!=shortPath)
		{
			ent.fName=shortPath;
		}
		ent.fName=ToSJISEncoding(ent.fName);
	}
}

/* static */ std::string FileSys::ToHostEncoding(std::string shiftJISString)
{
	const int CP_SJIS=932; // https://learn.microsoft.com/en-us/windows/win32/intl/code-page-identifiers

	auto len=MultiByteToWideChar(CP_SJIS,MB_PRECOMPOSED,shiftJISString.data(),shiftJISString.size(),nullptr,0);

	std::vector <wchar_t> utf16;
	utf16.resize(len+1); // Hope it's long enough.
	MultiByteToWideChar(CP_SJIS,MB_PRECOMPOSED,shiftJISString.data(),shiftJISString.size(),utf16.data(),len+1);

	BOOL usedDefChar;
	auto outLen=WideCharToMultiByte(CP_ACP,0,utf16.data(),len,nullptr,0,nullptr,&usedDefChar);

	std::vector <char> out;
	out.resize(outLen+1);
	WideCharToMultiByte(CP_ACP,0,utf16.data(),len,out.data(),out.size(),nullptr,&usedDefChar);

	return std::string(out.data());
}


/* static */ std::string FileSys::ToSJISEncoding(std::string hostString)
{
	const int CP_SJIS=932; // https://learn.microsoft.com/en-us/windows/win32/intl/code-page-identifiers

	auto len=MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,hostString.data(),hostString.size(),nullptr,0);

	std::vector <wchar_t> utf16;
	utf16.resize(len+1); // Hope it's long enough.
	MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,hostString.data(),hostString.size(),utf16.data(),len+1);

	BOOL usedDefChar;
	auto outLen=WideCharToMultiByte(CP_SJIS,0,utf16.data(),len,nullptr,0,nullptr,&usedDefChar);

	std::vector <char> out;
	out.resize(outLen+1);
	WideCharToMultiByte(CP_SJIS,0,utf16.data(),len,out.data(),out.size(),nullptr,&usedDefChar);

	return std::string(out.data());
}

/* static */ bool FileSys::FindContext::IsSJISFirstKanjiByte(unsigned char c)
{
	return (0x80<=c && c<=0x9F) || 0xE0<=c;
}
/* static */ bool FileSys::FindContext::SJISNameFitsWithin8dot3(std::string sjis)
{
	bool inKanji=false;
	int dotPos=-1,dotCount=0;
	unsigned char *ucPtr=(unsigned char *)sjis.data();
	for(int i=0; i<sjis.size(); ++i)
	{
		auto c=ucPtr[i];
		if(true==inKanji)
		{
			inKanji=false;
		}
		else
		{
			if(true==IsSJISFirstKanjiByte(c))
			{
				inKanji=true;
			}
			else if('.'==c)
			{
				dotPos=i;
				++dotCount;
			}
		}
	}

	if(0==dotCount) // No extension
	{
		return sjis.size()<=8;
	}
	else if(1==dotCount)
	{
		return dotPos<=8 && sjis.size()<=dotPos+4;
	}

	// More than one dots.
	return false;
}

FileSys::FileSys()
{
}
FileSys::~FileSys()
{
}



FileSys::FindContext *FileSys::CreateFindContext(void)
{
	return new FindContext;
}
void FileSys::DeleteFindContext(FindContext *find)
{
	delete find;
}
void FileSys::FindClose(FindContext *find)
{
	find->Close();
}

FileSys::DirectoryEntry FileSys::FindFirst(std::string subPath,FindContext *find)
{
	find->Close();

	auto path=MakeHostPath(ToHostEncoding(subPath));
	if(""==path || (path.back()!='/' && path.back()!='\\'))
	{
		path.push_back('/');
	}
	path+="*.*";

	WIN32_FIND_DATAA fd;
	find->hFind=FindFirstFileA(path.c_str(),&fd);

	DirectoryEntry ent;
	if(INVALID_HANDLE_VALUE==find->hFind)
	{
		ent.endOfDir=true;
	}
	else
	{
		ent.endOfDir=false;
		find->subPath=subPath;
		find->DirEntFromFd(ent,fd);
		find->ConvertDirEntToSJIS(ent,ToHostEncoding(subPath));
	}
	return ent;
}
FileSys::DirectoryEntry FileSys::FindNext(FindContext *find)
{
	DirectoryEntry ent;
	if(INVALID_HANDLE_VALUE==find->hFind)
	{
		// Called without FindFirst
		ent.endOfDir=true;
	}
	else
	{
		WIN32_FIND_DATAA fd;
		if(TRUE!=FindNextFileA(find->hFind,&fd))
		{
			ent.endOfDir=true;
			find->Close();
		}
		else
		{
			ent.endOfDir=false;
			find->DirEntFromFd(ent,fd);
			find->ConvertDirEntToSJIS(ent,ToHostEncoding(find->subPath));
		}
	}
	return ent;
}
FileSys::DirectoryEntry FileSys::GetFileAttrib(std::string fileName) const
{
	auto path=MakeHostPath(ToHostEncoding(fileName));
	WIN32_FIND_DATAA fd;
	auto hFind=FindFirstFileA(path.c_str(),&fd);

	DirectoryEntry ent;
	if(INVALID_HANDLE_VALUE==hFind)
	{
		ent.endOfDir=true;
	}
	else
	{
		ent.endOfDir=false;
		FindContext::DirEntFromFd(ent,fd);
		FindContext::ConvertDirEntToSJIS(ent,path);
		::FindClose(hFind);
	}
	return ent;
}
/* static */ std::string FileSys::Getcwd(void)
{
	char buf[1024];
	if(nullptr==_getcwd(buf,1023))
	{
		return "";
	}
	return buf;
}
/* static */ bool FileSys::Chdir(std::string str)
{
	if(0==_chdir(str.c_str()))
	{
		return true;
	}
	return false;
}
/* static */ bool FileSys::Mkdir(std::string str)
{
	if(0==_mkdir(str.c_str()))
	{
		return true;
	}
	return false;
}
/* static */ bool FileSys::Rmdir(std::string str)
{
	if(0==_rmdir(str.c_str()))
	{
		return true;
	}
	return false;
}
