#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include "filesys.h"
#include "sjis2utf8.h"

#include <mutex>



class FileSys::FindContext
{
public:
	DIR *dp=nullptr;
	std::string subPath;
	DirectoryEntry Read(std::string hostPath) const;

	~FindContext();
	void Close(void);
};
FileSys::FindContext::~FindContext()
{
	Close();
}
void FileSys::FindContext::Close(void)
{
	if(nullptr!=dp)
	{
		closedir(dp);
		dp=nullptr;
	}
}

static std::mutex sjisMutex;
static ShiftJIS_UTF8 *sjisUtf8=nullptr;
static void MakeSjisUtf8(void)
{
	std::lock_guard <std::mutex> lock(sjisMutex);
	if(nullptr==sjisUtf8)
	{
		sjisUtf8=new ShiftJIS_UTF8;
	}
}

/* static */ std::string FileSys::ToHostEncoding(std::string from)
{
	MakeSjisUtf8();
	return sjisUtf8->SJIStoUTF8(from);
}
/* static */ std::string FileSys::ToSJISEncoding(std::string from)
{
	MakeSjisUtf8();
	return sjisUtf8->UTF8toSJIS(from);
}

static void StatToDirectoryEntry(FileSys::DirectoryEntry &dirent,const struct stat &stat)
{
	dirent.attr=0;
	if(S_IFDIR==(stat.st_mode&S_IFMT))
	{
		dirent.attr|=FileSys::ATTR_DIR;
		dirent.length=0;
	}
	else
	{
		dirent.attr&=~FileSys::ATTR_DIR;
		dirent.length=stat.st_size;
	}
}

FileSys::DirectoryEntry FileSys::FindContext::Read(std::string hostPath) const
{
	DirectoryEntry ent;

	if(nullptr==dp)
	{
		ent.endOfDir=true;
		return ent;
	}
	
	struct dirent *de=readdir(dp);
	if(nullptr!=de)
	{
		if(0!=de->d_ino)
		{
			std::string ful=hostPath+"/"+subPath+"/"+de->d_name;
			struct stat st;
			stat(ful.c_str(),&st);
			StatToDirectoryEntry(ent,st);

			ent.endOfDir=false;
			ent.fName=ToSJISEncoding(de->d_name);
		}
	}
	else
	{
		ent.endOfDir=true;
	}
	return ent;
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
FileSys::DirectoryEntry FileSys::FindFirst(std::string subPath,FindContext *context)
{
	std::string path=MakeHostPath(ToHostEncoding(subPath));
	DirectoryEntry ent;

	context->Close();

	context->dp=opendir(path.c_str());
	if(nullptr==context->dp)
	{
		ent.endOfDir=true;
		return ent;
	}
	context->subPath=subPath;

	ent=context->Read(hostPath);
	if(true==ent.endOfDir)
	{
		closedir(context->dp);
		context->dp=nullptr;
	}
	return ent;
}
FileSys::DirectoryEntry FileSys::GetFileAttrib(std::string fileName) const
{
	std::string path=MakeHostPath(ToHostEncoding(fileName));

	DirectoryEntry ent;

	struct stat st;
	if(0==stat(path.c_str(),&st))
	{
		StatToDirectoryEntry(ent,st);
		ent.endOfDir=false;
	}
	else
	{
		ent.endOfDir=true;
	}

	return ent;
}
FileSys::DirectoryEntry FileSys::FindNext(FindContext *context)
{
	DirectoryEntry ent;
	ent=context->Read(hostPath);
	if(true==ent.endOfDir)
	{
		closedir(context->dp);
		context->dp=nullptr;
	}
	return ent;
}


/* static */ std::string FileSys::Getcwd(void)
{
	char buf[1024];
	if(nullptr==getcwd(buf,1023))
	{
		return "";
	}
	return buf;
}
/* static */ bool FileSys::Chdir(std::string str)
{
	if(0==chdir(str.c_str()))
	{
		return true;
	}
	return false;
}
/* static */ bool FileSys::Mkdir(std::string str)
{
	auto mode=S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IXGRP|S_IROTH|S_IWOTH|S_IXOTH;
	if(0==mkdir(str.c_str(),mode))
	{
		return true;
	}
	return false;
}
/* static */ bool FileSys::Rmdir(std::string str)
{
	if(0==rmdir(str.c_str()))
	{
		return true;
	}
	return false;
}
