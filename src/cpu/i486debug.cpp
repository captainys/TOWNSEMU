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
	//     03A4:00000D18 CD97                      INT     97H            <- タイマー仕掛け 未公開機能
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

	void BeforeRunOneInstruction(i486Debugger &debugger,i486DX &cpu,Memory &mem,InOut &io,const i486DX::Instruction &inst);
	void AfterRunOneInstruction(i486Debugger &debugger,unsigned int clocksPassed,i486DX &cpu,Memory &mem,InOut &io,const i486DX::Instruction &inst);
	void Interrupt(i486Debugger &debugger,const i486DX &cpu,unsigned int INTNum,Memory &mem,unsigned int numInstBytes);
	void MemWrite(i486Debugger &debugger,const i486DX &cpu,const i486DX::SegmentRegister &seg,unsigned int offset,unsigned int linear,unsigned int physical,unsigned int data,unsigned int lengthInBytes);
	void IOWrite(i486Debugger &debugger,const i486DX &cpu,unsigned int ioport,unsigned int data,unsigned int lengthInBytes);
	void IORead(i486Debugger &debugger,const i486DX &cpu,unsigned int ioport,unsigned int data,unsigned int lengthInBytes);
	std::vector <std::string> GetText(void) const;
};

void i486Debugger::SpecialDebugInfo::BeforeRunOneInstruction(i486Debugger &debugger,i486DX &cpu,Memory &mem,InOut &io,const i486DX::Instruction &inst)
{
}
void i486Debugger::SpecialDebugInfo::AfterRunOneInstruction(i486Debugger &debugger,unsigned int clocksPassed,i486DX &cpu,Memory &mem,InOut &io,const i486DX::Instruction &inst)
{
}
void i486Debugger::SpecialDebugInfo::Interrupt(i486Debugger &debugger,const i486DX &cpu,unsigned int INTNum,Memory &mem,unsigned int numInstBytes)
{
}
void i486Debugger::SpecialDebugInfo::MemWrite(i486Debugger &debugger,const i486DX &cpu,const i486DX::SegmentRegister &seg,unsigned int offset,unsigned int linear,unsigned int physical,unsigned int data,unsigned int lengthInBytes)
{
/*	unsigned DS=0x2F87;
	unsigned OFFSET1=0x1751,OFFSET2=0x1755;
	auto phys1=DS*0x10+OFFSET1,phys2=DS*0x10+OFFSET2;
	if(0x00461==physical || 0x3F000==physical || phys1==physical || phys2==physical)
	{
		debugger.ExternalBreak("Special Break Mem Write "+cpputil::Uitox(physical));
	} */
}
void i486Debugger::SpecialDebugInfo::IOWrite(i486Debugger &debugger,const i486DX &cpu,unsigned int ioport,unsigned int data,unsigned int lengthInBytes)
{
	/* if(0x3150==ioport || 0x3A5C==ioport || 0x3188==ioport)
	{
		debugger.ExternalBreak("Special Break IO Write "+cpputil::Uitox(ioport));
	} */
}
void i486Debugger::SpecialDebugInfo::IORead(i486Debugger &debugger,const i486DX &cpu,unsigned int ioport,unsigned int data,unsigned int lengthInBytes)
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
	breakOnIORead.resize(i486DX::I486_NUM_IOPORT);
	breakOnIOWrite.resize(i486DX::I486_NUM_IOPORT);
	for(auto iter=breakOnIORead.begin(); breakOnIORead.end()!=iter; ++iter)
	{
		*iter=false;
	}
	for(auto iter=breakOnIOWrite.begin(); breakOnIOWrite.end()!=iter; ++iter)
	{
		*iter=false;
	}
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

void i486Debugger::CleanUp(void)
{
	breakPoints.clear();
	stop=false;
	breakOnINT=0xffff;
	monitorIO=false;
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
}
void i486Debugger::AddBreakPoint(CS_EIP bp)
{
	auto iter=breakPoints.find(bp);
	if(breakPoints.end()==iter)
	{
		breakPoints.insert(bp);
	}
}
void i486Debugger::RemoveBreakPoint(CS_EIP bp)
{
	auto iter=breakPoints.find(bp);
	if(breakPoints.end()!=iter)
	{
		breakPoints.erase(iter);
	}
}
void i486Debugger::ClearBreakPoints(void)
{
	breakPoints.clear();
}
std::vector <i486Debugger::CS_EIP> i486Debugger::GetBreakPoints(void) const
{
	std::vector <CS_EIP> list;
	list.insert(list.end(),breakPoints.begin(),breakPoints.end());
	return list;
}

void i486Debugger::AddBreakOnIORead(unsigned int ioport)
{
	breakOnIORead[ioport%(i486DX::I486_NUM_IOPORT)]=true;
}
void i486Debugger::RemoveBreakOnIORead(unsigned int ioport)
{
	breakOnIORead[ioport%(i486DX::I486_NUM_IOPORT)]=false;
}
void i486Debugger::AddBreakOnIOWrite(unsigned int ioport)
{
	breakOnIOWrite[ioport%(i486DX::I486_NUM_IOPORT)]=true;
}
void i486Debugger::RemoveBreakOnIOWrite(unsigned int ioport)
{
	breakOnIOWrite[ioport%(i486DX::I486_NUM_IOPORT)]=false;
}
const std::vector <unsigned int> i486Debugger::GetBreakOnIORead(void) const
{
	std::vector <unsigned int> ioport;
	for(unsigned int i=0; i<i486DX::I486_NUM_IOPORT; ++i)
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
	for(unsigned int i=0; i<i486DX::I486_NUM_IOPORT; ++i)
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

void i486Debugger::BeforeRunOneInstruction(i486DX &cpu,Memory &mem,InOut &io,const i486DX::Instruction &inst)
{
	specialDebugInfo->BeforeRunOneInstruction(*this,cpu,mem,io,inst);

	CS_EIP cseip;
	cseip.SEG=cpu.state.CS().value;
	cseip.OFFSET=cpu.state.EIP;

	CSEIPLog[CSEIPLogPtr].SEG=cpu.state.CS().value;
	CSEIPLog[CSEIPLogPtr].OFFSET=cpu.state.EIP;
	CSEIPLog[CSEIPLogPtr].SS=cpu.state.SS().value;
	CSEIPLog[CSEIPLogPtr].ESP=cpu.state.ESP();
	CSEIPLog[CSEIPLogPtr].count=1;
	auto &prevCSEIPLog=CSEIPLog[(CSEIPLogPtr+CSEIP_LOG_MASK)&CSEIP_LOG_MASK];
	if(prevCSEIPLog!=CSEIPLog[CSEIPLogPtr])
	{
		CSEIPLogPtr=(CSEIPLogPtr+1)&CSEIP_LOG_MASK;
	}
	else
	{
		++prevCSEIPLog.count;
	}

	if(true==disassembleEveryStep && lastDisassembleAddr!=cseip)
	{
		i486DX::Operand op1,op2;
		auto inst=cpu.FetchInstruction(op1,op2,mem);
		auto disasm=cpu.Disassemble(inst,op1,op2,cpu.state.CS(),cpu.state.EIP,mem,GetSymTable());
		lastDisassembleAddr.SEG=cpu.state.CS().value;
		lastDisassembleAddr.OFFSET=cpu.state.EIP;
		std::cout << disasm << std::endl;
	}
}

void i486Debugger::AfterRunOneInstruction(unsigned int clocksPassed,i486DX &cpu,Memory &mem,InOut &io,const i486DX::Instruction &inst)
{
	specialDebugInfo->AfterRunOneInstruction(*this,clocksPassed,cpu,mem,io,inst);
	CheckForBreakPoints(cpu);
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

void i486Debugger::CheckForBreakPoints(i486DX &cpu)
{
	CS_EIP cseip;
	cseip.SEG=cpu.state.CS().value;
	cseip.OFFSET=cpu.state.EIP;

	if(breakPoints.find(cseip)!=breakPoints.end())
	{
		stop=true;
	}
	if(oneTimeBreakPoint==cseip)
	{
		stop=true;
		oneTimeBreakPoint.Nullify();
	}
}

void i486Debugger::SetBreakOnINT(unsigned int INTNum)
{
	breakOnINT=INTNum;
}

void i486Debugger::ClearBreakOnINT(void)
{
	breakOnINT=0xffffffff;
}

std::vector <std::string> i486Debugger::GetCallStackText(const i486DX &cpu) const
{
	auto &symTable=GetSymTable();
	std::vector <std::string> text;
	for(auto &s : cpu.callStack)
	{
		std::string str;
		str+="FR="+cpputil::Ustox(s.fromCS)+":"+cpputil::Uitox(s.fromEIP)+"  ";
		str+="TO="+cpputil::Ustox(s.procCS)+":"+cpputil::Uitox(s.procEIP)+"  ";
		str+="RET="+cpputil::Ustox(s.fromCS)+":"+cpputil::Uitox(s.fromEIP+s.callOpCodeLength);
		if(s.INTNum<0x100)
		{
			str+="  (INT ";
			str+=cpputil::Ubtox((unsigned char)s.INTNum);
			str+=",AX=";
			str+=cpputil::Ustox(s.AX);
			str+="H";

			auto INTLabel=symTable.GetINTLabel(s.INTNum);
			auto INTFuncLabelAH=symTable.GetINTFuncLabel(s.INTNum,(s.AX>>8)&0xFF);
			auto INTFuncLabelAX=symTable.GetINTFuncLabel(s.INTNum,s.AX);
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
}

void i486Debugger::ClearStopFlag(void)
{
	stop=false;
	externalBreakReason="";
}

void i486Debugger::Interrupt(const i486DX &cpu,unsigned int INTNum,Memory &mem,unsigned int numInstBytes)
{
	specialDebugInfo->Interrupt(*this,cpu,INTNum,mem,numInstBytes);
	if(breakOnINT==INTNum)
	{
		std::string str("Break on INT ");
		str+=cpputil::Ubtox(INTNum);
		ExternalBreak(str);
	}
}

void i486Debugger::MemWrite(const i486DX &cpu,const i486DX::SegmentRegister &seg,unsigned int offset,unsigned int linear,unsigned int physical,unsigned int data,unsigned int lengthInBytes)
{
	specialDebugInfo->MemWrite(*this,cpu,seg,offset,linear,physical,data,lengthInBytes);
}

void i486Debugger::IOWrite(const i486DX &cpu,unsigned int ioport,unsigned int data,unsigned int lengthInBytes)
{
	specialDebugInfo->IOWrite(*this,cpu,ioport,data,lengthInBytes);

	if(true==breakOnIOWrite[ioport&(i486DX::I486_NUM_IOPORT-1)])
	{
		ExternalBreak("IOWrite Port:"+cpputil::Uitox(ioport)+" Value:"+cpputil::Ubtox(data));
	}

	if(true==monitorIO && monitorIOMin<=ioport && ioport<=monitorIOMax)
	{
		std::cout << cpputil::Ustox(cpu.state.CS().value) << ":" << cpputil::Uitox(cpu.state.EIP) << " ";
		std::cout << "Write IO" << (lengthInBytes<<3) << ":[" << cpputil::Ustox(ioport) << "] " << cpputil::Ubtox(data);
		auto iter=ioLabel.find(ioport);
		if(ioLabel.end()!=iter)
		{
			std::cout << "(" << iter->second << ")";
		}
		std::cout << std::endl;
	}
}
void i486Debugger::IORead(const i486DX &cpu,unsigned int ioport,unsigned int data,unsigned int lengthInBytes)
{
	specialDebugInfo->IORead(*this,cpu,ioport,data,lengthInBytes);

	if(true==breakOnIORead[ioport&(i486DX::I486_NUM_IOPORT-1)])
	{
		ExternalBreak("IORead Port:"+cpputil::Uitox(ioport)+" Value:"+cpputil::Ubtox(data));
	}

	if(true==monitorIO && monitorIOMin<=ioport && ioport<=monitorIOMax)
	{
		std::cout << cpputil::Ustox(cpu.state.CS().value) << ":" << cpputil::Uitox(cpu.state.EIP) << " ";
		std::cout << "Read IO" << (lengthInBytes<<3) << ":[" << cpputil::Ustox(ioport) << "] " << cpputil::Ubtox(data);
		auto iter=ioLabel.find(ioport);
		if(ioLabel.end()!=iter)
		{
			std::cout << "(" << iter->second << ")";
		}
		std::cout << std::endl;
	}
}
