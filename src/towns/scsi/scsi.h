#ifndef SCSI_IS_INCLUDED
#define SCSI_IS_INCLUDED
/* { */

#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "discimg.h"

/*
Phase and I/O [0xC32] read
 PHASE_BUSFREE:       C_D=0x10  MSG=0x20  I_O=0x40     I/O [0xC32]  0x0*
 PHASE_DATA_OUT:      C_D=0x10  MSG=0x20  I_O=0x40     I/O [0xC32]  0x0*
 PHASE_COMMAND:       C_D=0x10  MSG=0x20  I_O=0x40     I/O [0xC32]  0x1*
                                                                   (0x2*)
 PHASE_MESSAGE_OUT:   C_D=0x10  MSG=0x20  I_O=0x40     I/O [0xC32]  0x3*
 PHASE_DATA_IN:       C_D=0x10  MSG=0x20  I_O=0x40     I/O [0xC32]  0x4*
 PHASE_STATUS:        C_D=0x10  MSG=0x20  I_O=0x40     I/O [0xC32]  0x5*
                                                                   (0x6*)
 PHASE_MESSAGE_IN:    C_D=0x10  MSG=0x20  I_O=0x40     I/O [0xC32]  0x7*
*/


class TownsSCSI : public Device
{
private:
	class FMTowns *townsPtr;
	class Outside_World *outsideworld=nullptr;
public:
	virtual const char *DeviceName(void) const{return "SCSI";}


	class SCSIIOThread
	{
	public:
		enum
		{
		CMD_NONE,
		CMD_FILEREAD,
		CMD_CDREAD,
		// CMD_WRITE,
		CMD_QUIT,
		};

		mutable std::mutex mutex;
		std::condition_variable cond;
		std::thread thr;

		unsigned int cmd=CMD_NONE;

		bool dataReady=false;
		std::string fName;
		uint64_t filePtr,length;
		const DiscImage *discImgPtr=nullptr;
		std::vector <unsigned char> data;

		/*! Must be created in the main thread.
		*/
		SCSIIOThread();

		/*! Send CMD_QUIT to the thread func and wait for the function to be done.
		*/
		~SCSIIOThread();

		void ThreadFunc(void);

		/*! Called from the main thread to check if the thread is busy.
		*/
		bool IsBusy(void) const;

		/*! Wait until the thread is ready.
		*/
		void WaitReady(void);

		/*! Called from the main thread.  Set up CMD_FILEREAD.
		    It will block until the thread is ready to take a command.
		*/
		void SetUpFileRead(std::string fName,uint64_t filePtr,uint64_t length);

		/*! Called from the main thread.  Set up CMD_FILEREAD.
		    It will block until the thread is ready to take a command.
		*/
		void SetUpCDRead(const DiscImage *discImgPtr,uint64_t LBA,uint64_t LEN);

		/*! Called from the main thread to get the data.
		    It returns nullptr if the data is not ready.
		*/
		const std::vector <unsigned char> *GetData(void);
	};


	enum
	{
		COMMAND_REQUEST_INTERVAL=500000,
		DATA_INTERVAL=500000,
		COMMAND_DELAY=500000,
		MESSAGE_DELAY=500000,
		STATUS_DELAY=500000,
		IOTHREAD_WAIT_INTERVAL=100000,
	};

	enum
	{
		HARDDISK_SECTOR_LENGTH=512,
		CDROM_SECTOR_LENGTH=2048
	};

	enum
	{
		PHASE_BUSFREE,
		PHASE_ARBITRATION,
		PHASE_SELECTION,
		PHASE_RESELECTION,
		PHASE_MESSAGE_OUT,
		PHASE_MESSAGE_IN,
		PHASE_COMMAND,
		PHASE_DATA_IN,
		PHASE_DATA_OUT,
		PHASE_STATUS,
	};
	enum
	{
		SCSIDEVICE_NONE,
		SCSIDEVICE_HARDDISK,
		SCSIDEVICE_CDROM,
	};
	enum
	{
		MAX_NUM_SCSIDEVICES=7,
		MAX_NUM_COMMAND_BYTES=64, // ?
	};

	// [9] 7.2 Command descriptor block
	enum
	{
		SCSICMD_TEST_UNIT_READY =0x00,  // [9] 8.2.16 TEST UNIT READY Command
		SCSICMD_REZERO_UNIT     =0x01,  // [9] 9.2.13 REZERO UNIT command
		SCSICMD_SENSE           =0x03,  // [9] 8.2.14 REQUEST SENSE Command
		SCSICMD_READ_6          =0x08,  // [9] 9.2.5 READ(6) command
		SCSICMD_WRITE_6         =0x0A,  // [9] 9.2.20 WRITE(6) command
		SCSICMD_SEEK_6          =0x0B,  // [9] 9.2.15 SEEK(6) and SEEK(10) commands
		SCSICMD_INQUIRY         =0x12,
		SCSICMD_PREVENT_REMOVAL =0x1E,  // [9] 9.2.4 PREVENT ALLOW MEDIUM REMOVAL command
		SCSICMD_READ_CAPACITY   =0x25,  // [9] 9.2.7 READ CAPACITY command
		SCSICMD_READ_10         =0x28,  // [9] 9.2.6 READ(10) command
		SCSICMD_WRITE_10        =0x2A,  // [9] 9.2.21 WRITE(10) command
		SCSICMD_SEEK_10         =0x2B,  // [9] 9.2.15 SEEK(6) and SEEK(10) commands
		SCSICMD_VERIFY_10       =0x2F,  // [9] 9.2.19 VERIFY command
		SCSICMD_READ_SUBCHANNEL =0x42,  // [9] 14.2.10 READ SUB-CHANNEL command
		SCSICMD_READTOC         =0x43,
		SCSICMD_PLAY_AUDIO_MSF  =0x47,
		SCSICMD_PAUSE_RESUME    =0x4B,  // [9] 14.2.1 PAUSE RESUME command.
		// When adding a support for command, don't forget to add commandLength[]= in the constructor.
	};

	// [9] Table 27 - Status byte code
	enum
	{
		STATUSCODE_GOOD                      =0,
		STATUSCODE_CHECK_CONDITION           =0x02,
		STATUSCODE_CONDITION_MET             =0x04,
		STATUSCODE_BUSY                      =0x08,
		STATUSCODE_INTERMEDIATE              =0x10,
		STATUSCODE_INTERMEDIATE_CONDITION_MET=0x14,
		STATUSCODE_RESERVATION_CONFLICT      =0x18,
		STATUSCODE_COMMAND_TERMINATED        =0x22,
		STATUSCODE_QUEUE_FULL                =0x28,
	};

	// [9] 8.2.14.3 Sense key and sense code definitions
	enum
	{
		SENSEKEY_NO_SENSE         =0x00,
		SENSEKEY_RECOVERED_ERROR  =0x01,
		SENSEKEY_NOT_READY        =0x02,
		SENSEKEY_MEDIUM_ERROR     =0x03,
		SENSEKEY_HARDWARE_ERROR   =0x04,
		SENSEKEY_ILLEGAL_REQUEST  =0x05,
		SENSEKEY_UNIT_ATTENTION   =0x06,
		SENSEKEY_DATA_PROTECT     =0x07,
		SENSEKEY_BLANK_CHECK      =0x08,
		SENSEKEY_VENDOR_SPECIFIC  =0x09,
		SENSEKEY_COPY_ABORTED     =0x0A,
		SENSEKEY_ABORTED_COMMAND  =0x0B,
		SENSEKEY_EQUAL            =0x0C,
		SENSEKEY_VOLUME_OVERFLOW  =0x0D,
		SENSEKEY_MISCOMPARE       =0x0E,
		SENSEKEY_RESERVED         =0x0F,
	};


	unsigned int commandLength[256];
	bool breakOnSCSICommand=false;
	bool breakOnDMATransfer=false;




	class SCSIDevice
	{
	public:
		unsigned int devType=SCSIDEVICE_NONE;
		std::string imageFName;
		long long int imageSize=0;
		DiscImage discImg;
	};

	class State
	{
	public:
		SCSIDevice dev[MAX_NUM_SCSIDEVICES];

		bool deviceConnected=false;

		unsigned int nCommandFilled=0;
		unsigned char commandBuffer[MAX_NUM_COMMAND_BYTES];
		unsigned int bytesTransferred=0;

		bool REQ,I_O,MSG,C_D,BUSY,INT,PERR;
		bool DMAE,SEL,ATN,IMSK,WEN;

		unsigned int selId;
		unsigned int phase=PHASE_BUSFREE;
		unsigned int lastDataByte=0;

		unsigned char status=0,message=0;
		unsigned int senseKey=0;

		void PowerOn(void);
		void Reset(void);
	};

	State state;
	bool monitorSCSICmd=false;

	SCSIIOThread ioThread;


	TownsSCSI(class FMTowns *townsPtr);

	void SetOutsideWorld(class Outside_World *ptr);

	virtual void PowerOn(void);
	virtual void Reset(void);

	bool LoadHardDiskImage(unsigned int scsiId,std::string fName);
	bool LoadCDImage(unsigned int scsiId,std::string fName);

	static std::string PhaseToStr(unsigned int phase);

	inline bool IRQEnabled(void)
	{
		// [2] tells IMSK meahs:
		//    true:  IRQ disabled
		//    false: IRQ enabled
		// It disagrees with the BIOS disassembly.
		return state.IMSK;
	}

	void SetUpIO_MSG_CDfromPhase(void);
	void EnterBusFreePhase(void);
	void EnterSelectionPhase(void);
	void EnterCommandPhase(void);
	void EnterDataInPhase(void);
	void EnterDataOutPhase(void);
	void EnterMessageInPhase(void);
	void EnterStatusPhase(void);

	virtual void IOWriteByte(unsigned int ioport,unsigned int data);

	virtual unsigned int IOReadByte(unsigned int ioport);

	virtual void RunScheduledTask(unsigned long long int townsTime);

	unsigned char PhaseReturnData(void);
	void ProcessPhaseData(unsigned int dataByte);
	void ExecSCSICommand(void);

	std::vector <unsigned char> MakeInquiryData(int scsiId) const;
	std::vector <unsigned char> MakeReadCapacityData(int scsiId) const;
	std::vector <unsigned char> MakeTOCData(int scsiId,unsigned int startTrack,unsigned int allocSize,bool MSF) const;

	std::vector <std::string> GetStatusText(void) const;


	virtual uint32_t SerializeVersion(void) const;
	virtual void SpecificSerialize(std::vector <unsigned char> &data,std::string stateFName) const;
	virtual bool SpecificDeserialize(const unsigned char *&data,std::string stateFName,uint32_t version);
};

/* } */
#endif
