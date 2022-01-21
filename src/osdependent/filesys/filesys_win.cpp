#include <windows.h>
#include <direct.h>
#include "filesys.h"



class FileSys::FindContext
{
public:
	HANDLE hFind=INVALID_HANDLE_VALUE;
	static void DirEntFromFd(DirectoryEntry &ent,WIN32_FIND_DATAA &fd);
};

/* static */ void FileSys::FindContext::DirEntFromFd(DirectoryEntry &ent,WIN32_FIND_DATAA &fd)
{
	ent.fName=fd.cFileName;
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


FileSys::FileSys()
{
	context=new FindContext;
}
FileSys::~FileSys()
{
	if(INVALID_HANDLE_VALUE!=context->hFind)
	{
		FindClose(context->hFind);
	}
	delete context;
	context=nullptr;
}
FileSys::DirectoryEntry FileSys::FindFirst(std::string subDir)
{
	if(INVALID_HANDLE_VALUE!=context->hFind)
	{
		FindClose(context->hFind);
	}

	auto path=hostPath;
	if(""!=subDir && "/"!=subDir && "\\"!=subDir)
	{
		path+=std::string("/")+subDir;
	}
	path+="/*.*";

	WIN32_FIND_DATAA fd;
	context->hFind=FindFirstFileA(path.c_str(),&fd);

	DirectoryEntry ent;
	if(INVALID_HANDLE_VALUE==context->hFind)
	{
		ent.endOfDir=true;
	}
	else
	{
		ent.endOfDir=false;
		context->DirEntFromFd(ent,fd);
	}
	return ent;
}
FileSys::DirectoryEntry FileSys::FindNext(void)
{
	DirectoryEntry ent;
	if(INVALID_HANDLE_VALUE==context->hFind)
	{
		// Called without FindFirst
		ent.endOfDir=true;
	}
	else
	{
		WIN32_FIND_DATAA fd;
		if(TRUE!=FindNextFileA(context->hFind,&fd))
		{
			ent.endOfDir=true;
			FindClose(context->hFind);
			context->hFind=INVALID_HANDLE_VALUE;
		}
		else
		{
			ent.endOfDir=false;
			context->DirEntFromFd(ent,fd);
		}
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
