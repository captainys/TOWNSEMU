#ifndef TOWNS_CMD_IS_INCLUDED
#define TOWNS_CMD_IS_INCLUDED
/* { */

#include <vector>
#include <string>
#include <unordered_map>

#include "towns.h"
#include "townsthread.h"

class TownsCommandInterpreter
{
private:
	std::unordered_map <std::string,unsigned int> primaryCmdMap;
	std::unordered_map <std::string,unsigned int> featureMap;
	std::unordered_map <std::string,unsigned int> printableMap;
	std::unordered_map <std::string,unsigned int> dumpableMap;
	std::unordered_map <std::string,unsigned int> breakEventMap;

public:
	enum
	{
		CMD_NONE,

		CMD_QUIT,

		CMD_HELP,

		CMD_RUN,
		CMD_RUN_ONE_INSTRUCTION,
		CMD_PAUSE,
		CMD_WAIT,

		CMD_RETURN_FROM_PROCEDURE,

		CMD_ENABLE,
		CMD_DISABLE,
		// ENABLE CMDLOG
		// DISABLE CMDLOG

		CMD_PRINT,
		CMD_DUMP,

		CMD_ADD_BREAKPOINT,
		CMD_DELETE_BREAKPOINT,
		CMD_CLEAR_BREAKPOINT,

		CMD_BREAK_ON,
		CMD_DONT_BREAK_ON,
	};

	enum
	{
		ENABLE_NONE,
		ENABLE_CMDLOG,
		ENABLE_DISASSEMBLE_EVERY_INST,
		ENABLE_IOMONITOR,
	};

	enum
	{
		PRINT_NONE,
		PRINT_CURRENT_STATUS,
		PRINT_CALLSTACK,
		PRINT_BREAKPOINT,
		PRINT_PIC,
	};

	enum
	{
		BREAK_ON_PIC_IWC1,
		BREAK_ON_PIC_IWC4,
		BREAK_ON_DMAC_REQUEST,
		BREAK_ON_FDC_COMMAND,
	};

	enum
	{
		DUMP_REAL_MODE_INT_VECTOR
	};

	enum
	{
		ERROR_TOO_FEW_ARGS,
		ERROR_DUMP_TARGET_UNDEFINED,
	};

	class Command
	{
	public:
		std::string cmdline;
		std::vector <std::string> argv;
		int primaryCmd;
	};

	bool waitVM;

	TownsCommandInterpreter();

	void PrintHelp(void) const;
	void PrintError(int errCode) const;

	Command Interpret(const std::string &cmdline) const;

	/*! Executes a command.
	    VM must be locked before calling.
	*/
	void Execute(TownsThread &thr,FMTowns &towns,Command &cmd);

	void Execute_Enable(FMTowns &towns,Command &cmd);
	void Execute_Disable(FMTowns &towns,Command &cmd);

	void Execute_Dump(FMTowns &towns,Command &cmd);

	void Execute_Print(FMTowns &towns,Command &cmd);

	void Execute_BreakOn(FMTowns &towns,Command &cmd);
	void Execute_ClearBreakOn(FMTowns &towns,Command &cmd);
};


/* } */
#endif
