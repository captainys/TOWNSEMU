#include "filesys.h"



FileSys::FileSys()
{
}
FileSys::~FileSys()
{
}
/* static */ std::string FileSys::ToHostEncoding(std::string sjis)
{
	return sjis;
}
/* static */ std::string FileSys::ToSJISEncoding(std::string host)
{
	return host;
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
