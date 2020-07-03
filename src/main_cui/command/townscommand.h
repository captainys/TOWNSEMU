/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#ifndef TOWNS_CMD_IS_INCLUDED
#define TOWNS_CMD_IS_INCLUDED
/* { */

#include <vector>
#include <string>
#include <set>
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

	std::set <unsigned int> memFilter;

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

		CMD_MAKE_MEMORY_FILTER,
		CMD_UPDATE_MEMORY_FILTER,

		CMD_RETURN_FROM_PROCEDURE,

		CMD_ENABLE,
		CMD_DISABLE,
		// ENABLE CMDLOG
		// DISABLE CMDLOG

		CMD_DUMP,
		CMD_PRINT_STATUS,
		CMD_PRINT_HISTORY,
		CMD_SAVE_HISTORY,

		CMD_ADD_BREAKPOINT,
		CMD_DELETE_BREAKPOINT,
		CMD_LIST_BREAKPOINTS,

		CMD_BREAK_ON,
		CMD_DONT_BREAK_ON,

		CMD_DISASM,
		CMD_DISASM16,
		CMD_DISASM32,
		CMD_TRANSLATE_ADDRESS,

		CMD_ADD_SYMBOL,
		CMD_ADD_LABEL,
		CMD_ADD_DATALABEL,
		CMD_ADD_COMMENT,
		CMD_DEF_RAW_BYTES,
		CMD_IMM_IS_IOPORT,
		CMD_DEL_SYMBOL,

		CMD_TYPE_KEYBOARD,
		CMD_KEYBOARD,

		CMD_LET,
		CMD_EDIT_MEMORY_BYTE,
		CMD_EDIT_MEMORY_WORD,
		CMD_EDIT_MEMORY_DWORD,
		CMD_REPLACE,

		CMD_CRTC_PAGE,

		CMD_CALCULATE,

		CMD_CMOSSAVE,
		CMD_CMOSLOAD,
		CMD_CDLOAD,
		CMD_CDOPENCLOSE,
		CMD_FD0LOAD,
		CMD_FD0EJECT,
		CMD_FD0WRITEPROTECT,
		CMD_FD0WRITEUNPROTECT,
		CMD_FD1LOAD,
		CMD_FD1EJECT,
		CMD_FD1WRITEPROTECT,
		CMD_FD1WRITEUNPROTECT,

		CMD_SAVE_EVENTLOG,
		CMD_LOAD_EVENTLOG,

		CMD_HOST_TO_VM_FILE_TRANSFER,

		CMD_FREQUENCY,
	};

	enum
	{
		ENABLE_NONE,
		ENABLE_CMDLOG,
		ENABLE_DISASSEMBLE_EVERY_INST,
		ENABLE_IOMONITOR,
		ENABLE_EVENTLOG,
		ENABLE_DEBUGGER,
		ENABLE_MOUSEINTEGRATION,
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
		DUMP_CRTC,
		DUMP_PALETTE,
		DUMP_TIMER,
		DUMP_GDT,
		DUMP_LDT,
		DUMP_IDT,
		DUMP_SOUND,
		DUMP_REAL_MODE_INT_VECTOR,
		DUMP_CSEIP_LOG,
		DUMP_SYMBOL_TABLE,
		DUMP_MEMORY,
		DUMP_CMOS,
		DUMP_CDROM,
		DUMP_EVENTLOG,
		DUMP_SCSI,
		DUMP_SCHEDULE,
		DUMP_TIME_BALANCE,
		DUMP_SPRITE,
		DUMP_SPRITE_AT,
		DUMP_MOUSE,
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
		BREAK_ON_IOREAD,
		BREAK_ON_IOWRITE,
		BREAK_ON_VRAMREAD,
		BREAK_ON_VRAMWRITE,
		BREAK_ON_VRAMREADWRITE,
		BREAK_ON_CDC_COMMAND,
		BREAK_ON_LBUTTON_UP,
		BREAK_ON_LBUTTON_DOWN,
		BREAK_ON_RETURN_KEY,
		BREAK_ON_SCSI_COMMAND,
		BREAK_ON_SCSI_DMA_TRANSFER,
		BREAK_ON_MEM_READ,
		BREAK_ON_MEM_WRITE,
	};

	enum
	{
		ERROR_TOO_FEW_ARGS,
		ERROR_DUMP_TARGET_UNDEFINED,
		ERROR_CANNOT_OPEN_FILE,
		ERROR_CANNOT_SAVE_FILE,
		ERROR_INCORRECT_FILE_SIZE,
		ERROR_SYMBOL_NOT_FOUND,
		ERROR_COULD_NOT_DELETE_SYMBOL,
		ERROR_UNDEFINED_KEYBOARD_MODE,
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
	void Execute(TownsThread &thr,FMTowns &towns,class Outside_World *outside_world,Command &cmd);

	void Execute_Enable(FMTowns &towns,Command &cmd);
	void Execute_Disable(FMTowns &towns,Command &cmd);

	void Execute_AddBreakPoint(FMTowns &towns,Command &cmd);
	void Execute_DeleteBreakPoint(FMTowns &towns,Command &cmd);
	void Execute_ListBreakPoints(FMTowns &towns,Command &cmd);

	void Execute_Dump(FMTowns &towns,Command &cmd);

	void Execute_Calculate(FMTowns &towns,Command &cmd);

	void Execute_BreakOn(FMTowns &towns,Command &cmd);
	void Execute_ClearBreakOn(FMTowns &towns,Command &cmd);

	void Execute_AddressTranslation(FMTowns &towns,Command &cmd);
	void Execute_Disassemble(FMTowns &towns,Command &cmd);
	void Execute_Disassemble16(FMTowns &towns,Command &cmd);
	void Execute_Disassemble32(FMTowns &towns,Command &cmd);

	void Execute_PrintHistory(FMTowns &towns,unsigned int n);
	void Execute_SaveHistory(FMTowns &towns,const std::string &fName);

	void Execute_SaveEventLog(FMTowns &towns,const std::string &fName);

	void Execute_AddSymbol(FMTowns &towns,Command &cmd);
	void Execute_DelSymbol(FMTowns &towns,Command &cmd);

	void Execute_TypeKeyboard(FMTowns &towns,Command &cmd);

	void Execute_Let(FMTowns &towns,Command &cmd);
	void Execute_EditMemory(FMTowns &towns,Command &cmd,unsigned int numBytes);
	void Execute_Replace(FMTowns &towns,Command &cmd);

	void Execute_CRTCPage(FMTowns &towns,Command &cmd);

	void Execute_CMOSLoad(FMTowns &towns,Command &cmd);
	void Execute_CMOSSave(FMTowns &towns,Command &cmd);
	void Execute_CDLoad(FMTowns &towns,Command &cmd);
	void Execute_FDLoad(int drv,FMTowns &towns,Command &cmd);
	void Execute_FDEject(int drv,FMTowns &towns,Command &cmd);

	void Execute_MakeMemoryFilter(FMTowns &towns,Command &cmd);
	void Execute_UpdateMemoryFilter(FMTowns &towns,Command &cmd);
};


/* } */
#endif
