#ifndef SCSI_IS_INCLUDED
#define SCSI_IS_INCLUDED
/* { */

class TownsSCSI : public Device
{
private:
	class FMTowns *townsPtr;
public:
	virtual const char *DeviceName(void) const{return "SCSI";}

	class State
	{
	public:
		bool REQ,I_O,MSG,C_D,BUSY,INT,PERR;
		bool DMAE,SEL,ATN,IMSK,WEN;

		void PowerOn(void);
		void Reset(void);
	};

	State state;

	TownsSCSI(class FMTowns *townsPtr);

	virtual void PowerOn(void);
	virtual void Reset(void);

	virtual void IOWriteByte(unsigned int ioport,unsigned int data);

	virtual unsigned int IOReadByte(unsigned int ioport);
};

/* } */
#endif
