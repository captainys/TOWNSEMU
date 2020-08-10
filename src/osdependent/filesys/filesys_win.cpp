#include <windows.h>
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
		ent.isDir=false;
		ent.length=((unsigned long long)fd.nFileSizeHigh<<32)+fd.nFileSizeLow;
	}
	else
	{
		ent.isDir=true;
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

	auto path=hostPath+"/"+subDir+"/*.*";

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
