#include <iostream>
#include <fstream>

#include "i486.h"
#include "i486inst.h"
#include "i486debug.h"
#include "cpputil.h"



i486Debugger::i486Debugger()
{
	CleanUp();
}
void i486Debugger::CleanUp(void)
{
	breakPoints.clear();
	stop=true;
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
	CS_EIP cseip;
	cseip.SEG=cpu.state.CS().value;
	cseip.OFFSET=cpu.state.EIP;

	CSEIPLog[CSEIPLogPtr].SEG=cpu.state.CS().value;
	CSEIPLog[CSEIPLogPtr].OFFSET=cpu.state.EIP;
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
		auto disasm=cpu.Disassemble(inst,cpu.state.CS(),cpu.state.EIP,mem);
		lastDisassembleAddr.SEG=cpu.state.CS().value;
		lastDisassembleAddr.OFFSET=cpu.state.EIP;
		std::cout << disasm << std::endl;
	}
}

void i486Debugger::AfterRunOneInstruction(unsigned int clocksPassed,i486DX &cpu,Memory &mem,InOut &io,const i486DX::Instruction &inst)
{
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

void i486Debugger::IOWriteByte(const i486DX &cpu,unsigned int ioport,unsigned int data)
{
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
