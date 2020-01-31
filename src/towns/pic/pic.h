#ifndef PIC_IS_INCLUDED
#define PIC_IS_INCLUDED
/* { */

#include <vector>
#include <string>

#include "device.h"
#include "townsdef.h"



// PIC?  Pilot In Command?
class TownsPIC : public Device
{
public:
	class I8259A
	{
	public:
		unsigned char IRR_ISR,IMR;
		unsigned char OCW[3];
		unsigned char ICW[4];
		unsigned int init_stage;
		unsigned int ocw_stage;
		void Reset(void);
		void WriteReg0(unsigned char data);

		/*! Returns true when ICW4 is written.
		*/
		bool WriteReg1(unsigned char data);

		/*! Returns A (A7-A5 in ICW[0], A15-A18 in ICW[1])
		*/
		inline unsigned int GetA(void) const
		{
			return (ICW[1]<<8)+(ICW[0]&0xE0);
		}

		/*! Returns T (T7-T3 in ICW[1])
		*/
		inline unsigned int GetT(void) const
		{
			return ICW[1]&0xF8;
		}
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

	std::vector <std::string> GetStateText(void) const;
};


/* } */
#endif
