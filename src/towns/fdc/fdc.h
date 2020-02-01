#ifndef FDC_IS_INCLUDED
#define FDC_IS_INCLUDED
/* { */


#include "device.h"

class TownsFDC : public Device
{
public:
	class State
	{
	public:
		class Drive
		{
		public:
			bool diskInserted; // Will be replaced with D77 disk image class
			int trackPos;      // Actual head location.
			int trackReg;      // Value in track register 0202H
			int sectorReg;     // Value in sector register 0x04H
			int dataReg;       // Value in data register 0x06H
		};

		Drive drive[4];
		bool driveSwitch;  // [2] pp.258
		bool busy;
		bool MODEB,HISPD;  // [2] pp.258, pp.809
		bool INUSE;
		unsigned int driveSelectBit;
		unsigned int lastCmd;
		unsigned int lastStatus;

		void Reset(void);

		void SendCommand(unsigned int data);

		unsigned int CommandToCommandType(unsigned int cmd) const;
		unsigned char MakeUpStatus(unsigned int cmd) const;
		unsigned int DriveSelect(void) const;

		bool DriveReady(void) const;
		bool WriteProtected(void) const;
		bool SeekError(void) const;
		bool CRCError(void) const;
		bool IndexHole(void) const;
		bool RecordType(void) const;
		bool RecordNotFound(void) const;
		bool LostData(void) const;
		bool DataRequest(void) const;
		bool WriteFault(void) const;
	};

	class FMTowns *townsPtr;

	State state;

	bool debugBreakOnCommandWrite;

	virtual const char *DeviceName(void) const{return "FDC";}

	TownsFDC(class FMTowns *townsPtr);
	virtual void IOWriteByte(unsigned int ioport,unsigned int data);
	virtual unsigned int IOReadByte(unsigned int ioport);

	virtual void Reset(void);
};


/* } */
#endif
