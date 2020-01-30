#ifndef CDROM_IS_INCLUDED
#define CDROM_IS_INCLUDED
/* { */

#include <string>
#include "device.h"
#include "townsdef.h"

/*! Disassembly of some game titles suggested that the command can be issued like:
      Push parameter queue -> and then send command, or
      Send command first -> and then push parameter queue.
    My guess is either:
      A the Internal CD drive starts doing something when both are given, or
      B there are some time after a command was written and before the CD controller starts reading parameter queue.
    I first try A and see if it works.
*/

class TownsCDROM : public Device
{
public:
	enum
	{
		PARAM_QUEUE_LEN=8,
		STATE_QUEUE_LEN=4,
	};

	// Reference [3] 
	enum
	{
		// Take &0x9F with byte data written to 04C2H
		CDCMD_SEEK=       0x00,
		CDCMD_MODE2READ=  0x01,
		CDCMD_MODE1READ=  0x02,
		CDCMD_RAWREAD=    0x03,
		CDCMD_CDDAPLAY=   0x04,
		CDCMD_TOCREAD=    0x05,
		CDCMD_SUBQREAD=   0x06,
		CDCMD_UNKNOWN1=   0x1F, // NOP and requst status? I guess?

		CDCMD_SETSTATE=   0x80,
		CDCMD_CDDASET=    0x81,
		CDCMD_CDDASTOP=   0x84,
		CDCMD_CDDAPAUSE=  0x85,
		CDCMD_UNKNOWN2=   0x86,
		CDCMD_CDDARESUME= 0x87,
	};

	class State
	{
	public:
		std::string imgFileName;

		bool SIRQ; // 4C0H bit 7
		bool DEI;  // 4C0H bit 6 (DMA End Flag)
		bool STSF; // 4C0H bit 5 (Software Transfer End)
		bool DTSF; // 4C0H bit 4 (DMA Transfer in progress)
		bool DRY;  // 4C0H bit 0 (Ready to receive command)

		bool enableSIRQ;
		bool enableDEI;

		bool cmdReceived;
		unsigned char cmd;
		int nParamQueue;
		unsigned char paramQueue[8];
		int nStatusQueue;
		unsigned char statusQueue[4];

		bool DMATransfer,CPUTransfer; // Both are not supposed to be 1, but I/O can set it that way.

		void Reset(void);
		void ResetMPU(void);
	};

	State state;

	virtual const char *DeviceName(void) const{return "CDROM";}

	TownsCDROM();

	virtual void PowerOn(void);
	virtual void Reset(void);

	virtual void IOWriteByte(unsigned int ioport,unsigned int data);
	virtual unsigned int IOReadByte(unsigned int ioport);
};


/* } */
#endif
