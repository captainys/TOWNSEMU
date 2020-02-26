#ifndef CDROM_IS_INCLUDED
#define CDROM_IS_INCLUDED
/* { */

// #include <queue>  std::queue turned out to be useless.
#include <vector>
#include <string>
#include "discimg.h"
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


/*! 4-byte Status Code from CD-ROM Drive
My guess based on the BIOS Disassembly:
00H xx  xx xx  Probably No Error
07H ??         The BIOS is checking, but the meaning is unknown.
xxH 01H xx xx  Probably Parameter Error
21H 01H xx xx  Probably Parameter Error
21H 02H        Probably Parameter Error
21H 0CH        Probably Parameter Error
21H 0FH        Probably Abnormal Termination
21H 05H        Probably Media Error (like bad sector)
21H 06H        Probably Media Error (like bad sector)
21H 07H        Probably Media Error (like bad sector)
21H 03H        Probably Hard Error
21H 04H        Probably Hard Error
21H 09H xx xx  Probably Hard Error
21H 0DH xx xx  Probably Hard Error
21H 08H        Probably Media Changed
    What about drive-not-ready?

Interpretation in the Linux for Towns source towns_cd.c (static void process_event(u_char st))
00H 09H xx xx  Media change?
00H xx  xx xx  No error
01H xx  xx xx  Command Accept Error
22H xx  xx xx  Data Ready
06H xx  xx xx  Read Done
07H xx  xx xx  CDDA Play Done
09H xx  xx xx  Door open
10H xx  xx xx  Door close Media not exists
11H xx  xx xx  Stop done
12H xx  xx xx  Pause done
13H xx  xx xx  Resume done
16H MM  xx xx  TOC Read  (MM>>4)==0 Mode 1  (MM>>4)==4 Mode 2  Else Mode 0  What's mode?
17H            TOC Read2
18H            SUBQ Read
19H            SUBQ Read2
20H            SUBQ Read3
21H 05H        Read Audio Track
21H 07H        Drive Not Ready
21H 08H        Media Changed
21H 0FH        Retry?
*/


class TownsCDROM : public Device
{
public:
	enum2
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
		std::vector <unsigned char> statusQueue;

		bool DMATransfer,CPUTransfer; // Both are not supposed to be 1, but I/O can set it that way.

		bool discChanged;

	private:
		DiscImage *imgPtr;
	public:
		State();
		~State();

		const DiscImage &GetDisc(void) const;
		DiscImage &GetDisc(void);

		void ClearStatusQueue(void);
		void PushStatusQueue(unsigned char d0,unsigned char d1,unsigned char d2,unsigned char d3);

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

	std::vector <std::string> GetStatusText(void) const;

	/*! Loads a disc-image file.  It can be .CUE or .ISO format file.
	    The return value is an error code of DiscImage class.
	    Use DiscImage::ErrorCodeToText(return_value) to get the text if
	    the return_value is not DiscImage::ERROR_NOERROR.
	    It also sets discChanged flag.
	*/
	unsigned int LoadDiscImage(const std::string &fName);

	/*! 
	*/
	void ExecuteCDROMCommand(void);
private:
	void SetStatusDriveNotReadyOrDiscChangedOrNoError(void);
	bool SetStatusDriveNotReadyOrDiscChanged(void);
	void SetStatusNoError(void);
	void SetStatusDriveNotReady(void);
	void SetStatusDiscChanged(void);
	void SetStatusQueueForTOC(void);
};


/* } */
#endif
