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
	unsigned char timerInfoBlock[10];
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
	void MemWriteByte(i486Debugger &debugger,const i486DX &cpu,const i486DX::SegmentRegister &seg,unsigned int offset,unsigned int linear,unsigned int physical,unsigned int data);
	void MemWriteWord(i486Debugger &debugger,const i486DX &cpu,const i486DX::SegmentRegister &seg,unsigned int offset,unsigned int linear,unsigned int physical,unsigned int data);
	void MemWriteDword(i486Debugger &debugger,const i486DX &cpu,const i486DX::SegmentRegister &seg,unsigned int offset,unsigned int linear,unsigned int physical,unsigned int data);
	void IOWriteByte(i486Debugger &debugger,const i486DX &cpu,unsigned int ioport,unsigned int data);
	void IOWriteWord(i486Debugger &debugger,const i486DX &cpu,unsigned int ioport,unsigned int data);
	void IOWriteDword(i486Debugger &debugger,const i486DX &cpu,unsigned int ioport,unsigned int data);
	void IOReadByte(i486Debugger &debugger,const i486DX &cpu,unsigned int ioport,unsigned int data);
	void IOReadWord(i486Debugger &debugger,const i486DX &cpu,unsigned int ioport,unsigned int data);
	void IOReadDword(i486Debugger &debugger,const i486DX &cpu,unsigned int ioport,unsigned int data);
	std::vector <std::string> GetText(void) const;
};

void i486Debugger::SpecialDebugInfo::BeforeRunOneInstruction(i486Debugger &debugger,i486DX &cpu,Memory &mem,InOut &io,const i486DX::Instruction &inst)
{
	inTheTargetRange=true;
	if(0x9B==inst.fwait)
	{
		//debugger.ExternalBreak("FWAIT Break");
	}
	if(cpu.state.CS().value==0)
	{
		debugger.ExternalBreak("SpecialBreak CS=0");
	}
	if(0==inst.opCode && 0==inst.operand[0])
	{
		std::cout << cpputil::Ustox(inst.opCode) << " " << cpputil::Ubtox(inst.operand[0]) << std::endl;
		debugger.ExternalBreak("SpecialBreak opCode==0 && operand[0]==0");
	}
	if(cpu.state.CS().value==0x03A4)
	{
		if(cpu.state.EIP==0x0D18)
		{
			passCount=0;
			int40HCount=0;
			otherIntCount=0;
			pass03A4_00000D26=0;
			timerInfoBlock[0]=cpu.FetchByte(cpu.state.CS().addressSize,cpu.state.DS(),cpu.state.DI()+0,mem);
			timerInfoBlock[1]=cpu.FetchByte(cpu.state.CS().addressSize,cpu.state.DS(),cpu.state.DI()+1,mem);
			timerInfoBlock[2]=cpu.FetchByte(cpu.state.CS().addressSize,cpu.state.DS(),cpu.state.DI()+2,mem);
			timerInfoBlock[3]=cpu.FetchByte(cpu.state.CS().addressSize,cpu.state.DS(),cpu.state.DI()+3,mem);
			timerInfoBlock[4]=cpu.FetchByte(cpu.state.CS().addressSize,cpu.state.DS(),cpu.state.DI()+4,mem);
			timerInfoBlock[5]=cpu.FetchByte(cpu.state.CS().addressSize,cpu.state.DS(),cpu.state.DI()+5,mem);
			timerInfoBlock[6]=cpu.FetchByte(cpu.state.CS().addressSize,cpu.state.DS(),cpu.state.DI()+6,mem);
			timerInfoBlock[7]=cpu.FetchByte(cpu.state.CS().addressSize,cpu.state.DS(),cpu.state.DI()+7,mem);
			timerInfoBlock[8]=cpu.FetchByte(cpu.state.CS().addressSize,cpu.state.DS(),cpu.state.DI()+8,mem);
			timerInfoBlock[9]=cpu.FetchByte(cpu.state.CS().addressSize,cpu.state.DS(),cpu.state.DI()+9,mem);
		}
		if(0x0D16<=cpu.state.EIP && cpu.state.EIP<=0x0D23)
		{
			inTheTargetRange=true;
		}
		if(0x0D26==cpu.state.EIP)
		{
			++pass03A4_00000D26;
		}
		if(0x0D1E==cpu.state.EIP)
		{
			++passCount;
			if(20<=int40HCount)
			{
				std::cout << "Pass 3A4:D26=" << pass03A4_00000D26 << std::endl;
				if(true==enableSpecialBreak3)
				{
					std::cout << "Pass 3A4:0D1E=" << passCount << std::endl;
					//debugger.ExternalBreak("SpecialBreak3");
					enableSpecialBreak3=false;
				}
			}
		}
		//if(0x0D25==cpu.state.EIP)
		//{
		//	std::cout << "Pass:" << passCount << std::endl;
		//	std::cout << "INT40:" << int40HCount << std::endl;
		//	std::cout << "INT4x:" << otherIntCount << std::endl;
		//}
	}
}
void i486Debugger::SpecialDebugInfo::AfterRunOneInstruction(i486Debugger &debugger,unsigned int clocksPassed,i486DX &cpu,Memory &mem,InOut &io,const i486DX::Instruction &inst)
{
}
void i486Debugger::SpecialDebugInfo::Interrupt(i486Debugger &debugger,const i486DX &cpu,unsigned int INTNum,Memory &mem,unsigned int numInstBytes)
{
	if(0x40==INTNum)
	{
		++int40HCount;
	}
	else if(0x40<=INTNum && INTNum<=0x4F)
	{
		++otherIntCount;
	}
}
void i486Debugger::SpecialDebugInfo::MemWriteByte(i486Debugger &debugger,const i486DX &cpu,const i486DX::SegmentRegister &seg,unsigned int offset,unsigned int linear,unsigned int physical,unsigned int data)
{
/*	unsigned DS=0x2F87;
	unsigned OFFSET1=0x1751,OFFSET2=0x1755;
	auto phys1=DS*0x10+OFFSET1,phys2=DS*0x10+OFFSET2;
	if(0x00461==physical || 0x3F000==physical || phys1==physical || phys2==physical)
	{
		debugger.ExternalBreak("Special Break Mem Write "+cpputil::Uitox(physical));
	} */
}
void i486Debugger::SpecialDebugInfo::MemWriteWord(i486Debugger &debugger,const i486DX &cpu,const i486DX::SegmentRegister &seg,unsigned int offset,unsigned int linear,unsigned int physical,unsigned int data)
{
/*	unsigned DS=0x2F87;
	unsigned OFFSET1=0x1751,OFFSET2=0x1755;
	auto phys1=DS*0x10+OFFSET1,phys2=DS*0x10+OFFSET2;
	if(0x00461==physical || 0x3F000==physical || phys1==physical || phys2==physical)
	{
		debugger.ExternalBreak("Special Break Mem Write "+cpputil::Uitox(physical));
	} */
}
void i486Debugger::SpecialDebugInfo::MemWriteDword(i486Debugger &debugger,const i486DX &cpu,const i486DX::SegmentRegister &seg,unsigned int offset,unsigned int linear,unsigned int physical,unsigned int data)
{
/*	unsigned DS=0x2F87;
	unsigned OFFSET1=0x1751,OFFSET2=0x1755;
	auto phys1=DS*0x10+OFFSET1,phys2=DS*0x10+OFFSET2;
	if(0x00461==physical || 0x3F000==physical || phys1==physical || phys2==physical)
	{
		debugger.ExternalBreak("Special Break Mem Write "+cpputil::Uitox(physical));
	} */
}
void i486Debugger::SpecialDebugInfo::IOWriteByte(i486Debugger &debugger,const i486DX &cpu,unsigned int ioport,unsigned int data)
{
}
void i486Debugger::SpecialDebugInfo::IOWriteWord(i486Debugger &debugger,const i486DX &cpu,unsigned int ioport,unsigned int data)
{
}
void i486Debugger::SpecialDebugInfo::IOWriteDword(i486Debugger &debugger,const i486DX &cpu,unsigned int ioport,unsigned int data)
{
}
void i486Debugger::SpecialDebugInfo::IOReadByte(i486Debugger &debugger,const i486DX &cpu,unsigned int ioport,unsigned int data)
{
}
void i486Debugger::SpecialDebugInfo::IOReadWord(i486Debugger &debugger,const i486DX &cpu,unsigned int ioport,unsigned int data)
{
}
void i486Debugger::SpecialDebugInfo::IOReadDword(i486Debugger &debugger,const i486DX &cpu,unsigned int ioport,unsigned int data)
{
}


////////////////////////////////////////////////////////////


i486Debugger::i486Debugger()
{
	specialDebugInfo=new SpecialDebugInfo;
	symTablePtr=new i486SymbolTable;
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
		auto inst=cpu.FetchInstruction(mem);
		auto disasm=cpu.Disassemble(inst,cpu.state.CS(),cpu.state.EIP,mem,GetSymTable());
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
			str+="H)";
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

void i486Debugger::MemWriteByte(const i486DX &cpu,const i486DX::SegmentRegister &seg,unsigned int offset,unsigned int linear,unsigned int physical,unsigned int data)
{
	specialDebugInfo->MemWriteByte(*this,cpu,seg,offset,linear,physical,data);
}
void i486Debugger::MemWriteWord(const i486DX &cpu,const i486DX::SegmentRegister &seg,unsigned int offset,unsigned int linear,unsigned int physical,unsigned int data)
{
	specialDebugInfo->MemWriteWord(*this,cpu,seg,offset,linear,physical,data);
}
void i486Debugger::MemWriteDword(const i486DX &cpu,const i486DX::SegmentRegister &seg,unsigned int offset,unsigned int linear,unsigned int physical,unsigned int data)
{
	specialDebugInfo->MemWriteDword(*this,cpu,seg,offset,linear,physical,data);
}

void i486Debugger::IOWriteByte(const i486DX &cpu,unsigned int ioport,unsigned int data)
{
	specialDebugInfo->IOWriteByte(*this,cpu,ioport,data);
	if(true==monitorIO)
	{
		std::cout << cpputil::Ustox(cpu.state.CS().value) << ":" << cpputil::Uitox(cpu.state.EIP) << " ";
		std::cout << "Write IO8:[" << cpputil::Ustox(ioport) << "] " << cpputil::Ubtox(data);
		auto iter=ioLabel.find(ioport);
		if(ioLabel.end()!=iter)
		{
			std::cout << "(" << iter->second << ")" << std::endl;
		}
		std::cout << std::endl;
	}
}
void i486Debugger::IOWriteWord(const i486DX &cpu,unsigned int ioport,unsigned int data)
{
	specialDebugInfo->IOWriteWord(*this,cpu,ioport,data);
	if(true==monitorIO)
	{
		std::cout << cpputil::Ustox(cpu.state.CS().value) << ":" << cpputil::Uitox(cpu.state.EIP) << " ";
		std::cout << "Write IO16:[" << cpputil::Ustox(ioport) << "] " << cpputil::Ustox(data);
		auto iter=ioLabel.find(ioport);
		if(ioLabel.end()!=iter)
		{
			std::cout << "(" << iter->second << ")" << std::endl;
		}
		std::cout << std::endl;
	}
}
void i486Debugger::IOWriteDword(const i486DX &cpu,unsigned int ioport,unsigned int data)
{
	specialDebugInfo->IOWriteDword(*this,cpu,ioport,data);
	if(true==monitorIO)
	{
		std::cout << cpputil::Ustox(cpu.state.CS().value) << ":" << cpputil::Uitox(cpu.state.EIP) << " ";
		std::cout << "Write IO32:[" << cpputil::Ustox(ioport) << "] " << cpputil::Uitox(data);
		auto iter=ioLabel.find(ioport);
		if(ioLabel.end()!=iter)
		{
			std::cout << "(" << iter->second << ")" << std::endl;
		}
		std::cout << std::endl;
	}
}
void i486Debugger::IOReadByte(const i486DX &cpu,unsigned int ioport,unsigned int data)
{
	specialDebugInfo->IOReadByte(*this,cpu,ioport,data);
	if(true==monitorIO)
	{
		std::cout << cpputil::Ustox(cpu.state.CS().value) << ":" << cpputil::Uitox(cpu.state.EIP) << " ";
		std::cout << "Read IO8:[" << cpputil::Ustox(ioport) << "] " << cpputil::Ubtox(data);
		auto iter=ioLabel.find(ioport);
		if(ioLabel.end()!=iter)
		{
			std::cout << "(" << iter->second << ")" << std::endl;
		}
		std::cout << std::endl;
	}
}
void i486Debugger::IOReadWord(const i486DX &cpu,unsigned int ioport,unsigned int data)
{
	specialDebugInfo->IOReadWord(*this,cpu,ioport,data);
	if(true==monitorIO)
	{
		std::cout << cpputil::Ustox(cpu.state.CS().value) << ":" << cpputil::Uitox(cpu.state.EIP) << " ";
		std::cout << "Read IO16:[" << cpputil::Ustox(ioport) << "] " << cpputil::Ubtox(data);
		auto iter=ioLabel.find(ioport);
		if(ioLabel.end()!=iter)
		{
			std::cout << "(" << iter->second << ")" << std::endl;
		}
		std::cout << std::endl;
	}
}
void i486Debugger::IOReadDword(const i486DX &cpu,unsigned int ioport,unsigned int data)
{
	specialDebugInfo->IOReadDword(*this,cpu,ioport,data);
	if(true==monitorIO)
	{
		std::cout << cpputil::Ustox(cpu.state.CS().value) << ":" << cpputil::Uitox(cpu.state.EIP) << " ";
		std::cout << "Read IO32:[" << cpputil::Ustox(ioport) << "] " << cpputil::Ubtox(data);
		auto iter=ioLabel.find(ioport);
		if(ioLabel.end()!=iter)
		{
			std::cout << "(" << iter->second << ")" << std::endl;
		}
		std::cout << std::endl;
	}
}
