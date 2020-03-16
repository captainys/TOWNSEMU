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

	typedef i486DX::FarPointer CS_EIP;

	std::set <CS_EIP> breakPoints;
	std::vector <bool> breakOnIORead,breakOnIOWrite;
	CS_EIP oneTimeBreakPoint;
	std::string externalBreakReason;
	std::map <unsigned int,std::string> ioLabel;

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

	class SpecialDebugInfo;
	SpecialDebugInfo *specialDebugInfo;


	unsigned int breakOnINT;
	bool stop;
	bool monitorIO;
	bool disassembleEveryStep;

	CS_EIP lastDisassembleAddr;


	i486Debugger();
	~i486Debugger();
	void CleanUp(void);

	void AddBreakPoint(CS_EIP bp);
	void RemoveBreakPoint(CS_EIP bp);
	void ClearBreakPoints(void);
	std::vector <CS_EIP> GetBreakPoints(void) const;

	void AddBreakOnIORead(unsigned int ioport);
	void RemoveBreakOnIORead(unsigned int ioport);
	void AddBreakOnIOWrite(unsigned int ioport);
	void RemoveBreakOnIOWrite(unsigned int ioport);
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

	/*! Clear break on INT
	*/
	void ClearBreakOnINT(void);

	/*! Return formatted call-stack text.
	*/
	std::vector <std::string> GetCallStackText(const i486DX &cpu) const;

	/*! Break from an external reason.
	    It sets stop==1 and externalBreakReason=reason.
	*/
	void ExternalBreak(const std::string &reason);

	/*! Clear stop flag and externalBreakReason.
	*/
	void ClearStopFlag(void);

	/*! Called from i486DX class when an interrupt is shot.
	*/
	void Interrupt(const i486DX &cpu,unsigned int INTNum,Memory &mem,unsigned int numInstBytes);

	/*! Called from i486DX when memory is written.
	*/
	void MemWrite(const i486DX &cpu,const i486DX::SegmentRegister &seg,unsigned int offset,unsigned int linear,unsigned int physical,unsigned int data,unsigned int lengthInBytes);

	/*! Called from i486DX class when data is written to I/O.
	*/
	void IOWrite(const i486DX &cpu,unsigned int ioport,unsigned int data,unsigned int lengthInBytes);

	/*! Called from i486DX class when data is read from I/O.
	*/
	void IORead(const i486DX &cpu,unsigned int ioport,unsigned int data,unsigned int lengthInBytes);

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
