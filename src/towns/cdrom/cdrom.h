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
