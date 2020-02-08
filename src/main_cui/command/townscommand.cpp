#include <iostream>
#include <fstream>

#include "townscommand.h"
#include "townscommandutil.h"
#include "cpputil.h"
#include "miscutil.h"



TownsCommandInterpreter::TownsCommandInterpreter()
{
	waitVM=false;

	primaryCmdMap["HELP"]=CMD_HELP;
	primaryCmdMap["HLP"]=CMD_HELP;
	primaryCmdMap["H"]=CMD_HELP;
	primaryCmdMap["QUIT"]=CMD_QUIT;
	primaryCmdMap["Q"]=CMD_QUIT;
	primaryCmdMap["?"]=CMD_HELP;
	primaryCmdMap["RUN"]=CMD_RUN;
	primaryCmdMap["PAUSE"]=CMD_PAUSE;
	primaryCmdMap["WAIT"]=CMD_WAIT;
	primaryCmdMap["PAU"]=CMD_PAUSE;
	primaryCmdMap["RET"]=CMD_RETURN_FROM_PROCEDURE;
	primaryCmdMap["RTS"]=CMD_RETURN_FROM_PROCEDURE;
	primaryCmdMap["ENABLE"]=CMD_ENABLE;
	primaryCmdMap["ENA"]=CMD_ENABLE;
	primaryCmdMap["DISABLE"]=CMD_DISABLE;
	primaryCmdMap["DIS"]=CMD_DISABLE;
	primaryCmdMap["PRINT"]=CMD_DUMP;
	primaryCmdMap["PRI"]=CMD_DUMP;
	primaryCmdMap["DUMP"]=CMD_DUMP;
	primaryCmdMap["DM"]=CMD_DUMP;
	primaryCmdMap["BP"]=CMD_ADD_BREAKPOINT;
	primaryCmdMap["BC"]=CMD_DELETE_BREAKPOINT;
	primaryCmdMap["BL"]=CMD_LIST_BREAKPOINTS;
	primaryCmdMap["T"]=CMD_RUN_ONE_INSTRUCTION;
	primaryCmdMap["U"]=CMD_DISASM;
	primaryCmdMap["U16"]=CMD_DISASM16;
	primaryCmdMap["U32"]=CMD_DISASM32;
	primaryCmdMap["BRKON"]=CMD_BREAK_ON;
	primaryCmdMap["CBRKON"]=CMD_DONT_BREAK_ON;
	primaryCmdMap["INTERRUPT"]=CMD_INTERRUPT;

	featureMap["CMDLOG"]=ENABLE_CMDLOG;
	featureMap["AUTODISASM"]=ENABLE_DISASSEMBLE_EVERY_INST;
	featureMap["IOMON"]=ENABLE_IOMONITOR;

	dumpableMap["CALLSTACK"]=DUMP_CALLSTACK;
	dumpableMap["CST"]=DUMP_CALLSTACK;
	dumpableMap["BREAKPOINT"]=DUMP_BREAKPOINT;
	dumpableMap["BRK"]=DUMP_BREAKPOINT;
	dumpableMap["STATUS"]=DUMP_CURRENT_STATUS;
	dumpableMap["STATE"]=DUMP_CURRENT_STATUS;
	dumpableMap["STA"]=DUMP_CURRENT_STATUS;
	dumpableMap["S"]=DUMP_CURRENT_STATUS;
	dumpableMap["CSEIPLOG"]=DUMP_CSEIP_LOG;
	dumpableMap["PIC"]=DUMP_PIC;
	dumpableMap["DMA"]=DUMP_DMAC;
	dumpableMap["DMAC"]=DUMP_DMAC;
	dumpableMap["FDC"]=DUMP_FDC;
	dumpableMap["TIMER"]=DUMP_TIMER;
	dumpableMap["GDT"]=DUMP_GDT;
	dumpableMap["IDT"]=DUMP_IDT;
	dumpableMap["SOUND"]=DUMP_SOUND;
	dumpableMap["RIDT"]=DUMP_REAL_MODE_INT_VECTOR;

	breakEventMap["IWC1"]=   BREAK_ON_PIC_IWC1;
	breakEventMap["IWC4"]=   BREAK_ON_PIC_IWC4;
	breakEventMap["DMACREQ"]=BREAK_ON_DMAC_REQUEST;
	breakEventMap["FDCCMD"]= BREAK_ON_FDC_COMMAND;
	breakEventMap["INT"]=    BREAK_ON_INT;
	breakEventMap["RDCVRAM"]=BREAK_ON_CVRAM_READ;
	breakEventMap["WRCVRAM"]=BREAK_ON_CVRAM_WRITE;
	breakEventMap["RDFMRVRAM"]=BREAK_ON_FMRVRAM_READ;
	breakEventMap["WRFMRVRAM"]=BREAK_ON_FMRVRAM_WRITE;
}


void TownsCommandInterpreter::PrintHelp(void) const
{
	std::cout << "<< Primary Command >>" << std::endl;
	std::cout << "HELP|HLP|H|?" << std::endl;
	std::cout << "  Print help." << std::endl;
	std::cout << "QUIT|Q" << std::endl;
	std::cout << "  Quit." << std::endl;
	std::cout << "RUN|RUN EIP|RUN CS:EIP" << std::endl;
	std::cout << "  Run.  Can specify temporary break point." << std::endl;
	std::cout << "T" << std::endl;
	std::cout << "  Trace.  Run one instruction." << std::endl;
	std::cout << "INTERRUPT INTNum" << std::endl;
	std::cout << "  Inject interrupt.  Same as CPU Instruction INT INTNum.  INTNum is hexadecimal." << std::endl;
	std::cout << "  For example, INTERRUPT 4B will work same as INT 4BH." << std::endl;
	std::cout << "U addr" << std::endl;
	std::cout << "  Unassemble (disassemble)" << std::endl;
	std::cout << "U16 addr" << std::endl;
	std::cout << "  Unassemble (disassemble) as 16-bit operand size" << std::endl;
	std::cout << "U32 addr" << std::endl;
	std::cout << "  Unassemble (disassemble) as 32-bit operand size" << std::endl;
	std::cout << "PAUSE|PAU" << std::endl;
	std::cout << "  Pause VM." << std::endl;
	std::cout << "WAIT" << std::endl;
	std::cout << "  Wait until VM becomes PAUSE state." << std::endl;
	std::cout << "RET|RTS" << std::endl;
	std::cout << "  Run until return from the current procedure." << std::endl;
	std::cout << "  Available only when call-stack is enabled." << std::endl;
	std::cout << "ENA feature|ENABLE feature" << std::endl;
	std::cout << "  Enable a feature." << std::endl;
	std::cout << "DIS feature|DISABLE feature" << std::endl;
	std::cout << "  Disable a feature." << std::endl;
	std::cout << "PRINT info|PRI info" << std::endl;
	std::cout << "DUMP info|DM info" << std::endl;
	std::cout << "  Print/Dump information." << std::endl;
	std::cout << "BP EIP|BRK EIP" << std::endl;
	std::cout << "BP CS:EIP|BRK CS:EIP" << std::endl;
	std::cout << "  Add a break point." << std::endl;
	std::cout << "BC Num" << std::endl;
	std::cout << "  Delete a break point." << std::endl;
	std::cout << "  Num is the number printed by PRINT BRK." << std::endl;
	std::cout << "  BC * to erase all break points." << std::endl;
	std::cout << "BL" << std::endl;
	std::cout << "  List break points." << std::endl;
	std::cout << "BRKON" << std::endl;
	std::cout << "  Break on event." << std::endl;
	std::cout << "CBRKON" << std::endl;
	std::cout << "  Clear break-on event." << std::endl;

	std::cout << "" << std::endl;

	std::cout << "<< Features that can be enabled|disabled >>" << std::endl;
	std::cout << "CMDLOG" << std::endl;
	std::cout << "  Command log.  Saved to CMD.LOG." << std::endl;
	std::cout << "AUTODISASM" << std::endl;
	std::cout << "  Disassemble while running." << std::endl;
	std::cout << "IOMON" << std::endl;
	std::cout << "  IO Monitor." << std::endl;

	std::cout << "" << std::endl;

	std::cout << "<< Information that can be printed >>" << std::endl;
	std::cout << "CALLSTACK|CST" << std::endl;
	std::cout << "  Call Stack"<<std::endl;
	std::cout << "CSEIPLOG" << std::endl;
	std::cout << "  Log of CS:EIP.  Can specify number of steps." << std::endl;
	std::cout << "GDT" << std::endl;
	std::cout << "  Protected-Mode Global Descriptor Table" << std::endl;
	std::cout << "IDT" << std::endl;
	std::cout << "  Protected-Mode Interrupt Descriptor Table" << std::endl;
	std::cout << "RIDT" << std::endl;
	std::cout << "  Real-mode Interrupt Descriptor Tables" << std::endl;
	std::cout << "BREAKPOINT|BRK" << std::endl;
	std::cout << "  Break Points"<<std::endl;
	std::cout << "STATUS|STATE|STA|S"<<std::endl;
	std::cout << "  Current status"<<std::endl;
	std::cout << "PIC" << std::endl;
	std::cout << "  Pilot-In-Command. No!  Programmable Interrupt Controller." << std::endl;
	std::cout << "DMA|DMAC" << std::endl;
	std::cout << "  DMA Controller." << std::endl;
	std::cout << "FDC" << std::endl;
	std::cout << "  Floppy Disk Controller." << std::endl;
	std::cout << "TIMER" << std::endl;
	std::cout << "  Interval Timer (i8253)" << std::endl;

	std::cout << "" << std::endl;

	std::cout << "<< Event that can break >>" << std::endl;
	std::cout << "IWC1" << std::endl;
	std::cout << "IWC4" << std::endl;
	std::cout << "DMACREQ" << std::endl;
	std::cout << "FDCCMD" << std::endl;
	std::cout << "INT n" << std::endl;
	std::cout << "RDCVRAM" << std::endl;
	std::cout << "WRCVRAM" << std::endl;
	std::cout << "RDFMRVRAM" << std::endl;
	std::cout << "WRFMRVRAM" << std::endl;
}

void TownsCommandInterpreter::PrintError(int errCode) const
{
	switch(errCode)
	{
	case ERROR_TOO_FEW_ARGS:
		std::cout << "Error: Too few arguments." << std::endl;
		break;
	case ERROR_DUMP_TARGET_UNDEFINED:
		std::cout << "Error: Do what?" << std::endl;
		break;
	default:
		std::cout << "Error" << std::endl;
		break;
	}
}

TownsCommandInterpreter::Command TownsCommandInterpreter::Interpret(const std::string &cmdline) const
{
	Command cmd;

	cmd.cmdline=cmdline;
	cmd.argv=cpputil::Parser(cmdline.c_str());
	cmd.primaryCmd=CMD_NONE;

	if(0<cmd.argv.size())
	{
		auto primaryCmd=cmd.argv[0];
		cpputil::Capitalize(primaryCmd);
		auto iter=primaryCmdMap.find(primaryCmd);
		if(primaryCmdMap.end()!=iter)
		{
			cmd.primaryCmd=iter->second;
		}
	}

	return cmd;
}

void TownsCommandInterpreter::Execute(TownsThread &thr,FMTowns &towns,Command &cmd)
{
	switch(cmd.primaryCmd)
	{
	case CMD_NONE:
		std::cout << "Do what?" << std::endl;
		break;
	case CMD_HELP:
		PrintHelp();
		break;
	case CMD_QUIT:
		thr.SetRunMode(TownsThread::RUNMODE_EXIT);
		break;
	case CMD_RUN:
		towns.debugger.ClearStopFlag();
		thr.SetRunMode(TownsThread::RUNMODE_DEBUGGER);
		if(1<cmd.argv.size())
		{
			auto farPtr=cmdutil::MakeFarPointer(cmd.argv[1]);
			if(farPtr.SEG==i486DX::FarPointer::NO_SEG)
			{
				farPtr.SEG=towns.cpu.state.CS().value;
			}
			towns.debugger.oneTimeBreakPoint=farPtr;
		}
		break;
	case CMD_INTERRUPT:
		if(1<cmd.argv.size())
		{
			towns.cpu.Interrupt(cpputil::Xtoi(cmd.argv[1].c_str()),towns.mem,0);
			towns.PrintStatus();
		}
		else
		{
			PrintError(ERROR_TOO_FEW_ARGS);
		}
		break;
	case CMD_DISASM:
		Execute_Disassemble(towns,cmd);
		break;
	case CMD_DISASM16:
		Execute_Disassemble16(towns,cmd);
		break;
	case CMD_DISASM32:
		Execute_Disassemble32(towns,cmd);
		break;
	case CMD_PAUSE:
		thr.SetRunMode(TownsThread::RUNMODE_PAUSE);
		thr.PrintStatus(towns);
		break;
	case CMD_WAIT:
		waitVM=true;
		break;
	case CMD_RETURN_FROM_PROCEDURE:
		if(0<towns.cpu.callStack.size())
		{
			auto s=towns.cpu.callStack.back();
			towns.debugger.ClearStopFlag();
			towns.debugger.SetOneTimeBreakPoint(s.fromCS,s.fromEIP+s.callOpCodeLength);
			thr.SetRunMode(TownsThread::RUNMODE_DEBUGGER);
		}
		break;

	case CMD_ENABLE:
		Execute_Enable(towns,cmd);
		break;
	case CMD_DISABLE:
		Execute_Disable(towns,cmd);
		break;

	case CMD_DUMP:
		Execute_Dump(towns,cmd);
		break;

	case CMD_RUN_ONE_INSTRUCTION:
		thr.SetRunMode(TownsThread::RUNMODE_ONE_INSTRUCTION);
		break;

	case CMD_ADD_BREAKPOINT:
		Execute_AddBreakPoint(towns,cmd);
		break;
	case CMD_DELETE_BREAKPOINT:
		Execute_DeleteBreakPoint(towns,cmd);
		break;
	case CMD_LIST_BREAKPOINTS:
		Execute_ListBreakPoints(towns,cmd);
		break;

	case CMD_BREAK_ON:
		Execute_BreakOn(towns,cmd);
		break;
	case CMD_DONT_BREAK_ON:
		Execute_ClearBreakOn(towns,cmd);
		break;
	}
}

void TownsCommandInterpreter::Execute_Enable(FMTowns &towns,Command &cmd)
{
	if(cmd.argv.size()<2)
	{
		PrintError(ERROR_TOO_FEW_ARGS);
		return;
	}
	auto argv1=cmd.argv[1];
	cpputil::Capitalize(argv1);
	auto iter=featureMap.find(argv1);
	if(featureMap.end()!=iter)
	{
		switch(iter->second)
		{
		case ENABLE_CMDLOG:
			break;
		case ENABLE_DISASSEMBLE_EVERY_INST:
			towns.debugger.disassembleEveryStep=true;
			std::cout << "Disassemble_Every_Step is ON." << std::endl;
			break;
		case ENABLE_IOMONITOR:
			towns.debugger.monitorIO=true;
			std::cout << "IO_Monitor is ON." << std::endl;
			break;
		}
	}
}
void TownsCommandInterpreter::Execute_Disable(FMTowns &towns,Command &cmd)
{
	if(cmd.argv.size()<2)
	{
		PrintError(ERROR_TOO_FEW_ARGS);
		return;
	}
	auto argv1=cmd.argv[1];
	cpputil::Capitalize(argv1);
	auto iter=featureMap.find(argv1);
	if(featureMap.end()!=iter)
	{
		switch(iter->second)
		{
		case ENABLE_CMDLOG:
			break;
		case ENABLE_DISASSEMBLE_EVERY_INST:
			towns.debugger.disassembleEveryStep=false;
			std::cout << "Disassemble_Every_Step is OFF." << std::endl;
			break;
		case ENABLE_IOMONITOR:
			towns.debugger.monitorIO=false;
			std::cout << "IO_Monitor is OFF." << std::endl;
			break;
		}
	}
}

void TownsCommandInterpreter::Execute_AddBreakPoint(FMTowns &towns,Command &cmd)
{
	if(cmd.argv.size()<2)
	{
		PrintError(ERROR_TOO_FEW_ARGS);
		return;
	}
	auto farPtr=cmdutil::MakeFarPointer(cmd.argv[1]);
	towns.debugger.AddBreakPoint(farPtr);
}
void TownsCommandInterpreter::Execute_DeleteBreakPoint(FMTowns &towns,Command &cmd)
{
	if(cmd.argv.size()<2)
	{
		PrintError(ERROR_TOO_FEW_ARGS);
		return;
	}
	if("*"==cmd.argv[1])
	{
		towns.debugger.ClearBreakPoints();
	}
	else
	{
		auto list=towns.debugger.GetBreakPoints();
		auto bpn=cpputil::Xtoi(cmd.argv[1].c_str());
		if(0<=bpn && bpn<list.size())
		{
			towns.debugger.RemoveBreakPoint(list[bpn]);
		}
	}
}
void TownsCommandInterpreter::Execute_ListBreakPoints(FMTowns &towns,Command &cmd)
{
	int bpn=0;
	for(auto bp : towns.debugger.GetBreakPoints())
	{
		std::cout << cpputil::Ubtox(bpn) << " " << cpputil::Ustox(bp.SEG) << ":" << cpputil::Uitox(bp.OFFSET) << std::endl;
		++bpn;
	}
}

void TownsCommandInterpreter::Execute_Dump(FMTowns &towns,Command &cmd)
{
	if(cmd.argv.size()<2)
	{
		PrintError(ERROR_TOO_FEW_ARGS);
		return;
	}
	auto argv1=cmd.argv[1];
	cpputil::Capitalize(argv1);
	auto printIter=dumpableMap.find(argv1);
	if(printIter!=dumpableMap.end())
	{
		switch(printIter->second)
		{
		case DUMP_REAL_MODE_INT_VECTOR:
			towns.DumpRealModeIntVectors();
			break;
		case DUMP_CURRENT_STATUS:
			towns.PrintStatus();
			break;
		case DUMP_GDT:
			towns.cpu.PrintGDT(towns.mem);
			break;
		case DUMP_IDT:
			towns.cpu.PrintIDT(towns.mem);
			break;
		case DUMP_SOUND:
			towns.PrintSound();
			break;
		case DUMP_CALLSTACK:
			towns.PrintCallStack();
			break;
		case DUMP_PIC:
			towns.PrintPIC();
			break;
		case DUMP_DMAC:
			towns.PrintDMAC();
			break;
		case DUMP_FDC:
			towns.PrintFDC();
			break;
		case DUMP_TIMER:
			towns.PrintTimer();
			break;
		case DUMP_CSEIP_LOG:
			{
				unsigned int n=20;
				if(3<=cmd.argv.size())
				{
					n=cpputil::Atoi(cmd.argv[2].c_str());
				}
				auto list=towns.debugger.GetCSEIPLog(n);
				for(auto iter=list.rbegin(); iter!=list.rend(); ++iter)
				{
					std::cout << cpputil::Ustox(iter->SEG) << ":" << cpputil::Uitox(iter->OFFSET);
					if(1<iter->count)
					{
						std::cout << "(" << cpputil::Itoa(iter->count) << ")";
					}
					std::cout << std::endl;
				}
			}
			break;
		}
	}
	else
	{
		auto farPtr=cmdutil::MakeFarPointer(cmd.argv[1]);
		if(i486DX::FarPointer::NO_SEG!=farPtr.SEG)
		{
			for(auto str : miscutil::MakeMemDump(towns.cpu,towns.mem,farPtr,256,/*shiftJIS*/false))
			{
				std::cout << str << std::endl;
			}
		}
		else
		{
			PrintError(ERROR_DUMP_TARGET_UNDEFINED);
			return;
		}
	}
}

void TownsCommandInterpreter::Execute_BreakOn(FMTowns &towns,Command &cmd)
{
	if(cmd.argv.size()<2)
	{
		PrintError(ERROR_TOO_FEW_ARGS);
		return;
	}
	auto argv1=cmd.argv[1];
	cpputil::Capitalize(argv1);
	auto iter=breakEventMap.find(argv1);
	if(iter!=breakEventMap.end())
	{
		std::string reason=iter->first;
		switch(iter->second)
		{
		case BREAK_ON_PIC_IWC1:
			towns.pic.debugBreakOnICW1Write=true;
			break;
		case BREAK_ON_PIC_IWC4:
			towns.pic.debugBreakOnICW4Write=true;
			break;
		case BREAK_ON_DMAC_REQUEST:
			towns.dmac.debugBreakOnDMACRequest=true;
			break;
		case BREAK_ON_FDC_COMMAND:
			towns.fdc.debugBreakOnCommandWrite=true;
			break;
		case BREAK_ON_INT:
			if(3<=cmd.argv.size())
			{
				towns.debugger.SetBreakOnINT(cpputil::Xtoi(cmd.argv[2].c_str()));
			}
			else
			{
				PrintError(ERROR_TOO_FEW_ARGS);
				return;
			}
			break;
		case BREAK_ON_CVRAM_READ:
			towns.mainRAMorFMRVRAMAccess.breakOnCVRAMRead=true;
			break;
		case BREAK_ON_CVRAM_WRITE:
			towns.mainRAMorFMRVRAMAccess.breakOnCVRAMWrite=true;
			break;
		case BREAK_ON_FMRVRAM_READ:
			towns.mainRAMorFMRVRAMAccess.breakOnFMRVRAMRead=true;
			break;
		case BREAK_ON_FMRVRAM_WRITE:
			towns.mainRAMorFMRVRAMAccess.breakOnFMRVRAMWrite=true;
			break;
		}
		std::cout << reason << " is ON." << std::endl;
	}
}
void TownsCommandInterpreter::Execute_ClearBreakOn(FMTowns &towns,Command &cmd)
{
	if(cmd.argv.size()<2)
	{
		PrintError(ERROR_TOO_FEW_ARGS);
		return;
	}
	auto argv1=cmd.argv[1];
	cpputil::Capitalize(argv1);
	auto iter=breakEventMap.find(argv1);
	if(iter!=breakEventMap.end())
	{
		switch(iter->second)
		{
		case BREAK_ON_PIC_IWC1:
			towns.pic.debugBreakOnICW1Write=false;
			break;
		case BREAK_ON_PIC_IWC4:
			towns.pic.debugBreakOnICW4Write=false;
			break;
		case BREAK_ON_DMAC_REQUEST:
			towns.dmac.debugBreakOnDMACRequest=false;
			break;
		case BREAK_ON_FDC_COMMAND:
			towns.fdc.debugBreakOnCommandWrite=false;
			break;
		case BREAK_ON_INT:
			towns.debugger.ClearBreakOnINT();
			break;
		case BREAK_ON_FMRVRAM_READ:
			towns.mainRAMorFMRVRAMAccess.breakOnFMRVRAMRead=false;
			break;
		case BREAK_ON_FMRVRAM_WRITE:
			towns.mainRAMorFMRVRAMAccess.breakOnFMRVRAMWrite=false;
			break;
		case BREAK_ON_CVRAM_READ:
			towns.mainRAMorFMRVRAMAccess.breakOnCVRAMRead=false;
			break;
		case BREAK_ON_CVRAM_WRITE:
			towns.mainRAMorFMRVRAMAccess.breakOnCVRAMWrite=false;
			break;
		}
		std::cout << iter->first << " is OFF." << std::endl;
	}
}

void TownsCommandInterpreter::Execute_Disassemble(FMTowns &towns,Command &cmd)
{
	auto farPtr=towns.var.disassemblePointer;
	if(2<=cmd.argv.size())
	{
		farPtr=cmdutil::MakeFarPointer(cmd.argv[1]);
		if(farPtr.SEG==i486DX::FarPointer::NO_SEG)
		{
			farPtr.SEG=towns.cpu.state.CS().value;
		}
		else if((farPtr.SEG&0xFFFF0000)==i486DX::FarPointer::SEG_REGISTER)
		{
			farPtr.SEG=towns.cpu.GetRegisterValue(farPtr.SEG&0xFFFF);
		}
		else if((farPtr.SEG&0xFFFF0000)==i486DX::FarPointer::PHYS_ADDR ||
		        (farPtr.SEG&0xFFFF0000)==i486DX::FarPointer::LINEAR_ADDR)
		{
			std::cout << "Disassembly cannot be from Linear or Physical address." << std::endl;
			return;
		}
	}

	i486DX::SegmentRegister seg;
	towns.cpu.LoadSegmentRegister(seg,farPtr.SEG,towns.mem);
	for(int i=0; i<16; ++i)
	{
		auto inst=towns.cpu.FetchInstruction(seg,farPtr.OFFSET,towns.mem);
		auto disasm=towns.cpu.Disassemble(inst,seg,farPtr.OFFSET,towns.mem);
		std::cout << disasm << std::endl;
		farPtr.OFFSET+=inst.numBytes;
	}
	towns.var.disassemblePointer=farPtr;
}
void TownsCommandInterpreter::Execute_Disassemble16(FMTowns &towns,Command &cmd)
{
	auto farPtr=towns.var.disassemblePointer;
	if(2<=cmd.argv.size())
	{
		farPtr=cmdutil::MakeFarPointer(cmd.argv[1]);
		if(farPtr.SEG==i486DX::FarPointer::NO_SEG)
		{
			farPtr.SEG=towns.cpu.state.CS().value;
		}
		else if((farPtr.SEG&0xFFFF0000)==i486DX::FarPointer::SEG_REGISTER)
		{
			farPtr.SEG=towns.cpu.GetRegisterValue(farPtr.SEG&0xFFFF);
		}
		else if((farPtr.SEG&0xFFFF0000)==i486DX::FarPointer::PHYS_ADDR ||
		        (farPtr.SEG&0xFFFF0000)==i486DX::FarPointer::LINEAR_ADDR)
		{
			std::cout << "Disassembly cannot be from Linear or Physical address." << std::endl;
			return;
		}
	}

	i486DX::SegmentRegister seg;
	towns.cpu.LoadSegmentRegister(seg,farPtr.SEG,towns.mem);
	for(int i=0; i<16; ++i)
	{
		auto inst=towns.cpu.FetchInstruction(seg,farPtr.OFFSET,towns.mem,16,16);
		auto disasm=towns.cpu.Disassemble(inst,seg,farPtr.OFFSET,towns.mem);
		std::cout << disasm << std::endl;
		farPtr.OFFSET+=inst.numBytes;
	}
	towns.var.disassemblePointer=farPtr;
}
void TownsCommandInterpreter::Execute_Disassemble32(FMTowns &towns,Command &cmd)
{
	auto farPtr=towns.var.disassemblePointer;
	if(2<=cmd.argv.size())
	{
		farPtr=cmdutil::MakeFarPointer(cmd.argv[1]);
		if(farPtr.SEG==i486DX::FarPointer::NO_SEG)
		{
			farPtr.SEG=towns.cpu.state.CS().value;
		}
		else if((farPtr.SEG&0xFFFF0000)==i486DX::FarPointer::SEG_REGISTER)
		{
			farPtr.SEG=towns.cpu.GetRegisterValue(farPtr.SEG&0xFFFF);
		}
		else if((farPtr.SEG&0xFFFF0000)==i486DX::FarPointer::PHYS_ADDR ||
		        (farPtr.SEG&0xFFFF0000)==i486DX::FarPointer::LINEAR_ADDR)
		{
			std::cout << "Disassembly cannot be from Linear or Physical address." << std::endl;
			return;
		}
	}

	i486DX::SegmentRegister seg;
	towns.cpu.LoadSegmentRegister(seg,farPtr.SEG,towns.mem);
	for(int i=0; i<16; ++i)
	{
		auto inst=towns.cpu.FetchInstruction(seg,farPtr.OFFSET,towns.mem,32,32);
		auto disasm=towns.cpu.Disassemble(inst,seg,farPtr.OFFSET,towns.mem);
		std::cout << disasm << std::endl;
		farPtr.OFFSET+=inst.numBytes;
	}
	towns.var.disassemblePointer=farPtr;
}
