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
#include <fstream>
#include <string.h> // for memcpy
#include "i486.h"



class i486Debugger
{
public:
	enum
	{
		CSEIP_LOG_SIZE=0x10000,
		CSEIP_LOG_MASK=0x0FFFF,

		/* Windows 3.1 uses INT 20H for VxD services.
		*/
		INT_WIN31_VxD=0x20,

		/* Windows 3.1 and 95 uses INT 2FH, AX=1684h for getting API entry from 16-bit process. */
		INT_WIN_DPMI_2FH=0x2F,

		DPMI_GET_API_ENTRY=0x1684,
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

		/* If set, it will print only CS:EIP and disassembly when it passes or stop at the breakpoint.
		*/
		BRKPNT_FLAG_SHORT_FORMAT=4,

		/*
		*/
		BRKPNT_FLAG_DONT_STOP=8,

		/* If set, break point will clear after breaking once.
		*/
		BRKPNT_FLAG_ONE_TIME=16,

		/* If set, it captures Windows 3.1/95 API Entry Point from ES:DI.
		*/
		BRKPNT_FLAG_CAPTURE_WIN_APIENTRY=32,
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
		/*
		*/
		uint16_t win_vmmId=0;
		/* Save machine state at this break point.
		*/
		std::string saveState;

		inline void Clear(void)
		{
			flags=0;
			win_vmmId=0;
			passCountUntilBreak=0;
			passedCount=0;
			saveState="";
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

	typedef i486DXCommon::FarPointer CS_EIP;

	std::map <CS_EIP,BreakPointInfo> breakPoints;
	BreakPointInfo lastBreakPointInfo;
	bool breakOnCS[65536];
	std::vector <bool> breakOnIORead,breakOnIOWrite;
	CS_EIP oneTimeBreakPoint;
	std::string externalBreakReason,additionalDisasm;
	std::map <unsigned int,std::string> ioLabel;

	std::map <unsigned int,std::string> VxDLabel;
	bool autoAnnotateVxDCall=false;
	std::map <unsigned int,std::map <unsigned int,std::string> > VxDFuncLabel;

	bool breakOnVM86Mode=false,prevVM86Mode=false;
	bool breakOnProtectedMode=false,prevProtectedMode=false;
	bool breakOnRealMode=false,prevRealMode=false;

	uint32_t breakOnCallStackDepth=0x7fffffff;

	// Monitoring Windows 3.1 VxD
	uint8_t breakOrMonitorOnVxDCall=BRKPNT_FLAG_NONE;
	uint32_t breakOnVxDId=~0,breakOnVxDServiceNumber=~0;

	uint32_t instHist[0xFFFF];

	class CSEIPLogType : public i486DXCommon::FarPointer
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
	std::ofstream logOfs;

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

		// If CSEIP.size()==0, any CS:EIP.
		// Else 00000000|EIP or CS|EIP.
		std::set <uint64_t> CSEIP;
	};
	BreakOnINTCondition breakOnINT[BreakOnINTCondition::NUM_INTERRUPTS];
	mutable bool stop;
	bool monitorIO;
	bool monitorIOports[65536];
	bool disassembleEveryStep=false,regDumpEveryStep=false;;

	CS_EIP lastDisassembleAddr;


	i486Debugger();
	~i486Debugger();
	void CleanUp(void);

	void AddBreakPoint(CS_EIP bp,BreakPointInfo info);
	void RemoveBreakPoint(CS_EIP bp);
	void ClearBreakPoints(void);
	void ClearCSBreakPoints(void);
	std::vector <CS_EIP> GetBreakPoints(void) const;
	std::vector <std::pair<CS_EIP,BreakPointInfo> > GetBreakPointsEx(void) const;
	std::vector <uint16_t> GetCSBreakPoints(void) const;

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

public:
	bool inInstruction=false;  // Set in BeforeRunOneInstruction and cleared in AfterRunOneInstruction
	class ExceptionVerification
	{
	public:
		bool dontCheckBecauseREP=false;
		uint32_t NULL_and_reg32[9];
		uint32_t EIP;
		uint16_t CS,DS,ES,FS,GS,SS;
		inline void SaveCPUState(const class i486DXCommon &cpu,unsigned int prefix)
		{
			dontCheckBecauseREP=(prefix==i486DXCommon::INST_PREFIX_REP ||  // =  0xF3, // REP/REPE/REPZ
			                     prefix==i486DXCommon::INST_PREFIX_REPE || // = 0xF3, // REP/REPE/REPZ
			                     prefix==i486DXCommon::INST_PREFIX_REPNE); // =0xF2, // REPNE/REPNZ

			memcpy(NULL_and_reg32,cpu.state.NULL_and_reg32,sizeof(NULL_and_reg32));
			EIP=cpu.state.EIP;
			CS=cpu.state.CS().value;
			DS=cpu.state.DS().value;
			ES=cpu.state.ES().value;
			FS=cpu.state.FS().value;
			GS=cpu.state.GS().value;
			SS=cpu.state.SS().value;
		};

		void PrintChanges(const class i486DXCommon &cpu) const
		{
			for(int j=1; j<8; ++j)
			{
				std::cout << "REG32_" << j-1 << " "<<
				    cpputil::Uitox(NULL_and_reg32[j]) << "->" << 
				    cpputil::Uitox(cpu.state.NULL_and_reg32[j]) << std::endl;
			}

			std::cout << "EIP " <<
				cpputil::Uitox(EIP) << "->" << cpputil::Ustox(cpu.state.EIP) << std::endl;
			std::cout << "CS " <<
				cpputil::Ustox(CS ) << "->" << cpputil::Ustox(cpu.state.CS().value) << std::endl;
			std::cout << "DS " <<
				cpputil::Ustox(DS ) << "->" << cpputil::Ustox(cpu.state.DS().value) << std::endl;
			std::cout << "ES " <<
				cpputil::Ustox(ES ) << "->" << cpputil::Ustox(cpu.state.ES().value) << std::endl;
			std::cout << "FS " <<
				cpputil::Ustox(FS ) << "->" << cpputil::Ustox(cpu.state.FS().value) << std::endl;
			std::cout << "GS " <<
				cpputil::Ustox(GS ) << "->" << cpputil::Ustox(cpu.state.GS().value) << std::endl;
			std::cout << "SS " <<
				cpputil::Ustox(SS ) << "->" << cpputil::Ustox(cpu.state.SS().value) << std::endl;
		}

		inline bool CPUStateChanged(const class i486DXCommon &cpu) const
		{
			if(true!=dontCheckBecauseREP)
			{
				for(int i=1; i<9; ++i)
				{
					if(NULL_and_reg32[i]!=cpu.state.NULL_and_reg32[i])
					{
						PrintChanges(cpu);
						return true;
					}
				}
				if(EIP!=cpu.state.EIP ||
				   CS!=cpu.state.CS().value ||
				   DS!=cpu.state.DS().value ||
				   ES!=cpu.state.ES().value ||
				   FS!=cpu.state.FS().value ||
				   GS!=cpu.state.GS().value ||
				   SS!=cpu.state.SS().value
				   )
				{
					return true;
				}
			}
			return false;
		}
	};
	ExceptionVerification verifyException;


public:
	/*! Callback from i486DXCommon::RunOneInstruction.
	*/
	void BeforeRunOneInstruction(i486DXCommon &cpu,Memory &mem,InOut &io,const i486DXCommon::Instruction &inst);

	/*! Callback from i486DXCommon::RunOneInstruction.
	*/
	void AfterRunOneInstruction(unsigned int clocksPassed,i486DXCommon &cpu,Memory &mem,InOut &io,const i486DXCommon::Instruction &inst,const i486DXCommon::Operand &op1,const i486DXCommon::Operand &op2);

	/*! Check for break points. 
	*/
	void CheckForBreakPoints(i486DXCommon &cpu);

	/*!
	*/
	void HandleException(i486DXCommon &cpu,Memory &mem,unsigned int instNumBytes);

	/*! Break on INT
	*/
	void SetBreakOnINT(unsigned int IRQNum);
	void SetBreakOnFOpen(std::string fName);
	void SetBreakOnINTwithAH(unsigned int IRQNum,unsigned int AH);
	void SetBreakOnINTwithAX(unsigned int IRQNum,unsigned int AX);
	void AddBreakOnINT_CSEIP(unsigned int IRQNum,uint32_t CS,uint32_t EIP);
	void AddBreakOnINT_EIP(unsigned int IRQNum,uint32_t EIP);

	/*! Clear break on INT
	*/
	void ClearBreakOnINT(void);
	void ClearBreakOnINT(unsigned int INTNum);

	/*! Return formatted call-stack text.
	*/
	std::vector <std::string> GetCallStackText(const i486DXCommon &cpu) const;

	/*! Break from an external reason.
	    It sets stop==1 and externalBreakReason=reason.
	    It also clears lastBreakPointInfo.
	*/
	void ExternalBreak(const std::string &reason);

	/*! Clear stop flag and externalBreakReason.
	    It also clears lastBreakPointInfo.
	*/
	void ClearStopFlag(void);

	/*! Called from i486DXCommon class when an interrupt is shot.
	*/
	void Interrupt(const i486DXCommon &cpu,unsigned int INTNum,Memory &mem,unsigned int numInstBytes);
private:
	/*!
	*/
	std::string INTExplanation(const i486DXCommon &cpu,unsigned int INTNum,Memory &mem) const;

public:
	/*! Called from i486DXCommon class when data is written to I/O.
	*/
	void IOWrite(const i486DXCommon &cpu,unsigned int ioport,unsigned int data,unsigned int lengthInBytes);

	/*! Called from i486DXCommon class when data is read from I/O.
	*/
	void IORead(const i486DXCommon &cpu,unsigned int ioport,unsigned int data,unsigned int lengthInBytes);

	/*!
	*/
	void MonitorIO(unsigned short portMin,unsigned short portMax);

	/*!
	*/
	void UnmonitorIO(unsigned short portMin,unsigned short portMax);
private:
	bool AtLeastOneMonitorIOPortIsSet(void) const;


public:
	std::vector <unsigned int> FindCaller(unsigned int procAddr,const i486DXCommon::SegmentRegister &seg,const i486DXCommon &cpu,const Memory &mem);

	void SetBreakOnCallStackDepth(uint32_t depth);
	void ClearBreakOnCallStackDepth(void);

	void MakeVxDLabelTable(void);
	void AutoAnnotateVxD(const i486DXCommon &cpu,const Memory &mem,const i486DXCommon::SegmentRegister &reg,uint32_t EIP);

	bool OpenLogFile(std::string logFileName);
	std::ofstream &LogFileStream(void);
	void CloseLogFile(void);
	void WriteLogFile(std::string str);
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
