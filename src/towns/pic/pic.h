/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#ifndef PIC_IS_INCLUDED
#define PIC_IS_INCLUDED
/* { */

#include <vector>
#include <string>

#include "device.h"
#include "i486.h"
#include "i486debug.h"
#include "memory.h"
#include "townsdef.h"



// PIC?  Pilot In Command?
class TownsPIC : public Device
{
public:
	class I8259A
	{
	friend class TownsPIC;

	public:
		enum
		{
			TRIGGER_EDGE,
			TRIGGER_LEVEL
		};
		enum
		{
			ICW4_MUPM=    0x01,
			ICW4_AEOI_BIT=0x02,
			ICW4_MS_BIT=  0x04,
			ICW4_BUF_BIT= 0x08,
			ICW4_SFNM_BIT=0x10,
		};

	private:
		TownsPIC *owner=nullptr;
		unsigned char IRR;
	public:
		unsigned char ISR,IMR;
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

		/*!
		*/
		bool GetInterruptRequestBit(unsigned int intNum) const;

		/*! Returns the highest-priority INT (0 to 7) that IRR is on and ISR is off.
		    If no INT is ready to go, it returns 0xffffffff.
		*/
		unsigned int INTToGo(void) const;

		/*! Returns true if SFNM bit of ICW4 is set.
		*/
		inline bool SpecialFullyNestedMode(void) const
		{
			return 0!=(ICW[3]&ICW4_SFNM_BIT);
		}

		bool HigherPriorityINTIsInService(unsigned int INTNum) const;

		/*! Fires an IRQ.  This function should be called after IRQ mask checked, and CPU's IF checked.
		    This function just fires up an IRQ without checking anything.
		*/
		void FireIRQ(i486DXCommon &cpu,Memory &mem,unsigned int INTToGo);

		void Serialize(std::vector <unsigned char> &data) const;
		bool Deserialize(const unsigned char *&data,uint32_t version);
	};
	class State
	{
	public:
		I8259A i8259A[2];

		mutable unsigned char IRR0_OR_IRR1_Cache=0; // Not saved in the state file.  Updated after state is loaded.
		void UpdateIRRCache(void) const
		{
			IRR0_OR_IRR1_Cache=(i8259A[0].IRR|i8259A[1].IRR);
		}

		void Reset(void);
	};

	State state;
	class FMTownsCommon *townsPtr;
	bool debugBreakOnICW1Write,debugBreakOnICW4Write;

	TownsPIC(class FMTownsCommon *townsPtr);

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

	bool GetInterruptRequestBit(unsigned int intNum) const;

	std::vector <std::string> GetStateText(void) const;

	inline void ProcessIRQ(i486DXCommon &cpu,Memory &mem)
	{
		if(0!=state.IRR0_OR_IRR1_Cache && cpu.GetIF() && false==cpu.state.holdIRQ)
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
				if(true==state.i8259A[chip].SpecialFullyNestedMode())
				{
					if(true==state.i8259A[chip].HigherPriorityINTIsInService(INTToGo))
					{
						return;
					}
				}
				state.i8259A[chip].FireIRQ(cpu,mem,INTToGo);
				// The second condition is needed for stopping when IRQ handler happens to be monitor point.
				// Otherwise, lastBreakPoint flags will be overwritten and won't break.
				if(nullptr!=cpu.debuggerPtr && true!=cpu.debuggerPtr->stop)
				{
					cpu.debuggerPtr->CheckForBreakPoints(cpu);
				}
			}
		}
	}


	virtual uint32_t SerializeVersion(void) const;
	virtual void SpecificSerialize(std::vector <unsigned char> &data,std::string stateFName) const;
	virtual bool SpecificDeserialize(const unsigned char *&data,std::string stateFName,uint32_t version);
};

/* } */
#endif
