/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>

#include "i486debugmemaccess.h"

#include "townscommand.h"
#include "townscommandutil.h"
#include "cpputil.h"
#include "miscutil.h"
#include "townslineparser.h"



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
	primaryCmdMap["P"]=CMD_DUMP;
	primaryCmdMap["DUMP"]=CMD_DUMP;
	primaryCmdMap["DM"]=CMD_DUMP;
	primaryCmdMap["STA"]=CMD_PRINT_STATUS;
	primaryCmdMap["HIST"]=CMD_PRINT_HISTORY;
	primaryCmdMap["SAVEHIST"]=CMD_SAVE_HISTORY;
	primaryCmdMap["BP"]=CMD_ADD_BREAKPOINT;
	primaryCmdMap["BC"]=CMD_DELETE_BREAKPOINT;
	primaryCmdMap["BL"]=CMD_LIST_BREAKPOINTS;
	primaryCmdMap["T"]=CMD_RUN_ONE_INSTRUCTION;
	primaryCmdMap["ADTR"]=CMD_TRANSLATE_ADDRESS;
	primaryCmdMap["U"]=CMD_DISASM;
	primaryCmdMap["U16"]=CMD_DISASM16;
	primaryCmdMap["U32"]=CMD_DISASM32;
	primaryCmdMap["BRKON"]=CMD_BREAK_ON;
	primaryCmdMap["CBRKON"]=CMD_DONT_BREAK_ON;
	primaryCmdMap["INTERRUPT"]=CMD_INTERRUPT;
	primaryCmdMap["EXCEPTION"]=CMD_EXCEPTION;
	primaryCmdMap["MKMEMFILTER"]=CMD_MAKE_MEMORY_FILTER;
	primaryCmdMap["UPDMEMFILTER"]=CMD_UPDATE_MEMORY_FILTER;
	primaryCmdMap["FIND"]=CMD_FIND;
	primaryCmdMap["FINDS"]=CMD_FIND_STRING;
	primaryCmdMap["FINDCALLER"]=CMD_FIND_CALLER;

	primaryCmdMap["ADDSYM"]=CMD_ADD_SYMBOL;
	primaryCmdMap["KEYBOARD"]=CMD_KEYBOARD;

	primaryCmdMap["ADDLAB"]=CMD_ADD_LABEL;
	primaryCmdMap["ADDLABEL"]=CMD_ADD_LABEL;
	primaryCmdMap["ADDDLB"]=CMD_ADD_DATALABEL;
	primaryCmdMap["ADDDATALABEL"]=CMD_ADD_DATALABEL;
	primaryCmdMap["ADDREM"]=CMD_ADD_COMMENT;
	primaryCmdMap["ADDCMT"]=CMD_ADD_COMMENT;
	primaryCmdMap["DEFRAW"]=CMD_DEF_RAW_BYTES;
	primaryCmdMap["DELSYM"]=CMD_DEL_SYMBOL;
	primaryCmdMap["IMMISIO"]=CMD_IMM_IS_IOPORT;
	primaryCmdMap["SAVEEVT"]=CMD_SAVE_EVENTLOG;
	primaryCmdMap["LOADEVT"]=CMD_LOAD_EVENTLOG;
	primaryCmdMap["SAVEYM2612LOG"]=CMD_SAVE_YM2612LOG;
	primaryCmdMap["FMCH"]=CMD_YM2612_CH_ON_OFF;

	primaryCmdMap["CALC"]=CMD_CALCULATE;

	primaryCmdMap["TYPE"]=CMD_TYPE_KEYBOARD;
	primaryCmdMap["LET"]=CMD_LET;
	primaryCmdMap["EMB"]=CMD_EDIT_MEMORY_BYTE;
	primaryCmdMap["EMW"]=CMD_EDIT_MEMORY_WORD;
	primaryCmdMap["EMD"]=CMD_EDIT_MEMORY_DWORD;
	primaryCmdMap["REPLACE"]=CMD_REPLACE;


	primaryCmdMap["CRTCPAGE"]=CMD_CRTC_PAGE;
	primaryCmdMap["CMOSLOAD"]=CMD_CMOSLOAD;
	primaryCmdMap["CMOSSAVE"]=CMD_CMOSSAVE;
	primaryCmdMap["CDLOAD"]=CMD_CDLOAD;
	primaryCmdMap["CDOPENCLOSE"]=CMD_CDOPENCLOSE;
	primaryCmdMap["FD0LOAD"]=CMD_FD0LOAD;
	primaryCmdMap["FD0EJECT"]=CMD_FD0EJECT;
	primaryCmdMap["FD0WP"]=CMD_FD0WRITEPROTECT;
	primaryCmdMap["FD0UP"]=CMD_FD0WRITEUNPROTECT;
	primaryCmdMap["FD1LOAD"]=CMD_FD1LOAD;
	primaryCmdMap["FD1EJECT"]=CMD_FD1EJECT;
	primaryCmdMap["FD1WP"]=CMD_FD1WRITEPROTECT;
	primaryCmdMap["FD1UP"]=CMD_FD1WRITEUNPROTECT;

	primaryCmdMap["HOST2VM"]=CMD_HOST_TO_VM_FILE_TRANSFER;

	primaryCmdMap["FREQ"]=CMD_FREQUENCY;

	primaryCmdMap["XMODEMCLR"]=CMD_XMODEM_CLEAR;
	primaryCmdMap["XMODEMTOVM"]=CMD_XMODEM_TO_VM;
	primaryCmdMap["XMODEMFROMVM"]=CMD_XMODEM_FROM_VM;

	featureMap["CMDLOG"]=ENABLE_CMDLOG;
	featureMap["AUTODISASM"]=ENABLE_DISASSEMBLE_EVERY_INST;
	featureMap["IOMON"]=ENABLE_IOMONITOR;
	featureMap["EVENTLOG"]=ENABLE_EVENTLOG;
	featureMap["DEBUGGER"]=ENABLE_DEBUGGER;
	featureMap["DEBUG"]=ENABLE_DEBUGGER;
	featureMap["MOUSEINTEG"]=ENABLE_MOUSEINTEGRATION;
	featureMap["YM2612LOG"]=ENABLE_YM2612_LOG;

	dumpableMap["CALLSTACK"]=DUMP_CALLSTACK;
	dumpableMap["CST"]=DUMP_CALLSTACK;
	dumpableMap["BREAKPOINT"]=DUMP_BREAKPOINT;
	dumpableMap["BRK"]=DUMP_BREAKPOINT;
	dumpableMap["STATUS"]=DUMP_CURRENT_STATUS;
	dumpableMap["STATE"]=DUMP_CURRENT_STATUS;
	dumpableMap["STA"]=DUMP_CURRENT_STATUS;
	dumpableMap["S"]=DUMP_CURRENT_STATUS;
	dumpableMap["HIST"]=DUMP_CSEIP_LOG;
	dumpableMap["PIC"]=DUMP_PIC;
	dumpableMap["DMA"]=DUMP_DMAC;
	dumpableMap["DMAC"]=DUMP_DMAC;
	dumpableMap["FDC"]=DUMP_FDC;
	dumpableMap["CRTC"]=DUMP_CRTC;
	dumpableMap["PALETTE"]=DUMP_PALETTE;
	dumpableMap["TIMER"]=DUMP_TIMER;
	dumpableMap["GDT"]=DUMP_GDT;
	dumpableMap["LDT"]=DUMP_LDT;
	dumpableMap["IDT"]=DUMP_IDT;
	dumpableMap["SOUND"]=DUMP_SOUND;
	dumpableMap["RIDT"]=DUMP_REAL_MODE_INT_VECTOR;
	dumpableMap["SYM"]=DUMP_SYMBOL_TABLE;
	dumpableMap["MEM"]=DUMP_MEMORY;
	dumpableMap["CMOS"]=DUMP_CMOS;
	dumpableMap["CDROM"]=DUMP_CDROM;
	dumpableMap["EVENTLOG"]=DUMP_EVENTLOG;
	dumpableMap["SCSI"]=DUMP_SCSI;
	dumpableMap["SCHED"]=DUMP_SCHEDULE;
	dumpableMap["SCHEDULE"]=DUMP_SCHEDULE;
	dumpableMap["TIMEBALANCE"]=DUMP_TIME_BALANCE;
	dumpableMap["SPRITE"]=DUMP_SPRITE;
	dumpableMap["SPRITEAT"]=DUMP_SPRITE_AT;
	dumpableMap["MOUSE"]=DUMP_MOUSE;
	dumpableMap["YM2612LOG"]=DUMP_YM2612_LOG;



	breakEventMap["ICW1"]=   BREAK_ON_PIC_IWC1;
	breakEventMap["ICW4"]=   BREAK_ON_PIC_IWC4;
	breakEventMap["DMACREQ"]=BREAK_ON_DMAC_REQUEST;
	breakEventMap["FDCCMD"]= BREAK_ON_FDC_COMMAND;
	breakEventMap["INT"]=    BREAK_ON_INT;
	breakEventMap["RDCVRAM"]=BREAK_ON_CVRAM_READ;
	breakEventMap["WRCVRAM"]=BREAK_ON_CVRAM_WRITE;
	breakEventMap["RDFMRVRAM"]=BREAK_ON_FMRVRAM_READ;
	breakEventMap["WRFMRVRAM"]=BREAK_ON_FMRVRAM_WRITE;
	breakEventMap["IOR"]=BREAK_ON_IOREAD;
	breakEventMap["IOW"]=BREAK_ON_IOWRITE;
	breakEventMap["VRAMR"]=BREAK_ON_VRAMREAD;
	breakEventMap["VRAMW"]=BREAK_ON_VRAMWRITE;
	breakEventMap["VRAMRW"]=BREAK_ON_VRAMREADWRITE;
	breakEventMap["CDCCMD"]=BREAK_ON_CDC_COMMAND;
	breakEventMap["LBUTTONUP"]=BREAK_ON_LBUTTON_UP;
	breakEventMap["LBUTTONDOWN"]=BREAK_ON_LBUTTON_DOWN;
	breakEventMap["RETKEY"]=BREAK_ON_RETURN_KEY;
	breakEventMap["RETURNKEY"]=BREAK_ON_RETURN_KEY;
	breakEventMap["SCSICMD"]=BREAK_ON_SCSI_COMMAND;
	breakEventMap["SCSIDMA"]=BREAK_ON_SCSI_DMA_TRANSFER;
	breakEventMap["MEMREAD"]=BREAK_ON_MEM_READ;
	breakEventMap["MEMR"]=BREAK_ON_MEM_READ;
	breakEventMap["MEMWRITE"]=BREAK_ON_MEM_WRITE;
	breakEventMap["MEMW"]=BREAK_ON_MEM_WRITE;
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
	std::cout << "FREQ frequency_in_MHz" << std::endl;
	std::cout << "  Change frequency.  Minimum 1MHz." << std::endl;
	std::cout << "CDLOAD filename" << std::endl;
	std::cout << "  Load CD-ROM image." << std::endl;
	std::cout << "CDOPENCLOSE" << std::endl;
	std::cout << "  Virtually open and close the internal CD-ROM drive." << std::endl;
	std::cout << "FD0LOAD filename" << std::endl;
	std::cout << "FD1LOAD filename" << std::endl;
	std::cout << "  Load FD image.  The number 0 or 1 is the drive number." << std::endl;
	std::cout << "FD0EJECT" << std::endl;
	std::cout << "FD1EJECT" << std::endl;
	std::cout << "  Eject FD." << std::endl;
	std::cout << "FD0WP" << std::endl;
	std::cout << "FD1WP" << std::endl;
	std::cout << "  Write protect floppy disk." << std::endl;
	std::cout << "FD0UP" << std::endl;
	std::cout << "FD1UP" << std::endl;
	std::cout << "  Write un-protect floppy disk." << std::endl;
	std::cout << "PAUSE|PAU" << std::endl;
	std::cout << "  Pause VM." << std::endl;

	std::cout << "XMODEMTOVM filename" << std::endl;
	std::cout << "  File transfer to the VM with XMODEM.  Type this command first," << std::endl;
	std::cout << "  then start XMODEM in FM TOWNS application." << std::endl;
	std::cout << "XMODEMFROMVM filename" << std::endl;
	std::cout << "  File transfer from the VM with XMODEM.  Start XMODEM in FM TOWNS application," << std::endl;
	std::cout << "  and then type this command." << std::endl;
	std::cout << "XMODEMCLR" << std::endl;
	std::cout << "  Cancel XMODEM file transfer." << std::endl;

	std::cout << "T" << std::endl;
	std::cout << "  Trace.  Run one instruction." << std::endl;
	std::cout << "INTERRUPT INTNum" << std::endl;
	std::cout << "  Inject interrupt.  Same as CPU Instruction INT INTNum.  INTNum is hexadecimal." << std::endl;
	std::cout << "  For example, INTERRUPT 4B will work same as INT 4BH." << std::endl;

	std::cout << "EXCEPTION Type" << std::endl;
	std::cout << "  Intentionally cause an exception." << std::endl;
	std::cout << "  Type can be GENERAL, PAGEFAULT, or DIVISION." << std::endl;

	std::cout << "MKMEMFILTER byteData" << std::endl;
	std::cout << "  Make memory filter.  Memory filter caches physical addresses that has the given value." << std::endl;
	std::cout << "UPDMEMFILTER byteData" << std::endl;
	std::cout << "  Update memory filter.  Physical addresses that does not have the given value" << std::endl;
	std::cout << "  are deleted from the memory filter." << std::endl;

	std::cout << "FIND bytedata" << std::endl;
	std::cout << "  Find byte sequence in main RAM and VRAM." << std::endl;
	std::cout << "  bytedata can be written like 01020304 for 32-bit value." << std::endl;
	std::cout << "  Also can be mixed like 88 6809 0a0B0C." << std::endl;
	std::cout << "  If you type only six digits, it is taken as 24-bit value." << std::endl;

	std::cout << "FINDS string" << std::endl;
	std::cout << "  Find a string in main RAM and VRAM." << std::endl;

	std::cout << "FINDCALLER offset" << std::endl;
	std::cout << "  Find JMP rel8, JMP rel, CALL rel, CALLF, Jcc instructions within the current CS" << std::endl;
	std::cout << "  that jump to the given offset." << std::endl;
	std::cout << "  Currently it will miss indirect jump/call." << std::endl;

	std::cout << "ADTR SEG:OFFSET" << std::endl;
	std::cout << "  Translate address to linear address and physical address." << std::endl;
	std::cout << "U addr" << std::endl;
	std::cout << "  Unassemble (disassemble)" << std::endl;
	std::cout << "U16 addr" << std::endl;
	std::cout << "  Unassemble (disassemble) as 16-bit operand size" << std::endl;
	std::cout << "U32 addr" << std::endl;
	std::cout << "  Unassemble (disassemble) as 32-bit operand size" << std::endl;
	std::cout << "ADDSYM SEG:OFFSET label" << std::endl;
	std::cout << "  Add a symbol.  An address can have one symbol,label,data label, or data, and one comment." << std::endl;
	std::cout << "  If a symbol is added to an address that already has a symbol, label, or data label," << std::endl;
	std::cout << "  the address's label, or data label will be overwritten as a symbol." << std::endl;
	std::cout << "ADDLAB|ADDLABEL SEG:OFFSET label" << std::endl;
	std::cout << "  Add a label.  An address can have one symbol,label, or data label, or data and one comment." << std::endl;
	std::cout << "  If a symbol is added to an address that already has a symbol, label, or data label," << std::endl;
	std::cout << "  the address's label, or data label will be overwritten as a label." << std::endl;
	std::cout << "ADDDLB|ADDDATALABEL SEG:OFFSET label" << std::endl;
	std::cout << "  Add a data label.  An address can have one symbol,label, or data label, or data and one comment." << std::endl;
	std::cout << "  If a symbol is added to an address that already has a symbol, label, or data label," << std::endl;
	std::cout << "  the address's label, or data label will be overwritten as a data label." << std::endl;
	std::cout << "ADDREM|ADDCMT SEG:OFFSET label" << std::endl;
	std::cout << "  Add a comment.  An address can have one symbol,label, or data label, and one comment." << std::endl;
	std::cout << "DEFRAW SEG:OFFSET label numBytes" << std::endl;
	std::cout << "  Define raw data bytes.  Disassembler will take this address as raw data." << std::endl;
	std::cout << "IMMISIO SEG:OFFSET" << std::endl;
	std::cout << "  Take Imm operand of the address as IO-port address." << std::endl;
	std::cout << "DELSYM SEG:OFFSET label" << std::endl;
	std::cout << "  Delete a symbol.  A symbol and comment associated with the address will be deleted." << std::endl;
	std::cout << "WAIT" << std::endl;
	std::cout << "  Wait until VM becomes PAUSE state." << std::endl;
	std::cout << "RET|RTS" << std::endl;
	std::cout << "  Run until return from the current procedure." << std::endl;
	std::cout << "  Available only when call-stack is enabled." << std::endl;
	std::cout << "ENA feature|ENABLE feature" << std::endl;
	std::cout << "  Enable a feature." << std::endl;
	std::cout << "DIS feature|DISABLE feature" << std::endl;
	std::cout << "  Disable a feature." << std::endl;
	std::cout << "PRINT info|PRI info|P info" << std::endl;
	std::cout << "DUMP info|DM info" << std::endl;
	std::cout << "  Print/Dump information." << std::endl;
	std::cout << "CALC formula" << std::endl;
	std::cout << "  Caluclate a value." << std::endl;
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
	std::cout << "TYPE characters" << std::endl;
	std::cout << "  Send keyboard codes." << std::endl;
	std::cout << "KEYBOARD keyboardMode" <<std::endl;
	std::cout << "  Select TRANSLATE or DIRECT mode." << std::endl;
	std::cout << "  TRANSLATE or TRANS mode will be good for typing commands, but" << std::endl;
	std::cout << "  cannot sense key release correctly." << std::endl;
	std::cout << "  DIRECT mode is good for games, but affected by the keyboard layout." << std::endl;
	std::cout << "  US keyboard cannot type some of the characters." << std::endl;
	std::cout << "LET register value" << std::endl;
	std::cout << "  Load a register value." << std::endl;
	std::cout << "EMB seg:offset data" <<std::endl;
	std::cout << "EMW seg:offset data" <<std::endl;
	std::cout << "EMD seg:offset data" <<std::endl;
	std::cout << "  Edit memory byte/word/dword respectively." << std::endl;
	std::cout << "REPLACE hexadecimal-data hexadecimal-data" << std::endl;
	std::cout << "  Replace in main memory." << std::endl;
	std::cout << "CRTCPAGE 1|0 1|0" << std::endl;
	std::cout << "  Turn on/off display page." << std::endl;
	std::cout << "CMOSLOAD filename" << std::endl;
	std::cout << "  Load CMOS." << std::endl;
	std::cout << "CMOSSAVE filename" << std::endl;
	std::cout << "  Save CMOS." << std::endl;
	std::cout << "SAVEHIST filename.txt" << std::endl;
	std::cout << "  Save CS:EIP Log to file." << std::endl;
	std::cout << "SAVEEVT filename.txt" << std::endl;
	std::cout << "  Save Event Log to file." << std::endl;
	std::cout << "HOST2VM hostFileName vmFileName" << std::endl;
	std::cout << "  Schedule Host to VM file transfer." << std::endl;
	std::cout << "  File will be transferred when FTCLIENT.EXP is running." << std::endl;
	std::cout << "" << std::endl;

	std::cout << "<< Features that can be enabled|disabled >>" << std::endl;
	std::cout << "CMDLOG" << std::endl;
	std::cout << "  Command log.  Saved to CMD.LOG." << std::endl;
	std::cout << "AUTODISASM" << std::endl;
	std::cout << "  Disassemble while running." << std::endl;
	std::cout << "IOMON iopotMin ioportMax" << std::endl;
	std::cout << "  IO Monitor." << std::endl;
	std::cout << "  ioportMin and ioportMax are optional." << std::endl;
	std::cout << "  Can specify multiple range by enabling IOMON multiple times." << std::endl;
	std::cout << "EVENTLOG" << std::endl;
	std::cout << "  Event Log." << std::endl;
	std::cout << "DEBUGGER" << std::endl;
	std::cout << "DEBUG" << std::endl;
	std::cout << "  Debugger." << std::endl;
	std::cout << "MOUSEINTEG" << std::endl;
	std::cout << "  Mouse Integration." << std::endl;
	std::cout << "YM2612LOG" << std::endl;
	std::cout << "  YM2612 register-write log. (Previous log is cleared)" << std::endl;

	std::cout << "" << std::endl;

	std::cout << "<< Information that can be printed >>" << std::endl;
	std::cout << "CALLSTACK|CST" << std::endl;
	std::cout << "  Call Stack"<<std::endl;
	std::cout << "SYM" << std::endl;
	std::cout << "  Symbol table" << std::endl;
	std::cout << "HIST" << std::endl;
	std::cout << "  Log of CS:EIP.  Can specify number of steps.  Same as HIST command." << std::endl;
	std::cout << "GDT" << std::endl;
	std::cout << "  Protected-Mode Global Descriptor Table" << std::endl;
	std::cout << "LDT" << std::endl;
	std::cout << "  Protected-Mode Local Descriptor Table" << std::endl;
	std::cout << "IDT" << std::endl;
	std::cout << "  Protected-Mode Interrupt Descriptor Table" << std::endl;
	std::cout << "RIDT" << std::endl;
	std::cout << "  Real-mode Interrupt Descriptor Tables" << std::endl;
	std::cout << "BREAKPOINT|BRK" << std::endl;
	std::cout << "  Break Points"<<std::endl;
	std::cout << "STATUS|STATE|STA|S"<<std::endl;
	std::cout << "  Current status.  Same as STA command."<<std::endl;
	std::cout << "PIC" << std::endl;
	std::cout << "  Pilot-In-Command. No!  Programmable Interrupt Controller." << std::endl;
	std::cout << "DMA|DMAC" << std::endl;
	std::cout << "  DMA Controller." << std::endl;
	std::cout << "FDC" << std::endl;
	std::cout << "  Floppy Disk Controller." << std::endl;
	std::cout << "SCSI" << std::endl;
	std::cout << "  SCSI Controller." << std::endl;
	std::cout << "CRTC" << std::endl;
	std::cout << "  CRTC." << std::endl;
	std::cout << "PALETTE" << std::endl;
	std::cout << "  PALETTE." << std::endl;
	std::cout << "TIMER" << std::endl;
	std::cout << "  Interval Timer (i8253)" << std::endl;
	std::cout << "MEM" << std::endl;
	std::cout << "  Memory Settings" << std::endl;
	std::cout << "CMOS addr" << std::endl;
	std::cout << "  CMOS RAM" << std::endl;
	std::cout << "CDROM" << std::endl;
	std::cout << "  CD-ROM Status." << std::endl;
	std::cout << "SCHED" << std::endl;
	std::cout << "  Device call-back schedule." << std::endl;
	std::cout << "SPRITE" << std::endl;
	std::cout << "  Sprite status." << std::endl;
	std::cout << "SPRITEAT x y" << std::endl;
	std::cout << "  Status of the sprite at coordinate x y." << std::endl;
	std::cout << "  x and y are in decimal, not in hexadecimal." << std::endl;
	std::cout << "MOUSE" << std::endl;
	std::cout << "  Mouse status." << std::endl;
	std::cout << "YM2612LOG" << std::endl;
	std::cout << "  YM2612 register-write log." << std::endl;
	std::cout << "FMCH 0/1 0/1 0/1 0/1 0/1 0/1" << std::endl;
	std::cout << "  Mute/Unmute YM2612 channels." << std::endl;

	std::cout << "" << std::endl;

	std::cout << "<< Event that can break >>" << std::endl;
	std::cout << "ICW1" << std::endl;
	std::cout << "ICW4" << std::endl;
	std::cout << "DMACREQ" << std::endl;
	std::cout << "FDCCMD" << std::endl;
	std::cout << "CDCCMD" << std::endl;
	std::cout << "CDCCMD cmdValueInHex" << std::endl;
	std::cout << "INT n" << std::endl;
	std::cout << "INT n AH=hh" << std::endl;
	std::cout << "INT n AX=hhhh" << std::endl;
	std::cout << "RDCVRAM" << std::endl;
	std::cout << "WRCVRAM" << std::endl;
	std::cout << "RDFMRVRAM" << std::endl;
	std::cout << "WRFMRVRAM" << std::endl;
	std::cout << "IOR ioport" << std::endl;
	std::cout << "IOW ioport" << std::endl;
	std::cout << "VRAMR" << std::endl;
	std::cout << "VRAMW" << std::endl;
	std::cout << "VRAMRW" << std::endl;
	std::cout << "LBUTTONUP" << std::endl;
	std::cout << "LBUTTONDOWN" << std::endl;
	std::cout << "RETKEY" << std::endl;
	std::cout << "RETURNKEY" << std::endl;
	std::cout << "SCSICMD" << std::endl;
	std::cout << "SCSIDMA" << std::endl;
	std::cout << "MEMREAD physAddr" << std::endl;
	std::cout << "MEMWRITE physAddr" << std::endl;
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
	case ERROR_CANNOT_OPEN_FILE:
		std::cout << "Error: Cannot open file." << std::endl;
		break;
	case ERROR_CANNOT_SAVE_FILE:
		std::cout << "Error: Cannot save file." << std::endl;
		break;
	case ERROR_INCORRECT_FILE_SIZE:
		std::cout << "Error: Incorrect File Size." << std::endl;
		break;
	case ERROR_SYMBOL_NOT_FOUND:
		std::cout << "Error: Symbol/Comment not set for the address." << std::endl;
		break;
	case ERROR_COULD_NOT_DELETE_SYMBOL:
		std::cout << "Error: Could not delete a symbol." << std::endl;
		break;
	case ERROR_UNDEFINED_KEYBOARD_MODE:
		std::cout << "Error: Undefined Keyboard Mode." << std::endl;
		break;
	case ERROR_NO_DATA_GIVEN:
		std::cout << "Error: No data given." << std::endl;
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

void TownsCommandInterpreter::Execute(TownsThread &thr,FMTowns &towns,class Outside_World *outside_world,Command &cmd)
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
		thr.SetRunMode(TownsThread::RUNMODE_RUN);
		if(1<cmd.argv.size())
		{
			auto farPtr=cmdutil::MakeFarPointer(cmd.argv[1],towns.cpu);
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
	case CMD_EXCEPTION:
		Execute_Exception(towns,cmd);
		break;

	case CMD_MAKE_MEMORY_FILTER:
		Execute_MakeMemoryFilter(towns,cmd);
		break;
	case CMD_UPDATE_MEMORY_FILTER:
		Execute_UpdateMemoryFilter(towns,cmd);
		break;

	case CMD_FIND:
		Execute_Search_Bytes(towns,cmd);
		break;
	case CMD_FIND_STRING:
		Execute_Search_String(towns,cmd);
		break;
	case CMD_FIND_CALLER:
		Execute_Find_Caller(towns,cmd);
		break;

	case CMD_TRANSLATE_ADDRESS:
		Execute_AddressTranslation(towns,cmd);
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
			thr.SetRunMode(TownsThread::RUNMODE_RUN);
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

	case CMD_CALCULATE:
		Execute_Calculate(towns,cmd);
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

	case CMD_PRINT_HISTORY:
		if(2<=cmd.argv.size())
		{
			Execute_PrintHistory(towns,cpputil::Atoi(cmd.argv[1].c_str()));
		}
		else
		{
			Execute_PrintHistory(towns,20);
		}
		break;
	case CMD_SAVE_HISTORY:
		if(2<=cmd.argv.size())
		{
			Execute_SaveHistory(towns,cmd.argv[1]);
		}
		else
		{
			PrintError(ERROR_TOO_FEW_ARGS);
		}
		break;
	case CMD_SAVE_EVENTLOG:
		if(2<=cmd.argv.size())
		{
			Execute_SaveEventLog(towns,cmd.argv[1]);
		}
		else
		{
			PrintError(ERROR_TOO_FEW_ARGS);
		}
		break;
	case CMD_LOAD_EVENTLOG:
		if(true==towns.eventLog.LoadEventLog(cmd.argv[1]))
		{
			towns.eventLog.BeginPlayback();
		}
		else
		{
			PrintError(ERROR_CANNOT_OPEN_FILE);
		}
		break;
	case CMD_SAVE_YM2612LOG:
		if(2<=cmd.argv.size())
		{
			Execute_SaveYM2612Log(towns,cmd.argv[1]);
		}
		else
		{
			PrintError(ERROR_TOO_FEW_ARGS);
		}
		break;
	case CMD_YM2612_CH_ON_OFF:
		if(7<=cmd.argv.size())
		{
			towns.sound.state.ym2612.channelMute[0]=(0==cpputil::Atoi(cmd.argv[1].c_str()));
			towns.sound.state.ym2612.channelMute[1]=(0==cpputil::Atoi(cmd.argv[2].c_str()));
			towns.sound.state.ym2612.channelMute[2]=(0==cpputil::Atoi(cmd.argv[3].c_str()));
			towns.sound.state.ym2612.channelMute[3]=(0==cpputil::Atoi(cmd.argv[4].c_str()));
			towns.sound.state.ym2612.channelMute[4]=(0==cpputil::Atoi(cmd.argv[5].c_str()));
			towns.sound.state.ym2612.channelMute[5]=(0==cpputil::Atoi(cmd.argv[6].c_str()));
			std::cout << "Set YM2612 chnanle mute/unmute" << std::endl;
		}
		else
		{
			PrintError(ERROR_TOO_FEW_ARGS);
		}
		break;
	case CMD_PRINT_STATUS:
		towns.PrintStatus();
		break;

	case CMD_ADD_SYMBOL:
	case CMD_ADD_LABEL:
	case CMD_ADD_DATALABEL:
	case CMD_ADD_COMMENT:
	case CMD_DEF_RAW_BYTES:
	case CMD_IMM_IS_IOPORT:
		Execute_AddSymbol(towns,cmd);
		break;
	case CMD_DEL_SYMBOL:
		Execute_DelSymbol(towns,cmd);
		break;

	case CMD_TYPE_KEYBOARD:
		Execute_TypeKeyboard(towns,cmd);
		break;
	case CMD_KEYBOARD:
		if(nullptr!=outside_world)
		{
			std::string MODE=cmd.argv[1];
			cpputil::Capitalize(MODE);
			if("TRANS"==MODE || "TRANSLATE"==MODE)
			{
				outside_world->keyboardMode=Outside_World::KEYBOARD_MODE_TRANSLATION;
				std::cout << "Keyboard TRANSLATION Mode" << std::endl;
			}
			else if("DIRECT"==MODE)
			{
				outside_world->keyboardMode=Outside_World::KEYBOARD_MODE_DIRECT;
				std::cout << "Keyboard DIRECT Mode" << std::endl;
			}
			else
			{
				PrintError(ERROR_UNDEFINED_KEYBOARD_MODE);
			}
		}
		break;

	case CMD_LET:
		Execute_Let(towns,cmd);
		break;
	case CMD_EDIT_MEMORY_BYTE:
		Execute_EditMemory(towns,cmd,1);
		break;
	case CMD_EDIT_MEMORY_WORD:
		Execute_EditMemory(towns,cmd,2);
		break;
	case CMD_EDIT_MEMORY_DWORD:
		Execute_EditMemory(towns,cmd,4);
		break;
	case CMD_REPLACE:
		Execute_Replace(towns,cmd);
		break;

	case CMD_CRTC_PAGE:
		Execute_CRTCPage(towns,cmd);
		break;
	case CMD_CMOSLOAD:
		Execute_CMOSLoad(towns,cmd);
		break;
	case CMD_CMOSSAVE:
		Execute_CMOSSave(towns,cmd);
		break;

	case CMD_CDLOAD:
		Execute_CDLoad(towns,cmd);
		break;
	case CMD_CDOPENCLOSE:
		towns.cdrom.state.discChanged=true;
		break;
	case CMD_FD0LOAD:
		Execute_FDLoad(0,towns,cmd);
		break;
	case CMD_FD1LOAD:
		Execute_FDLoad(1,towns,cmd);
		break;
	case CMD_FD0EJECT:
		Execute_FDEject(0,towns,cmd);
		break;
	case CMD_FD1EJECT:
		Execute_FDEject(1,towns,cmd);
		break;
	case CMD_FD0WRITEPROTECT:
		towns.fdc.SetWriteProtect(0,true);
		break;
	case CMD_FD0WRITEUNPROTECT:
		towns.fdc.SetWriteProtect(0,false);
		break;
	case CMD_FD1WRITEPROTECT:
		towns.fdc.SetWriteProtect(1,true);
		break;
	case CMD_FD1WRITEUNPROTECT:
		towns.fdc.SetWriteProtect(1,false);
		break;
	case CMD_FREQUENCY:
		if(2<=cmd.argv.size())
		{
			int MHz=cpputil::Atoi(cmd.argv[1].c_str());
			if(MHz<1)
			{
				MHz=1;
			}
			towns.state.freq=MHz;
			std::cout << "Set CPU frequency to " << MHz << "MHz." << std::endl;
		}
		else
		{
			PrintError(ERROR_TOO_FEW_ARGS);
		}
		break;

	case CMD_HOST_TO_VM_FILE_TRANSFER:
		if(3<=cmd.argv.size())
		{
			if(true==cpputil::FileExists(cmd.argv[1]))
			{
				towns.var.ftfr.AddHostToVM(cmd.argv[1],cmd.argv[2]);
			}
			else
			{
				std::cout << "File not found." << std::endl;
			}
		}
		break;

	case CMD_XMODEM_TO_VM:
		Execute_XMODEMtoVM(towns,cmd);
		break;
	case CMD_XMODEM_FROM_VM:
		Execute_XMODEMfromVM(towns,cmd);
		break;
	case CMD_XMODEM_CLEAR:
		towns.serialport.defaultClient.ClearXMODEM();
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
			std::cout << "IO_Monitor is ON." << std::endl;
			if(4<=cmd.argv.size())
			{
				auto portMin=cpputil::Xtoi(cmd.argv[2].c_str());
				auto portMax=cpputil::Xtoi(cmd.argv[3].c_str());
				towns.debugger.MonitorIO(portMin,portMax);
				std::cout << "Range:";
				std::cout << cpputil::Ustox(portMin);
				std::cout << " to ";
				std::cout << cpputil::Ustox(portMax);
				std::cout << std::endl;
			}
			else if(3<=cmd.argv.size())
			{
				auto port=cpputil::Xtoi(cmd.argv[2].c_str());
				towns.debugger.MonitorIO(port,port);
				std::cout << "Port:";
				std::cout << cpputil::Ustox(port);
				std::cout << std::endl;
			}
			else
			{
				towns.debugger.MonitorIO(0,0xFFFF);
			}
			break;
		case ENABLE_EVENTLOG:
			towns.eventLog.BeginRecording(towns.state.townsTime);
			towns.var.pauseOnPowerOff=true;
			std::cout << "Event Logging is ON." << std::endl;
			std::cout << "Pause on Power OFF is ON." << std::endl;
			break;
		case ENABLE_DEBUGGER:
			towns.EnableDebugger();
			std::cout << "Debugger is ON." << std::endl;
			break;
		case ENABLE_MOUSEINTEGRATION:
			towns.var.mouseIntegration=true;
			std::cout << "Mouse Integration is Enabled." << std::endl;
			break;
		case ENABLE_YM2612_LOG:
			towns.sound.state.ym2612.takeRegLog=true;
			towns.sound.state.ym2612.regWriteLog.clear();
			std::cout << "YM2612 register write log is Enabled." << std::endl;
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
			if(4<=cmd.argv.size())
			{
				std::cout << "IO_Monitor is OFF for:" << std::endl;
				auto portMin=cpputil::Xtoi(cmd.argv[2].c_str());
				auto portMax=cpputil::Xtoi(cmd.argv[3].c_str());
				towns.debugger.UnmonitorIO(portMin,portMax);
				std::cout << "Range:";
				std::cout << cpputil::Ustox(portMin);
				std::cout << " to ";
				std::cout << cpputil::Ustox(portMax);
				std::cout << std::endl;
			}
			else if(3<=cmd.argv.size())
			{
				auto port=cpputil::Xtoi(cmd.argv[2].c_str());
				towns.debugger.UnmonitorIO(port,port);
				std::cout << "Port:";
				std::cout << cpputil::Ustox(port);
				std::cout << std::endl;
			}
			else
			{
				std::cout << "IO_Monitor is OFF" << std::endl;
				towns.debugger.UnmonitorIO(0,0xFFFF);
			}
			break;
		case ENABLE_EVENTLOG:
			towns.eventLog.mode=TownsEventLog::MODE_NONE;
			std::cout << "Event Logging is OFF." << std::endl;
			break;
		case ENABLE_DEBUGGER:
			towns.DisableDebugger();
			std::cout << "Debugger is OFF." << std::endl;
			break;
		case ENABLE_MOUSEINTEGRATION:
			towns.var.mouseIntegration=false;
			towns.DontControlMouse();
			std::cout << "Mouse Integration is Disabled." << std::endl;
			break;
		case ENABLE_YM2612_LOG:
			towns.sound.state.ym2612.takeRegLog=false;
			std::cout << "YM2612 register write log is Disabled." << std::endl;
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

	auto addrAndSym=towns.debugger.GetSymTable().FindSymbolFromLabel(cmd.argv[1]);
	if(addrAndSym.second.label==cmd.argv[1])
	{
		towns.debugger.AddBreakPoint(addrAndSym.first);
	}
	else
	{
		auto farPtr=towns.cpu.TranslateFarPointer(cmdutil::MakeFarPointer(cmd.argv[1],towns.cpu));
		towns.debugger.AddBreakPoint(farPtr);
	}
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
	auto &breakOnIORead=towns.debugger.GetBreakOnIORead();
	if(true!=breakOnIORead.empty())
	{
		std::cout << "Break on IORead:" << std::endl;
		for(auto ioport : breakOnIORead)
		{
			std::cout << cpputil::Uitox(ioport) << std::endl;
		}
	}
	auto &breakOnIOWrite=towns.debugger.GetBreakOnIOWrite();
	if(true!=breakOnIOWrite.empty())
	{
		std::cout << "Break on IOWrite:" << std::endl;
		for(auto ioport : breakOnIOWrite)
		{
			std::cout << cpputil::Uitox(ioport) << std::endl;
		}
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
		case DUMP_LDT:
			towns.cpu.PrintLDT(towns.mem);
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
		case DUMP_CRTC:
			for(auto str : towns.crtc.GetStatusText())
			{
				std::cout << str << std::endl;
			}
			break;
		case DUMP_PALETTE:
			for(auto str : towns.crtc.GetPaletteText())
			{
				std::cout << str << std::endl;
			}
			break;
		case DUMP_TIMER:
			towns.PrintTimer();
			break;
		case DUMP_CSEIP_LOG:
			if(3<=cmd.argv.size())
			{
				Execute_PrintHistory(towns,cpputil::Atoi(cmd.argv[2].c_str()));
			}
			else
			{
				Execute_PrintHistory(towns,20);
			}
			break;
		case DUMP_SYMBOL_TABLE:
			for(auto str : towns.debugger.GetSymTable().GetList())
			{
				std::cout << str << std::endl;
			}
			break;
		case DUMP_MEMORY:
			for(auto str : towns.physMem.GetStatusText())
			{
				std::cout << str << std::endl;
			}
			break;
		case DUMP_CMOS:
			if(3<=cmd.argv.size())
			{
				auto addr=cpputil::Xtoi(cmd.argv[2].c_str());
				if(addr<0x2000)
				{
					auto length=std::min(256,0x2000-addr);
					std::vector <unsigned char> RAM;
					for(int i=0; i<length; ++i)
					{
						// RAM.push_back((unsigned char)towns.ioRAM.state.RAM[addr+i]);
						RAM.push_back(towns.physMem.state.CMOSRAM[addr+i]);
					}
					for(auto str : cpputil::MakeDump(addr,RAM.size(),RAM.data()))
					{
						std::cout << str << std::endl;
					}
				}
			}
			else
			{
				PrintError(ERROR_TOO_FEW_ARGS);
			}
			break;
		case DUMP_CDROM:
			for(auto str : towns.cdrom.GetStatusText())
			{
				std::cout << str << std::endl;
			}
			break;
		case DUMP_EVENTLOG:
			for(auto str : towns.eventLog.GetText())
			{
				std::cout << str << std::endl;
			}
			break;
		case DUMP_SCSI:
			for(auto str : towns.scsi.GetStatusText())
			{
				std::cout << str << std::endl;
			}
			break;
		case DUMP_SCHEDULE:
			for(auto str: towns.GetScheduledTasksText())
			{
				std::cout << str << std::endl;
			}
			break;
		case DUMP_TIME_BALANCE:
			for(unsigned int i=0; i<FMTowns::Variable::TIME_ADJUSTMENT_LOG_LEN; ++i)
			{
				std::cout << towns.var.timeAdjustLog[(towns.var.timeAdjustLogPtr+i+1)%FMTowns::Variable::TIME_ADJUSTMENT_LOG_LEN];
				std::cout << std::endl;
			}
			break;
		case DUMP_SPRITE:
			for(auto str : towns.sprite.GetStatusText(towns.physMem.state.spriteRAM.data()))
			{
				std::cout << str << std::endl;
			}
			break;
		case DUMP_SPRITE_AT:
			if(4<=cmd.argv.size())
			{
				auto x=cpputil::Atoi(cmd.argv[2].c_str());
				auto y=cpputil::Atoi(cmd.argv[3].c_str());
				for(auto str : towns.sprite.GetStatusTextSpriteAt(towns.physMem.state.spriteRAM.data(),x,y))
				{
					std::cout << str << std::endl;
				}
			}
			else
			{
				PrintError(ERROR_TOO_FEW_ARGS);
			}
			break;
		case DUMP_MOUSE:
			for(auto str : towns.GetMouseStatusText())
			{
				std::cout << str << std::endl;
			}
			break;
		case DUMP_YM2612_LOG:
			for(auto rwl : towns.sound.state.ym2612.regWriteLog)
			{
				std::cout << "0x" << cpputil::Ubtox(rwl.chBase) << "," <<
				             "0x" << cpputil::Ubtox(rwl.reg) << "," <<
				             "0x" << cpputil::Ubtox(rwl.data) << "," << "//(" << rwl.count << ")" << std::endl;
			}
			break;
		}
	}
	else
	{
		auto farPtr=cmdutil::MakeFarPointer(cmd.argv[1],towns.cpu);
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

void TownsCommandInterpreter::Execute_Calculate(FMTowns &towns,Command &cmd)
{
	if(cmd.argv.size()<2)
	{
		PrintError(ERROR_TOO_FEW_ARGS);
		return;
	}
	for(int i=1; i<cmd.argv.size(); ++i)
	{
		TownsLineParser parser(&towns.cpu);
		if(true==parser.Analyze(cmd.argv[i]))
		{
			auto value=parser.Evaluate();
			std::cout << cmd.argv[i] << "=" << value << "(" << cpputil::Uitox(value&0xFFFFFFFF) << ")" << std::endl;
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
				if(4<=cmd.argv.size())
				{
					std::string cond=cmd.argv[3];
					cpputil::Capitalize(cond);
					if('A'==cond[0] && 'H'==cond[1] && '='==cond[2])
					{
						unsigned int AH=cpputil::Xtoi(cond.c_str()+3);
						towns.debugger.SetBreakOnINTwithAH(cpputil::Xtoi(cmd.argv[2].c_str()),AH);
					}
					else if('A'==cond[0] && 'X'==cond[1] && '='==cond[2])
					{
						unsigned int AX=cpputil::Xtoi(cond.c_str()+3);
						towns.debugger.SetBreakOnINTwithAX(cpputil::Xtoi(cmd.argv[2].c_str()),AX);
					}
				}
			}
			else
			{
				PrintError(ERROR_TOO_FEW_ARGS);
				return;
			}
			break;
		case BREAK_ON_CVRAM_READ:
			towns.physMem.FMRVRAMAccess.breakOnCVRAMRead=true;
			break;
		case BREAK_ON_CVRAM_WRITE:
			towns.physMem.FMRVRAMAccess.breakOnCVRAMWrite=true;
			break;
		case BREAK_ON_FMRVRAM_READ:
			towns.physMem.FMRVRAMAccess.breakOnFMRVRAMRead=true;
			break;
		case BREAK_ON_FMRVRAM_WRITE:
			towns.physMem.FMRVRAMAccess.breakOnFMRVRAMWrite=true;
			break;
		case BREAK_ON_IOREAD:
			if(4<=cmd.argv.size())
			{
				auto portMin=cpputil::Xtoi(cmd.argv[2].c_str());
				auto portMax=cpputil::Xtoi(cmd.argv[3].c_str());
				for(unsigned int ioport=portMin; ioport<portMax; ++ioport)
				{
					towns.debugger.AddBreakOnIORead(ioport);
				}
				std::cout << "Range:";
				std::cout << cpputil::Ustox(portMin);
				std::cout << " to ";
				std::cout << cpputil::Ustox(portMax);
			}
			else if(3<=cmd.argv.size())
			{
				auto ioport=cpputil::Xtoi(cmd.argv[2].c_str());
				towns.debugger.AddBreakOnIORead(ioport);
				std::cout << "Port:" << cpputil::Uitox(ioport) << std::endl;
			}
			else
			{
				PrintError(ERROR_TOO_FEW_ARGS);
			}
			break;
		case BREAK_ON_IOWRITE:
			if(4<=cmd.argv.size())
			{
				auto portMin=cpputil::Xtoi(cmd.argv[2].c_str());
				auto portMax=cpputil::Xtoi(cmd.argv[3].c_str());
				for(unsigned int ioport=portMin; ioport<portMax; ++ioport)
				{
					towns.debugger.AddBreakOnIOWrite(ioport);
				}
				std::cout << "Range:";
				std::cout << cpputil::Ustox(portMin);
				std::cout << " to ";
				std::cout << cpputil::Ustox(portMax);
			}
			else if(3<=cmd.argv.size())
			{
				auto ioport=cpputil::Xtoi(cmd.argv[2].c_str());
				towns.debugger.AddBreakOnIOWrite(ioport);
				std::cout << "Port:" << cpputil::Ustox(ioport) << std::endl;
			}
			else
			{
				PrintError(ERROR_TOO_FEW_ARGS);
			}
			break;
		case BREAK_ON_VRAMREAD:
			towns.SetUpVRAMAccess(true,false);
			break;
		case BREAK_ON_VRAMWRITE:
			towns.SetUpVRAMAccess(false,true);
			break;
		case BREAK_ON_VRAMREADWRITE:
			towns.SetUpVRAMAccess(true,true);
			break;
		case BREAK_ON_CDC_COMMAND:
			towns.cdrom.var.debugBreakOnCommandWrite=true;
			if(3<=cmd.argv.size())
			{
				towns.cdrom.var.debugBreakOnSpecificCommand=cpputil::Xtoi(cmd.argv[2].c_str());
			}
			else
			{
				towns.cdrom.var.debugBreakOnSpecificCommand=0xFFFF;
			}
			break;
		case BREAK_ON_LBUTTON_UP:
			towns.var.debugBreakOnLButtonUp=true;
			break;
		case BREAK_ON_LBUTTON_DOWN:
			towns.var.debugBreakOnLButtonDown=true;
			break;
		case BREAK_ON_RETURN_KEY:
			towns.keyboard.debugBreakOnReturnKey=true;
			break;
		case BREAK_ON_SCSI_COMMAND:
			towns.scsi.breakOnSCSICommand=true;
			break;
		case BREAK_ON_SCSI_DMA_TRANSFER:
			towns.scsi.breakOnDMATransfer=true;
			break;
		case BREAK_ON_MEM_READ:
			if(4<=cmd.argv.size())
			{
				unsigned int addr0=cpputil::Xtoi(cmd.argv[2].c_str());
				unsigned int addr1=cpputil::Xtoi(cmd.argv[3].c_str());
				if(addr1<addr0)
				{
					std::swap(addr0,addr1);
				}
				for(auto addr=addr0; addr<=addr1; ++addr)
				{
					i486DebugMemoryAccess::SetBreakOnMemRead(towns.mem,towns.debugger,addr);
				}
				std::cout << "Break on Memory Read" << std::endl;
				std::cout << "  from PHYS:" << cpputil::Uitox(addr0) << std::endl;
				std::cout << "  to PHYS:  " << cpputil::Uitox(addr1) << std::endl;
			}
			else if(3<=cmd.argv.size())
			{
				i486DebugMemoryAccess::SetBreakOnMemRead(towns.mem,towns.debugger,cpputil::Xtoi(cmd.argv[2].c_str()));
				std::cout << "Break on Memory Read PHYS:" << cpputil::Uitox(cpputil::Xtoi(cmd.argv[2].c_str())) << std::endl;
			}
			else
			{
				PrintError(ERROR_TOO_FEW_ARGS);
				return;
			}
			break;
		case BREAK_ON_MEM_WRITE:
			if(4<=cmd.argv.size())
			{
				unsigned int addr0=cpputil::Xtoi(cmd.argv[2].c_str());
				unsigned int addr1=cpputil::Xtoi(cmd.argv[3].c_str());
				if(addr1<addr0)
				{
					std::swap(addr0,addr1);
				}
				for(auto addr=addr0; addr<=addr1; ++addr)
				{
					i486DebugMemoryAccess::SetBreakOnMemWrite(towns.mem,towns.debugger,addr);
				}
				std::cout << "Break on Memory Write" << std::endl;
				std::cout << "  from PHYS:" << cpputil::Uitox(addr0) << std::endl;
				std::cout << "  to PHYS:  " << cpputil::Uitox(addr1) << std::endl;
			}
			else if(3<=cmd.argv.size())
			{
				i486DebugMemoryAccess::SetBreakOnMemWrite(towns.mem,towns.debugger,cpputil::Xtoi(cmd.argv[2].c_str()));
				std::cout << "Break on Memory Write PHYS:" << cpputil::Uitox(cpputil::Xtoi(cmd.argv[2].c_str())) << std::endl;
			}
			else
			{
				PrintError(ERROR_TOO_FEW_ARGS);
				return;
			}
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
			if(3<=cmd.argv.size())
			{
				towns.debugger.ClearBreakOnINT(cpputil::Xtoi(cmd.argv[2].c_str()));
			}
			else
			{
				towns.debugger.ClearBreakOnINT();
			}
			break;
		case BREAK_ON_FMRVRAM_READ:
			towns.physMem.FMRVRAMAccess.breakOnFMRVRAMRead=false;
			break;
		case BREAK_ON_FMRVRAM_WRITE:
			towns.physMem.FMRVRAMAccess.breakOnFMRVRAMWrite=false;
			break;
		case BREAK_ON_CVRAM_READ:
			towns.physMem.FMRVRAMAccess.breakOnCVRAMRead=false;
			break;
		case BREAK_ON_CVRAM_WRITE:
			towns.physMem.FMRVRAMAccess.breakOnCVRAMWrite=false;
			break;
		case BREAK_ON_IOREAD:
			if(4<=cmd.argv.size())
			{
				auto portMin=cpputil::Xtoi(cmd.argv[2].c_str());
				auto portMax=cpputil::Xtoi(cmd.argv[3].c_str());
				for(unsigned int ioport=portMin; ioport<portMax; ++ioport)
				{
					towns.debugger.RemoveBreakOnIORead(ioport);
				}
				std::cout << "Range:";
				std::cout << cpputil::Ustox(portMin);
				std::cout << " to ";
				std::cout << cpputil::Ustox(portMax) << std::endl;
			}
			else if(3<=cmd.argv.size())
			{
				auto ioport=cpputil::Xtoi(cmd.argv[2].c_str());
				towns.debugger.RemoveBreakOnIORead(ioport);
				std::cout << "Port:" << cpputil::Uitox(ioport) << std::endl;
			}
			else
			{
				towns.debugger.RemoveBreakOnIORead();
			}
			break;
		case BREAK_ON_IOWRITE:
			if(4<=cmd.argv.size())
			{
				auto portMin=cpputil::Xtoi(cmd.argv[2].c_str());
				auto portMax=cpputil::Xtoi(cmd.argv[3].c_str());
				for(unsigned int ioport=portMin; ioport<portMax; ++ioport)
				{
					towns.debugger.RemoveBreakOnIOWrite(ioport);
				}
				std::cout << "Range:";
				std::cout << cpputil::Ustox(portMin);
				std::cout << " to ";
				std::cout << cpputil::Ustox(portMax) << std::endl;
			}
			else if(3<=cmd.argv.size())
			{
				auto ioport=cpputil::Xtoi(cmd.argv[2].c_str());
				towns.debugger.RemoveBreakOnIOWrite(ioport);
				std::cout << "Port:" << cpputil::Ustox(ioport) << std::endl;
			}
			else
			{
				towns.debugger.RemoveBreakOnIOWrite();
			}
			break;
		case BREAK_ON_VRAMREAD:
		case BREAK_ON_VRAMWRITE:
		case BREAK_ON_VRAMREADWRITE:
			towns.SetUpVRAMAccess(false,false);
			break;
		case BREAK_ON_CDC_COMMAND:
			towns.cdrom.var.debugBreakOnCommandWrite=false;
			break;
		case BREAK_ON_LBUTTON_UP:
			towns.var.debugBreakOnLButtonUp=false;
			break;
		case BREAK_ON_LBUTTON_DOWN:
			towns.var.debugBreakOnLButtonDown=false;
			break;
		case BREAK_ON_RETURN_KEY:
			towns.keyboard.debugBreakOnReturnKey=false;
			break;
		case BREAK_ON_SCSI_COMMAND:
			towns.scsi.breakOnSCSICommand=false;
			break;
		case BREAK_ON_SCSI_DMA_TRANSFER:
			towns.scsi.breakOnDMATransfer=false;
			break;
		case BREAK_ON_MEM_READ:
			if(4<=cmd.argv.size())
			{
				unsigned int addr0=cpputil::Xtoi(cmd.argv[2].c_str());
				unsigned int addr1=cpputil::Xtoi(cmd.argv[3].c_str());
				if(addr1<addr0)
				{
					std::swap(addr0,addr1);
				}
				for(auto addr=addr0; addr<=addr1; ++addr)
				{
					i486DebugMemoryAccess::ClearBreakOnMemRead(towns.mem,addr);
				}
				std::cout << "Clear Break on Memory Read" << std::endl;
				std::cout << "  from PHYS:" << cpputil::Uitox(addr0) << std::endl;
				std::cout << "  to PHYS:  " << cpputil::Uitox(addr1) << std::endl;
			}
			else if(3<=cmd.argv.size())
			{
				i486DebugMemoryAccess::ClearBreakOnMemRead(towns.mem,cpputil::Xtoi(cmd.argv[2].c_str()));
				std::cout << "Clear Break on Memory Read:" << cpputil::Uitox(cpputil::Xtoi(cmd.argv[2].c_str())) << std::endl;
			}
			else
			{
				i486DebugMemoryAccess::ClearBreakOnMemRead(towns.mem);
				std::cout << "Clear All Break on Memory Read" << std::endl;
			}
			break;
		case BREAK_ON_MEM_WRITE:
			if(4<=cmd.argv.size())
			{
				unsigned int addr0=cpputil::Xtoi(cmd.argv[2].c_str());
				unsigned int addr1=cpputil::Xtoi(cmd.argv[3].c_str());
				if(addr1<addr0)
				{
					std::swap(addr0,addr1);
				}
				for(auto addr=addr0; addr<=addr1; ++addr)
				{
					i486DebugMemoryAccess::ClearBreakOnMemWrite(towns.mem,addr);
				}
				std::cout << "Clear Break on Memory Write" << std::endl;
				std::cout << "  from PHYS:" << cpputil::Uitox(addr0) << std::endl;
				std::cout << "  to PHYS:  " << cpputil::Uitox(addr1) << std::endl;
			}
			else if(3<=cmd.argv.size())
			{
				i486DebugMemoryAccess::ClearBreakOnMemWrite(towns.mem,cpputil::Xtoi(cmd.argv[2].c_str()));
				std::cout << "Clear Break on Memory Write:" << cpputil::Uitox(cpputil::Xtoi(cmd.argv[2].c_str())) << std::endl;
			}
			else
			{
				i486DebugMemoryAccess::ClearBreakOnMemWrite(towns.mem);
				std::cout << "Clear All Break on Memory Write" << std::endl;
			}
			break;
		}
		std::cout << iter->first << " is OFF." << std::endl;
	}
}

void TownsCommandInterpreter::Execute_AddressTranslation(FMTowns &towns,Command &cmd)
{
	if(2<=cmd.argv.size())
	{
		auto farPtr=cmdutil::MakeFarPointer(cmd.argv[1],towns.cpu);
		farPtr=towns.cpu.TranslateFarPointer(farPtr);
		std::cout << farPtr.Format() << std::endl;

		i486DX::SegmentRegister seg;
		towns.cpu.LoadSegmentRegister(seg,farPtr.SEG,towns.mem);
		auto linear=seg.baseLinearAddr+farPtr.OFFSET;
		std::cout << "LINE:" << cpputil::Uitox(linear) << std::endl;

		if(true==towns.cpu.PagingEnabled())
		{
			auto physical=towns.cpu.LinearAddressToPhysicalAddress(linear,towns.mem);
			std::cout << "PHYS:" << cpputil::Uitox(physical) << std::endl;
		}
		else 
		{
			std::cout << "PHYS:" << cpputil::Uitox(linear) << std::endl;
		}
	}
	else
	{
		PrintError(ERROR_TOO_FEW_ARGS);
	}
}

void TownsCommandInterpreter::Execute_Disassemble(FMTowns &towns,Command &cmd)
{
	auto farPtr=towns.var.disassemblePointer;
	if(2<=cmd.argv.size())
	{
		farPtr=cmdutil::MakeFarPointer(cmd.argv[1],towns.cpu);
		if((farPtr.SEG&0xFFFF0000)==i486DX::FarPointer::PHYS_ADDR)
		{
			std::cout << "Disassembly cannot be from Physical address." << std::endl;
			return;
		}
	}

	i486DX::SegmentRegister seg;
	farPtr.LoadSegmentRegister(seg,towns.cpu,towns.mem);
	farPtr=towns.cpu.TranslateFarPointer(farPtr);
	for(int i=0; i<16; ++i)
	{
		i486DX::Instruction inst;
		i486DX::Operand op1,op2;
		MemoryAccess::ConstMemoryWindow emptyMemWin;

		towns.debugger.GetSymTable().PrintIfAny(farPtr.SEG,farPtr.OFFSET);
		towns.cpu.FetchInstruction(emptyMemWin,inst,op1,op2,seg,farPtr.OFFSET,towns.mem);
		auto nRawBytes=towns.debugger.GetSymTable().GetRawDataBytes(farPtr);
		if(0<nRawBytes)
		{
			unsigned int unitBytes=1,segBytes=0,repeat=nRawBytes,chopOff=16;
			std::cout << towns.cpu.DisassembleData(inst.addressSize,seg,farPtr.OFFSET,towns.mem,unitBytes,segBytes,repeat,chopOff) << std::endl;
			farPtr.OFFSET+=nRawBytes;
		}
		else
		{
			auto disasm=towns.cpu.Disassemble(inst,op1,op2,seg,farPtr.OFFSET,towns.mem,towns.debugger.GetSymTable(),towns.debugger.GetIOTable());
			std::cout << disasm << std::endl;
			farPtr.OFFSET+=inst.numBytes;
		}
	}
	towns.var.disassemblePointer=farPtr;
}
void TownsCommandInterpreter::Execute_Disassemble16(FMTowns &towns,Command &cmd)
{
	auto farPtr=towns.var.disassemblePointer;
	if(2<=cmd.argv.size())
	{
		farPtr=cmdutil::MakeFarPointer(cmd.argv[1],towns.cpu);
		if((farPtr.SEG&0xFFFF0000)==i486DX::FarPointer::PHYS_ADDR)
		{
			std::cout << "Disassembly cannot be from Physical address." << std::endl;
			return;
		}
	}

	i486DX::SegmentRegister seg;
	farPtr.LoadSegmentRegister(seg,towns.cpu,towns.mem);
	farPtr=towns.cpu.TranslateFarPointer(farPtr);
	for(int i=0; i<16; ++i)
	{
		i486DX::Instruction inst;
		i486DX::Operand op1,op2;
		MemoryAccess::ConstMemoryWindow emptyMemWin;

		towns.debugger.GetSymTable().PrintIfAny(farPtr.SEG,farPtr.OFFSET);
		towns.cpu.FetchInstruction(emptyMemWin,inst,op1,op2,seg,farPtr.OFFSET,towns.mem,16,16);
		auto nRawBytes=towns.debugger.GetSymTable().GetRawDataBytes(farPtr);
		if(0<nRawBytes)
		{
			unsigned int unitBytes=1,segBytes=0,repeat=nRawBytes,chopOff=16;
			std::cout << towns.cpu.DisassembleData(inst.addressSize,seg,farPtr.OFFSET,towns.mem,unitBytes,segBytes,repeat,chopOff) << std::endl;
			farPtr.OFFSET+=nRawBytes;
		}
		else
		{
			auto disasm=towns.cpu.Disassemble(inst,op1,op2,seg,farPtr.OFFSET,towns.mem,towns.debugger.GetSymTable(),towns.debugger.GetIOTable());
			std::cout << disasm << std::endl;
			farPtr.OFFSET+=inst.numBytes;
		}
	}
	towns.var.disassemblePointer=farPtr;
}
void TownsCommandInterpreter::Execute_Disassemble32(FMTowns &towns,Command &cmd)
{
	auto farPtr=towns.var.disassemblePointer;
	if(2<=cmd.argv.size())
	{
		farPtr=cmdutil::MakeFarPointer(cmd.argv[1],towns.cpu);
		if((farPtr.SEG&0xFFFF0000)==i486DX::FarPointer::PHYS_ADDR)
		{
			std::cout << "Disassembly cannot be from Physical address." << std::endl;
			return;
		}
	}

	i486DX::SegmentRegister seg;
	farPtr.LoadSegmentRegister(seg,towns.cpu,towns.mem);
	farPtr=towns.cpu.TranslateFarPointer(farPtr);
	for(int i=0; i<16; ++i)
	{
		i486DX::Instruction inst;
		i486DX::Operand op1,op2;
		MemoryAccess::ConstMemoryWindow emptyMemWin;

		towns.debugger.GetSymTable().PrintIfAny(farPtr.SEG,farPtr.OFFSET);
		towns.cpu.FetchInstruction(emptyMemWin,inst,op1,op2,seg,farPtr.OFFSET,towns.mem,32,32); // Fetch it anyway to have inst.addressSize
		auto nRawBytes=towns.debugger.GetSymTable().GetRawDataBytes(farPtr);
		if(0<nRawBytes)
		{
			unsigned int unitBytes=1,segBytes=0,repeat=nRawBytes,chopOff=16;
			std::cout << towns.cpu.DisassembleData(inst.addressSize,seg,farPtr.OFFSET,towns.mem,unitBytes,segBytes,repeat,chopOff) << std::endl;
			farPtr.OFFSET+=nRawBytes;
		}
		else
		{
			auto disasm=towns.cpu.Disassemble(inst,op1,op2,seg,farPtr.OFFSET,towns.mem,towns.debugger.GetSymTable(),towns.debugger.GetIOTable());
			std::cout << disasm << std::endl;
			farPtr.OFFSET+=inst.numBytes;
		}
	}
	towns.var.disassemblePointer=farPtr;
}

void TownsCommandInterpreter::Execute_PrintHistory(FMTowns &towns,unsigned int n)
{
	auto list=towns.debugger.GetCSEIPLog(n);
	auto &symTable=towns.debugger.GetSymTable();
	for(auto iter=list.rbegin(); iter!=list.rend(); ++iter)
	{
		std::cout << cpputil::Ustox(iter->SEG) << ":" << cpputil::Uitox(iter->OFFSET);
		std::cout << " ";
		std::cout << "SS=" << cpputil::Ustox(iter->SS);
		std::cout << " ";
		std::cout << "ESP=" << cpputil::Uitox(iter->ESP);
		if(1<iter->count)
		{
			std::cout << "(" << cpputil::Itoa((unsigned int)iter->count) << ")";
		}
		auto symbolPtr=symTable.Find(iter->SEG,iter->OFFSET);
		if(nullptr!=symbolPtr)
		{
			std::cout << " " << symbolPtr->Format();
		}
		std::cout << std::endl;
	}
}

void TownsCommandInterpreter::Execute_SaveHistory(FMTowns &towns,const std::string &fName)
{
	std::ofstream ofp(fName);
	if(ofp.is_open())
	{
		auto list=towns.debugger.GetCSEIPLog();
		auto &symTable=towns.debugger.GetSymTable();
		for(auto iter=list.rbegin(); iter!=list.rend(); ++iter)
		{
			ofp << cpputil::Ustox(iter->SEG) << ":" << cpputil::Uitox(iter->OFFSET);
			ofp << " ";
			ofp << "SS=" << cpputil::Ustox(iter->SS);
			ofp << " ";
			ofp << "ESP=" << cpputil::Uitox(iter->ESP);
			if(1<iter->count)
			{
				ofp << "(" << cpputil::Itoa((unsigned int)iter->count) << ")";
			}
			auto symbolPtr=symTable.Find(iter->SEG,iter->OFFSET);
			if(nullptr!=symbolPtr)
			{
				ofp << " " << symbolPtr->Format();
			}
			ofp << std::endl;
		}
		ofp.close();
	}
	else
	{
		PrintError(ERROR_CANNOT_SAVE_FILE);
	}
}

void TownsCommandInterpreter::Execute_SaveEventLog(FMTowns &towns,const std::string &fName)
{
	if(true!=towns.eventLog.SaveEventLog(fName))
	{
		PrintError(ERROR_CANNOT_SAVE_FILE);
	}
	else
	{
		std::cout << "Saved Event Log." << std::endl;
	}
}

void TownsCommandInterpreter::Execute_AddSymbol(FMTowns &towns,Command &cmd)
{
	if(3<=cmd.argv.size() || (2<=cmd.argv.size() && CMD_IMM_IS_IOPORT==cmd.primaryCmd))
	{
		auto &symTable=towns.debugger.GetSymTable();

		switch(cmd.primaryCmd)
		{
		case CMD_ADD_SYMBOL:
		case CMD_ADD_LABEL:
		case CMD_ADD_DATALABEL:
			{
				auto farPtr=cmdutil::MakeFarPointer(cmd.argv[1],towns.cpu);
				farPtr=towns.cpu.TranslateFarPointer(farPtr);
				auto *newSym=symTable.Update(farPtr,cmd.argv[2]);
				switch(cmd.primaryCmd)
				{
				case CMD_ADD_SYMBOL:
					newSym->symType=i486Symbol::SYM_PROCEDURE;
					break;
				case CMD_ADD_LABEL:
					newSym->symType=i486Symbol::SYM_JUMP_DESTINATION;
					break;
				case CMD_ADD_DATALABEL:
					newSym->symType=i486Symbol::SYM_DATA_LABEL;
					break;
				}
				std::cout << "Added symbol " << cmd.argv[2] << std::endl;
			}
			break;
		case CMD_DEF_RAW_BYTES:
			if(4<=cmd.argv.size())
			{
				auto numBytes=cpputil::Xtoi(cmd.argv[3].c_str());
				if(0<numBytes)
				{
					auto farPtr=cmdutil::MakeFarPointer(cmd.argv[1],towns.cpu);
					farPtr=towns.cpu.TranslateFarPointer(farPtr);
					auto *newSym=symTable.Update(farPtr,cmd.argv[2]);
					newSym->symType=i486Symbol::SYM_RAW_DATA;
					newSym->rawDataBytes=numBytes;
					std::cout << "Added raw byte data " << cmd.argv[2] << std::endl;
				}
				else
				{
					std::cout << "Ignored 0-byte data " << cmd.argv[2] << std::endl;
				}
			}
			else
			{
				PrintError(ERROR_TOO_FEW_ARGS);
				return;
			}
			break;
		case CMD_ADD_COMMENT:
			{
				auto farPtr=cmdutil::MakeFarPointer(cmd.argv[1],towns.cpu);
				farPtr=towns.cpu.TranslateFarPointer(farPtr);
				symTable.SetComment(farPtr,cmd.argv[2]);
				std::cout << "Added comment " << cmd.argv[2] << std::endl;
			}
			break;
		case CMD_IMM_IS_IOPORT:
			{
				auto farPtr=cmdutil::MakeFarPointer(cmd.argv[1],towns.cpu);
				farPtr=towns.cpu.TranslateFarPointer(farPtr);
				symTable.SetImmIsIOPort(farPtr);
			}
			break;
		}

		if(true!=towns.debugger.GetSymTable().AutoSave())
		{
			std::cout << "Auto-Saving of Symbol Table Failed." << std::endl;
			std::cout << "File name is not specified or invalid." << std::endl;
		}
	}
	else
	{
		PrintError(ERROR_TOO_FEW_ARGS);
	}
}

void TownsCommandInterpreter::Execute_DelSymbol(FMTowns &towns,Command &cmd)
{
	auto &symTable=towns.debugger.GetSymTable();
	if(2<=cmd.argv.size())
	{
		auto farPtr=cmdutil::MakeFarPointer(cmd.argv[1],towns.cpu);
		farPtr=towns.cpu.TranslateFarPointer(farPtr);
		if(true==symTable.Delete(farPtr))
		{
			std::cout << "Symbol&|Comment deleted." << std::endl;
		}
		else if(nullptr==symTable.Find(farPtr))
		{
			PrintError(ERROR_SYMBOL_NOT_FOUND);
		}
		else
		{
			PrintError(ERROR_COULD_NOT_DELETE_SYMBOL);
		}
	}
}

void TownsCommandInterpreter::Execute_TypeKeyboard(FMTowns &towns,Command &cmd)
{
	for(int i=0; i<cmd.cmdline.size(); ++i)
	{
		if(' '==cmd.cmdline[i] || '\t'==cmd.cmdline[i])
		{
			for(int j=i+1; j<cmd.cmdline.size(); ++j)
			{
				unsigned char byteData[2];
				if(0<TownsKeyboard::TranslateChar(byteData,cmd.cmdline[j]))
				{
					towns.keyboard.PushFifo(byteData[0]|TOWNS_KEYFLAG_PRESS  ,byteData[1]);
					towns.keyboard.PushFifo(byteData[0]|TOWNS_KEYFLAG_RELEASE,byteData[1]);
				}
			}
			unsigned char byteData[2];
			TownsKeyboard::TranslateChar(byteData,0x0D);
			towns.keyboard.PushFifo(byteData[0]|TOWNS_KEYFLAG_PRESS  ,byteData[1]);
			towns.keyboard.PushFifo(byteData[0]|TOWNS_KEYFLAG_RELEASE,byteData[1]);
			return;
		}
	}
}

void TownsCommandInterpreter::Execute_Let(FMTowns &towns,Command &cmd)
{
	if(3<=cmd.argv.size())
	{
		std::string cap=cmd.argv[1];
		cpputil::Capitalize(cap);
		auto reg=towns.cpu.StrToReg(cap);
		if(
		   i486DX::REG_AL==reg ||
		   i486DX::REG_AH==reg ||
		   i486DX::REG_AX==reg ||
		   i486DX::REG_EAX==reg ||
		   i486DX::REG_BL==reg ||
		   i486DX::REG_BH==reg ||
		   i486DX::REG_BX==reg ||
		   i486DX::REG_EBX==reg ||
		   i486DX::REG_CL==reg ||
		   i486DX::REG_CH==reg ||
		   i486DX::REG_CX==reg ||
		   i486DX::REG_ECX==reg ||
		   i486DX::REG_DL==reg ||
		   i486DX::REG_DH==reg ||
		   i486DX::REG_DX==reg ||
		   i486DX::REG_EDX==reg ||
		   i486DX::REG_SI==reg ||
		   i486DX::REG_ESI==reg ||
		   i486DX::REG_DI==reg ||
		   i486DX::REG_EDI==reg ||
		   i486DX::REG_SP==reg ||
		   i486DX::REG_ESP==reg ||
		   i486DX::REG_BP==reg ||
		   i486DX::REG_EBP==reg
		)
		{
			TownsLineParserHexadecimal parser(&towns.cpu);
			if(true==parser.Analyze(cmd.argv[2]))
			{
				towns.cpu.SetRegisterValue(reg,parser.Evaluate());
				std::cout << "Loaded register value." << std::endl;
			}
		}
		else
		{
			std::cout << "Cannot load a value to this register/flag." << std::endl;
		}
	}
}

void TownsCommandInterpreter::Execute_EditMemory(FMTowns &towns,Command &cmd,unsigned int numBytes)
{
	if(3<=cmd.argv.size())
	{
		auto farPtr=cmdutil::MakeFarPointer(cmd.argv[1],towns.cpu);
		if(farPtr.SEG==i486DX::FarPointer::NO_SEG)
		{
			farPtr.SEG=towns.cpu.state.DS().value;
		}
		TownsLineParserHexadecimal parser(&towns.cpu);
		if(true==parser.Analyze(cmd.argv[2]))
		{
			if((farPtr.SEG&0xFFFF0000)==i486DX::FarPointer::PHYS_ADDR)
			{
				switch(numBytes)
				{
				case 1:
					towns.mem.StoreByte(farPtr.OFFSET,parser.Evaluate());
					break;
				case 2:
					towns.mem.StoreWord(farPtr.OFFSET,parser.Evaluate());
					break;
				case 4:
					towns.mem.StoreDword(farPtr.OFFSET,parser.Evaluate());
					break;
				}
			}
			else
			{
				i486DX::SegmentRegister seg;
				farPtr.LoadSegmentRegister(seg,towns.cpu,towns.mem);
				switch(numBytes)
				{
				case 1:
					towns.cpu.DebugStoreByte(towns.mem,32,seg,farPtr.OFFSET,parser.Evaluate());
					break;
				case 2:
					towns.cpu.DebugStoreWord(towns.mem,32,seg,farPtr.OFFSET,parser.Evaluate());
					break;
				case 4:
					towns.cpu.DebugStoreDword(towns.mem,32,seg,farPtr.OFFSET,parser.Evaluate());
					break;
				}
			}
			std::cout << "Stored value to memory." << std::endl;
		}
	}
}
void TownsCommandInterpreter::Execute_Replace(FMTowns &towns,Command &cmd)
{
	std::cout << "Not implemented yet." << std::endl;
}

void TownsCommandInterpreter::Execute_CRTCPage(FMTowns &towns,Command &cmd)
{
	if(2<=cmd.argv.size() && towns.crtc.InSinglePageMode())
	{
		towns.crtc.state.showPageFDA0[0]=(0!=cpputil::Atoi(cmd.argv[1].c_str()));
		towns.crtc.state.showPageFDA0[1]=towns.crtc.state.showPageFDA0[0];
		towns.crtc.state.showPage0448[0]=towns.crtc.state.showPageFDA0[0];
		towns.crtc.state.showPage0448[1]=towns.crtc.state.showPageFDA0[1];
	}
	else if(3<=cmd.argv.size() && true!=towns.crtc.InSinglePageMode())
	{
		towns.crtc.state.showPageFDA0[0]=(0!=cpputil::Atoi(cmd.argv[1].c_str()));
		towns.crtc.state.showPageFDA0[1]=(0!=cpputil::Atoi(cmd.argv[2].c_str()));
		towns.crtc.state.showPage0448[0]=towns.crtc.state.showPageFDA0[0];
		towns.crtc.state.showPage0448[1]=towns.crtc.state.showPageFDA0[1];
	}
	else
	{
		PrintError(ERROR_TOO_FEW_ARGS);
	}
}

void TownsCommandInterpreter::Execute_CMOSLoad(FMTowns &towns,Command &cmd)
{
	if(2<=cmd.argv.size())
	{
		auto dat=cpputil::ReadBinaryFile(cmd.argv[1]);
		if(0==dat.size())
		{
			PrintError(ERROR_CANNOT_OPEN_FILE);
		}
		else if(TOWNS_CMOS_SIZE!=dat.size())
		{
			PrintError(ERROR_INCORRECT_FILE_SIZE);
		}
		towns.physMem.SetCMOS(dat);
		std::cout << "Loaded CMOS." << std::endl;
	}
	else
	{
		PrintError(ERROR_TOO_FEW_ARGS);
	}
}
void TownsCommandInterpreter::Execute_CMOSSave(FMTowns &towns,Command &cmd)
{
	if(2<=cmd.argv.size())
	{
		if(true!=cpputil::WriteBinaryFile(cmd.argv[1],TOWNS_CMOS_SIZE,TOWNS_CMOS_SIZE,towns.physMem.state.CMOSRAM))
		{
			PrintError(ERROR_CANNOT_SAVE_FILE);
		}
		else
		{
			std::cout << "Saved CMOS." << std::endl;
		}
	}
	else
	{
		PrintError(ERROR_TOO_FEW_ARGS);
	}
}
void TownsCommandInterpreter::Execute_CDLoad(FMTowns &towns,Command &cmd)
{
	if(2<=cmd.argv.size())
	{
		auto errCode=towns.cdrom.LoadDiscImage(cmd.argv[1]);
		std::cout << "[" << cmd.argv[1] << "]" << std::endl;
		if(DiscImage::ERROR_NOERROR==errCode)
		{
			std::cout << "Loaded Disc Image:" << cmd.argv[1] << std::endl;
		}
		else
		{
			std::cout << "Load Error:" << DiscImage::ErrorCodeToText(errCode) << std::endl;
		}
	}
}
void TownsCommandInterpreter::Execute_FDLoad(int drv,FMTowns &towns,Command &cmd)
{
	if(2<=cmd.argv.size())
	{
		if(true==towns.fdc.LoadRawBinary(drv,cmd.argv[1].c_str(),false))
		{
			std::cout << "Loaded FD image." << std::endl;
		}
		else
		{
			std::cout << "Failed to load FD image." << std::endl;
		}
	}
}

void TownsCommandInterpreter::Execute_FDEject(int drv,FMTowns &towns,Command &cmd)
{
	towns.fdc.Eject(drv);
	std::cout << "Ejected Floppy Drive " << drv << std::endl;
}

void TownsCommandInterpreter::Execute_MakeMemoryFilter(FMTowns &towns,Command &cmd)
{
	if(2<=cmd.argv.size())
	{
		auto byteData=cpputil::Xtoi(cmd.argv[1].c_str());
		memFilter.clear();

		unsigned int physAddr=0;
		for(auto memByte : towns.physMem.state.RAM)
		{
			if(memByte==byteData)
			{
				memFilter.insert(physAddr);
			}
			++physAddr;
		}

		std::cout << memFilter.size() << " occurrences" << std::endl;
	}
	else
	{
		PrintError(ERROR_TOO_FEW_ARGS);
	}
}
void TownsCommandInterpreter::Execute_UpdateMemoryFilter(FMTowns &towns,Command &cmd)
{
	if(2<=cmd.argv.size())
	{
		auto byteData=cpputil::Xtoi(cmd.argv[1].c_str());

		std::vector <decltype(memFilter.begin())> toErase;

		for(auto iter=memFilter.begin(); memFilter.end()!=iter; ++iter)
		{
			if(byteData!=towns.physMem.state.RAM[*iter])
			{
				toErase.push_back(iter);
			}
		}
		for(auto er : toErase)
		{
			memFilter.erase(er);
		}
		std::cout << memFilter.size() << " occurrences" << std::endl;
	}
	else
	{
		PrintError(ERROR_TOO_FEW_ARGS);
	}
}

void TownsCommandInterpreter::Execute_SaveYM2612Log(FMTowns &towns,std::string fName)
{
	std::ofstream ofp(fName);
	if(ofp.is_open())
	{
		for(auto rwl : towns.sound.state.ym2612.regWriteLog)
		{
			ofp << "0x" << cpputil::Ubtox(rwl.chBase) << "," <<
			       "0x" << cpputil::Ubtox(rwl.reg) << "," <<
			       "0x" << cpputil::Ubtox(rwl.data) << "," << "//(" << rwl.count << ")" << std::endl;
		}
		ofp.close();
		std::cout << "Saved " << fName << std::endl;
	}
	else
	{
		PrintError(ERROR_CANNOT_SAVE_FILE);
	}
}

void TownsCommandInterpreter::Execute_Search_Bytes(FMTowns &towns,Command &cmd)
{
	std::vector <unsigned char> bytes;
	for(int i=1; i<cmd.argv.size(); ++i)
	{
		auto arg=cmd.argv[i];
		char hex[3]={0,0,0};
		for(int j=arg.size()-1; 0<=j; --j)
		{
			if('h'==arg[j] || 'H'==arg[j])
			{
				continue;
			}
			if(1==j && arg[0]=='0' && (arg[1]=='x' || arg[1]=='X'))
			{
				break;
			}

			if(0==hex[1])
			{
				hex[1]=arg[j];
				if(0==j)
				{
					bytes.push_back(cpputil::Xtoi(hex+1));
					hex[0]=0;
					hex[1]=0;
				}
			}
			else
			{
				hex[0]=arg[j];
				bytes.push_back(cpputil::Xtoi(hex));
				hex[0]=0;
				hex[1]=0;
			}
		}
	}
	return Execute_Search_ByteSequence(towns,bytes);
}
void TownsCommandInterpreter::Execute_Search_String(FMTowns &towns,Command &cmd)
{
	if(2<=cmd.argv.size())
	{
		for(unsigned int i=1; i<cmd.argv.size(); ++i)
		{
			std::vector <unsigned char> bytes;
			for(auto c : cmd.argv[i])
			{
				bytes.push_back(c);
			}
			std::cout << "Searching: " << cmd.argv[i] << std::endl;
			Execute_Search_ByteSequence(towns,bytes);
		}
	}
	else
	{
		PrintError(ERROR_TOO_FEW_ARGS);
	}
}

void TownsCommandInterpreter::Execute_Search_ByteSequence(FMTowns &towns,const std::vector <unsigned char> &bytes)
{
	if(0==bytes.size())
	{
		PrintError(ERROR_NO_DATA_GIVEN);
		return;
	}
	std::cout << "Search is limited in the main RAM and VRAM only." << std::endl;

	int maxCount=100;
	for(unsigned int addr=0; addr+bytes.size()<=towns.physMem.state.RAM.size(); ++addr)
	{
		if(true==cpputil::Match(bytes.size(),bytes.data(),towns.physMem.state.RAM.data()+addr))
		{
			FoundAt(towns,addr);
		}
	}
	for(unsigned int addr=0; addr+bytes.size()<=towns.physMem.state.VRAM.size(); ++addr)
	{
		if(true==cpputil::Match(bytes.size(),bytes.data(),towns.physMem.state.VRAM.data()+addr))
		{
			FoundAt(towns,TOWNSADDR_VRAM0_BASE+addr);
			if(--maxCount<=0)
			{
				std::cout << "Reached maximum count." << std::endl;
			}
		}
	}
}

void TownsCommandInterpreter::FoundAt(FMTowns &towns,unsigned int physAddr)
{
	auto linearAddr=towns.cpu.PhysicalAddressToLinearAddress(physAddr,towns.mem);
	std::cout << "Found at PHYS:" << cpputil::Uitox(physAddr) << "  LINEAR:" << cpputil::Uitox(linearAddr) << std::endl;
	FoundAt("CS:",towns.cpu.state.CS().baseLinearAddr,linearAddr);
	FoundAt("DS:",towns.cpu.state.DS().baseLinearAddr,linearAddr);
	FoundAt("SS:",towns.cpu.state.SS().baseLinearAddr,linearAddr);
	FoundAt("ES:",towns.cpu.state.SS().baseLinearAddr,linearAddr);
	if(true!=towns.cpu.IsInRealMode() && 0x000C!=towns.cpu.state.CS().value)
	{
		i486DX::SegmentRegister seg;
		towns.cpu.DebugLoadSegmentRegister(seg,0x000c,towns.mem,false);
		FoundAt("000C:",seg.baseLinearAddr,linearAddr);
	}
	if(true!=towns.cpu.IsInRealMode() && 0x0014!=towns.cpu.state.DS().value)
	{
		i486DX::SegmentRegister seg;
		towns.cpu.DebugLoadSegmentRegister(seg,0x0014,towns.mem,false);
		FoundAt("0014:",seg.baseLinearAddr,linearAddr);
	}
	if(true!=towns.cpu.IsInRealMode())
	{
		i486DX::SegmentRegister seg;
		towns.cpu.DebugLoadSegmentRegister(seg,0x0110,towns.mem,false);
		FoundAt("0110:",seg.baseLinearAddr,linearAddr);
	}
}

void TownsCommandInterpreter::FoundAt(std::string segLabel,unsigned int linearBase,unsigned int linearAddr)
{
	std::cout << segLabel << cpputil::Uitox(linearAddr-linearBase) << std::endl;
}

void TownsCommandInterpreter::Execute_Find_Caller(FMTowns &towns,Command &cmd)
{
	if(2<=cmd.argv.size())
	{
		auto procAddr=cpputil::Xtoi(cmd.argv[1].c_str());
		for(auto callerAddr : towns.debugger.FindCaller(procAddr,towns.cpu.state.CS(),towns.cpu,towns.mem))
		{
			auto &seg=towns.cpu.state.CS();

			i486DX::Instruction inst;
			i486DX::Operand op1,op2;
			MemoryAccess::ConstMemoryWindow emptyMemWin;

			towns.debugger.GetSymTable().PrintIfAny(seg.value,callerAddr);
			towns.cpu.FetchInstruction(emptyMemWin,inst,op1,op2,seg,callerAddr,towns.mem);

			auto disasm=towns.cpu.Disassemble(inst,op1,op2,seg,callerAddr,towns.mem,towns.debugger.GetSymTable(),towns.debugger.GetIOTable());
			std::cout << disasm << std::endl;
		}
	}
	else
	{
		PrintError(ERROR_TOO_FEW_ARGS);
	}
}

void TownsCommandInterpreter::Execute_Exception(FMTowns &towns,Command &cmd)
{
	if(2<=cmd.argv.size())
	{
		auto typeStr=cmd.argv[1];
		cpputil::Capitalize(typeStr);
		if("GENERAL"==typeStr)
		{
			std::cout << "Not supported yet" << std::endl;
		}
		else if("PAGEFAULT"==typeStr)
		{
			towns.cpu.Interrupt(i486DX::INT_PAGE_FAULT,towns.mem,0);
			towns.cpu.Push(towns.mem,32,0);
			towns.PrintStatus();
		}
		else if("DIVISION"==typeStr)
		{
			towns.cpu.Interrupt(i486DX::INT_DIVISION_BY_ZERO,towns.mem,0);
			towns.PrintStatus();
		}
		else
		{
			std::cout << "What exception?" << std::endl;
		}
	}
	else
	{
		PrintError(ERROR_TOO_FEW_ARGS);
	}
}
void TownsCommandInterpreter::Execute_XMODEMtoVM(FMTowns &towns,Command &cmd)
{
	if(2<=cmd.argv.size())
	{
		auto dat=cpputil::ReadBinaryFile(cmd.argv[1]);
		if(0==dat.size())
		{
			PrintError(ERROR_CANNOT_OPEN_FILE);
		}
		else
		{
			if(towns.serialport.state.intel8251.clientPtr==&towns.serialport.defaultClient)
			{
				towns.serialport.defaultClient.SetUpXMODEMtoVM(dat);
				std::cout << "Ready to send " << cmd.argv[1] << std::endl;
				std::cout << "Start XMODEM in FM TOWNS!" << std::endl;
			}
			else
			{
				std::cout << "Default serial-port client is not in charge." << std::endl;
			}
		}
	}
	else
	{
		PrintError(ERROR_TOO_FEW_ARGS);
	}
}
void TownsCommandInterpreter::Execute_XMODEMfromVM(FMTowns &towns,Command &cmd)
{
	if(2<=cmd.argv.size())
	{
	}
	else
	{
		PrintError(ERROR_TOO_FEW_ARGS);
	}
}
