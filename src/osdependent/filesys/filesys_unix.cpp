#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include "filesys.h"



class FileSys::FindContext
{
public:
	DIR *dp=nullptr;
	std::string subDir;
	DirectoryEntry Read(std::string hostPath) const;
};
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

			std::string ful=hostPath+"/"+subDir+"/"+de->d_name;

			struct stat st;
			stat(ful.c_str(),&st);

			ent.fName=de->d_name;
			if(S_IFDIR==(st.st_mode&S_IFMT))
			{
				ent.isDir=true;
				ent.length=0;
			}
			else
			{
				ent.isDir=false;
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
FileSys::DirectoryEntry FileSys::FindFirst(std::string subDir)
{
	std::string path=hostPath+"/"+subDir;
	DirectoryEntry ent;

	if(nullptr!=context->dp)
	{
		closedir(context->dp);
	}
	context->dp=opendir(path.c_str());
	if(nullptr==context->dp)
	{
		ent.endOfDir=true;
		return ent;
	}
	context->subDir=subDir;

	ent=context->Read(hostPath);
	if(true==ent.endOfDir)
	{
		closedir(context->dp);
		context->dp=nullptr;
	}
	return ent;
}
FileSys::DirectoryEntry FileSys::FindNext(void)
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
