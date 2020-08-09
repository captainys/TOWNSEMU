#ifndef FILESYS_IS_INCLUDED
#define FILESYS_IS_INCLUDED
/* { */



#include <string>

class FileSys
{
public:
	class FindContext;

	bool linked=false;
	std::string hostPath;

	class DirectoryEntry
	{
	public:
		bool endOfDir=false;
		bool isDir=false;
		std::string fName;
	};
	FileSys();
	~FileSys();
	DirectoryEntry FindFirst(void);
	DirectoryEntry FindNext(void);
};



/* } */
#endif
