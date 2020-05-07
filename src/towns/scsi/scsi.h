#ifndef SCSI_IS_INCLUDED
#define SCSI_IS_INCLUDED
/* { */

#include <vector>
#include <string>

class TownsSCSI : public Device
{
private:
	class FMTowns *townsPtr;
public:
	virtual const char *DeviceName(void) const{return "SCSI";}

	enum
	{
		COMMAND_REQUEST_INTERVAL=500,
		DATA_INTERVAL=500,
		COMMAND_DELAY=500,
		MESSAGE_DELAY=500,
		STATUS_DELAY=500,
	};

	enum
	{
		HARDDISK_SECTOR_LENGTH=512
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
		SCSICMD_INQUIRY         =0x12,
		SCSICMD_READ_CAPACITY   =0x25,  // [9] 9.2.7 READ CAPACITY command
		SCSICMD_READ_10         =0x28,  // [9] 9.2.6 READ(10) command
		SCSICMD_WRITE_10        =0x2A,  // [9] 9.2.21 WRITE(10) command
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
	};

	class State
	{
	public:
		SCSIDevice dev[MAX_NUM_SCSIDEVICES];

		bool deviceConnected=false;

		unsigned int nCommandFilled=0;
		unsigned char commandBuffer[MAX_NUM_COMMAND_BYTES];

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

	TownsSCSI(class FMTowns *townsPtr);

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

	std::vector <std::string> GetStatusText(void) const;
};

/* } */
#endif
