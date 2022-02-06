#include "filesys.h"
#include "cpputil.h"

uint16_t FileSys::HasTimeStamp::FormatDOSTime(void) const
{
	return (hours<<11)|(minutes<<5)|(seconds/2);
}
uint16_t FileSys::HasTimeStamp::FormatDOSDate(void) const
{
	return ((year-1980)<<9)|(month<<5)|(day);
}
bool FileSys::SystemFileTable::IsOpen(void) const
{
	return fp.is_open();
}
const uint32_t FileSys::SystemFileTable::GetFileSize(void)
{
	auto curPos=fp.tellg();

	fp.seekg(0,std::ios::end);
	auto sz=fp.tellg();

	fp.seekg(curPos,std::ios::beg);

	return sz;
}
const uint32_t FileSys::SystemFileTable::GetFilePointer(void)
{
	return fp.tellg();
}

std::vector <unsigned char> FileSys::SystemFileTable::Read(uint32_t len)
{
	std::vector <unsigned char> buf;
	buf.resize(len);
	fp.read((char *)buf.data(),len);

	auto actualRead=fp.gcount();
	if(actualRead<len)
	{
		buf.resize(actualRead);
	}

	return buf;
}
bool FileSys::SubPathIsDirectory(const std::string &subPath)
{
	auto findContext=CreateFindContext();
	auto dirent=FindFirst(subPath,findContext);
	DeleteFindContext(findContext);
	return 0!=(dirent.attr&ATTR_DIR);
}
int FileSys::OpenExistingFile(std::string subPath,unsigned int openMode)
{
	auto sftIdx=FindAvailableSFT();
	if(0<=sftIdx)
	{
		auto fullPath=cpputil::MakeFullPathName(hostPath,subPath);
		if(true!=cpputil::FileExists(fullPath))
		{
			return -1;
		}

		sft[sftIdx].fName=subPath;
		sft[sftIdx].mode=openMode;
		switch(openMode)
		{
		case OPENMODE_READ:
			sft[sftIdx].fp.open(fullPath,std::ios::in|std::ios::binary);
			break;
		case OPENMODE_WRITE:
			sft[sftIdx].fp.open(fullPath,std::ios::out|std::ios::binary);
			break;
		case OPENMODE_RW:
			sft[sftIdx].fp.open(fullPath,std::ios::in|std::ios::out|std::ios::binary);
			break;
		}
		if(true==sft[sftIdx].fp.is_open())
		{
			return sftIdx;
		}
	}
	return -1;
}
bool FileSys::CloseFile(int sftIdx)
{
	if(0<=sftIdx &&
	   sftIdx<MAX_NUM_OPEN_FILE &&
	   true==sft[sftIdx].fp.is_open())
	{
		sft[sftIdx].fp.close();
		return true;
	}
	return false;
}
int FileSys::FindAvailableSFT(void) const
{
	for(int i=0; i<MAX_NUM_OPEN_FILE; ++i)
	{
		if(true!=sft[i].fp.is_open())
		{
			return i;
		}
	}
	return -1;
}

/*! Template="FILENAMEEXT" 11-letter format
    File=Normal
*/
/* static */ bool FileSys::DOSTemplateMatch(const std::string &templ11,const std::string &fName11)
{
	for(int i=0; i<11; ++i)
	{
		if('?'==templ11[i])
		{
			// Pass
		}
		else if(cpputil::Capitalize(templ11[i])!=cpputil::Capitalize(fName11[i]))
		{
			return false;
		}
	}
	return true;
}
