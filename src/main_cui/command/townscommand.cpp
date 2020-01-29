#include <iostream>
#include <fstream>

#include "townscommand.h"
#include "townscommandutil.h"
#include "cpputil.h"



TownsCommandInterpreter::TownsCommandInterpreter()
{
	primaryCmdMap["HELP"]=CMD_HELP;
	primaryCmdMap["HLP"]=CMD_HELP;
	primaryCmdMap["H"]=CMD_HELP;
	primaryCmdMap["QUIT"]=CMD_QUIT;
	primaryCmdMap["Q"]=CMD_QUIT;
	primaryCmdMap["?"]=CMD_HELP;
	primaryCmdMap["RUN"]=CMD_RUN;
	primaryCmdMap["PAUSE"]=CMD_PAUSE;
	primaryCmdMap["PAU"]=CMD_PAUSE;
	primaryCmdMap["RET"]=CMD_RETURN_FROM_PROCEDURE;
	primaryCmdMap["RTS"]=CMD_RETURN_FROM_PROCEDURE;
	primaryCmdMap["ENABLE"]=CMD_ENABLE;
	primaryCmdMap["ENA"]=CMD_ENABLE;
	primaryCmdMap["DISABLE"]=CMD_DISABLE;
	primaryCmdMap["DIS"]=CMD_DISABLE;
	primaryCmdMap["PRINT"]=CMD_PRINT;
	primaryCmdMap["PRI"]=CMD_PRINT;
	primaryCmdMap["BRK"]=CMD_ADD_BREAKPOINT;
	primaryCmdMap["BREAK"]=CMD_ADD_BREAKPOINT;
	primaryCmdMap["DLBRK"]=CMD_DELETE_BREAKPOINT;
	primaryCmdMap["CLBRK"]=CMD_CLEAR_BREAKPOINT;

	featureMap["CMDLOG"]=ENABLE_CMDLOG;
	featureMap["AUTODISASM"]=ENABLE_DISASSEMBLE_EVERY_INST;

	printableMap["CALLSTACK"]=PRINT_CALLSTACK;
	printableMap["BREAKPOINT"]=PRINT_BREAKPOINT;
	printableMap["BRK"]=PRINT_BREAKPOINT;
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
	std::cout << "PAUSE|PAU" << std::endl;
	std::cout << "  Pause VM." << std::endl;
	std::cout << "RET|RTS" << std::endl;
	std::cout << "  Run until return from the current procedure." << std::endl;
	std::cout << "  Available only when call-stack is enabled." << std::endl;
	std::cout << "ENA feature|ENABLE feature" << std::endl;
	std::cout << "  Enable a feature." << std::endl;
	std::cout << "DIS feature|DISABLE feature" << std::endl;
	std::cout << "  Disable a feature." << std::endl;
	std::cout << "PRINT info|PRI info" << std::endl;
	std::cout << "  Print information." << std::endl;
	std::cout << "BREAK EIP|BRK EIP" << std::endl;
	std::cout << "BREAK CS:EIP|BRK CS:EIP" << std::endl;
	std::cout << "  Add a break point." << std::endl;
	std::cout << "DLBRK Num" << std::endl;
	std::cout << "  Delete a break point." << std::endl;
	std::cout << "  Num is the number printed by PRINT BRK." << std::endl;
	std::cout << "CLBRK" << std::endl;
	std::cout << "  Clear all break points." << std::endl;

	std::cout << "" << std::endl;

	std::cout << "<< Features that can be enabled|disabled >>" << std::endl;
	std::cout << "CMDLOG" << std::endl;
	std::cout << "  Command log.  Saved to CMD.LOG." << std::endl;
	std::cout << "AUTODISASM" << std::endl;
	std::cout << "  Disassemble while running." << std::endl;

	std::cout << "" << std::endl;

	std::cout << "<< Information that can be printed >>" << std::endl;
	std::cout << "CALLSTACK" << std::endl;
	std::cout << "BREAKPOINT|BRK" << std::endl;
}

void TownsCommandInterpreter::PrintError_TooFewArguments(void) const
{
	std::cout << "Error: Too few arguments." << std::endl;
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
	case CMD_HELP:
		PrintHelp();
		break;
	case CMD_QUIT:
		thr.SetRunMode(TownsThread::RUNMODE_EXIT);
		break;
	case CMD_RUN:
		towns.debugger.stop=false;
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
	case CMD_PAUSE:
		thr.SetRunMode(TownsThread::RUNMODE_PAUSE);
		thr.PrintStatus(towns);
		break;
	case CMD_RETURN_FROM_PROCEDURE:
		break;

	case CMD_ENABLE:
		Execute_Enable(towns,cmd);
		break;
	case CMD_DISABLE:
		Execute_Disable(towns,cmd);
		break;

	case CMD_PRINT:
		break;

	case CMD_ADD_BREAKPOINT:
		break;
	case CMD_DELETE_BREAKPOINT:
		break;
	case CMD_CLEAR_BREAKPOINT:
		break;

	}
}

void TownsCommandInterpreter::Execute_Enable(FMTowns &towns,Command &cmd)
{
	if(cmd.argv.size()<2)
	{
		PrintError_TooFewArguments();
		return;
	}
	auto iter=featureMap.find(cmd.argv[1]);
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
		}
	}
}
void TownsCommandInterpreter::Execute_Disable(FMTowns &towns,Command &cmd)
{
	if(cmd.argv.size()<2)
	{
		PrintError_TooFewArguments();
		return;
	}
	auto iter=featureMap.find(cmd.argv[1]);
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
		}
	}
}
