#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include "filesys.h"



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
			ent.endOfDir=false;

			std::string ful=hostPath+"/"+subPath+"/"+de->d_name;

			struct stat st;
			stat(ful.c_str(),&st);

			ent.fName=de->d_name;
			if(S_IFDIR==(st.st_mode&S_IFMT))
			{
				ent.attr|=ATTR_DIR;
				ent.length=0;
			}
			else
			{
				ent.attr&=~ATTR_DIR;
				ent.length=st.st_size;
			}
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
	context=new FindContext;
}
FileSys::~FileSys()
{
	delete context;
	context=nullptr;
}
FileSys::DirectoryEntry FileSys::FindFirst(std::string subPath)
{
	return FindFirst(subPath,this->context);
}
FileSys::DirectoryEntry FileSys::FindNext(void)
{
	return FindNext(this->context);
}


FileSys::FindContext *FileSys::CreateFindContext(void)
{
	return new FindContext;
}
void FileSys::DeleteFindContext(FindContext *find)
{
	delete find;
}
FileSys::DirectoryEntry FileSys::FindFirst(std::string subPath,FindContext *context)
{
	std::string path=hostPath;
	if(""!=subPath && "/"!=subPath && "\\"!=subPath)
	{
		if(""==path || (path.back()!='/' && path.back()!='\\'))
		{
			path.push_back('/');
		}
		path+=subPath;
	}
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
	if(0==mkdir(str.c_str(),0x777))
	{
		return true;
	}
	return false;
}
