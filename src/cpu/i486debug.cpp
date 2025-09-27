/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include <iostream>
#include <fstream>
#include <stdint.h>

#include "i486.h"
#include "i486inst.h"
#include "i486debug.h"
#include "i486symtable.h"

#include "cpputil.h"



class i486Debugger::SpecialDebugInfo
{
public:
	// 2020/02/08
	//   Broken Timer Interrupt Problem
	//   Infinite loop waiting for the Timer Interrupt to clear [0443H]
	//     03A4:00000D16 B410                      MOV     AH,10H
	//     03A4:00000D18 CD97                      INT     97H            <- �^�C�}�[�d�|�� �����J�@�\
	//     03A4:00000D1A 0AE4                      OR      AH,AH
	//     03A4:00000D1C 75F8                      JNE     00000D16
	//     03A4:00000D1E F606430480                TEST    BYTE PTR [0443H],80H
	//     03A4:00000D23 75F9                      JNE     00000D1E
	//   Another pattern is jump to CS=7501H where no instructions are written.
	unsigned int passCount;
	unsigned int int40HCount,otherIntCount;
	bool inTheTargetRange;
	bool enableSpecialBreak3,pass03A4_00000D26;

	unsigned int SS,SP,pass223501f0=0;

	SpecialDebugInfo()
	{
		passCount=0;
		inTheTargetRange=false;
		enableSpecialBreak3=true;
	}

	void BeforeRunOneInstruction(i486Debugger &debugger,i486DXCommon &cpu,Memory &mem,InOut &io,const i486DXCommon::Instruction &inst);
	void AfterRunOneInstruction(i486Debugger &debugger,unsigned int clocksPassed,i486DXCommon &cpu,Memory &mem,InOut &io,const i486DXCommon::Instruction &inst);
	void Interrupt(i486Debugger &debugger,const i486DXCommon &cpu,unsigned int INTNum,Memory &mem,unsigned int numInstBytes);

	void IOWrite(i486Debugger &debugger,const i486DXCommon &cpu,unsigned int ioport,unsigned int data,unsigned int lengthInBytes);
	void IORead(i486Debugger &debugger,const i486DXCommon &cpu,unsigned int ioport,unsigned int data,unsigned int lengthInBytes);
	std::vector <std::string> GetText(void) const;
};

void i486Debugger::SpecialDebugInfo::BeforeRunOneInstruction(i486Debugger &debugger,i486DXCommon &cpu,Memory &mem,InOut &io,const i486DXCommon::Instruction &inst)
{
}
void i486Debugger::SpecialDebugInfo::AfterRunOneInstruction(i486Debugger &debugger,unsigned int clocksPassed,i486DXCommon &cpu,Memory &mem,InOut &io,const i486DXCommon::Instruction &inst)
{
}
void i486Debugger::SpecialDebugInfo::Interrupt(i486Debugger &debugger,const i486DXCommon &cpu,unsigned int INTNum,Memory &mem,unsigned int numInstBytes)
{
}
void i486Debugger::SpecialDebugInfo::IOWrite(i486Debugger &debugger,const i486DXCommon &cpu,unsigned int ioport,unsigned int data,unsigned int lengthInBytes)
{
	/* if(0x3150==ioport || 0x3A5C==ioport || 0x3188==ioport)
	{
		debugger.ExternalBreak("Special Break IO Write "+cpputil::Uitox(ioport));
	} */
}
void i486Debugger::SpecialDebugInfo::IORead(i486Debugger &debugger,const i486DXCommon &cpu,unsigned int ioport,unsigned int data,unsigned int lengthInBytes)
{
	/* if(0x5e8==ioport || 0x3A5C==ioport)
	{
		debugger.ExternalBreak("Special Break IO Read "+cpputil::Uitox(ioport));
	} */
}


////////////////////////////////////////////////////////////


i486Debugger::i486Debugger()
{
	specialDebugInfo=new SpecialDebugInfo;
	symTablePtr=new i486SymbolTable;
	breakOnIORead.resize(i486DXCommon::I486_NUM_IOPORT);
	breakOnIOWrite.resize(i486DXCommon::I486_NUM_IOPORT);
	for(auto iter=breakOnIORead.begin(); breakOnIORead.end()!=iter; ++iter)
	{
		*iter=false;
	}
	for(auto iter=breakOnIOWrite.begin(); breakOnIOWrite.end()!=iter; ++iter)
	{
		*iter=false;
	}
	for(auto &CS : breakOnCS)
	{
		CS=false;
	}
	MakeVxDLabelTable();
	CleanUp();
}
i486Debugger::~i486Debugger()
{
	delete specialDebugInfo;
	delete symTablePtr;
}

i486SymbolTable &i486Debugger::GetSymTable(void)
{
	return *symTablePtr;
}
const i486SymbolTable &i486Debugger::GetSymTable(void) const
{
	return *symTablePtr;
}
std::map <unsigned int,std::string> &i486Debugger::GetIOTable(void)
{
	return ioLabel;
}
const std::map <unsigned int,std::string> &i486Debugger::GetIOTable(void) const
{
	return ioLabel;
}

void i486Debugger::CleanUp(void)
{
	breakPoints.clear();
	stop=false;
	ClearBreakOnINT();
	monitorIO=false;
	for(auto &b : monitorIOports)
	{
		b=false;
	}
	disassembleEveryStep=false;
	lastDisassembleAddr.Nullify();

	CSEIPLog.resize(CSEIP_LOG_SIZE);
	for(auto &log : CSEIPLog)
	{
		log.SEG=0;
		log.OFFSET=0;
		log.count=0;
		log.SS=0;
		log.ESP=0;
	}
	CSEIPLogPtr=0;

	for(auto &h : instHist)
	{
		h=0;
	}
}
void i486Debugger::AddBreakPoint(CS_EIP bp,BreakPointInfo info)
{
	if((bp.SEG&i486DXCommon::FarPointer::SPECIAL_SEG_MASK)==i486DXCommon::FarPointer::SEG_WILDCARD)
	{
		breakOnCS[bp.SEG&0xFFFF]=true;
	}
	else
	{
		breakPoints[bp]=info;
	}
}
void i486Debugger::RemoveBreakPoint(CS_EIP bp)
{
	if((bp.SEG&i486DXCommon::FarPointer::SPECIAL_SEG_MASK)==i486DXCommon::FarPointer::SEG_WILDCARD)
	{
		breakOnCS[bp.SEG&0xFFFF]=false;
	}
	else
	{
		auto iter=breakPoints.find(bp);
		if(breakPoints.end()!=iter)
		{
			breakPoints.erase(iter);
		}
	}
}
void i486Debugger::ClearBreakPoints(void)
{
	breakPoints.clear();
	oneTimeBreakPoint.Nullify();
	ClearCSBreakPoints();
}

void i486Debugger::ClearCSBreakPoints(void)
{
	for(auto &CS : breakOnCS)
	{
		CS=false;
	}
}

std::vector <i486Debugger::CS_EIP> i486Debugger::GetBreakPoints(void) const
{
	std::vector <CS_EIP> list;
	for(auto bp : breakPoints)
	{
		list.push_back(bp.first);
	}
	return list;
}

std::vector <std::pair <i486Debugger::CS_EIP,i486Debugger::BreakPointInfo> > i486Debugger::GetBreakPointsEx(void) const
{
	std::vector <std::pair<CS_EIP,BreakPointInfo> > list;
	for(auto bp : breakPoints)
	{
		list.push_back(bp);
	}
	return list;
}

std::vector <uint16_t> i486Debugger::GetCSBreakPoints(void) const
{
	std::vector <uint16_t> list;
	uint16_t cs=0;
	for(auto b : breakOnCS)
	{
		if(true==b)
		{
			list.push_back(cs);
		}
		++cs;
	}
	return list;
}

void i486Debugger::AddBreakOnIORead(unsigned int ioport)
{
	breakOnIORead[ioport%(i486DXCommon::I486_NUM_IOPORT)]=true;
}
void i486Debugger::RemoveBreakOnIORead(unsigned int ioport)
{
	breakOnIORead[ioport%(i486DXCommon::I486_NUM_IOPORT)]=false;
}
void i486Debugger::RemoveBreakOnIORead(void)
{
	for(unsigned int i=0; i<i486DXCommon::I486_NUM_IOPORT; ++i)
	{
		breakOnIORead[i]=false;
	}
}
void i486Debugger::AddBreakOnIOWrite(unsigned int ioport)
{
	breakOnIOWrite[ioport%(i486DXCommon::I486_NUM_IOPORT)]=true;
}
void i486Debugger::RemoveBreakOnIOWrite(unsigned int ioport)
{
	breakOnIOWrite[ioport%(i486DXCommon::I486_NUM_IOPORT)]=false;
}
void i486Debugger::RemoveBreakOnIOWrite(void)
{
	for(unsigned int i=0; i<i486DXCommon::I486_NUM_IOPORT; ++i)
	{
		breakOnIOWrite[i]=false;
	}
}
const std::vector <unsigned int> i486Debugger::GetBreakOnIORead(void) const
{
	std::vector <unsigned int> ioport;
	for(unsigned int i=0; i<i486DXCommon::I486_NUM_IOPORT; ++i)
	{
		if(true==breakOnIORead[i])
		{
			ioport.push_back(i);
		}
	}
	return ioport;
}
const std::vector <unsigned int> i486Debugger::GetBreakOnIOWrite(void) const
{
	std::vector <unsigned int> ioport;
	for(unsigned int i=0; i<i486DXCommon::I486_NUM_IOPORT; ++i)
	{
		if(true==breakOnIOWrite[i])
		{
			ioport.push_back(i);
		}
	}
	return ioport;
}

void i486Debugger::SetOneTimeBreakPoint(unsigned int CS,unsigned int EIP)
{
	oneTimeBreakPoint.SEG=CS;
	oneTimeBreakPoint.OFFSET=EIP;
}

void i486Debugger::BeforeRunOneInstruction(i486DXCommon &cpu,Memory &mem,InOut &io,const i486DXCommon::Instruction &inst)
{
	specialDebugInfo->BeforeRunOneInstruction(*this,cpu,mem,io,inst);

	++instHist[inst.opCode];

	prevMode=cpu.state.mode;

	if(BRKPNT_FLAG_NONE!=breakOrMonitorOnVxDCall)
	{
		if(true!=cpu.IsInRealMode() && true!=cpu.GetVM() && I486_OPCODE_INT==inst.opCode && INT_WIN31_VxD==inst.EvalUimm8())
		{
			auto EIP=cpu.GetEIP();

			uint32_t svc=cpu.DebugFetchWord(inst.addressSize,cpu.state.CS(),EIP+2,mem);
			uint32_t VxDId=cpu.DebugFetchWord(inst.addressSize,cpu.state.CS(),EIP+4,mem);
			bool match=true;
			if(breakOnVxDId<0x10000 && VxDId!=breakOnVxDId)
			{
				match=false;
			}
			if(breakOnVxDServiceNumber<0x10000 && svc!=breakOnVxDServiceNumber)
			{
				match=false;
			}
			if(true==match)
			{
				std::string msg;
				msg="VxD Call ID="+cpputil::Ustox(VxDId)+" SVC="+cpputil::Ustox(svc);
				WriteLogFile(msg);

				if(BRKPNT_FLAG_MONITOR_ONLY!=breakOrMonitorOnVxDCall)
				{
					stop=true;
					externalBreakReason=msg;
				}
				else
				{
					std::cout << msg << std::endl;
				}
			}
		}
	}

	CS_EIP cseip;
	cseip.SEG=cpu.state.CS().value;
	cseip.OFFSET=cpu.state.EIP;

	CSEIPLog[CSEIPLogPtr].SEG=cpu.state.CS().value;
	CSEIPLog[CSEIPLogPtr].OFFSET=cpu.state.EIP;
	CSEIPLog[CSEIPLogPtr].SS=cpu.state.SS().value;
	CSEIPLog[CSEIPLogPtr].ESP=cpu.state.ESP();
	CSEIPLog[CSEIPLogPtr].count=1;
	auto &prevCSEIPLog=CSEIPLog[(CSEIPLogPtr+CSEIP_LOG_MASK)&CSEIP_LOG_MASK];
	if(prevCSEIPLog==CSEIPLog[CSEIPLogPtr])
	{
		++prevCSEIPLog.count;
	}
	else if(CSEIPLog[(CSEIPLogPtr+CSEIP_LOG_MASK-2)&CSEIP_LOG_MASK]==CSEIPLog[(CSEIPLogPtr+CSEIP_LOG_MASK)&CSEIP_LOG_MASK] &&
	        CSEIPLog[(CSEIPLogPtr+CSEIP_LOG_MASK-1)&CSEIP_LOG_MASK]==CSEIPLog[CSEIPLogPtr])
	{
		// CSEIP_LOG_MASK=length-1
		// Pattern like:
		//    BUSYWAIT:  TEST DS:[12345H],00H
		//               JNE BUSYWAIT
		// In that case execution should be:
		//    BUSYWAIT:  TEST DS:[12345H],00H      CSEIPLog[currentPtr+CSEIP_LOG_MASK-2]
		//               JNE BUSYWAIT              CSEIPLog[currentPtr+CSEIP_LOG_MASK-1]
		//    BUSYWAIT:  TEST DS:[12345H],00H      CSEIPLog[currentPtr+CSEIP_LOG_MASK]
		//               JNE BUSYWAIT              CSEIPLog[currentPtr]
		++CSEIPLog[(CSEIPLogPtr+CSEIP_LOG_MASK-2)&CSEIP_LOG_MASK].count;
		++CSEIPLog[(CSEIPLogPtr+CSEIP_LOG_MASK-1)&CSEIP_LOG_MASK].count;

		CSEIPLog[(CSEIPLogPtr+CSEIP_LOG_MASK-1)&CSEIP_LOG_MASK].SS=cpu.state.SS().value;
		CSEIPLog[(CSEIPLogPtr+CSEIP_LOG_MASK-1)&CSEIP_LOG_MASK].ESP=cpu.state.ESP();

		CSEIPLogPtr=(CSEIPLogPtr+CSEIP_LOG_MASK-0)&CSEIP_LOG_MASK;
	}
	else if(CSEIPLog[(CSEIPLogPtr+CSEIP_LOG_MASK-4)&CSEIP_LOG_MASK]==CSEIPLog[(CSEIPLogPtr+CSEIP_LOG_MASK-1)&CSEIP_LOG_MASK] &&
	        CSEIPLog[(CSEIPLogPtr+CSEIP_LOG_MASK-3)&CSEIP_LOG_MASK]==CSEIPLog[(CSEIPLogPtr+CSEIP_LOG_MASK)&CSEIP_LOG_MASK] &&
	        CSEIPLog[(CSEIPLogPtr+CSEIP_LOG_MASK-2)&CSEIP_LOG_MASK]==CSEIPLog[CSEIPLogPtr])
	{
		// CSEIP_LOG_MASK=length-1
		// Pattern like:
		//    BUSYWAIT:  IN  AL,IOPORT
		//               TEST AL,1
		//               JNE BUSYWAIT
		// In that case execution should be:
		//    BUSYWAIT:  IN AL,IOPORT              CSEIPLog[currentPtr+CSEIP_LOG_MASK-4]
		//               TEST AL,1                 CSEIPLog[currentPtr+CSEIP_LOG_MASK-3]
		//               JNE BUSYWAIT              CSEIPLog[currentPtr+CSEIP_LOG_MASK-2]
		//    BUSYWAIT:  TEST DS:[12345H],00H      CSEIPLog[currentPtr+CSEIP_LOG_MASK-1]
		//               TEST AL,1                 CSEIPLog[currentPtr+CSEIP_LOG_MASK]
		//               JNE BUSYWAIT              CSEIPLog[currentPtr]
		++CSEIPLog[(CSEIPLogPtr+CSEIP_LOG_MASK-4)&CSEIP_LOG_MASK].count;
		++CSEIPLog[(CSEIPLogPtr+CSEIP_LOG_MASK-3)&CSEIP_LOG_MASK].count;
		++CSEIPLog[(CSEIPLogPtr+CSEIP_LOG_MASK-2)&CSEIP_LOG_MASK].count;

		CSEIPLog[(CSEIPLogPtr+CSEIP_LOG_MASK-2)&CSEIP_LOG_MASK].SS=cpu.state.SS().value;
		CSEIPLog[(CSEIPLogPtr+CSEIP_LOG_MASK-2)&CSEIP_LOG_MASK].ESP=cpu.state.ESP();

		CSEIPLogPtr=(CSEIPLogPtr+CSEIP_LOG_MASK-1)&CSEIP_LOG_MASK;
	}
	else if(CSEIPLog[(CSEIPLogPtr+CSEIP_LOG_MASK-6)&CSEIP_LOG_MASK]==CSEIPLog[(CSEIPLogPtr+CSEIP_LOG_MASK-2)&CSEIP_LOG_MASK] &&
	        CSEIPLog[(CSEIPLogPtr+CSEIP_LOG_MASK-5)&CSEIP_LOG_MASK]==CSEIPLog[(CSEIPLogPtr+CSEIP_LOG_MASK-1)&CSEIP_LOG_MASK] &&
	        CSEIPLog[(CSEIPLogPtr+CSEIP_LOG_MASK-4)&CSEIP_LOG_MASK]==CSEIPLog[(CSEIPLogPtr+CSEIP_LOG_MASK)&CSEIP_LOG_MASK] &&
	        CSEIPLog[(CSEIPLogPtr+CSEIP_LOG_MASK-3)&CSEIP_LOG_MASK]==CSEIPLog[CSEIPLogPtr])
	{
		++CSEIPLog[(CSEIPLogPtr+CSEIP_LOG_MASK-6)&CSEIP_LOG_MASK].count;
		++CSEIPLog[(CSEIPLogPtr+CSEIP_LOG_MASK-5)&CSEIP_LOG_MASK].count;
		++CSEIPLog[(CSEIPLogPtr+CSEIP_LOG_MASK-4)&CSEIP_LOG_MASK].count;
		++CSEIPLog[(CSEIPLogPtr+CSEIP_LOG_MASK-3)&CSEIP_LOG_MASK].count;

		CSEIPLog[(CSEIPLogPtr+CSEIP_LOG_MASK-3)&CSEIP_LOG_MASK].SS=cpu.state.SS().value;
		CSEIPLog[(CSEIPLogPtr+CSEIP_LOG_MASK-3)&CSEIP_LOG_MASK].ESP=cpu.state.ESP();

		CSEIPLogPtr=(CSEIPLogPtr+CSEIP_LOG_MASK-2)&CSEIP_LOG_MASK;
	}
	else
	{
		CSEIPLogPtr=(CSEIPLogPtr+1)&CSEIP_LOG_MASK;
	}

	if(true==disassembleEveryStep && lastDisassembleAddr!=cseip)
	{
		i486DXCommon::InstructionAndOperand instOp;
		MemoryAccess::ConstMemoryWindow emptyMemWindow;
		cpu.DebugFetchInstruction(emptyMemWindow,instOp,mem);
		auto disasm=cpu.Disassemble(instOp.inst,instOp.op1,instOp.op2,cpu.state.CS(),cpu.state.EIP,mem,GetSymTable(),GetIOTable());
		lastDisassembleAddr.SEG=cpu.state.CS().value;
		lastDisassembleAddr.OFFSET=cpu.state.EIP;
		std::cout << disasm << std::endl;
		WriteLogFile(disasm);
		if(true==regDumpEveryStep)
		{
			std::string regDump="             ";
			regDump+=" "+cpputil::Uitox(cpu.state.EAX());
			regDump+=" "+cpputil::Uitox(cpu.state.EBX());
			regDump+=" "+cpputil::Uitox(cpu.state.ECX());
			regDump+=" "+cpputil::Uitox(cpu.state.EDX());
			regDump+=" "+cpputil::Uitox(cpu.state.ESI());
			regDump+=" "+cpputil::Uitox(cpu.state.EDI());
			regDump+=" "+cpputil::Uitox(cpu.state.EBP());
			regDump+=" "+cpputil::Uitox(cpu.state.ESP());
			regDump+=" "+cpputil::Uitox(cpu.state.EFLAGS);
			regDump+=" "+cpputil::Ustox(cpu.state.DS().value);
			regDump+=" "+cpputil::Ustox(cpu.state.ES().value);
			regDump+=" "+cpputil::Ustox(cpu.state.FS().value);
			regDump+=" "+cpputil::Ustox(cpu.state.GS().value);
			regDump+=" "+cpputil::Ustox(cpu.state.SS().value);
			std::cout << regDump << std::endl;
			WriteLogFile(regDump);
		}
	}

	inInstruction=true;
}

void i486Debugger::AfterRunOneInstruction(unsigned int clocksPassed,i486DXCommon &cpu,Memory &mem,InOut &io,const i486DXCommon::Instruction &inst,const i486DXCommon::Operand &op1,const i486DXCommon::Operand &op2)
{
	inInstruction=false;

	specialDebugInfo->AfterRunOneInstruction(*this,clocksPassed,cpu,mem,io,inst);
	if(true!=stop && true!=cpu.state.exception)
	{
		CheckForBreakPoints(cpu);
		if(true==breakOnVM86Mode)
		{
			if(i486DXCommon::MODE_VM86!=prevMode && i486DXCommon::MODE_VM86==cpu.state.mode)
			{
				stop=true;
				externalBreakReason="Entered VM86 Mode";
			}
		}
		if(true==breakOnProtectedMode)
		{
			if(i486DXCommon::MODE_REAL==prevMode && i486DXCommon::MODE_REAL!=cpu.state.mode)
			{
				stop=true;
				externalBreakReason="Entered Protected Mode";
			}
		}
		if(true==breakOnRealMode)
		{
			if(i486DXCommon::MODE_REAL!=prevMode && i486DXCommon::MODE_REAL==cpu.state.mode)
			{
				stop=true;
				externalBreakReason="Entered Real Mode";
			}
		}
	}
	else
	{
		auto &prevCSEIPLog=CSEIPLog[(CSEIPLogPtr+CSEIP_LOG_MASK)&CSEIP_LOG_MASK];
		i486DXCommon::SegmentRegister CS;
		cpu.DebugLoadSegmentRegister(CS,prevCSEIPLog.SEG,mem,false,prevMode);
		if(prevMode==cpu.state.mode)
		{
			additionalDisasm=cpu.Disassemble(inst,op1,op2,CS,prevCSEIPLog.OFFSET,mem,GetSymTable(),GetIOTable());
		}
		else
		{
			additionalDisasm=cpputil::Ustox(prevCSEIPLog.SEG)+":"+cpputil::Uitox(prevCSEIPLog.OFFSET);
			additionalDisasm+=" (Mode Changed) ";
			cpputil::ExtendString(additionalDisasm,40);
			additionalDisasm+=inst.Disassemble(op1,op2,CS,prevCSEIPLog.OFFSET,GetSymTable(),GetIOTable());
		}
	}
}

std::vector <i486Debugger::CSEIPLogType> i486Debugger::GetCSEIPLog(void)
{
	return GetCSEIPLog((unsigned int)CSEIPLog.size());
}

std::vector <i486Debugger::CSEIPLogType> i486Debugger::GetCSEIPLog(unsigned int steps)
{
	std::vector <CSEIPLogType> list;
	unsigned int offset=CSEIP_LOG_MASK;
	for(unsigned int i=0; i<steps && i<CSEIPLog.size(); ++i)
	{
		auto idx=(CSEIPLogPtr+offset)&CSEIP_LOG_MASK;
		list.push_back(CSEIPLog[idx]);
		--offset;
	}
	return list;
}

void i486Debugger::CheckForBreakPoints(i486DXCommon &cpu)
{
	CS_EIP cseip;
	cseip.SEG=cpu.state.CS().value;
	cseip.OFFSET=cpu.state.EIP;

	{
		auto found=breakPoints.find(cseip);
		if(found!=breakPoints.end())
		{
			stop=(0==(found->second.flags&BRKPNT_FLAG_DONT_STOP));
			found->second.SteppedOn();
			if(0!=(found->second.flags&BRKPNT_FLAG_CAPTURE_WIN_APIENTRY))
			{
				unsigned int SEG=cpu.state.ES().value;
				unsigned int OFF=cpu.GetDI();
				std::cout << "VMID:" << cpputil::Ustox(found->second.win_vmmId) << " API Entry " << cpputil::Ustox(SEG) << ":" << cpputil::Ustox(OFF) << std::endl;
				GetSymTable().AddWindowsAPIEntry(found->second.win_vmmId,SEG,OFF);
				found->second.flags&=~BRKPNT_FLAG_CAPTURE_WIN_APIENTRY;
			}
			lastBreakPointInfo=found->second;

			if(0!=(found->second.flags&BRKPNT_FLAG_ONE_TIME))
			{
				breakPoints.erase(found);
			}
		}
	}
	if(breakOnCS[cseip.SEG])
	{
		stop=true;
	}
	if(oneTimeBreakPoint==cseip)
	{
		stop=true;
		oneTimeBreakPoint.Nullify();
	}
}

void i486Debugger::HandleException(i486DXCommon &cpu,Memory &mem,unsigned int instNumBytes)
{
	auto &prevCSEIPLog=CSEIPLog[(CSEIPLogPtr+CSEIP_LOG_MASK)&CSEIP_LOG_MASK];
	if(true==inInstruction && cpu.state.ESP()!=prevCSEIPLog.ESP)
	{
		std::string msg="ESP value changed before handling exception. ";
		msg+=cpputil::Uitox(prevCSEIPLog.ESP);
		msg+=" to ";
		msg+=cpputil::Uitox(cpu.state.ESP());
		ExternalBreak(msg);
	}
}

void i486Debugger::SetBreakOnINT(unsigned int INTNum)
{
	breakOnINT[INTNum&(BreakOnINTCondition::NUM_INTERRUPTS-1)].cond=BreakOnINTCondition::COND_ALWAYS;
	breakOnINT[INTNum&(BreakOnINTCondition::NUM_INTERRUPTS-1)].CSEIP.clear();
}
void i486Debugger::SetBreakOnINTwithAH(unsigned int INTNum,unsigned int AH)
{
	breakOnINT[INTNum&(BreakOnINTCondition::NUM_INTERRUPTS-1)].cond=BreakOnINTCondition::COND_AH;
	breakOnINT[INTNum&(BreakOnINTCondition::NUM_INTERRUPTS-1)].condValue=AH;
	breakOnINT[INTNum&(BreakOnINTCondition::NUM_INTERRUPTS-1)].CSEIP.clear();
}
void i486Debugger::SetBreakOnINTwithAX(unsigned int INTNum,unsigned int AX)
{
	breakOnINT[INTNum&(BreakOnINTCondition::NUM_INTERRUPTS-1)].cond=BreakOnINTCondition::COND_AX;
	breakOnINT[INTNum&(BreakOnINTCondition::NUM_INTERRUPTS-1)].condValue=AX;
	breakOnINT[INTNum&(BreakOnINTCondition::NUM_INTERRUPTS-1)].CSEIP.clear();
}

void i486Debugger::AddBreakOnINT_CSEIP(unsigned int INTNum,uint32_t CS,uint32_t EIP)
{
	uint64_t key=CS;
	key<<=32;
	key|=EIP;
	breakOnINT[INTNum&(BreakOnINTCondition::NUM_INTERRUPTS-1)].CSEIP.insert(key);
}

void i486Debugger::AddBreakOnINT_EIP(unsigned int INTNum,uint32_t EIP)
{
	breakOnINT[INTNum&(BreakOnINTCondition::NUM_INTERRUPTS-1)].CSEIP.insert(EIP);
}

void i486Debugger::SetBreakOnFOpen(std::string fName)
{
	breakOnINT[0x21].cond=BreakOnINTCondition::COND_FOPEN_FCREATE;
	breakOnINT[0x21].fName=fName;
	breakOnINT[0x30].cond=BreakOnINTCondition::COND_FOPEN_FCREATE;
	breakOnINT[0x30].fName=fName;
}

void i486Debugger::ClearBreakOnINT(void)
{
	for(int i=0; i<BreakOnINTCondition::NUM_INTERRUPTS; ++i)
	{
		breakOnINT[i].cond=BreakOnINTCondition::COND_NEVER;
	}
}

void i486Debugger::ClearBreakOnINT(unsigned int INTNum)
{
	breakOnINT[INTNum&(BreakOnINTCondition::NUM_INTERRUPTS-1)].cond=BreakOnINTCondition::COND_NEVER;
}

std::vector <std::string> i486Debugger::GetCallStackText(const i486DXCommon &cpu) const
{
	auto &symTable=GetSymTable();
	std::vector <std::string> text;
	for(auto &s : cpu.callStack)
	{
		std::string str;
		str+="FR="+cpputil::Ustox(s.fromCS)+":"+cpputil::Uitox(s.fromEIP)+"  ";
		str+="TO="+cpputil::Ustox(s.procCS)+":"+cpputil::Uitox(s.procEIP)+"  ";
		str+="RET="+cpputil::Ustox(s.fromCS)+":"+cpputil::Uitox(s.fromEIP+s.callOpCodeLength);
		auto symbolPtr=symTable.Find(s.procCS,s.procEIP);
		if(nullptr!=symbolPtr && (i486Symbol::SYM_PROCEDURE==symbolPtr->symType || i486Symbol::SYM_ANY==symbolPtr->symType))
		{
			str+="{";
			str+=symbolPtr->Format();
			str+="}";
		}
		if(s.INTNum<0x100)
		{
			str+="  (INT ";
			str+=cpputil::Ubtox((unsigned char)s.INTNum);
			switch(s.INTNum)
			{
			case 0x0:
				str+=" 0DIV";
				break;
			case 0x1:
				str+=" DBG";
				break;
			case 0x2:
				str+=" NMI";
				break;
			case 0x3:
				str+=" BRKPT";
				break;
			case 0x4:
				str+=" OVRFL";
				break;
			case 0x5:
				str+=" BOUND";
				break;
			case 0x6:
				str+=" INVOP";
				break;
			case 0x7:
				str+=" DEVUV";
				break;
			case 0x8:
				str+=" DBLFL";
				break;
			case 0x9:
				break;
			case 0xA:
				str+=" IVTSK";
				break;
			case 0xB:
				str+=" SEGNP";
				break;
			case 0xC:
				str+=" STK";
				break;
			case 0xD:
				str+=" GP";
				break;
			case 0xE:
				str+=" PG";
				break;
			case 0x10:
				str+=" FPU";
				break;
			case 0x11:
				str+=" ALIGN";
				break;
			}
			str+=",AX=";
			str+=cpputil::Ustox(s.AX);
			str+="H";

			auto INTNum=s.INTNum;
			auto AX=s.AX;
			if(0xFFFF!=s.INTNum0)
			{
				str+="<-INT ";
				str+=cpputil::Ubtox(s.INTNum0);
				str+="H,AX=";
				str+=cpputil::Ustox(s.AX0);
				str+="H";
				INTNum=s.INTNum0;
                AX=s.AX0;
			}

			auto INTLabel=symTable.GetINTLabel(INTNum);
			auto INTFuncLabelAH=symTable.GetINTFuncLabel(INTNum,(AX>>8)&0xFF);
			auto INTFuncLabelAX=symTable.GetINTFuncLabel(INTNum,AX);
			std::string INTFuncLabel;
			if(0<INTFuncLabelAH.size() && INTFuncLabelAX.size())
			{
				INTFuncLabel=INTFuncLabelAH+"|"+INTFuncLabelAX;
			}
			else if(0<INTFuncLabelAH.size())
			{
				INTFuncLabel=INTFuncLabelAH;
			}
			else if(0<INTFuncLabelAX.size())
			{
				INTFuncLabel=INTFuncLabelAX;
			}

			if(""!=INTLabel)
			{
				str.push_back(' ');
				str+=INTLabel;
			}
			if(""!=INTFuncLabel)
			{
				if(""!=INTLabel)
				{
					str.push_back('.');
				}
				str+=INTFuncLabel;
			}

			if(0<s.str.size())
			{
				str.push_back(' ');
				str+=s.str;
			}

			str+=")";
		}
		text.push_back((std::string &&)str);
	}
	return text;
}

void i486Debugger::ExternalBreak(const std::string &reason)
{
	stop=true;
	externalBreakReason=reason;
	lastBreakPointInfo.Clear();
}

void i486Debugger::MonitorButDoNotStop(const std::string &reason)
{
	stop=true;
	externalBreakReason=reason;
	lastBreakPointInfo.Clear();
	lastBreakPointInfo.flags|=BRKPNT_FLAG_MONITOR_ONLY;
}

void i486Debugger::ClearStopFlag(void)
{
	stop=false;
	externalBreakReason="";
	additionalDisasm="";
	lastBreakPointInfo.Clear();
}

void i486Debugger::Interrupt(const i486DXCommon &cpu,unsigned int INTNum,Memory &mem,unsigned int numInstBytes,bool SWI)
{
	specialDebugInfo->Interrupt(*this,cpu,INTNum,mem,numInstBytes);
	if(INT_WIN31_VxD==INTNum && true==SWI)
	{
		AutoAnnotateVxD(cpu,mem,cpu.state.CS(),cpu.state.EIP);
	}
	if(INT_WIN_DPMI_2FH==INTNum && true==SWI)
	{
		if(DPMI_GET_API_ENTRY==cpu.GetAX())
		{
			CS_EIP bp;
			bp.SEG=cpu.state.CS().value;
			bp.OFFSET=cpu.state.EIP+2;
			auto found=breakPoints.find(bp);
			if(breakPoints.end()==found)
			{
				BreakPointInfo info;
				info.flags=BRKPNT_FLAG_CAPTURE_WIN_APIENTRY|BRKPNT_FLAG_ONE_TIME|BRKPNT_FLAG_DONT_STOP;
				info.win_vmmId=cpu.GetBX();
				AddBreakPoint(bp,info);
			}
			else
			{
				found->second.flags|=BRKPNT_FLAG_CAPTURE_WIN_APIENTRY;
				found->second.win_vmmId=cpu.GetBX();
			}
		}
	}
	if(true==captureDOSStdout && INTNum==0x21)
	{
		if(2==cpu.GetAH() || (6==cpu.GetAH() && 0xFF!=cpu.GetDL()))
		{
			DOSStdout.push_back(cpu.GetDL());
		}
		else if(9==cpu.GetAH())
		{
			std::string str;
			if(true==cpu.IsInRealMode())  // Real Mode
			{
				str=cpu.DebugFetchDOSString(16,cpu.state.DS(),cpu.GetDX(),mem);
			}
			else
			{
				str=cpu.DebugFetchDOSString(32,cpu.state.DS(),cpu.GetEDX(),mem);
			}
			for(auto c : str)
			{
				DOSStdout.push_back(c);
			}
		}
		else if(0x40==cpu.GetAH() && (1==cpu.GetBX() || 3==cpu.GetBX()))
		{
			if(true==cpu.IsInRealMode())  // Real Mode
			{
				for(int i=0; i<cpu.GetCX(); ++i)
				{
					DOSStdout.push_back(cpu.DebugFetchByte(16,cpu.state.DS(),cpu.GetDX()+i,mem));
				}
			}
			else
			{
				for(int i=0; i<cpu.GetECX(); ++i)
				{
					DOSStdout.push_back(cpu.DebugFetchByte(32,cpu.state.DS(),cpu.GetEDX()+i,mem));
				}
			}
		}
	}
	if(breakOnINT[INTNum&0xFF].cond!=BreakOnINTCondition::COND_NEVER)
	{
		if(0<breakOnINT[INTNum&0xFF].CSEIP.size())
		{
			uint64_t CS=cpu.state.CS().value;
			uint64_t EIP=cpu.state.EIP;
			uint64_t key=(CS<<32)|EIP;
			if(breakOnINT[INTNum&0xFF].CSEIP.end()==breakOnINT[INTNum&0xFF].CSEIP.find(key) &&
			   breakOnINT[INTNum&0xFF].CSEIP.end()==breakOnINT[INTNum&0xFF].CSEIP.find(EIP))
			{
				return;
			}
		}

		switch(breakOnINT[INTNum&0xFF].cond)
		{
		case BreakOnINTCondition::COND_FOPEN_FCREATE:
			{
				bool brk=false;
				std::string str;
				if(cpu.GetAH()==0x3D)
				{
					str="Break on FOPEN ";
					brk=true;
				}
				if(cpu.GetAH()==0x3C)
				{
					str="Break on FCREATE_OR_TRUNCATE ";
					brk=true;
				}
				if(cpu.GetAH()==0x5B)
				{
					str="Break on FCREATE ";
					brk=true;
				}
				if(true==brk)
				{
					auto targetFName=breakOnINT[INTNum&0xFF].fName;
					std::string openingFName;
					if(true==cpu.IsInRealMode())  // Real Mode
					{
						openingFName=cpu.DebugFetchString(16,cpu.state.DS(),cpu.GetDX(),mem);
					}
					else
					{
						openingFName=cpu.DebugFetchString(32,cpu.state.DS(),cpu.GetEDX(),mem);
					}
					if(true==cpputil::WildCardCompare(targetFName,openingFName)) // Pattern,Filename
					{
						str+=openingFName;
						ExternalBreak(str);
					}
					else if(true!=cpputil::StrIncludes(targetFName,'\\') &&
					        true!=cpputil::StrIncludes(targetFName,'/')) // std::find sucks
					{
						std::string openingBaseName,openingPath;
						cpputil::SeparatePathFile(openingPath,openingBaseName,openingFName);
						if(true==cpputil::WildCardCompare(targetFName,openingBaseName)) // Pattern,Filename
						{
							str+=openingFName;
							ExternalBreak(str);
						}
					}
				}
			}
			break;
		case BreakOnINTCondition::COND_ALWAYS:
			{
				std::string str("Break on INT ");
				str+=cpputil::Ubtox(INTNum)+" ";
				str+=INTExplanation(cpu,INTNum,mem);
				ExternalBreak(str);
			}
			break;
		case BreakOnINTCondition::COND_AH:
			if(cpu.GetAH()==breakOnINT[INTNum&0xFF].condValue)
			{
				std::string str("Break on INT ");
				str+=cpputil::Ubtox(INTNum);
				str+=" AH=";
				str+=cpputil::Ubtox((unsigned char)breakOnINT[INTNum&0xFF].condValue)+" ";
				str+=INTExplanation(cpu,INTNum,mem);
				ExternalBreak(str);
			}
			break;
		case BreakOnINTCondition::COND_AX:
			if(cpu.GetAX()==breakOnINT[INTNum&0xFF].condValue)
			{
				std::string str("Break on INT ");
				str+=cpputil::Ubtox(INTNum);
				str+=" AX=";
				str+=cpputil::Ustox(breakOnINT[INTNum&0xFF].condValue)+" ";
				str+=" "+INTExplanation(cpu,INTNum,mem);
				ExternalBreak(str);
			}
			break;
		}
	}
}

void i486Debugger::StartCaptureDOSStdout(void)
{
	captureDOSStdout=true;
	DOSStdout.clear();
}
void i486Debugger::EndCaptureDOSStdout(void)
{
	captureDOSStdout=false;
}
std::vector <char> i486Debugger::GetDOSStdout(void) const
{
	return DOSStdout;
}

std::string i486Debugger::INTExplanation(const i486DXCommon &cpu,unsigned int INTNum,Memory &mem) const
{
	auto &symTable=GetSymTable();
	auto INTLabel=symTable.GetINTLabel(INTNum);
	if(0x21==INTNum)
	{
		auto INTFuncLabelAH=symTable.GetINTFuncLabel(INTNum,(cpu.GetAH()));
		auto INTFuncLabelAX=symTable.GetINTFuncLabel(INTNum,cpu.GetAX());
		if(0<INTFuncLabelAH.size() && INTFuncLabelAX.size())
		{
			INTLabel+=" "+INTFuncLabelAH+"|"+INTFuncLabelAX;
		}
		else if(0<INTFuncLabelAH.size())
		{
			INTLabel+=" "+INTFuncLabelAH;
		}
		else if(0<INTFuncLabelAX.size())
		{
			INTLabel+=" "+INTFuncLabelAX;
		}

		if(0x3D00==(cpu.GetAX()&0xFF00) ||
		   0x3C00==(cpu.GetAX()&0xFF00) ||
		   0x5B00==(cpu.GetAX()&0xFF00) ||
		   0x4B00==(cpu.GetAX()&0xFF00))
		{
			if(true==cpu.IsInRealMode() || true==cpu.GetVM())  // Real Mode or VM86 Mode
			{
				INTLabel+=" "+cpu.DebugFetchString(16,cpu.state.DS(),cpu.GetDX(),mem);
			}
			else
			{
				INTLabel+=" "+cpu.DebugFetchString(32,cpu.state.DS(),cpu.GetEDX(),mem);
			}
		}
	}
	return INTLabel;
}

void i486Debugger::IOWrite(const i486DXCommon &cpu,unsigned int ioport,unsigned int data,unsigned int lengthInBytes)
{
	specialDebugInfo->IOWrite(*this,cpu,ioport,data,lengthInBytes);

	if(true==breakOnIOWrite[ioport&(i486DXCommon::I486_NUM_IOPORT-1)])
	{
		std::string msg;
		if(4==lengthInBytes)
		{
			msg="IOWrite Port:"+cpputil::Uitox(ioport)+" Value:"+cpputil::Uitox(data);
		}
		else if(2==lengthInBytes)
		{
			msg="IOWrite Port:"+cpputil::Uitox(ioport)+" Value:"+cpputil::Ustox(data);
		}
		else
		{
			msg="IOWrite Port:"+cpputil::Uitox(ioport)+" Value:"+cpputil::Ubtox(data);
		}
		ExternalBreak(msg);
		WriteLogFile(msg);
	}

	if(true==monitorIO && true==monitorIOports[ioport&0xFFFF])
	{
		for(int i=0; i<2; ++i)
		{
			std::ostream &ofs=(0==i ? std::cout : logOfs);

			ofs<< cpputil::Ustox(cpu.state.CS().value) << ":" << cpputil::Uitox(cpu.state.EIP) << " ";
			ofs<< "Write IO" << (lengthInBytes<<3) << ":[" << cpputil::Ustox(ioport) << "] ";
			if(4==lengthInBytes)
			{
				ofs<< cpputil::Uitox(data);;
			}
			else if(2==lengthInBytes)
			{
				ofs<< cpputil::Ustox(data);;
			}
			else
			{
				ofs<< cpputil::Ubtox(data);;
			}

			auto iter=ioLabel.find(ioport);
			if(ioLabel.end()!=iter)
			{
				ofs<< "(" << iter->second << ")";
			}
			ofs<< std::endl;

			if(true!=logOfs.is_open())
			{
				break;
			}
		}
	}
}
void i486Debugger::IORead(const i486DXCommon &cpu,unsigned int ioport,unsigned int data,unsigned int lengthInBytes)
{
	specialDebugInfo->IORead(*this,cpu,ioport,data,lengthInBytes);

	if(true==breakOnIORead[ioport&(i486DXCommon::I486_NUM_IOPORT-1)])
	{
		std::string msg="IORead Port:"+cpputil::Uitox(ioport)+" Value:"+cpputil::Ubtox(data);
		ExternalBreak(msg);
		WriteLogFile(msg);
	}

	if(true==monitorIO && true==monitorIOports[ioport&0xFFFF])
	{
		for(int i=0; i<2; ++i)
		{
			std::ostream &ofs=(0==i ? std::cout : logOfs);

			ofs << cpputil::Ustox(cpu.state.CS().value) << ":" << cpputil::Uitox(cpu.state.EIP) << " ";
			ofs << "Read IO" << (lengthInBytes<<3) << ":[" << cpputil::Ustox(ioport) << "] " << cpputil::Ubtox(data);
			auto iter=ioLabel.find(ioport);
			if(ioLabel.end()!=iter)
			{
				ofs << "(" << iter->second << ")";
			}
			ofs << std::endl;

			if(true!=logOfs.is_open())
			{
				break;
			}
		}
	}
}

void i486Debugger::MonitorIO(unsigned short portMin,unsigned short portMax)
{
	for(auto p=portMin; p<=portMax; ++p)
	{
		monitorIOports[p]=true;
		if(p==portMax) // In case portMax=0xffff
		{
			break;
		}
	}
	monitorIO=AtLeastOneMonitorIOPortIsSet();
}
void i486Debugger::UnmonitorIO(unsigned short portMin,unsigned short portMax)
{
	for(auto p=portMin; p<=portMax; ++p)
	{
		monitorIOports[p]=false;
		if(p==portMax) // In case portMax=0xffff
		{
			break;
		}
	}
	monitorIO=AtLeastOneMonitorIOPortIsSet();
}
bool i486Debugger::AtLeastOneMonitorIOPortIsSet(void) const
{
	for(auto b : monitorIOports)
	{
		if(true==b)
		{
			return true;
		}
	}
	return false;
}

std::vector <unsigned int> i486Debugger::FindCaller(unsigned int procAddr,const i486DXCommon::SegmentRegister &seg,const i486DXCommon &cpu,const Memory &mem)
{
	std::vector <unsigned int> caller;

	unsigned int limit=0;
	if(cpu.IsInRealMode())
	{
		limit=0xFFFF;
	}
	else
	{
		limit=seg.limit;
	}

	MemoryAccess::ConstMemoryWindow memWindow;
	memWindow.ptr=nullptr;
	for(unsigned int EIP=0; EIP<limit; ++EIP)
	{
		if(0==(EIP&0x7FFF))
		{
			std::cout << "Searching..." << cpputil::Uitox(EIP) << std::endl;
		}
		i486DXCommon::InstructionAndOperand instOp;
		cpu.DebugFetchInstruction(memWindow,instOp,seg,EIP,mem);
		auto &inst=instOp.inst;
		auto &op1=instOp.op1;
		auto &op2=instOp.op2;
		switch(inst.opCode)
		{
		case I486_OPCODE_CALL_FAR://   0x9A,
		case I486_OPCODE_JMP_FAR://          0xEA,   // cd or cp
			{
				op1.DecodeFarAddr(inst.addressSize,inst.operandSize,inst.operand);
				if(op1.offset==procAddr)
				{
					caller.push_back(EIP);
				}
			}
			break;
		case I486_OPCODE_INC_DEC_CALL_CALLF_JMP_JMPF_PUSH ://0xFF, // INC(REG=0),DEC(REG=1),CALL(REG=2),CALLF(REG=3),JMP(REG=4),JMPF(REG=5),PUSH(REG=6)
			// Call indirect.
			break;

		case I486_OPCODE_JMP_REL8://         0xEB,   // cb
		case I486_OPCODE_JECXZ_REL8://0xE3,  // Depending on the operand size
		case I486_OPCODE_JA_REL8://   0x77,
		case I486_OPCODE_JAE_REL8://  0x73,
		case I486_OPCODE_JB_REL8://   0x72,
		case I486_OPCODE_JBE_REL8://  0x76,
		case I486_OPCODE_JE_REL8://   0x74,
		case I486_OPCODE_JG_REL8://   0x7F,
		case I486_OPCODE_JGE_REL8://  0x7D,
		case I486_OPCODE_JL_REL8://   0x7C,
		case I486_OPCODE_JLE_REL8://  0x7E,
		case I486_OPCODE_JNE_REL8://  0x75,
		case I486_OPCODE_JNO_REL8://  0x71,
		case I486_OPCODE_JNP_REL8://  0x7B,
		case I486_OPCODE_JNS_REL8://  0x79,
		case I486_OPCODE_JO_REL8://   0x70,
		case I486_OPCODE_JP_REL8://   0x7A,
		case I486_OPCODE_JS_REL8://   0x78,
			{
				auto offset=inst.EvalSimm8();
				auto destin=EIP+offset+inst.numBytes;
				if(procAddr==destin)
				{
					caller.push_back(EIP);
				}
			}
			break;
		case I486_OPCODE_JMP_REL://          0xE9,   // cw or cd
		case I486_OPCODE_JA_REL://    0x0F87,
		case I486_OPCODE_JAE_REL://   0x0F83,
		case I486_OPCODE_JB_REL://    0x0F82,
		case I486_OPCODE_JBE_REL://   0x0F86,
		case I486_OPCODE_JE_REL://    0x0F84,
		case I486_OPCODE_JG_REL://    0x0F8F,
		case I486_OPCODE_JGE_REL://   0x0F8D,
		case I486_OPCODE_JL_REL://    0x0F8C,
		case I486_OPCODE_JLE_REL://   0x0F8E,
		case I486_OPCODE_JNE_REL://   0x0F85,
		case I486_OPCODE_JNO_REL://   0x0F81,
		case I486_OPCODE_JNP_REL://   0x0F8B,
		case I486_OPCODE_JNS_REL://   0x0F89,
		case I486_OPCODE_JO_REL://    0x0F80,
		case I486_OPCODE_JP_REL://    0x0F8A,
		case I486_OPCODE_JS_REL://    0x0F88,
		case I486_OPCODE_CALL_REL://   0xE8,
			{
				auto offset=inst.EvalSimm16or32(inst.operandSize);
				auto destin=EIP+offset+inst.numBytes;
				if(procAddr==destin)
				{
					caller.push_back(EIP);
				}
			}
			break;
		}
	}

	return caller;
}
bool i486Debugger::OpenLogFile(std::string logFileName)
{
	std::ofstream ofs(logFileName);
	if(ofs.is_open())
	{
		if(logOfs.is_open())
		{
			logOfs.close();
		}
		std::swap(logOfs,ofs);
		return logOfs.is_open();
	}
	return false;
}
std::ofstream &i486Debugger::LogFileStream(void)
{
	return logOfs;
}
void i486Debugger::CloseLogFile(void)
{
	logOfs.close();
}
void i486Debugger::WriteLogFile(std::string str)
{
	if(logOfs.is_open())
	{
		logOfs << str << std::endl;
	}
}

void i486Debugger::SetBreakOnCallStackDepth(uint32_t depth)
{
	breakOnCallStackDepth=depth;
}
void i486Debugger::ClearBreakOnCallStackDepth(void)
{
	breakOnCallStackDepth=0x7FFFFFFF;
}
