#ifndef TOWNS_CMD_IS_INCLUDED
#define TOWNS_CMD_IS_INCLUDED
/* { */

#include <vector>
#include <string>
#include <unordered_map>

#include "towns.h"
#include "townsthread.h"
#include "i486symtable.h"

class TownsCommandInterpreter
{
private:
	std::unordered_map <std::string,unsigned int> primaryCmdMap;
	std::unordered_map <std::string,unsigned int> featureMap;
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

		CMD_INTERRUPT,

		CMD_RETURN_FROM_PROCEDURE,

		CMD_ENABLE,
		CMD_DISABLE,
		// ENABLE CMDLOG
		// DISABLE CMDLOG

		CMD_DUMP,
		CMD_PRINT_STATUS,
		CMD_PRINT_HISTORY,

		CMD_ADD_BREAKPOINT,
		CMD_DELETE_BREAKPOINT,
		CMD_LIST_BREAKPOINTS,

		CMD_BREAK_ON,
		CMD_DONT_BREAK_ON,

		CMD_DISASM,
		CMD_DISASM16,
		CMD_DISASM32,

		CMD_ADD_SYMBOL,
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
		DUMP_NONE,
		DUMP_CURRENT_STATUS,
		DUMP_CALLSTACK,
		DUMP_BREAKPOINT,
		DUMP_PIC,
		DUMP_DMAC,
		DUMP_FDC,
		DUMP_TIMER,
		DUMP_GDT,
		DUMP_LDT,
		DUMP_IDT,
		DUMP_SOUND,
		DUMP_REAL_MODE_INT_VECTOR,
		DUMP_CSEIP_LOG,
		DUMP_SYMBOL_TABLE,
	};

	enum
	{
		BREAK_ON_PIC_IWC1,
		BREAK_ON_PIC_IWC4,
		BREAK_ON_DMAC_REQUEST,
		BREAK_ON_FDC_COMMAND,
		BREAK_ON_INT,
		BREAK_ON_CVRAM_READ,
		BREAK_ON_CVRAM_WRITE,
		BREAK_ON_FMRVRAM_READ,
		BREAK_ON_FMRVRAM_WRITE,
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

	void Execute_AddBreakPoint(FMTowns &towns,Command &cmd);
	void Execute_DeleteBreakPoint(FMTowns &towns,Command &cmd);
	void Execute_ListBreakPoints(FMTowns &towns,Command &cmd);

	void Execute_Dump(FMTowns &towns,Command &cmd);

	void Execute_BreakOn(FMTowns &towns,Command &cmd);
	void Execute_ClearBreakOn(FMTowns &towns,Command &cmd);

	void Execute_Disassemble(FMTowns &towns,Command &cmd);
	void Execute_Disassemble16(FMTowns &towns,Command &cmd);
	void Execute_Disassemble32(FMTowns &towns,Command &cmd);

	void Execute_PrintHistory(FMTowns &towns,unsigned int n);

	void Execute_AddSymbol(FMTowns &towns,Command &cmd);
};


/* } */
#endif
