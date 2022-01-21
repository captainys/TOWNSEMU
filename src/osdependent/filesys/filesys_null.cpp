#include "filesys.h"



FileSys::FileSys()
{
}
FileSys::~FileSys()
{
}
FileSys::DirectoryEntry FileSys::FindFirst(std::string subDir)
{
	DirectoryEntry ent;
	return ent;
}
FileSys::DirectoryEntry FileSys::FindNext(void)
{
	DirectoryEntry ent;
	return ent;
}
/* static */ std::string FileSys::Getcwd(void)
{
	return "";
}
/* static */ bool FileSys::Chdir(std::string str)
{
	return false;
}
/* static */ bool FileSys::Mkdir(std::string str)
{
	return false;
}
