#ifndef VNDRV_IS_INCLUDED
#define VNDRV_IS_INCLUDED
/* { */

#include "filesys.h"

class TownsVnDrv : public Device
{
private:
	class FMTownsCommon *townsPtr;
public:
	virtual const char *DeviceName(void) const{return "VNDRV";}

	enum
	{
		DISABLE_CODE=0,
		ENABLE_CODE=1,
		MAX_NUM_SHARED_DIRECTORIES=8,
	};

	FileSys sharedDir[MAX_NUM_SHARED_DIRECTORIES];

	class State
	{
	public:
		void PowerOn(void);
		void Reset(void);

		unsigned lastAPICheckWrite=0xffff;
		bool enabled=false;
	};

	State state;

	TownsVnDrv(class FMTownsCommon *townsPtr);

	FileSys *GetSharedDir(unsigned int drvNum);
	const FileSys *GetSharedDir(unsigned int drvNum) const;

	const unsigned int NumDrives(void) const;

	virtual void PowerOn(void);
	virtual void Reset(void);

	virtual void IOWriteByte(unsigned int ioport,unsigned int data);
	virtual void IOWriteWord(unsigned int ioport,unsigned int data);

	virtual unsigned int IOReadByte(unsigned int ioport);
	virtual unsigned int IOReadWord(unsigned int ioport);

	void ExecPrimaryCommand(unsigned int cmd);
	void ExecAuxCommand(unsigned int cmd);
	void StoreDirEnt(const FileSys::DirectoryEntry &dirEnt);
};

/* } */
#endif
