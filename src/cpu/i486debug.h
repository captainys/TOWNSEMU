/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#ifndef I486DEBUG_IS_INCLUDED
#define I486DEBUG_IS_INCLUDED
/* { */


#include <string>
#include <set>
#include <map>
#include "i486.h"



class i486Debugger
{
public:
	enum
	{
		CSEIP_LOG_SIZE=0x10000,
		CSEIP_LOG_MASK=0x0FFFF
	};

	enum
	{
		BRKPNT_FLAG_NONE=0,

		/* If BRKPNT_FLAG_MONITOR_STATUS is set, the debugger will break the execution.
		   The Virtual Machine thread should check lastBreakPointInfo for the break-point
		   flags, and if BRKPNT_FLAG_MONITOR_STATUS is set, print the machine state and
		   resume execution.
		   Since the debugger does not know all the Virtual Machine state, the debugger
		   cannot print all the information.
		*/
		BRKPNT_FLAG_MONITOR_ONLY=1,

		/* If BRKPNT_FLAG_MONITOR_BEFORE_PASSCOUNT is set,
		   debugger should print the monitoring information before the pass count is reached.
		*/
		BRKPNT_FLAG_SILENT_UNTIL_BREAK=2,
	};
	class BreakPointInfo
	{
	public:
		uint32_t flags=0;
		/* passCountUntilBreak is decremented IF BRKPNT_FLAG_MONITOR_ONLY is clear.
		*/
		uint64_t passCountUntilBreak=0;
		/* passedCount is always incremented when CS:EIP matches the breakpoint.
		*/
		uint64_t passedCount=0;
		inline void Clear(void)
		{
			flags=0;
			passCountUntilBreak=0;
			passedCount=0;
		}

		/*! Call this function when reached the break point.
		*/
		inline void SteppedOn(void)
		{
			++passedCount;
			if(0==(flags&BRKPNT_FLAG_MONITOR_ONLY))
			{
				if(0!=passCountUntilBreak)
				{
					--passCountUntilBreak;
				}
			}
		}

		/*! Returns true if VM should break.
		*/
		inline bool ShouldBreak(void) const
		{
			return (0==(flags&BRKPNT_FLAG_MONITOR_ONLY) && 0==passCountUntilBreak);
		}
	};

	typedef i486DX::FarPointer CS_EIP;

	std::map <CS_EIP,BreakPointInfo> breakPoints;
	BreakPointInfo lastBreakPointInfo;
	bool breakOnCS[65536];
	std::vector <bool> breakOnIORead,breakOnIOWrite;
	CS_EIP oneTimeBreakPoint;
	std::string externalBreakReason;
	std::map <unsigned int,std::string> ioLabel;

	uint32_t instHist[0xFFFF];

	class CSEIPLogType : public i486DX::FarPointer
	{
	public:
		unsigned int SS,ESP;
		size_t count;
		bool operator==(const CSEIPLogType &from)
		{
			return (from.SEG==SEG && from.OFFSET==OFFSET);
		}
		bool operator!=(const CSEIPLogType &from)
		{
			return (from.SEG!=SEG || from.OFFSET!=OFFSET);
		}
	};
	size_t CSEIPLogPtr;
	std::vector <CSEIPLogType> CSEIPLog;

private:
	class i486SymbolTable *symTablePtr;
public:
	i486SymbolTable &GetSymTable(void);
	const i486SymbolTable &GetSymTable(void) const;

public:
	std::map <unsigned int,std::string> &GetIOTable(void);
	const std::map <unsigned int,std::string> &GetIOTable(void) const;


	class SpecialDebugInfo;
	SpecialDebugInfo *specialDebugInfo;


	class BreakOnINTCondition
	{
	public:
		enum
		{
			NUM_INTERRUPTS=256
		};
		enum
		{
			COND_NEVER,
			COND_ALWAYS,
			COND_AH,
			COND_AX,
			COND_FOPEN_FCREATE,
		};
		unsigned short cond=COND_NEVER;
		unsigned short condValue=0;
		std::string fName;
	};
	BreakOnINTCondition breakOnINT[BreakOnINTCondition::NUM_INTERRUPTS];
	mutable bool stop;
	bool monitorIO;
	bool monitorIOports[65536];
	bool disassembleEveryStep;

	CS_EIP lastDisassembleAddr;


	i486Debugger();
	~i486Debugger();
	void CleanUp(void);

	void AddBreakPoint(CS_EIP bp,BreakPointInfo info);
	void RemoveBreakPoint(CS_EIP bp);
	void ClearBreakPoints(void);
	std::vector <CS_EIP> GetBreakPoints(void) const;

	void AddBreakOnIORead(unsigned int ioport);
	void RemoveBreakOnIORead(unsigned int ioport);
	void RemoveBreakOnIORead(void);
	void AddBreakOnIOWrite(unsigned int ioport);
	void RemoveBreakOnIOWrite(unsigned int ioport);
	void RemoveBreakOnIOWrite(void);
	const std::vector <unsigned int> GetBreakOnIORead(void) const;
	const std::vector <unsigned int> GetBreakOnIOWrite(void) const;

	std::vector <CSEIPLogType> GetCSEIPLog(void);
	std::vector <CSEIPLogType> GetCSEIPLog(unsigned int steps);

	void SetOneTimeBreakPoint(unsigned int CS,unsigned int EIP);

	/*! Callback from i486DX::RunOneInstruction.
	*/
	void BeforeRunOneInstruction(i486DX &cpu,Memory &mem,InOut &io,const i486DX::Instruction &inst);

	/*! Callback from i486DX::RunOneInstruction.
	*/
	void AfterRunOneInstruction(unsigned int clocksPassed,i486DX &cpu,Memory &mem,InOut &io,const i486DX::Instruction &inst);

	/*! Check for break points. 
	*/
	void CheckForBreakPoints(i486DX &cpu);

	/*! Break on INT
	*/
	void SetBreakOnINT(unsigned int IRQNum);
	void SetBreakOnFOpen(std::string fName);
	void SetBreakOnINTwithAH(unsigned int IRQNum,unsigned int AH);
	void SetBreakOnINTwithAX(unsigned int IRQNum,unsigned int AX);

	/*! Clear break on INT
	*/
	void ClearBreakOnINT(void);
	void ClearBreakOnINT(unsigned int INTNum);

	/*! Return formatted call-stack text.
	*/
	std::vector <std::string> GetCallStackText(const i486DX &cpu) const;

	/*! Break from an external reason.
	    It sets stop==1 and externalBreakReason=reason.
	    It also clears lastBreakPointInfo.
	*/
	void ExternalBreak(const std::string &reason);

	/*! Clear stop flag and externalBreakReason.
	    It also clears lastBreakPointInfo.
	*/
	void ClearStopFlag(void);

	/*! Called from i486DX class when an interrupt is shot.
	*/
	void Interrupt(const i486DX &cpu,unsigned int INTNum,Memory &mem,unsigned int numInstBytes);
private:
	/*!
	*/
	std::string INTExplanation(const i486DX &cpu,unsigned int INTNum,Memory &mem) const;

public:
	/*! Called from i486DX class when data is written to I/O.
	*/
	void IOWrite(const i486DX &cpu,unsigned int ioport,unsigned int data,unsigned int lengthInBytes);

	/*! Called from i486DX class when data is read from I/O.
	*/
	void IORead(const i486DX &cpu,unsigned int ioport,unsigned int data,unsigned int lengthInBytes);

	/*!
	*/
	void MonitorIO(unsigned short portMin,unsigned short portMax);

	/*!
	*/
	void UnmonitorIO(unsigned short portMin,unsigned short portMax);
private:
	bool AtLeastOneMonitorIOPortIsSet(void) const;


public:
	std::vector <unsigned int> FindCaller(unsigned int procAddr,const i486DX::SegmentRegister &seg,const i486DX &cpu,const Memory &mem);
};

template <>
struct std::hash <i486Debugger::CS_EIP>
{
	std::size_t operator()(const i486Debugger::CS_EIP &bp) const noexcept
	{
		size_t high=bp.SEG;
		size_t low=bp.OFFSET;
		return (high<<32)|low;
	}
};

/* } */
#endif
