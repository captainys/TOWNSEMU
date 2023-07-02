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

	bool linked=false;
	std::string hostPath;

	enum
	{
		MAX_NUM_OPEN_DIRECTORY=16,
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
		void ClearTimeStamp(void);
		void CopyTimeStampFrom(const HasTimeStamp &incoming);
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

	class FindStruct
	{
	public:
		unsigned int PSP=0;
		mutable unsigned int usedTime=0;
		bool used=false;
		std::string subPath;
		FindContext *findContext=nullptr;
		FindStruct();
		~FindStruct();
	};
	mutable unsigned int usedTimeSeed=0;
	FindStruct findStruct[MAX_NUM_OPEN_DIRECTORY];

	FileSys();
	~FileSys();



	// In OS-Dependent part >>
	static FindContext *CreateFindContext(void);
	static void DeleteFindContext(FindContext *find);
	static void FindClose(FindContext *);


	/*! SubPath needs to be a directory.
	    "/*.*" or "/*" will be added for file listing.
	    Returns an index to find struct.
	*/
	int FindFirst(DirectoryEntry &ent,unsigned int PSP,const std::string &subPath);

	DirectoryEntry FindNext(int findStructIdx);

	/*! Find Struct used in _dos_findfirst and _dos_findnext are not a dynamically
	    allocated resource, while Windows uses HANDLE, and unix DIR *, which are
	    dynamically-allocated resource.

	    Since DOS does not have a function like _dos_findclose, unless somehow
	    the host finds a timing to close handle, it will use up handles for
	    directory listing.

	    One option is to close when FindNext reaches the end of the directory.
	    However, if the DOS program stops in the middle, for example stops when
	    it found what it was looking for, the host will not have a timing to close
	    the handle.

	    Two solutions to prevent resource exhaustion:
	    (1) Reuse find struct when the DOS program is searching in the same
	        directory again.  Therefore FindAvailableFindStruct needs the
	        directory.
	    (2) If nothing is found, sacrifice the oldest find struct.  For this
	        purpose, usedTime member has been added.
	*/
	int FindAvailableFindStruct(const std::string &subPath) const;

	bool FindStructValid(int findStructIdx) const;


	/*! SubPath needs to be a directory.
	    "/*.*" or "/*" will be added for file listing.
	*/
	DirectoryEntry FindFirst(std::string subPath,FindContext *find);

	DirectoryEntry FindNext(FindContext *find);

	/*! Returns file attribute in directory entry.
	    If file not found, endOfDir member will be true.
	*/
	DirectoryEntry GetFileAttrib(std::string subPath) const;
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
		unsigned int PSP=0;
		std::fstream fp;
		bool IsOpen(void) const;
		const uint32_t GetFileSize(void); // Can't be const.  seekg is a modifier.
		const uint32_t GetFilePointer(void); // Can't be const.  tellg is a modifier.  Seriously?
		std::vector <unsigned char> Read(uint32_t len);
		uint32_t Write(const std::vector <unsigned char> &data);
	};
	SystemFileTable sft[MAX_NUM_OPEN_FILE];
	/*!
	*/
	bool SubPathIsDirectory(std::string subPath);

	/*! Returns index to SFT.
	    subPath is relative to the virtual drive in the VM,
	    relative to the shared directory in the host.
	*/
	int OpenExistingFile(unsigned int PSP,std::string subPath,unsigned int openMode);
	int OpenFileNotTruncate(unsigned int PSP,std::string subPath,unsigned int openMode);
	int OpenFileTruncate(unsigned int PSP,std::string subPath,unsigned int openMode);

	bool CheckFileExist(std::string subPath);

	void Seek(int SftIdx,uint32_t pos);

	uint32_t Fsize(int SftIdx);

	void TruncateToSize(int SftIdx,uint32_t pos);

	/*! SftIdx is an index returned by one of open functions.
	*/
	bool CloseFile(int SftIdx);


	/*!
	*/
	bool RenameSubPath(std::string subPathFrom,std::string subPathTo);

	/*!
	*/
	bool DeleteSubPathFile(std::string subPath);

	/*!
	*/
	bool MkdirSubPath(std::string subPath);

	/*!
	*/
	bool RmdirSubPath(std::string subPath);


	/*!
	*/
	void CloseAllForPSP(unsigned int PSP);

	/*! Returns -1 if none
	*/
	int FindAvailableSFT(void) const;



	// Until C++17
	// Damn it!  How long did it take for C++ people to admit that there is a thing called a file system!!
	// Hope they admit that there is a thing called a home directory soon.
	static std::string Getcwd(void);
	static void BackSlashToSlash(std::string &src);
	static bool Chdir(std::string str);
	static bool Mkdir(std::string str);
	static bool Rmdir(std::string str);

	/*! Template="FILENAMEEXT" 11-letter format
	*/
	static bool DOSTemplateMatch(const std::string &templ11,const std::string &fName11);

	/*! Weird DOS attribute matching.
	    sAttr: Attribute searching for.
	    fAttr: Attribute of file.
	*/
	static bool DOSAttrMatch(unsigned int sAttr,unsigned int fAttr);

	std::string MakeHostPath(const std::string &subPath) const;
};



/* } */
#endif
