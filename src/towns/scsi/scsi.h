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
	};

	class SCSIDevice
	{
	public:
		unsigned int devType=SCSIDEVICE_NONE;
	};

	class State
	{
	public:
		SCSIDevice dev[MAX_NUM_SCSIDEVICES];

		bool REQ,I_O,MSG,C_D,BUSY,INT,PERR;
		bool DMAE,SEL,ATN,IMSK,WEN;

		unsigned int selId;
		unsigned int phase=PHASE_BUSFREE;
		unsigned int lastDataByte=0;

		void PowerOn(void);
		void Reset(void);
	};

	State state;

	TownsSCSI(class FMTowns *townsPtr);

	virtual void PowerOn(void);
	virtual void Reset(void);

	static std::string PhaseToStr(unsigned int phase);

	void SetUpIO_MSG_CDfromPhase(void);
	void EnterSelectionPhase(void);
	void EndSelectionPhase(void);

	virtual void IOWriteByte(unsigned int ioport,unsigned int data);

	virtual unsigned int IOReadByte(unsigned int ioport);

	std::vector <std::string> GetStatusText(void) const;
};

/* } */
#endif
