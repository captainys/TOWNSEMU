#ifndef FILESYS_IS_INCLUDED
#define FILESYS_IS_INCLUDED
/* { */



#include <fstream>
#include <vector>
#include <string>

// When C++17's filesystem library is commonly available and stable, I'll change it to
// non-platform dependent library.
// I'm so glad after too long years C++ insiders finally admitted there is a thing
// called file system.
// I hope they admit there is a thing called home directory soon.

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

	class HasFileAttrib
	{
	public:
		unsigned int attr=0;
	};
	class HasTimeStamp
	{
	public:
		unsigned int year=2004,month=06,day=13,hours=00,minutes=00,seconds=00;
		uint16_t FormatDOSTime(void) const;
		uint16_t FormatDOSDate(void) const;
	};
	class HasFileName
	{
	public:
		std::string fName;
	};

	class DirectoryEntry : public HasFileAttrib, public HasTimeStamp, public HasFileName
	{
	public:
		bool endOfDir=false;
		unsigned long long int length=0;
		std::string fName;
	};
	FileSys();
	~FileSys();
	DirectoryEntry FindFirst(std::string subDir);
	DirectoryEntry FindNext(void);
	// Potential Problem:
	//   DOS can stop directly listing and just forget about find-context.
	//   Find-context is not allocated resource.
	//   In the modern system, find-context is allocated resource.
	//   Potentially DOS program can stop listing before reaching the end,
	//   in which case, the current implementation will leak a find-context.
	//   Also the current implementation cannot support multiple find-contexts
	//   running simultaneously.

	// In OS-Dependent part >>
	FindContext *CreateFindContext(void);
	void DeleteFindContext(FindContext *find);
	DirectoryEntry FindFirst(std::string subPath,FindContext *find);
	DirectoryEntry FindNext(FindContext *find);
	// In OS-Dependent part <<



	enum
	{
		MAX_NUM_OPEN_FILE=32,
		OPENMODE_READ=0,   // Keep this number.  Compatible with DOS SFT
		OPENMODE_WRITE=1,  // Keep this number.  Compatible with DOS SFT
		OPENMODE_RW=2,     // Keep this number.  Compatible with DOS SFT
	};
	class SystemFileTable : public HasFileAttrib, public HasTimeStamp, public HasFileName
	// Corresponds to DOS SFT
	{
	public:
		unsigned int mode=0;
		std::fstream fp;
		bool IsOpen(void) const;
		const uint32_t GetFileSize(void); // Can't be const.  seekg is a modifier.
		const uint32_t GetFilePointer(void); // Can't be const.  tellg is a modifier.  Seriously?
		std::vector <unsigned char> Read(uint32_t len);
	};
	SystemFileTable sft[MAX_NUM_OPEN_FILE];
	/*!
	*/
	bool SubPathIsDirectory(const std::string &subPath);

	/*! Returns index to SFT.
	    subPath is relative to the virtual drive in the VM,
	    relative to the shared directory in the host.
	*/
	int OpenExistingFile(std::string subPath,unsigned int openMode);
	/*! SftIdx is an index returned by one of open functions.
	*/
	bool CloseFile(int SftIdx);
	/*! Returns -1 if none
	*/
	int FindAvailableSFT(void) const;



	// Until C++17
	// Damn it!  How long did it take for C++ people to admit that there is a thing called a file system!!
	// Hope they admit that there is a thing called a home directory soon.
	static std::string Getcwd(void);
	static bool Chdir(std::string str);
	static bool Mkdir(std::string str);

	/*! Template="FILENAMEEXT" 11-letter format
	*/
	static bool DOSTemplateMatch(const std::string &templ11,const std::string &fName11);

	/*! Weird DOS attribute matching.
	    sAttr: Attribute searching for.
	    fAttr: Attribute of file.
	*/
	static bool DOSAttrMatch(unsigned int sAttr,unsigned int fAttr);
};



/* } */
#endif
