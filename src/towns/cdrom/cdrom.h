/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#ifndef CDROM_IS_INCLUDED
#define CDROM_IS_INCLUDED
/* { */

// #include <queue>  std::queue turned out to be useless.
#include <vector>
#include <string>
#include "discimg.h"
#include "device.h"
#include "townsdef.h"
#include "i486.h"

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
00H 03H xx xx  CDDA is playing
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

My guess based on the Boot-ROM Disassembly:
04H xxH xx xx   Probably Seek done (After issuing 20H Seek, it waits for 00H No Error, and then 04H)
00H 04H xx xx   CDROM BIOS re-shoots command A0H if CDROM returns this code.       (0b00000100)
00H 08H xx xx   CDROM BIOS re-shoots command A0H if CDROM returns this code.       (0b00001000)
00H 0DH xx xx   CDROM BIOS Checking (2ndByte)&0x0D and wait for it to be non zero. (0b00001101)

Interpretation in the Linux for Towns source towns_cd.c (static void process_event(u_char st))
00H 09H xx xx  Media change?                                                       (0b00001001)
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
private:
	class FMTowns *townsPtr;
	class TownsPIC *PICPtr;
	class TownsDMAC *DMACPtr;
	class Outside_World *OutsideWorld=nullptr;

public:
	enum
	{
		PARAM_QUEUE_LEN=8,
		STATE_QUEUE_LEN=4,
		CMDFLAG_STATUS_REQUEST=0x20,
		CMDFLAG_IRQ=0x40,
	};

	enum
	{
		DELAYED_STATUS_IRQ_TIME=  50000,  // Tentatively  50us
		READ_SECTOR_TIME=       5000000,  // Tentatively   5ms  1X CD-ROM should be 1second/75frames=13.3ms per sector
		NOTIFICATION_TIME=      1000000,  // Tentatively   1ms
		CDDASTOP_TIME=          1000000,  // Tentatively   1ms
		SEEK_TIME=            100000000,  // Tentatively 100ms
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

		CDCMD_SETSTATE=   0x80, // Linux for FM TOWNS source label this as "SETSTATE" but isn't it "GETSTATE"?
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

		unsigned int readingSectorHSG,endSectorHSG;

		bool DMATransfer,CPUTransfer; // Both are not supposed to be 1, but I/O can set it that way.

		bool discChanged;


		// Fractal Engine Demo does the following:
		// (1) Issue command 62H (MODE1READ+SIRQ Request+Status Request),
		// (2) Clear SIRQ and DEI, and then
		// (3) Wait for SIRQ.
		// It is a near coding error. (1) and (2) must happen in the reverse order.
		// It was working only because SIRQ from (1) comes with a delay, and
		// comes after (2).  If the CD-ROM drive had been much faster, the program
		// won't run because SIRQ from (1) is cleared in (2), and (3) will wait
		// for an IRQ forever.
		// To emulate this, I need to introduce delayed status IRQ.
		bool delayedSIRQ=false;
		long long int nextScheduleTimeAfterDelayedSIRQ=0;

		// RAYXANBER waits until the CDDA playing time reaches track 15 during the "DATAWEST" logo screen.
		// However, .WAV file takes slightly less time to finish, and the playing time returned from CD-ROM
		// does not reach track 15.  To make sure the virtual CD-ROM reports the last playing time reaching
		// the last frame given at the time of CDDA Play command, CDDA must take three states:
		//    IDLE, PLAYING, STOPPING
		// while stopping, the virtual CD-ROM pretends that CDDA is playing the last frame.

		// BIOS disassembly suggests that command A0H should return status:
		//   00 00 00 00 07 00 00 00
		// once CDDA play ended.  For this purpose, the CDDA must have an additional state ENDED.
		enum
		{
			CDDA_IDLE,
			CDDA_PLAYING,
			CDDA_STOPPING,
			CDDA_ENDED,
		};
		enum
		{
			CDDA_POLLING_INTERVAL=1000000000/75, // 1 frame = 1/75 second.
		};
		unsigned int CDDAState=CDDA_IDLE;
		long long int nextCDDAPollingTime=0;
		DiscImage::MinSecFrm CDDAEndTime;

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

	class Variables
	{
	public:
		// For debugging purposes
		unsigned char lastParam[8]={0,0,0,0,0,0,0,0};
		// For debugging purposes
		i486DX::FarPointer lastCmdIssuedAt;
		i486DX::FarPointer lastParamWrittenAt;

		bool debugBreakOnCommandWrite=false;

		// If debugBreakOnCommandWrite==true and 0xffff!=debugBreakOnSpecificCommand,
		// it breaks the VM only if a specific command is sent.
		// debugBreakOnSpecificCommand is ignored if debugBreakOnCommandWrite!=true.
		unsigned int debugBreakOnSpecificCommand=0xffff;
	};

	State state;
	Variables var;

	virtual const char *DeviceName(void) const{return "CDROM";}

	TownsCDROM(class FMTowns *townsPtr,class TownsPIC *PICPtr,class TownsDMAC *DMACPtr);


	virtual void PowerOn(void);
	virtual void Reset(void);

	void SetOutsideWorld(class Outside_World *outside_world);
	inline void UpdateCDDAState(long long int townsTime,Outside_World &outside_world)
	{
		if(state.nextCDDAPollingTime<townsTime)
		{
			UpdateCDDAStateInternal(townsTime,outside_world);
		}
	}
	static inline bool StatusRequestBit(unsigned char cmd)
	{
		// Status request bit is supposed to be bit 5 of the command according to [2] pp.225.

		// Fractal Engine Demo issues command 00H (Seek without STATUS REQUEST Bit and IRQ Bit),
		// and still expects to get SRQ and IRQ.  This 00H may not be intended, and may happen to be working.

		// Shadow of the Beast issues command 02H (MODE1READ), again without STATUS REQUEST.
		// This is intended.  I am positive.  And then it waits for the Data Ready status (22H 0 0 0).
		// At this time, I am leaning toward Status Request bit of the CDC command has no effect.
		// Or, "Command Status" and other status may be different, and of course such information is
		// undocumented, and I need to make a guess.

		// However, totally disregarding this flag and always return status will prevent ChaseHQ from starting.
		// ChaseHQ issues command C1H (CDDASET) and expects no status is reported.

		// At the end of day, the reaction to this STATUS REQUEST bit may need to be different for different command.

		return (0!=(cmd&CMDFLAG_STATUS_REQUEST)); // This interpretation of [2] prevents Shadow of the Beast from starting.
	}
private:
	void UpdateCDDAStateInternal(long long int townsTime,Outside_World &outside_world);
public:
	inline bool CDDAIsPlaying(void) const
	{
		return (State::CDDA_PLAYING==state.CDDAState || State::CDDA_STOPPING==state.CDDAState);
	}

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

	void DelayedCommandExecution(unsigned long long int townsTime);

	void BreakOnCommandCheck(const char phase[]);

	/*! Call-back from FMTowns class.
	*/
	void RunScheduledTask(unsigned long long int townsTime);
private:
	void SetStatusDriveNotReadyOrDiscChangedOrNoError(void);
	bool SetStatusDriveNotReadyOrDiscChanged(void);
	void SetStatusNoError(void);
	void SetStatusDriveNotReady(void);
	void SetStatusDiscChanged(void);
	void SetStatusReadDone(void);
	void SetStatusHardError(void);
	void SetStatusParameterError(void);
	void SetStatusQueueForTOC(void);
	void SetStatusDataReady(void);
	void PushStatusCDDAStopDone(void);
	void SetStatusSubQRead(void);
	void PushStatusCDDAPlayEnded(void);

	void StopCDDA(void); // Placeholder for later.

	/* Turn on IRR flag if status queue is not empty.
	*/
	void SetSIRQ_IRR(void);
};


/* } */
#endif
