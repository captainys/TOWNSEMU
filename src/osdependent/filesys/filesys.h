#ifndef FILESYS_IS_INCLUDED
#define FILESYS_IS_INCLUDED
/* { */



#include <string>

class FileSys
{
public:
	class FindContext;

	FindContext *context=nullptr;
	bool linked=false;
	std::string hostPath;

	class DirectoryEntry
	{
	public:
		bool endOfDir=false;
		bool isDir=false;
		unsigned long long int length=0;
		std::string fName;
	};
	FileSys();
	~FileSys();
	DirectoryEntry FindFirst(std::string subDir);
	DirectoryEntry FindNext(void);
};



/* } */
#endif
