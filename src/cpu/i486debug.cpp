#include <iostream>
#include <fstream>

#include "i486.h"
#include "i486inst.h"
#include "i486debug.h"



void i486Debugger::CS_EIP::Nullify(void)
{
	CS=0;
	EIP=0;
}


////////////////////////////////////////////////////////////


i486Debugger::i486Debugger()
{
	CleanUp();
}
void i486Debugger::CleanUp(void)
{
	breakPoint.clear();
	stop=true;
	disassembleEveryStep=false;
	lastDisassembleAddr.Nullify();
}
void i486Debugger::AddBreakPoint(unsigned int CS,unsigned int EIP)
{
	CS_EIP bp;
	bp.CS=CS;
	bp.EIP=EIP;
	auto iter=breakPoint.find(bp);
	if(breakPoint.end()!=iter)
	{
		breakPoint.insert(bp);
	}
}
void i486Debugger::RemoveBreakPoint(unsigned int CS,unsigned int EIP)
{
	CS_EIP bp;
	bp.CS=CS;
	bp.EIP=EIP;
	auto iter=breakPoint.find(bp);
	if(breakPoint.end()!=iter)
	{
		breakPoint.erase(iter);
	}
}

void i486Debugger::SetOneTimeBreakPoint(unsigned int CS,unsigned int EIP)
{
	oneTimeBreakPoint.CS=CS;
	oneTimeBreakPoint.EIP=EIP;
}

unsigned int i486Debugger::RunOneInstruction(i486DX &cpu,Memory &mem,InOut &io)
{
	auto clocksPassed=cpu.RunOneInstruction(mem,io);

	CS_EIP cseip;
	cseip.CS=cpu.state.CS().value;
	cseip.EIP=cpu.state.EIP;

	if(true==disassembleEveryStep && lastDisassembleAddr!=cseip)
	{
		auto inst=cpu.FetchInstruction(mem);
		auto disasm=cpu.Disassemble(inst,cpu.state.CS(),cpu.state.EIP,mem);
		std::cout << disasm << std::endl;
	}

	if(breakPoint.find(cseip)!=breakPoint.end())
	{
		stop=true;
	}
	if(oneTimeBreakPoint==cseip)
	{
		stop=true;
		oneTimeBreakPoint.Nullify();
	}

	return clocksPassed;
}
