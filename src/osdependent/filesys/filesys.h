#ifndef FILESYS_IS_INCLUDED
#define FILESYS_IS_INCLUDED
/* { */



#include <string>

class FileSys
{
private:
	// Make it uncopyable.
	FileSys(const FileSys &incoming);
	FileSys &operator=(const FileSys &incoming);
public:
	class FindContext;

	FindContext *context=nullptr;
	bool linked=false;
	std::string hostPath;

	enum
	{
		ATTR_ARCHIVE= 0x20,
		ATTR_DIR=     0x10,
		ATTR_VOLUME=  0x08,
		ATTR_SYSTEM=  0x04,
		ATTR_HIDDEN=  0x02,
		ATTR_READONLY=0x01,
	};

	class DirectoryEntry
	{
	public:
		bool endOfDir=false;
		unsigned long long int length=0;
		unsigned int attr=0;
		unsigned int year=2004,month=06,day=13,hours=00,minutes=00,seconds=00;
		std::string fName;
	};
	FileSys();
	~FileSys();
	DirectoryEntry FindFirst(std::string subDir);
	DirectoryEntry FindNext(void);



	// Until C++17
	// Damn it!  How long did it take for C++ people to admit that there is a thing called a file system!!
	// Hope they admit that there is a thing called a home directory soon.
	static std::string Getcwd(void);
	static bool Chdir(std::string str);
	static bool Mkdir(std::string str);
};



/* } */
#endif
