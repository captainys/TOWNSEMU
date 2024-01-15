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
#include <map>
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

	std::map <unsigned int,unsigned int> memFilter;

public:
	enum
	{
		CMD_NONE,

		CMD_QUIT,
		CMD_FORCE_QUIT,

		CMD_RESET,

		CMD_HELP,

		CMD_RUN,
		CMD_RUN_ONE_INSTRUCTION,
		CMD_PAUSE,
		CMD_WAIT,

		CMD_INTERRUPT,
		CMD_EXCEPTION,

		CMD_MAKE_MEMORY_FILTER,
		CMD_UPDATE_MEMORY_FILTER,

		CMD_FIND,
		CMD_FIND_STRING,
		CMD_FIND_CALLER,

		CMD_RETURN_FROM_PROCEDURE,

		CMD_ENABLE,
		CMD_DISABLE,
		// ENABLE CMDLOG
		// DISABLE CMDLOG

		CMD_DUMP,
		CMD_MEMDUMP,
		CMD_PRINT_STATUS,
		CMD_PRINT_HISTORY,
		CMD_PRINT_SYMBOL,
		CMD_PRINT_SYMBOL_LABEL_PROC,
		CMD_PRINT_SYMBOL_PROC,
		CMD_PRINT_SYMBOL_FIND,
		CMD_SAVE_HISTORY,

		CMD_ADD_BREAKPOINT,
		CMD_ADD_BREAKPOINT_WITH_PASSCOUNT,
		CMD_ADD_MONITORPOINT,
		CMD_ADD_MONITORPOINT_SHORT_FORMAT,
		CMD_DELETE_BREAKPOINT,
		CMD_DELETE_CS_BREAKPOINTS,
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
		CMD_IMM_IS_LABEL,
		CMD_IMM_IS_ASCII,
		CMD_OFFSET_IS_LABEL,
		CMD_DEL_SYMBOL,
		CMD_IMPORT_EXP_SYMTABLE,
		CMD_LOAD_SYMTABLE,

		CMD_TYPE_KEYBOARD,
		CMD_KEYBOARD,

		CMD_LET,
		CMD_EDIT_MEMORY_BYTE,
		CMD_EDIT_MEMORY_WORD,
		CMD_EDIT_MEMORY_DWORD,
		CMD_EDIT_MEMORY_STRING,
		CMD_EDIT_MEMORY,
		CMD_REPLACE,
		CMD_SAVE_MEMORY_DUMP,

		CMD_CRTC_PAGE,
		CMD_CRTC_SWAP_FMRGVRAMPAGE,

		CMD_CALCULATE,
		CMD_STRING_TO_ASCII,
		CMD_ASCII_TO_STRING,

		CMD_CMOSSAVE,
		CMD_CMOSLOAD,
		CMD_CDLOAD,
		CMD_CDCACHE,
		CMD_SCSICD0LOAD,
		CMD_SCSICD1LOAD,
		CMD_SCSICD2LOAD,
		CMD_SCSICD3LOAD,
		CMD_SCSICD4LOAD,
		CMD_SCSICD5LOAD,
		CMD_SCSICD6LOAD,
		CMD_CDOPENCLOSE,
		CMD_CDDASTOP,
		CMD_CDDAMUTE,
		CMD_CDDAUNMUTE,
		CMD_CDEJECT,
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
		CMD_PLAY_EVENTLOG,
		CMD_STOP_EVENTLOG,
		CMD_MAKE_REPEAT_EVENTLOG,

		CMD_SAVE_KEYMAP,
		CMD_LOAD_KEYMAP,

		CMD_SAVE_YM2612LOG,
		CMD_YM2612_CH_ON_OFF,

		CMD_FMVOL,
		CMD_PCMVOL,
		CMD_PCM_CH_ON_OFF,
		CMD_LOADWAV,

		CMD_HOST_TO_VM_FILE_TRANSFER,
		CMD_VM_TO_HOST_FILE_TRANSFER,
		CMD_SAVE_VM_TO_HOST_DUMP,

		CMD_FREQUENCY,

		CMD_XMODEM_CLEAR,
		CMD_XMODEM_TO_VM,
		CMD_XMODEM1K_TO_VM,
		CMD_XMODEM_FROM_VM,
		CMD_XMODEMCRC_FROM_VM,

		CMD_RS232C_TO_TCP,
		CMD_DISCONNECT_TCP,

		CMD_SAVE_SCREENSHOT,
		CMD_SAVE_VRAMLAYER,

		CMD_SPECIAL_DEBUG,
		CMD_DOSSEG,

		CMD_START_FMPCM_RECORDING,
		CMD_END_FMPCM_RECORDING,
		CMD_SAVE_FMPCM_RECORDING,

		CMD_START_VGM_RECORDING,
		CMD_END_VGM_RECORDING,
		CMD_SAVE_VGM_RECORDING,

		CMD_SAVE_WAVERAM,

		CMD_SAVE_STATE,
		CMD_LOAD_STATE,
		CMD_SAVE_STATE_AT,

		CMD_GAMEPORT,

		CMD_TOGGLE_HOST_MOUSE_CURSOR,

		CMD_QUICK_SCREENSHOT,
		CMD_QUICK_SCREENSHOT_DIR,

		CMD_QUICK_SAVESTATE,
		CMD_QUICK_LOADSTATE,

		CMD_AUTOSHOT,

		CMD_PWD,
		CMD_CHDIR,
		CMD_LS,

		CMD_OPEN_DEBUG_LOG,
		CMD_CLOSE_DEBUG_LOG,
	};

	enum
	{
		ENABLE_NONE,
		ENABLE_CMDLOG,
		ENABLE_DISASSEMBLE_EVERY_INST,
		ENABLE_DISASSEMBLE_EVERY_INST_WITH_REG,
		ENABLE_IOMONITOR,
		ENABLE_EVENTLOG,
		ENABLE_DEBUGGER,
		ENABLE_AUTO_ANNOTATE_VXDCALL,
		ENABLE_MOUSEINTEGRATION,
		ENABLE_YM2612_LOG,
		ENABLE_SCSICMDMONITOR,
		ENABLE_FPU,
		ENABLE_FDCMONITOR,
		ENABLE_CDCMONITOR,
		ENABLE_VXDMONITOR,
		ENABLE_CRTC2MONITOR,
		ENABLE_AUTOQSS,
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
		DUMP_CRTCDUMP,
		DUMP_HIRESCRTC,
		DUMP_PALETTE,
		DUMP_HIRESPALETTE,
		DUMP_TIMER,
		DUMP_GDT,
		DUMP_LDT,
		DUMP_IDT,
		DUMP_TSS,
		DUMP_TSS_IOMAP,
		DUMP_DR,
		DUMP_TEST,
		DUMP_SOUND,
		DUMP_SEGMENT_REGISTER_DETAILS,
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
		DUMP_SPRITE_PALETTE,
		DUMP_SPRITE_PATTERN_4BIT,
		DUMP_SPRITE_PATTERN_16BIT,
		DUMP_MOUSE,
		DUMP_YM2612_LOG,
		DUMP_DOS_INFO,
		DUMP_MEMORY_FILTER,
		DUMP_FPU,
		DUMP_WHERE_I_AM,
		DUMP_INSTRUCTION_HISTOGRAM,
		DUMP_HIGHRES_PCM,
	};

	enum
	{
		BREAK_ON_PIC_IWC1,
		BREAK_ON_PIC_IWC4,
		BREAK_ON_DMAC_REQUEST,
		BREAK_ON_FDC_COMMAND,
		BREAK_ON_INT,
		BREAK_ON_FOPEN,
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
		BREAK_ON_CDC_DEI,
		BREAK_ON_CDC_DATAREADY,
		BREAK_ON_LBUTTON_UP,
		BREAK_ON_LBUTTON_DOWN,
		BREAK_ON_RETURN_KEY,
		BREAK_ON_SCSI_COMMAND,
		BREAK_ON_SCSI_DMA_TRANSFER,
		BREAK_ON_MEM_READ,
		BREAK_ON_MEM_WRITE,
		BREAK_ON_BEEP,
		BREAK_ON_PROTECTED_MODE,
		BREAK_ON_REAL_MODE,
		BREAK_ON_VM86_MODE,
		BREAK_ON_VXD_CALL,
		BREAK_ON_CALLSTACK_DEPTH,
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
		ERROR_NO_DATA_GIVEN,
		ERROR_WRONG_PARAMETER,
		ERROR_VRAM_LAYER_UNAVAILABLE,
		ERROR_UNDEFINED_BREAK_POINT_OPTION,
		ERROR_CANNOT_CHDIR,
		ERROR_DEBUGGER_NOT_ENABLED,
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
	void Execute(TownsThread &thr,FMTownsCommon &towns,class Outside_World *outside_world,class Outside_World::Sound *sound,Command &cmd);

	void Execute_Enable(FMTownsCommon &towns,Command &cmd);
	void Execute_Disable(FMTownsCommon &towns,Command &cmd);

	void Execute_AddBreakPoint(FMTownsCommon &towns,Command &cmd);
	void Execute_AddBreakPointWithPassCount(FMTownsCommon &towns,Command &cmd);
	void Execute_AddSavePoint(FMTownsCommon &towns,Command &cmd);
	void Execute_AddMonitorPoint(FMTownsCommon &towns,Command &cmd);
	void Execute_DeleteBreakPoint(FMTownsCommon &towns,Command &cmd);
	void Execute_ListBreakPoints(FMTownsCommon &towns,Command &cmd);

	void Execute_Dump(FMTownsCommon &towns,Command &cmd);
	void Execute_Dump_DOSInfo(FMTownsCommon &towns,Command &cmd);

	void Execute_MemoryDump(FMTownsCommon &towns,Command &cmd);

	void Execute_Calculate(FMTownsCommon &towns,Command &cmd);

	void Execute_BreakOn(FMTownsCommon &towns,Command &cmd);
	void Execute_ClearBreakOn(FMTownsCommon &towns,Command &cmd);

	void Execute_AddressTranslation(FMTownsCommon &towns,Command &cmd);
	void Execute_Disassemble(FMTownsCommon &towns,Command &cmd);
	void Execute_Disassemble16(FMTownsCommon &towns,Command &cmd);
	void Execute_Disassemble32(FMTownsCommon &towns,Command &cmd);

	void Execute_PrintHistory(FMTownsCommon &towns,unsigned int n);
	void Execute_SaveHistory(FMTownsCommon &towns,const std::string &fName);

	void Execute_SaveEventLog(FMTownsCommon &towns,const std::string &fName);

	void Execute_AddSymbol(FMTownsCommon &towns,Command &cmd);
	void Execute_DelSymbol(FMTownsCommon &towns,Command &cmd);
	void Execute_ImportEXPSymbolTable(FMTownsCommon &towns,Command &cmd);
	void Execute_LoadSymbolTable(FMTownsCommon &towns,Command &cmd);

	void Execute_SymbolInquiry(FMTownsCommon &towns,Command &cmd);

	void Execute_TypeKeyboard(FMTownsCommon &towns,Command &cmd);

	void Execute_Let(FMTownsCommon &towns,Command &cmd);
	void Execute_EditMemory(FMTownsCommon &towns,Command &cmd,unsigned int numBytes);
	void Execute_Replace(FMTownsCommon &towns,Command &cmd);

	void Execute_CRTCPage(FMTownsCommon &towns,Command &cmd);
	void Execute_CRTC_SwapFMRGVRAMPage(FMTownsCommon &towns,Command &cmd);

	void Execute_CMOSLoad(FMTownsCommon &towns,Command &cmd);
	void Execute_CMOSSave(FMTownsCommon &towns,Command &cmd);
	void Execute_CDLoad(FMTownsCommon &towns,Command &cmd);
	void Execute_SCSICDLoad(unsigned int SCSIID,FMTownsCommon &towns,const Command &cmd);
	void Execute_FDLoad(int drv,FMTownsCommon &towns,Command &cmd);
	void Execute_FDEject(int drv,FMTownsCommon &towns,Command &cmd);

	void Execute_MakeMemoryFilter(FMTownsCommon &towns,Command &cmd);
	void Execute_UpdateMemoryFilter(FMTownsCommon &towns,Command &cmd);

	void Execute_Exception(FMTownsCommon &towns,Command &cmd);

	void Execute_SaveYM2612Log(FMTownsCommon &towns,std::string fName);

	void Execute_SaveKeyMap(const Outside_World &outside_world,const Command &cmd);
	void Execute_LoadKeyMap(Outside_World &outside_world,const Command &cmd);

	void Execute_Search_Bytes(FMTownsCommon &towns,Command &cmd);
	void Execute_Search_String(FMTownsCommon &towns,Command &cmd);
	void Execute_Search_ByteSequence(FMTownsCommon &towns,const std::vector <unsigned char> &bytes);
	void FoundAt(FMTownsCommon &towns,unsigned int physAddr);
	void FoundAt(std::string segLabel,unsigned int linearBase,unsigned int linearAddr);

	void Execute_Find_Caller(FMTownsCommon &towns,Command &cmd);

	void Execute_XMODEMtoVM(FMTownsCommon &towns,Command &cmd,uint32_t packetLength);
	void Execute_XMODEMfromVM(FMTownsCommon &towns,Command &cmd);
	void Execute_XMODEMCRCfromVM(FMTownsCommon &towns,Command &cmd);

	void Execute_SaveScreenShot(FMTownsCommon &towns,Command &cmd);
	void Execute_SaveVRAMLayer(FMTownsCommon &towns,Command &cmd);

	void SaveScreenShot(FMTownsCommon &towns,TownsRender &render,std::string &fName);

	void Execute_SpecialDebug(FMTownsCommon &towns,Command &cmd);

	void Execute_SaveMemDump(FMTownsCommon &towns,Command &cmd);

	void Execute_Gameport(FMTownsCommon &towns,Outside_World *outside_world,Command &cmd);

	void Execute_QuickScreenShot(FMTownsCommon &towns,Command &cmd);
	void Execute_QuickScreenShotPages(FMTownsCommon &towns,Command &cmd);
	void Execute_QuickScreenShotDirectory(FMTownsCommon &towns,Command &cmd);

	void Execute_AutoShot(FMTownsCommon &towns,Command &cmd);

	void Execute_BreakOnMemoryWrite(FMTownsCommon &towns,Command &cmd);

	void Execute_LS(FMTownsCommon &towns,Command &cmd);
};


/* } */
#endif
