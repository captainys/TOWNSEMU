#ifndef PIC_IS_INCLUDED
#define PIC_IS_INCLUDED
/* { */

#include <vector>
#include <string>

#include "device.h"
#include "i486.h"
#include "memory.h"
#include "townsdef.h"



// PIC?  Pilot In Command?
class TownsPIC : public Device
{
public:
	class I8259A
	{
	public:
		enum
		{
			TRIGGER_EDGE,
			TRIGGER_LEVEL
		};

		unsigned char IRR,ISR,IMR;
		unsigned char OCW[3];
		unsigned char ICW[4];
		unsigned int highestPriorityInt;
		unsigned int init_stage;
		unsigned int ocw_stage;

		bool SMM; // Special Mask Mode: Use IMR as a mask for ISR instead of IRR
		bool autoRotateOnAEOI;

		inline unsigned char &ICW1(void){return ICW[0];}
		inline unsigned char &ICW2(void){return ICW[1];}
		inline unsigned char &ICW3(void){return ICW[2];}
		inline unsigned char &ICW4(void){return ICW[3];}
		inline unsigned char &OCW1(void){return OCW[0];}
		inline unsigned char &OCW2(void){return OCW[1];}
		inline unsigned char &OCW3(void){return OCW[2];}

		inline unsigned char ICW1(void) const{return ICW[0];}
		inline unsigned char ICW2(void) const{return ICW[1];}
		inline unsigned char ICW3(void) const{return ICW[2];}
		inline unsigned char ICW4(void) const{return ICW[3];}
		inline unsigned char OCW1(void) const{return OCW[0];}
		inline unsigned char OCW2(void) const{return OCW[1];}
		inline unsigned char OCW3(void) const{return OCW[2];}


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

		/*! Bit2 of OCW3 */
		bool PollingMode(void) const;

		/*! Bit 1 of ICW4 */
		bool AutoEOIMode(void) const;

		/*! LTIM bit of ICW1 */
		unsigned int TriggerMode(void) const;

		/*! Returns the highest-priority INT in service. */
		unsigned int GetHighestPriorityINTInService(void) const;

		/*! Set IRR based on the current configuration and intNum.
		    To set an IRR bit, request is true.
		    To clear an IRR bit, request is false.
		*/
		void SetInterruptRequestBit(unsigned int intNum,bool request);

		/*! Returns the highest-priority INT (0 to 7) that IRR is on and ISR is off.
		    If no INT is ready to go, it returns 0xffffffff.
		*/
		unsigned int INTToGo(void) const;

		/*! Fires an IRQ.  This function should be called after IRQ mask checked, and CPU's IF checked.
		    This function just fires up an IRQ without checking anything.
		*/
		void FireIRQ(i486DX &cpu,Memory &mem,unsigned int INTToGo);
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

	/*! intNum must be 0 to 15.  Bit 3 will identify which chip request is sent.
	    To set an IRR bit, request is true.
	    To clear an IRR bit, request is false.
	*/
	void SetInterruptRequestBit(unsigned int intNum,bool request);

	std::vector <std::string> GetStateText(void) const;

	inline void ProcessIRQ(i486DX &cpu,Memory &mem)
	{
		if(cpu.GetIF() && (0!=state.i8259A[0].IRR || 0!=state.i8259A[1].IRR))
		{
			unsigned int chip=0;
			if(7==state.i8259A[0].highestPriorityInt) // in which case i8259A[1], then i8259A[0].
			{
				chip=1;
			}
			auto INTToGo=state.i8259A[chip].INTToGo();
			if(8<=INTToGo && 0==state.i8259A[chip].ISR)
			{
				chip=1-chip;
				INTToGo=state.i8259A[chip].INTToGo();
			}
			if(INTToGo<8)
			{
				state.i8259A[chip].FireIRQ(cpu,mem,INTToGo);
			}
		}
	}
};

/* } */
#endif
