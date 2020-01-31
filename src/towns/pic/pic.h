#ifndef PIC_IS_INCLUDED
#define PIC_IS_INCLUDED
/* { */

#include "device.h"
#include "townsdef.h"



// PIC?  Pilot In Command?
class TownsPIC : public Device
{
public:
	class I8259A
	{
	public:
		unsigned char IRR_ISR,IMR,OCW;
		unsigned char ICW[4];
		unsigned int init_stage;
		void Reset(void);
		void WriteReg0(unsigned char data);

		/*! Returns true when ICW4 is written.
		*/
		bool WriteReg1(unsigned char data);
	};
	class State
	{
	public:
		I8259A i8259A[2];
		void Reset(void);
	};

	State state;
	class FMTowns *townsPtr;
	bool debugBreakOnICW1Write,debugBreakOnICW4Write;

	TownsPIC(class FMTowns *townsPtr);

	virtual const char *DeviceName(void) const{return "PIC";}

	virtual void PowerOn(void);
	virtual void Reset(void);

	virtual void IOWriteByte(unsigned int ioport,unsigned int data);
	virtual unsigned int IOReadByte(unsigned int ioport);
};


/* } */
#endif
