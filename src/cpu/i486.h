/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#ifndef I486_IS_INCLUDED
#define I486_IS_INCLUDED
/* { */

#include <map>
#include <string>
#include <iostream>

#include "cpu.h"
#include "ramrom.h"
#include "inout.h"
#include "cpputil.h"
#include "i486inst.h"

// References
// [1]  i486 Processor Programmers Reference Manual

// Todo:
//   Move function should raise protection fault / real-mode exception

class i486SymbolTable;


class i486DX : public CPU
{
public:
	#define NUM_BYTES_MASK static const unsigned int numBytesMask[5]={0,0xFF,0xFFFF,0xFFFFFF,0xFFFFFFFF};
	#define NUM_BYTES_TO_BASIC_REG_BASE static const unsigned int numBytesToBasicRegBase[5]={REG_8BIT_REG_BASE,REG_8BIT_REG_BASE,REG_16BIT_REG_BASE,REG_16BIT_REG_BASE,REG_32BIT_REG_BASE};
	#define NUM_BYTES_TO_REGISTER_OPERAND_TYPE \
	static const unsigned int numBytesToRegisterOperandType[]= \
	{ \
		OPER_REG8, \
		OPER_REG8, \
		OPER_REG16, \
		OPER_REG16, \
		OPER_REG32, \
	};


	inline unsigned int BitToMask(unsigned char bit)
	{
		unsigned int mask=(1<<(bit-1));
		return mask|(mask-1);
	}

	static const char *const Reg8Str[8];
	static const char *const Reg16Str[8];
	static const char *const Reg32Str[8];
	static const char *const Sreg[8];
	static const bool ParityTable[256];

	/* opCodeRenumberTable is for making a single jump table for switch-case statement.
	   Visual C++ makes multiple jump tables for one switch-case statement if the case numbers 
	   are sparse.  Ends up checking number range for each jump table before branching.
	   To avoid this blunder, numbers must be crunched into one sequence.
	   opCodeRenumberTable is to map instruction code to one sequence of numbers.
	     I486_OPCODE_xxxx to I486_RENUMBER_xxxx
	*/
	static unsigned short opCodeRenumberTable[I486_OPCODE_MAX+1];

	enum
	{
		I486_OPCODE_NEED_SECOND_BYTE=0x0F,
		I486_NUM_IOPORT=65536,
		MAX_REP_BUNDLE_COUNT=128,
	};

	enum
	{
		// Do not change the order below.  ExtractSegmentAndOffset assumes the order. >>
		REG_NULL,         // NULL_and_reg32[REG_NULL] must always be zero. (Initialized in the constructor.)
		REG_EAX,          // NULL_and_reg32[REG_EAX to REG_EDI] must return corresponding register value.
		REG_ECX,          // NULL_and_reg32[(REG_AX to REG_DI)&15] must return corresponding register value.
		REG_EDX,          // ExtractSegmentAndOffset, EvaluateOperand, StoreOperandValue, and LEA instruction
		REG_EBX,          // uses these assumptions.
		REG_ESP,          // 
		REG_EBP,          // 
		REG_ESI,          // 
		REG_EDI,          // 

		REG_AL,          // To satisfy above condition, 8 registers are needed between REG_EDI and REG_AX.
		REG_CL,          // 8-bit registers fit well for this space.
		REG_DL,          // 
		REG_BL,          // 
		REG_AH,
		REG_CH,
		REG_DH,
		REG_BH,

		REG_AX,
		REG_CX,
		REG_DX,
		REG_BX,
		REG_SP,
		REG_BP,
		REG_SI,
		REG_DI,
		// << Do not change the order above.  ExtractSegmentAndOffset assumes the order.

		REG_IP,
		REG_EIP,
		REG_EFLAGS,

		// Segment order is in [1] pp.26-10
		REG_ES,
		REG_CS,
		REG_SS,
		REG_DS,
		REG_FS,
		REG_GS,

		REG_GDT,
		REG_LDT,
		REG_TR,
		REG_IDTR,
		REG_CR0,
		REG_CR1,
		REG_CR2,
		REG_CR3,
		REG_DR0,
		REG_DR1,
		REG_DR2,
		REG_DR3,
		REG_DR4,
		REG_DR5,
		REG_DR6,
		REG_DR7,

		REG_TEST0,
		REG_TEST1,
		REG_TEST2,
		REG_TEST3,
		REG_TEST4,
		REG_TEST5,
		REG_TEST6,
		REG_TEST7,

	REG_TOTAL_NUMBER_OF_REGISTERS
	};
	enum
	{
		REG_8BIT_REG_BASE=REG_AL,
		REG_16BIT_REG_BASE=REG_AX,
		REG_32BIT_REG_BASE=REG_EAX,
		REG_SEGMENT_REG_BASE=REG_ES,
		REG_CR_REG_BASE=REG_CR0,
		REG_DR_REG_BASE=REG_DR0,
		REG_TEST_REG_BASE=REG_TEST0,
	};
	static const char *const RegToStr[REG_TOTAL_NUMBER_OF_REGISTERS];

	class SegmentRegister
	{
	public:
		unsigned short value;

		// Cache
		unsigned int baseLinearAddr;
		unsigned int operandSize;
		unsigned int addressSize;
		unsigned int limit;
	};


	enum
	{
		NUM_SEGMENT_REGISTERS=REG_GS-REG_SEGMENT_REG_BASE+1
	};
	/*! sregIndexToSregPtrTable[i] should be &state.sreg[i] except sregIndexToSregPtrTable[NUM_SEGMENT_REGISTERS].
	   It is used for quickly finding the segment register from segment override prefix.
	*/
	mutable SegmentRegister *sregIndexToSregPtrTable[NUM_SEGMENT_REGISTERS+1];
	/* segPrefixToSregIndex maps segment-override prefix to segment-register number.
	   For a number that is a segment-override prefix, it is 0 to NUM_SEGMENT_REGISTERS-1.
	   For a number that is not a segment-override prefix, it is NUM_SEGMENT_REGISTERS.
	*/
	unsigned int segPrefixToSregIndex[256];
	/*! 
	*/
	SegmentRegister *baseRegisterToDefaultSegment[REG_TOTAL_NUMBER_OF_REGISTERS];


	class SystemAddressRegister
	{
	public:
		unsigned int linearBaseAddr;
		unsigned short limit;
	};
	class SystemAddressRegisterAndSelector : public SystemAddressRegister
	{
	public:
		unsigned short selector;
	};
	enum
	{
		TSS_OFFSET_ESP0=0x04,
		TSS_OFFSET_SS0= 0x08,
	};
	class TaskRegister : public SegmentRegister
	{
	public:
		unsigned int attrib;  // Should it keep attribute?
	};
	class FarPointer
	{
	public:
		/*! If SEG&0xFFFF0000 is:
		      SEG_REGISTER, SEG&0xFFFF is one of REG_ES,REG_CS,REG_SS,REG_DS,REG_FS,REG_GS.
	          LINEAR_ADDR, OFFSET should be interpreted as linear address.
	          PHYS_ADDR, OFFSET should be interpreted as physical address.
		*/
		unsigned int SEG;
		unsigned int OFFSET;

		enum
		{
			SPECIAL_SEG_MASK=0xFFFF0000,
			NO_SEG=0xFFFF0000,
			SEG_WILDCARD=0xFFFE0000, // Any offeset
			SEG_REGISTER=0x00010000,
			LINEAR_ADDR=0x00020000,
			PHYS_ADDR=0x00030000,
			REAL_ADDR=0x00040000,
		};

		inline unsigned long long int Combine(void) const
		{
			unsigned long long LSEG=SEG;
			return (LSEG<<32)|OFFSET;
		}
		inline void Nullify(void)
		{
			SEG=0;
			OFFSET=0;
		}

		bool operator==(const FarPointer rv) const
		{
			return this->Combine()==rv.Combine();
		}
		bool operator!=(const FarPointer rv) const
		{
			return this->Combine()!=rv.Combine();
		}
		bool operator<(const FarPointer rv) const
		{
			return this->Combine()<rv.Combine();
		}
		bool operator>(const FarPointer rv) const
		{
			return this->Combine()>rv.Combine();
		}
		bool operator<=(const FarPointer rv) const
		{
			return this->Combine()<=rv.Combine();
		}
		bool operator>=(const FarPointer rv) const
		{
			return this->Combine()>=rv.Combine();
		}

		std::string Format(void) const;
		void MakeFromString(const std::string &str);

		void LoadSegmentRegister(SegmentRegister &seg,i486DX &cpu,const Memory &mem) const;
	};

	/*! Translate far pointer.
	    If NO_SEG is given as SEG, it will replace SEG with CS value.
	    If a segment register is specified in SEG, it will place values accordingly.
	    It does nothing for LINEAR_ADDR and PHYS_ADDR types.
	*/
	FarPointer TranslateFarPointer(FarPointer ptr) const;

	class FPUState
	{
	public:
		bool FPUEnabled;

		void FNINIT(void);
		bool ExceptionPending(void) const;
		unsigned int GetStatusWord(void) const;
		unsigned int GetControlWord(void) const;

		FPUState();
	};

	class InterruptDescriptor : public FarPointer
	{
	public:
		unsigned short flags;
		inline unsigned int GetType(void) const
		{
			return (flags>>8)&0x1F;
		}
	};

	class State
	{
	public:
		unsigned int NULL_and_reg32[9];

		inline unsigned int *reg32(void)
		{
			return NULL_and_reg32+1;
		}
		inline const unsigned int *reg32(void) const
		{
			return NULL_and_reg32+1;
		}

		inline unsigned int EAX(void) const
		{
			return reg32()[REG_EAX-REG_EAX];
		}
		inline unsigned int EBX(void) const
		{
			return reg32()[REG_EBX-REG_EAX];
		}
		inline unsigned int ECX(void) const
		{
			return reg32()[REG_ECX-REG_EAX];
		}
		inline unsigned int EDX(void) const
		{
			return reg32()[REG_EDX-REG_EAX];
		}
		inline unsigned int ESI(void) const
		{
			return reg32()[REG_ESI-REG_EAX];
		}
		inline unsigned int EDI(void) const
		{
			return reg32()[REG_EDI-REG_EAX];
		}
		inline unsigned int EBP(void) const
		{
			return reg32()[REG_EBP-REG_EAX];
		}
		inline unsigned int ESP(void) const
		{
			return reg32()[REG_ESP-REG_EAX];
		}
		inline unsigned int &EAX(void)
		{
			return reg32()[REG_EAX-REG_EAX];
		}
		inline unsigned int &EBX(void)
		{
			return reg32()[REG_EBX-REG_EAX];
		}
		inline unsigned int &ECX(void)
		{
			return reg32()[REG_ECX-REG_EAX];
		}
		inline unsigned int &EDX(void)
		{
			return reg32()[REG_EDX-REG_EAX];
		}
		inline unsigned int &ESI(void)
		{
			return reg32()[REG_ESI-REG_EAX];
		}
		inline unsigned int SI(void)
		{
			return reg32()[REG_ESI-REG_EAX]&0xffff;
		}
		inline unsigned int &EDI(void)
		{
			return reg32()[REG_EDI-REG_EAX];
		}
		inline unsigned int DI(void)
		{
			return reg32()[REG_EDI-REG_EAX]&0xffff;
		}
		inline unsigned int &EBP(void)
		{
			return reg32()[REG_EBP-REG_EAX];
		}
		inline unsigned int BP(void)
		{
			return reg32()[REG_EBP-REG_EAX]&0xffff;
		}
		inline unsigned int &ESP(void)
		{
			return reg32()[REG_ESP-REG_EAX];
		}
		inline unsigned int SP(void)
		{
			return reg32()[REG_ESP-REG_EAX]&0xffff;
		}

		unsigned int EIP;
		unsigned int EFLAGS;   // bit 1=Always 1 ([1] pp.2-14)
		SegmentRegister sreg[6];
		SystemAddressRegister GDTR,IDTR;
		SystemAddressRegisterAndSelector LDTR;
		TaskRegister TR;
	private:
		unsigned int CR[4];
	public:
		MemoryAccess::ConstPointer pageDirectoryCache; // This must be re-cached on state-load.
		MemoryAccess::ConstMemoryWindow CSEIPWindow;   // This must be cleared on state-load.
		MemoryAccess::MemoryWindow SSESPWindow;         // This must be cleared on state-load.
	public:
		unsigned int DR[8];
		unsigned int TEST[8];

		inline const SegmentRegister &ES(void) const
		{
			return sreg[REG_ES-REG_SEGMENT_REG_BASE];
		}
		inline const SegmentRegister &CS(void) const
		{
			return sreg[REG_CS-REG_SEGMENT_REG_BASE];
		}
		inline const SegmentRegister &SS(void) const
		{
			return sreg[REG_SS-REG_SEGMENT_REG_BASE];
		}
		inline const SegmentRegister &DS(void) const
		{
			return sreg[REG_DS-REG_SEGMENT_REG_BASE];
		}
		inline const SegmentRegister &FS(void) const
		{
			return sreg[REG_FS-REG_SEGMENT_REG_BASE];
		}
		inline const SegmentRegister &GS(void) const
		{
			return sreg[REG_GS-REG_SEGMENT_REG_BASE];
		}
		inline SegmentRegister &ES(void)
		{
			return sreg[REG_ES-REG_SEGMENT_REG_BASE];
		}
		inline SegmentRegister &CS(void)
		{
			return sreg[REG_CS-REG_SEGMENT_REG_BASE];
		}
		inline SegmentRegister &SS(void)
		{
			return sreg[REG_SS-REG_SEGMENT_REG_BASE];
		}
		inline SegmentRegister &DS(void)
		{
			return sreg[REG_DS-REG_SEGMENT_REG_BASE];
		}
		inline SegmentRegister &FS(void)
		{
			return sreg[REG_FS-REG_SEGMENT_REG_BASE];
		}
		inline SegmentRegister &GS(void)
		{
			return sreg[REG_GS-REG_SEGMENT_REG_BASE];
		}

		/*! Returns Segment-Register.  REG must be REG_CS,REG_DS,REG_ES,REG_FS,REG_FS,REG_SS.
		*/
		inline SegmentRegister GetSegmentRegister(unsigned int reg) const
		{
			switch(reg)
			{
			case REG_CS:
				return CS();
			case REG_DS:
				return DS();
			case REG_ES:
				return ES();
			case REG_FS:
				return FS();
			case REG_GS:
				return GS();
			case REG_SS:
				return SS();
			}
			return DS();
		}

		inline unsigned int GetCR(unsigned int num) const
		{
			return CR[num&3];
		}

		/*! This function must NOT be used anywhere outside i486DX::SetCR.
		*/
		inline void _SetCR(unsigned int num,unsigned int value)
		{
			CR[num]=value;
		}

		FPUState fpuState;

		bool halt;

		// [1] pp.26-211 in the description of the MOV instruction
		// "Loading to SS register inhibits all interrupts until after the execution of the next instruction"
		bool holdIRQ;

		// True when an instruction raised an exception.
		bool exception;
		unsigned int exceptionCode,exceptionType;
	};

	enum
	{
		EFLAGS_CARRY=      0x00001,
		EFLAGS_PARITY=     0x00004,
		EFLAGS_AUX_CARRY=  0x00010,
		EFLAGS_ZERO=       0x00040,
		EFLAGS_SIGN=       0x00080,
		EFLAGS_TRAP=       0x00100,
		EFLAGS_INT_ENABLE= 0x00200,
		EFLAGS_DIRECTION=  0x00400,
		EFLAGS_OVERFLOW=   0x00800,
		EFLAGS_IOPL=       0x03000,
		EFLAGS_NESTED=     0x04000,
		EFLAGS_RESUME=     0x10000,
		EFLAGS_VIRTUAL86=  0x20000,
		EFLAGS_ALIGN_CHECK=0x40000,

		CR0_PAGING_ENABLED=       0x80000000,  // [1] Page 4-6
		CR0_CACHE_DISABLE=        0x40000000,
		CR0_NOT_WRITE_THROUGH=    0x20000000,
		CR0_ALIGNMENT_MASK=       0x00040000,
		CR0_WRITE_PROTECT=        0x00010000,
		CR0_EXTENSION_TYPE=       0x00000010,
		CR0_TASK_SWITCHED=        0x00000008,
		CR0_EMULATION=            0x00000004,
		CR0_MATH_PRESENT=         0x00000002,
		CR0_PROTECTION_ENABLE=    0x00000001,

		CR3_PG_LVL_CACHE_DISABLE= 0x00000010,
		CR3_PG_LVL_WRITE_TRNSPRNT=0x00000008,
	};

	enum
	{
		// [1] pp.26-2
		INST_PREFIX_REP=  0xF3, // REP/REPE/REPZ
		INST_PREFIX_REPE= 0xF3, // REP/REPE/REPZ
		INST_PREFIX_REPNE=0xF2, // REPNE/REPNZ
		INST_PREFIX_LOCK= 0xF0, // LOCK

		// 0xF3 can precede:
		//   INS and interpreted as REP
		//   MOVS and interpreted as REP
		//   OUTS and interpreted as REP
		//   STOS and interpreted as REP
		//   CMPS and interpreted as REPE
		//   SCAS and interpreted as REPE
		// 0xF2 can precede:
		//   LODS and interpreted as REP    <- Seriously?  Prob typo.  386ASM writes F2 AC
		//   CMPS and interpreted as REPNE
		//   SCAS and interpreted as REPNE
		


		SEG_OVERRIDE_CS=  0x2E,
		SEG_OVERRIDE_SS=  0x36,
		SEG_OVERRIDE_DS=  0x3E,
		SEG_OVERRIDE_ES=  0x26,
		SEG_OVERRIDE_FS=  0x64,
		SEG_OVERRIDE_GS=  0x65,

		// [1] pp.26-1
		OPSIZE_OVERRIDE=  0x66,
		ADDRSIZE_OVERRIDE=0x67,


		// 
		FPU_FWAIT=        0x9B,
	};
	enum
	{
		// [1] pp. 10-3
		RESET_EFLAGS=        2,
		RESET_EIP=      0xFFF0,
		RESET_CS=       0xF000,
		RESET_DS=       0x0000,
		RESET_SS=       0x0000,
		RESET_ES=       0x0000,
		RESET_FS=       0x0000,
		RESET_GS=       0x0000,
		RESET_GDTRBASE=      0,
		RESET_GDTRLIMIT=     0,
		RESET_IDTRBASE=      0,
		RESET_IDTRLIMIT=0x03FF,
		RESET_LDTRBASE=      0,
		RESET_LDTRLIMIT=     0,
		RESET_LDTRSELECTOR=  0,
		RESET_TRBASE=        0,
		RESET_TRLIMIT=       0,
		RESET_TRSELECTOR=    0,
		RESET_TRATTRIB=      0,
		RESET_DR7=           0,
		RESET_EAX=           0,
		RESET_DX=       0x0400,
		RESET_CR0=           0,

		RESET_FPU_CTRL_WORD=   0x37F,
		RESET_FPU_STATUS_WOR=      0,
		RESET_FPU_TAG_WORD=   0xffff,
		RESET_FPU_IP_OFFSET=       0,
		RESET_FPU_DATA_OP_OFFSET=  0,
		RESET_FPU_CS_SELECTOR=     0,
		RESET_FPU_OP_SELECTOR=     0,
		RESET_FPU_OPCODE=          0,
	};

	enum
	{
		EXCEPTION_NONE,
		EXCEPTION_GP,
		EXCEPTION_ND,
		EXCEPTION_UD,
		EXCEPTION_SS,
		EXCEPTION_PF
	};

	class Operand;

	class Instruction
	{
	public:
		unsigned int numBytes;
		unsigned int instPrefix;
		unsigned int segOverride;
		unsigned int operandSize;
		unsigned int addressSize;
		unsigned int codeAddressSize; // 2020/03/07 Turned out, I need to keep code-segment address size for fetching instructions.
		unsigned int fwait;

		unsigned int opCode;
		unsigned int operandLen;
		unsigned char operand[12];  // Is 8 bytes maximum?  Maybe FPU instruction use up to 10 bytes.  I'll see.
		unsigned char imm[4];       // Probably 4 bytes is good enough.

		inline void Clear(void)
		{
			numBytes=0;
			instPrefix=0;
			segOverride=0;
			operandLen=0;
			fwait=0;
		}

		std::string Disassemble(const Operand &op1,const Operand &op2,SegmentRegister reg,unsigned int offset,const class i486SymbolTable &symTable,const std::map <unsigned int,std::string> &ioTable) const;
	private:
		/* operandSize is 8, 16, or 32 */
		std::string DisassembleTypicalOneOperand(std::string inst,const Operand &op,int operandSize) const;
		std::string DisassembleTypicalOneOperandAndImm(std::string inst,const Operand &op,unsigned int imm,int operandSize) const;
		std::string DisassembleTypicalRM8_I8(std::string inst,const Operand &op1,unsigned int I8) const;
		std::string DisassembleTypicalRM_I8(std::string inst,const Operand &op1,unsigned int I8) const;
		std::string DisassembleTypicalTwoOperands(std::string inst,const Operand &op1,const Operand &op2) const;
		std::string DisassembleTypicalOneImm(std::string inst,unsigned int imm,int operandSize) const;

		std::string DisassembleIOLabel(unsigned int CS,unsigned int EIP,const i486SymbolTable &symTable,const std::map <unsigned int,std::string> &ioTable,unsigned int imm) const;

	public:
		/*! Returns REG of MODR/M byte from the first byte of operand internally
		    stored in this class. */
		inline unsigned int GetREG(void) const
		{
			return (operand[0]>>3)&7;
		}
		/*! Returns REG of MODR/M byte, when MODR/M byte is already known. */
		static inline unsigned int GetREG(unsigned int MODR_M)
		{
			return (MODR_M>>3)&7;
		}



		/*! Returns Unsigned Imm8 (last byte in the operand) after decoding. */
		inline unsigned int EvalUimm8(void) const
		{
			return imm[0];
		}

		/*! Returns Unsigned Imm16 (last 2 byte in the operand) after decoding. */
		inline unsigned int EvalUimm16(void) const
		{
			return cpputil::GetWord(imm);
		}

		/*! Returns Unsigned Imm32 (last 4 byte in the operand) after decoding. */
		inline unsigned int EvalUimm32(void) const
		{
			return cpputil::GetDword(imm);
		}

		/*! Returns Unsigned Imm8 or Imm16 or Imm32 after decoding. */
		unsigned int EvalUimm8or16or32(unsigned int operandSize) const
		{
			static const unsigned int sizeMask[3]=
			{
				0x000000FF,
				0x0000FFFF,
				0xFFFFFFFF,
			};
			return cpputil::GetDword(imm)&sizeMask[operandSize>>4];
		}

		/*! Returns Signed Imm8 (last byte in the operand) after decoding. */
		inline int EvalSimm8(void) const
		{
			return cpputil::GetSignedByte(imm[0]);
		}

		/*! Returns Signed Imm16 (last 2 byte in the operand) after decoding. */
		inline int EvalSimm16(void) const
		{
			return cpputil::GetSignedWord(imm);
		}

		/*! Returns Signed Imm32 (last 4 byte in the operand) after decoding. */
		inline int EvalSimm32(void) const
		{
			return cpputil::GetSignedDword(imm);
		}

		/*! Returns Signed Imm16 or Imm32 after decoding. */
		int EvalSimm16or32(unsigned int operandSize) const
		{
			if(16==operandSize)
			{
				return EvalSimm16();
			}
			else
			{
				return EvalSimm32();
			}
		}



		static std::string SegmentOverrideString(int segOverridePrefix);

		static std::string SegmentOverrideSIorESIString(int segOverridePrefix,int addressSize);
	};

	/*! FM TOWNS BIOS uses:
			MOV		AX,0110H
			MOV		FS,AX
			CALL	FAR PTR FS:[0040H]
		for reading from a mouse.  That is a perfect opportunity for the emulator to
		identify the operating system version.  The CPU class fires:
			mouseBIOSInterceptorPtr->Intercept();
		when indirect CALL to 0110:[0040H].
	*/
	class FMTownsMouseBiosInterceptor
	{
	public:
		virtual void InterceptMouseBIOS(void)=0;
	};
	FMTownsMouseBiosInterceptor *mouseBIOSInterceptorPtr=nullptr;



	/*! INT21HInterceptor will intercept DOS function calls (INT 21H) when call stack is enabled.
	    For AH=3DH (fopen) and AH=4BH (load or exec), the file name will be given.
	*/
	class INT21HInterceptor
	{
	public:
		virtual void InterceptINT21H(unsigned int AX,const std::string fName)=0;
	};
	INT21HInterceptor *int21HInterceptorPtr=nullptr;


	enum
	{
		OPER_UNDEFINED,
		OPER_ADDR,    // BaseReg+IndexReg*IndexScaling+Offset
		OPER_FARADDR,
		OPER_REG,
		OPER_REG32, // EAX,ECX,EDX,EBX,ESP,EBP,ESI,EDI
		OPER_REG16, // AX,CX,DX,BX,SP,BP,SI,DI
		OPER_REG8,  // AL,CL,DL,BL,AH,CH,DH,BH
		OPER_SREG,  // ES,CS,SS,DS,FS,GS,
	};
	class Operand
	{
	public:
		int operandType;

		// For OPER_REG* operand type
		int reg;
		// For OPER_IMM* operand type
		int imm;
		// For OPER_ADDR and OPER_FARADDR operand type
		int baseReg,indexReg,indexShift,offset;
		int offsetBits;
		// For OPER_FARADDR operand type.  NOT USED in any other operand types.
		int seg;

		inline Operand(){}

		/*! Constructor for decoding on construction.
		*/
		Operand(int addressSize,int dataSize,const unsigned char operand[]);

		/*! operandType=OPER_UNDEFINED
		*/
		inline void Clear(void)
		{
			operandType=OPER_UNDEFINED;
			indexShift=0;
		}

		/*! Decode operand and returns the number of bytes.
		*/
		unsigned int Decode(int addressSize,int dataSize,const unsigned char operand[]);
		/*! Decode operand MODR/M byte.
		    MODR/M byte only tells a register number between 0 and 7.
		    Same number like number 0 can be AL, AX, or EAX.
		    This function identifies which of possible registers based on dataSize.
		    Subsequent StoreOperandValue will correctly store a value to the identified register.
		*/
		void DecodeMODR_MForRegister(int dataSize,unsigned char MODR_M);
		/*! Decode operand and returns the number of bytes.
		*/
		void DecodeMODR_MForSegmentRegister(unsigned char MODR_M);
		/*! Decode operand for a CR register.
		*/
		void DecodeMODR_MForCRRegister(unsigned char MODR_M);
		/*! Decode operand for a DR register.
		*/
		void DecodeMODR_MForDRRegister(unsigned char MODR_M);
		/*! Decode operand for a Test register.
		*/
		void DecodeMODR_MForTestRegister(unsigned char MODR_M);
		/*! Decode operand and returns the number of bytes.
		*/
		void MakeByRegisterNumber(int dataSize,int regNum);

		/*! Make Simple Offset from segment base.  Like for MOV(A0H,A1H,A2H,A3H)
		    Number of bytes evaluated will be inst.addressSize/8.
		*/
		void MakeSimpleAddressOffsetFromImm(const Instruction &inst);

		/*! Decode FAR address and returns the number of bytes used.
		*/
		unsigned int DecodeFarAddr(int addressSize,int dataSize,const unsigned char operand[]);

		/*! Returns a disassembly of the operand after decoding.
		*/
		std::string Disassemble(void) const;
	private:
		std::string DisassembleAsAddr(void) const;
		std::string DisassembleAsFarAddr(void) const;
		std::string DisassembleAsReg(void) const;
		std::string DisassembleAsImm(int immSize) const;

	public:
		/*! Returns size qualifier if the operand is address.
		*/
		static std::string GetSizeQualifierToDisassembly(const Operand &op,int operandSize);

		static std::string GetSegmentQualifierToDisassembly(int segOverride,const Operand &op);

		/*! Returns the size in number of bytes if the size can be determined.
		    Returns zero otherwise.
		*/
		unsigned int GetSize(void) const;
	};
	/*! OperandValue class is an evaluated operand value, or a value to be stored to
	    the destination described by the operand.
	    In 80486, operand itself may not know its size if it is an address operand.
	    The size is defined by the instruction an the operand size.
	    This OperandValue is after the operand size is evaluated therefore it knows its size.
	*/
	class OperandValue
	{
	public:
		enum
		{
			MAX_NUM_BYTES=10
		};

		unsigned int numBytes;
		unsigned char byteData[MAX_NUM_BYTES];

		/*! Returns a value as a unsigned dword.
		    It won't evaluate beyond numBytes.
		*/
		inline unsigned int GetAsDword(void) const
		{
			static const unsigned int numBytesMask[MAX_NUM_BYTES]=
			{
				0x00000000,
				0x000000FF,
				0x0000FFFF,
				0x00FFFFFF,
				0xFFFFFFFF,
				0xFFFFFFFF,
				0xFFFFFFFF,
				0xFFFFFFFF,
				0xFFFFFFFF,
				0xFFFFFFFF,
			};
			return cpputil::GetDword(byteData)&numBytesMask[numBytes];
		}
		/*! Returns a value as an unsigned byte.
		    It won't evaluate beyond numBytes.
		*/
		inline unsigned int GetAsByte(void) const
		{
			return byteData[0];
		}

		/*! Returns a value as a unsigned wword.
		    It won't evaluate beyond numBytes.
		*/
		inline unsigned int GetAsWord(void) const
		{
			static const unsigned int numBytesMask[MAX_NUM_BYTES]=
			{
				0x00000000,
				0x000000FF,
				0x0000FFFF,
				0x0000FFFF,
				0x0000FFFF,
				0x0000FFFF,
				0x0000FFFF,
				0x0000FFFF,
				0x0000FFFF,
				0x0000FFFF,
			};

			unsigned int word=cpputil::GetWord(byteData);
			word&=numBytesMask[numBytes];
			return word;
		}

		/*! Returns a value as a signed byte, word, or dword.
		*/
		inline int GetAsSignedDword(void) const
		{
			static const long long int ANDptn[]=
			{
				0x0000007F,0x00000080,  // 0 byte
				0x0000007F,0x00000080,  // 1 byte
				0x00007FFF,0x00008000,  // 2 bytes
				0x00007FFF,0x00008000,  // 3 bytes
				0x7FFFFFFF,0x80000000,  // 4 bytes
			};
			long long int uValue=GetAsDword();
			uValue=(uValue&ANDptn[numBytes<<1])-(uValue&ANDptn[(numBytes<<1)+1]);
			return (int)uValue;
		}

		/*! SetDword does not change numBytes.
		    It updates first 4 bytes of the value, but only numBytes will be evaluated.
		    If it needs to be made 4-byte long, use MakeDword instead.
		*/
		inline void SetDword(unsigned int dword)
		{
			cpputil::PutDword(byteData,dword);
		}

		/*! SetWord does not change numBytes.
		    It updates first 2 bytes of the value, but only numBytes will be evaluated.
		    If it needs to be made 4-byte long, use MakeDword instead.
		*/
		inline void SetWord(unsigned int word)
		{
			cpputil::PutWord(byteData,word);
		}

		inline void SetSignedDword(int dword)
		{
			unsigned int uDword;
			// I probably can just say uDword=*((unsigned int *)&dword).
			// But, officially the behavior is undefined by C/C++.
			uDword=(dword&0x7FFFFFFF);
			if(dword<0)
			{
				uDword|=0x80000000;
			}
			SetDword(uDword);
		}

		/*! MakeDword makes a 4-byte long OperandValue.
		    It updates numByte to 4.
		*/
		inline void MakeDword(unsigned int dword)
		{
			numBytes=4;
			cpputil::PutDword(byteData,dword);
		}
		/*! MakeWord makes a 2-byte long OperandValue.
		    It updates numByte to 2.
		*/
		inline void MakeWord(unsigned int word)
		{
			numBytes=2;
			cpputil::PutWord(byteData,word);
		}
		/*! MakeByte makes a 1-byte long OperandValue.
		    It updates numByte to 1.
		*/
		inline void MakeByte(unsigned int word)
		{
			numBytes=1;
			byteData[0]=(word&255);
		}
		/*! Makes a word or dword value.  The size depends on the operandSize. */
		inline void MakeByteWordOrDword(unsigned int operandSize,unsigned int value)
		{
			numBytes=(operandSize>>3);
			cpputil::PutDword(byteData,value);
		}

		/*! Get Segment part of FWORD PTR, which is last two bytes of the byte data.
		*/
		inline unsigned int GetFwordSegment(void) const
		{
			return byteData[numBytes-2]|(byteData[numBytes-1]<<8);
		}
	};


	// Debugger >>
	class CallStack
	{
	public:
		unsigned int CR0;
		unsigned short INTNum,AX;
		unsigned int fromCS,fromEIP;
		unsigned int callOpCodeLength;
		unsigned int procCS,procEIP;
		std::string str;  // File Name for INT 21H AH=3DH and 4BH
	};
	bool enableCallStack;
	std::vector <CallStack> callStack;
	class i486Debugger *debuggerPtr;


	/*! Make a call-stack entry.
	    INTNum and AX are valid only if isInterrupt is true.  Otherwise, give 0xffff for both.
	*/
	inline CallStack MakeCallStack(
	    bool isInterrupt,unsigned short INTNum,unsigned short AX,
	    unsigned int CR0,
	    unsigned int fromCS,unsigned int fromEIP,unsigned int callOpCodeLength,
	    unsigned int procCS,unsigned int procEIP);
	/*! Make a call-stack entry, and push to the call stack.
	    INTNum and AX are valid only if isInterrupt is true.  Otherwise, give 0xffff for both.
	*/
	void PushCallStack(
	    bool isInterrupt,unsigned short INTNum,unsigned short AX,
	    unsigned int CR0,
	    unsigned int fromCS,unsigned int fromEIP,unsigned int callOpCodeLength,
	    unsigned int procCS,unsigned int procEIP,
	    const Memory &mem);
	/*! Pop an entry from call stack.
	    Some interrupt handlers like INT B2H adds SP by 6 at the end to double-IRET from the service routine.
	    To deal with irregular return, it compares return address and pops stack until CS:EIP matches the
	    bottom of the stack.
	*/
	void PopCallStack(unsigned int CS,unsigned int EIP);

	/*! Attaches a debugger.
	*/
	void AttachDebugger(i486Debugger *debugger);

	/*! Detaches a debugger.
	*/
	void DetachDebugger(void);

	// Debugger <<


	State state;

	inline unsigned int GetEAX(void) const
	{
		return state.EAX();
	}
	inline unsigned int GetAX(void) const
	{
		return state.EAX()&0xffff;
	}
	inline unsigned int GetAL(void) const
	{
		return state.EAX()&0xff;
	}
	inline unsigned int GetAH(void) const
	{
		return (state.EAX()>>8)&0xff;
	}
	inline void SetEAX(unsigned int value)
	{
		state.EAX()=value;
	}
	inline void SetAX(unsigned int value)
	{
		state.EAX()&=0xffff0000;
		state.EAX()|=(value&0xFFFF);
	}
	inline void SetAL(unsigned int value)
	{
		state.EAX()&=0xffffff00;
		state.EAX()|=(value&0xFF);
	}
	inline void SetAH(unsigned int value)
	{
		state.EAX()&=0xffff00ff;
		state.EAX()|=((value&0xFF)<<8);
	}


	inline unsigned int GetEBX(void) const
	{
		return state.EBX();
	}
	inline unsigned int GetBX(void) const
	{
		return state.EBX()&0xffff;
	}
	inline unsigned int GetBL(void) const
	{
		return state.EBX()&0xff;
	}
	inline unsigned int GetBH(void) const
	{
		return (state.EBX()>>8)&0xff;
	}
	inline void SetEBX(unsigned int value)
	{
		state.EBX()=value;
	}
	inline void SetBX(unsigned int value)
	{
		state.EBX()&=0xffff0000;
		state.EBX()|=(value&0xFFFF);
	}
	inline void SetBL(unsigned int value)
	{
		state.EBX()&=0xffffff00;
		state.EBX()|=(value&0xFF);
	}
	inline void SetBH(unsigned int value)
	{
		state.EBX()&=0xffff00ff;
		state.EBX()|=((value&0xFF)<<8);
	}


	inline unsigned int GetECX(void) const
	{
		return state.ECX();
	}
	inline unsigned int GetCX(void) const
	{
		return state.ECX()&0xffff;
	}
	inline unsigned int GetCL(void) const
	{
		return state.ECX()&0xff;
	}
	inline unsigned int GetCH(void) const
	{
		return (state.ECX()>>8)&0xff;
	}
	inline void SetECX(unsigned int value)
	{
		state.ECX()=value;
	}
	inline void SetCX(unsigned int value)
	{
		state.ECX()&=0xffff0000;
		state.ECX()|=(value&0xFFFF);
	}
	inline void SetCL(unsigned int value)
	{
		state.ECX()&=0xffffff00;
		state.ECX()|=(value&0xFF);
	}
	inline void SetCH(unsigned int value)
	{
		state.ECX()&=0xffff00ff;
		state.ECX()|=((value&0xFF)<<8);
	}
	inline unsigned int GetCXorECX(unsigned int bits) const
	{
		const static unsigned int mask[2]=
		{
			0x0000FFFF,
			0xFFFFFFFF,
		};
		return GetECX()&mask[bits>>5];
	}
	inline void SetCXorECX(unsigned int bits,unsigned int newECX)
	{
		const static unsigned int mask[2]=
		{
			0x0000FFFF,
			0xFFFFFFFF,
		};
		const static unsigned int ANDptn[2]=
		{
			0xFFFF0000,
			0x00000000
		};
		SetECX((GetECX()&ANDptn[bits>>5])|(newECX&mask[bits>>5]));
	}


	inline unsigned int GetEDX(void) const
	{
		return state.EDX();
	}
	inline unsigned int GetDX(void) const
	{
		return state.EDX()&0xffff;
	}
	inline unsigned int GetDL(void) const
	{
		return state.EDX()&0xff;
	}
	inline unsigned int GetDH(void) const
	{
		return (state.EDX()>>8)&0xff;
	}
	inline void SetEDX(unsigned int value)
	{
		state.EDX()=value;
	}
	inline void SetDX(unsigned int value)
	{
		state.EDX()&=0xffff0000;
		state.EDX()|=(value&0xFFFF);
	}
	inline void SetDL(unsigned int value)
	{
		state.EDX()&=0xffffff00;
		state.EDX()|=(value&0xFF);
	}
	inline void SetDH(unsigned int value)
	{
		state.EDX()&=0xffff00ff;
		state.EDX()|=((value&0xFF)<<8);
	}

	inline unsigned int GetSI(void) const
	{
		return state.ESI()&0xffff;
	}
	inline void SetSI(unsigned int value)
	{
		state.ESI()&=0xffff0000;
		state.ESI()|=(value&0xffff);
	}
	inline unsigned int GetESI(void) const
	{
		return state.ESI();
	}
	inline void SetESI(unsigned int value)
	{
		state.ESI()=value;
	}

	inline unsigned int GetDI(void) const
	{
		return state.EDI()&0xffff;
	}
	inline void SetDI(unsigned int value)
	{
		state.EDI()&=0xffff0000;
		state.EDI()|=(value&0xffff);
	}
	inline unsigned int GetEDI(void) const
	{
		return state.EDI();
	}
	inline void SetEDI(unsigned int value)
	{
		state.EDI()=value;
	}

	inline unsigned int GetSP(void) const
	{
		return state.ESP()&0xffff;
	}
	inline void SetSP(unsigned int value)
	{
		state.ESP()&=0xffff0000;
		state.ESP()|=(value&0xffff);
	}
	inline unsigned int GetESP(void) const
	{
		return state.ESP();
	}
	inline void SetESP(unsigned int value)
	{
		state.ESP()=value;
	}

	inline unsigned int GetBP(void) const
	{
		return state.EBP()&0xffff;
	}
	inline void SetBP(unsigned int value)
	{
		state.EBP()&=0xffff0000;
		state.EBP()|=(value&0xffff);
	}
	inline unsigned int GetEBP(void) const
	{
		return state.EBP();
	}
	inline void SetEBP(unsigned int value)
	{
		state.EBP()=value;
	}

	inline unsigned int GetIP(void) const
	{
		return state.EIP&0xffff;
	}
	inline unsigned int GetEIP(void) const
	{
		return state.EIP;
	}
	inline void SetIP(unsigned int value)
	{
		state.EIP&=0xffff0000;
		state.EIP|=(value&0xffff);
	}
	inline void SetEIP(unsigned int value)
	{
		state.EIP=value;
	}
	inline void SetIPorEIP(unsigned int operandSize,unsigned int value)
	{
		if(16==operandSize)
		{
			SetIP(value);
		}
		else
		{
			SetEIP(value);
		}
	}


	inline void SetFLAGSorEFLAGS(unsigned int operandSize,unsigned int value)
	{
		if(16==operandSize)
		{
			state.EFLAGS=(state.EFLAGS&0xffff0000)|(value&0xffff);
		}
		else
		{
			state.EFLAGS=value;
		}
	}
	inline void SetEFLAGSBit(bool flag,unsigned int bit)
	{
		if(true==flag)
		{
			state.EFLAGS|=bit;
		}
		else
		{
			state.EFLAGS&=(~bit);
		}
	}

	inline bool GetCF(void) const
	{
		return 0!=(state.EFLAGS&EFLAGS_CARRY);
	}
	inline bool GetPF(void) const
	{
		return 0!=(state.EFLAGS&EFLAGS_PARITY);
	}
	inline bool GetAF(void) const
	{
		return 0!=(state.EFLAGS&EFLAGS_AUX_CARRY);
	}
	inline bool GetZF(void) const
	{
		return 0!=(state.EFLAGS&EFLAGS_ZERO);
	}
	inline bool GetSF(void) const
	{
		return 0!=(state.EFLAGS&EFLAGS_SIGN);
	}
	inline bool GetTF(void) const
	{
		return 0!=(state.EFLAGS&EFLAGS_TRAP);
	}
	inline bool GetIF(void) const
	{
		return 0!=(state.EFLAGS&EFLAGS_INT_ENABLE);
	}
	inline bool GetDF(void) const
	{
		return 0!=(state.EFLAGS&EFLAGS_DIRECTION);
	}
	inline bool GetOF(void) const
	{
		return 0!=(state.EFLAGS&EFLAGS_OVERFLOW);
	}
	inline unsigned int GetIOPL(void) const
	{
		return (state.EFLAGS&EFLAGS_IOPL)>>12;
	}
	inline bool GetNT(void) const
	{
		return 0!=(state.EFLAGS&EFLAGS_NESTED);
	}
	inline bool GetRF(void) const
	{
		return 0!=(state.EFLAGS&EFLAGS_RESUME);
	}
	inline bool GetVM(void) const
	{
		return 0!=(state.EFLAGS&EFLAGS_VIRTUAL86);
	}
	inline bool GetAC(void) const
	{
		return 0!=(state.EFLAGS&EFLAGS_ALIGN_CHECK);
	}

	inline void SetCF(bool flag)
	{
		SetEFLAGSBit(flag,EFLAGS_CARRY);
	}
	inline void SetDF(bool flag)
	{
		SetEFLAGSBit(flag,EFLAGS_DIRECTION);
	}
	inline void SetIF(bool flag)
	{
		SetEFLAGSBit(flag,EFLAGS_INT_ENABLE);
	}
	inline void SetOF(bool flag)
	{
		SetEFLAGSBit(flag,EFLAGS_OVERFLOW);
	}
	inline void SetSF(bool flag)
	{
		SetEFLAGSBit(flag,EFLAGS_SIGN);
	}
	inline void SetTF(bool flag)
	{
		SetEFLAGSBit(flag,EFLAGS_TRAP);
	}
	inline void SetZF(bool flag)
	{
		SetEFLAGSBit(flag,EFLAGS_ZERO);
	}
	inline void SetAF(bool flag)
	{
		SetEFLAGSBit(flag,EFLAGS_AUX_CARRY);
	}
	inline void SetPF(bool flag)
	{
		SetEFLAGSBit(flag,EFLAGS_PARITY);
	}


	/*! RaiseFlag functions raises the flag if the parameter is true.
	    But, will never clear the flag.
	*/
	inline void RaiseCF(bool flag)
	{
		if(true==flag)
		{
			state.EFLAGS|=EFLAGS_CARRY;
		}
	}
	inline void RaiseDF(bool flag)
	{
		if(true==flag)
		{
			state.EFLAGS|=EFLAGS_DIRECTION;
		}
	}
	inline void RaiseIF(bool flag)
	{
		if(true==flag)
		{
			state.EFLAGS|=EFLAGS_INT_ENABLE;
		}
	}
	inline void RaiseOF(bool flag)
	{
		if(true==flag)
		{
			state.EFLAGS|=EFLAGS_OVERFLOW;
		}
	}
	inline void RaiseSF(bool flag)
	{
		if(true==flag)
		{
			state.EFLAGS|=EFLAGS_SIGN;
		}
	}
	inline void RaiseTF(bool flag)
	{
		if(true==flag)
		{
			state.EFLAGS|=EFLAGS_TRAP;
		}
	}
	inline void RaiseZF(bool flag)
	{
		if(true==flag)
		{
			state.EFLAGS|=EFLAGS_ZERO;
		}
	}
	inline void RaiseAF(bool flag)
	{
		if(true==flag)
		{
			state.EFLAGS|=EFLAGS_AUX_CARRY;
		}
	}
	inline void RaisePF(bool flag)
	{
		if(true==flag)
		{
			state.EFLAGS|=EFLAGS_PARITY;
		}
	}


	inline void ClearCFOF(void)
	{
		state.EFLAGS&=(~(EFLAGS_CARRY|EFLAGS_OVERFLOW));
	}
	inline void SetCFOF(void)
	{
		state.EFLAGS|=(EFLAGS_CARRY|EFLAGS_OVERFLOW);
	}

	inline bool CheckParity(unsigned char lowByte)
	{
		return ParityTable[lowByte];
		/* int n=0;
		for(int i=0; i<8; ++i)
		{
			if(0!=(lowByte&1))
			{
				++n;
			}
			lowByte>>=1;
		}
		return 0==(n&1); */
	}

	inline bool CondJA(void) const  // Unsigned above
	{
		return true!=GetCF() && true!=GetZF();
	}
	inline bool CondJAE(void) const  // Unsigned above or equal
	{
		return true!=GetCF();
	}
	inline bool CondJB(void) const // Unsigned below
	{
		return GetCF();
	}
	inline bool CondJBE(void) const // Unsgned below or equal
	{
		return GetCF() || GetZF();
	}
	inline bool CondJC(void) const
	{
		return GetCF();
	}
	inline bool CondJCXZ(void) const
	{
		return GetCX()==0;
	}
	inline bool CondJECXZ(void) const
	{
		return GetECX()==0;
	}
	inline bool CondJE(void) const
	{
		return GetZF();
	}
	inline bool CondJG(void) const // Signed greater
	{
		return true!=GetZF() && GetSF()==GetOF();
	}
	inline bool CondJGE(void) const
	{
		return GetSF()==GetOF();
	}
	inline bool CondJL(void) const // Signed less
	{
		return GetSF()!=GetOF();
	}
	inline bool CondJLE(void) const
	{
		return GetZF() || GetSF()!=GetOF();
	}
	inline bool CondJNA(void) const
	{
		return GetCF() || GetZF();
	}
	inline bool CondJNAE(void) const
	{
		return GetCF();
	}
	inline bool CondJNB(void) const
	{
		return true!=GetCF();
	}
	inline bool CondJNBE(void) const
	{
		return true!=GetCF() && true!=GetZF();
	}
	inline bool CondJNC(void) const
	{
		return true!=GetCF();
	}
	inline bool CondJNE(void) const
	{
		return true!=GetZF();
	}
	inline bool CondJNG(void) const
	{
		return GetZF() || GetSF()!=GetOF();
	}
	inline bool CondJNGE(void) const
	{
		return GetSF()!=GetOF();
	}
	inline bool CondJNL(void) const
	{
		return GetSF()==GetOF();
	}
	inline bool CondJNLE(void) const
	{
		return true!=GetZF() && GetSF()==GetOF();
	}
	inline bool CondJNO(void) const
	{
		return true!=GetOF();
	}
	inline bool CondJNP(void) const
	{
		return true!=GetPF();
	}
	inline bool CondJNS(void) const
	{
		return true!=GetSF();
	}
	inline bool CondJNZ(void) const
	{
		return true!=GetZF();
	}
	inline bool CondJO(void) const
	{
		return GetOF();
	}
	inline bool CondJP(void) const
	{
		return GetPF();
	}
	inline bool CondJPE(void) const
	{
		return GetPF();
	}
	inline bool CondJPO(void) const
	{
		return true!=GetPF();
	}
	inline bool CondJS(void) const
	{
		return GetSF();
	}
	inline bool CondJZ(void) const
	{
		return GetZF();
	}

	inline void UpdateDIorEDIAfterStringOp(unsigned int addressSize,unsigned int operandSize)
	{
		if(16==addressSize)
		{
			auto DI=state.EDI();
			if(true==GetDF())
			{
				DI-=operandSize/8;
			}
			else
			{
				DI+=operandSize/8;
			}
			state.EDI()=(state.EDI()&0xffff0000)|(DI&0xffff);
		}
		else
		{
			if(true==GetDF())
			{
				state.EDI()-=operandSize/8;
			}
			else
			{
				state.EDI()+=operandSize/8;
			}
		}
	}
	inline void UpdateSIorESIAfterStringOp(unsigned int addressSize,unsigned int operandSize)
	{
		if(16==addressSize)
		{
			auto SI=state.ESI();
			if(true==GetDF())
			{
				SI-=operandSize/8;
			}
			else
			{
				SI+=operandSize/8;
			}
			state.ESI()=(state.ESI()&0xffff0000)|(SI&0xffff);
		}
		else
		{
			if(true==GetDF())
			{
				state.ESI()-=operandSize/8;
			}
			else
			{
				state.ESI()+=operandSize/8;
			}
		}
	}
	inline void UpdateESIandEDIAfterStringOp(unsigned int addressSize,unsigned int operandSize)
	{
		if(16==addressSize)
		{
			auto DI=state.EDI();
			auto SI=state.ESI();
			if(true==GetDF())
			{
				DI-=operandSize/8;
				SI-=operandSize/8;
			}
			else
			{
				DI+=operandSize/8;
				SI+=operandSize/8;
			}
			state.EDI()=(state.EDI()&0xffff0000)|(DI&0xffff);
			state.ESI()=(state.ESI()&0xffff0000)|(SI&0xffff);
		}
		else
		{
			if(true==GetDF())
			{
				state.EDI()-=operandSize/8;
				state.ESI()-=operandSize/8;
			}
			else
			{
				state.EDI()+=operandSize/8;
				state.ESI()+=operandSize/8;
			}
		}
	}

	/*! Write to Control Register.  If num==3, it builds Page Table cache.
	*/
	inline void SetCR(unsigned int num,unsigned int value)
	{
		state._SetCR(num,value);
		if(3==num)
		{
			Abort("CR3 must be set from SetCR(num,value,mem)");
		}
	}

	/*! Write to Control Register.  If num==3, it builds Page Table cache.
	*/
	inline void SetCR(unsigned int num,unsigned int value,const Memory &mem)
	{
		state._SetCR(num,value);
		if(3==num)
		{
			auto memWin=mem.GetConstMemoryWindow(value&0xFFFFF000);
			if(nullptr!=memWin.ptr)
			{
				state.pageDirectoryCache=memWin.GetReadAccessPointer(value&0xFFFFF000);
			}
			else
			{
				state.pageDirectoryCache.ptr=nullptr;
				state.pageDirectoryCache.length=0;
			}
		}
	}


	/*! Issue an interrupt.
	    This function does not check Interrupt-Enabled Flag (IF),
	    nor check mask state of PIC.
	    This function should be called when it is certain that the interrupt 
	    needs to be handled by the CPU.

		numInstbyte should tell how many bytes that EIP should be displaced for the return address.
		For hardware-interrupt, it should be 1.
		For INT, INTO it should be 2.
		For INT3, it should be 1.
		For interrupt by exception, it should be 0 to allow re-try.
	*/
	inline void Interrupt(unsigned int intNum,Memory &mem,unsigned int numInstBytes);
	inline void IOOut8(InOut &io,unsigned int ioport,unsigned int data);
	inline void IOOut16(InOut &io,unsigned int ioport,unsigned int data);
	inline void IOOut32(InOut &io,unsigned int ioport,unsigned int data);
	inline unsigned int IOIn8(InOut &io,unsigned int ioport);
	inline unsigned int IOIn16(InOut &io,unsigned int ioport);
	inline unsigned int IOIn32(InOut &io,unsigned int ioport);



	virtual const char *DeviceName(void) const{return "486DX";}

	/*! Default constructor.  As you can see.
	*/
	i486DX(VMBase *vmPtr);
private:
	void MakeOpCodeRenumberTable(void);


public:
	/*! Resets the CPU.
	*/
	void Reset(void);

	/*! Returns the state text.
	*/
	std::vector <std::string> GetStateText(void) const;

	/*! Returns the segment register text.
	*/
	std::vector <std::string> GetSegRegText(void) const;

	/*! Returns GDT text.
	*/
	std::vector <std::string> GetGDTText(const Memory &mem) const;

	/*! Returns LDT text.
	*/
	std::vector <std::string> GetLDTText(const Memory &mem) const;

	/*! Returns IDT text.
	*/
	std::vector <std::string> GetIDTText(const Memory &mem) const;

	/*! Return Task Status Segment text.
	*/
	std::vector <std::string> GetTSSText(const Memory &mem) const;

	/*! Print state.
	*/
	void PrintState(void) const;

	/*! Print IDT. */
	void PrintIDT(const Memory &mem) const;

	/*! Print GDT. */
	void PrintGDT(const Memory &mem) const;

	/*! Print LDT. */
	void PrintLDT(const Memory &mem) const;

private:
	class LoadSegmentRegisterClass;
	class DebugLoadSegmentRegisterClass;
	template <class CPUCLASS,class FUNC>
	class LoadSegmentRegisterTemplate;
public:
	enum
	{
		DESC_TYPE_16BIT_CALL_GATE=0x04,
		DESC_TYPE_32BIT_CALL_GATE=0x0C,
	};

	/*! Loads a segment register.
	    If reg is SS, it raise holdIRQ flag.
	    How the segment linear base address is set depends on the CPU mode,
	    and in the protected mode, it needs to look at GDT and LDT.
	    Therefore it needs a reference to memory.

		It returns the upper-4 bytes of the descriptor.  In real mode, it always returns 0xFFFFFFFF.
	*/
	unsigned int LoadSegmentRegister(SegmentRegister &reg,unsigned int value,const Memory &mem);

	/*! It works the same as LoadSegmentRegister function except it takes isInRealMode flag from the outside.

		It returns the upper-4 bytes of the descriptor.  In real mode, it always returns 0xFFFFFFFF.
	*/
	unsigned int LoadSegmentRegister(SegmentRegister &reg,unsigned int value,const Memory &mem,bool isInRealMode);

	/*! Loads a segment register in real mode.
	    If reg is SS, it raise holdIRQ flag.
	*/
	void LoadSegmentRegisterRealMode(SegmentRegister &reg,unsigned int value);

	/*! Get Call Gate.
	    This function may raise exception.
	*/
	FarPointer GetCallGate(unsigned int selector,const Memory &mem);

	/*! Get Call Gate.
	    This function will not raise exception.
	*/
	FarPointer DebugGetCallGate(unsigned int selector,const Memory &mem) const;

	/*! Load Task Register.
	*/
	void LoadTaskRegister(unsigned int value,const Memory &mem);

	/*! Loads a segment register.
	    It does not rely on the current CPU state, instead isInRealMode is given as a parameter.
	    Even if reg==SS, it does not update holdIRQ flag.

		It returns the upper-4 bytes of the descriptor.  In real mode, it always returns 0xFFFFFFFF.
	*/
	unsigned int DebugLoadSegmentRegister(SegmentRegister &reg,unsigned int value,const Memory &mem,bool isInRealMode) const;

	/*! Loads limit and linear base address to a descriptor table register.
	    How many bytes are loaded depends on operand size.  [1] 26-194.
	*/
	void LoadDescriptorTableRegister(SystemAddressRegister &reg,int operandSize,const unsigned char byteData[]);

	/*! Retrieve Interrupt Descriptor from the current IDT.
	    It could raise page fault.
	*/
	InterruptDescriptor GetInterruptDescriptor(unsigned int INTNum,const Memory &mem);

	/*! Retrieve Interrupt Descriptor from the current IDT.
	    This will not change the CPU state including exceptions.
	*/
	InterruptDescriptor DebugGetInterruptDescriptor(unsigned int INTNum,const Memory &mem) const;

	/*! Make an OperandValue from a descriptor table register.
	*/
	OperandValue DescriptorTableToOperandValue(const SystemAddressRegister &reg,int operandSize) const;

	inline bool IsInRealMode(void) const
	{
		return (0==(state.GetCR(0)&CR0_PROTECTION_ENABLE));
	}
	inline unsigned int Return0InRealMode1InProtectedMode(void) const
	{
		// CR0_PROTECTION_ENABLE is 1.
		return state.GetCR(0)&CR0_PROTECTION_ENABLE;
	}

	/*! Returns a register value. 
	*/
	inline unsigned int GetRegisterValue(int reg) const;

	/*! Returns an 8-bit register value.
	    May crash if reg is not REG_AL,REG_BL,REG_CL,REG_DL,REG_AH,REG_BH,REG_CH,REG_DH.
	*/
	inline unsigned int GetRegisterValue8(int reg) const;

	/*! Sets a register value.
	    reg can be conventional registers only.
	    Trying to set a value to other registers with this function will Abort the VM.
	*/
	inline void SetRegisterValue(unsigned int reg,unsigned int value);

	/*! Sets an 8-bit register value.
	    May crash if reg is not REG_AL,REG_BL,REG_CL,REG_DL,REG_AH,REG_BH,REG_CH,REG_DH. */
	inline void SetRegisterValue8(unsigned int reg,unsigned char value);

	/*! Returns a register size in number of bytes. 
	*/
	static unsigned int GetRegisterSize(int reg);

	/*! Returns a mask for addressing.
	    If it is in real mode or 16-bit addressing, it returns 0xFFFF.
	    If it is in 32-bit addressing, it returns 0xFFFFFFFF.
	*/
	inline unsigned int AddressMask(unsigned char addressSize) const
	{
		// Segment limit may be beyond 64K in real mode.  According to https://wiki.osdev.org/Unreal_Mode
		// See also LoadSegmentRegisterRealMode and LoadSegmentRegister
		//if(true==IsInRealMode())
		//{
		//	return 0xFFFF;
		//}
		unsigned int mask=(1<<(addressSize-1));
		return mask|(mask-1);
	}


	/*! Returns the addressing size (16 or 32) of the stack segment.
	*/
	inline unsigned int GetStackAddressingSize(void) const
	{
		return *stackAddressSizePointer[Return0InRealMode1InProtectedMode()];
	}
private:
	unsigned int sixteen=16;
	/* Set in the constructore.  stackAddressSizePointer[0] points to sixteen.
	   stackAddressSizePointer[1] points to SS.addressSize.
	*/
	unsigned int *stackAddressSizePointer[2];

public:
	/*! Returns true if Paging is enabled.
	*/
	inline bool PagingEnabled(void) const
	{
		return 0!=(state.GetCR(0)&CR0_PAGING_ENABLED);
	}

	/*! Returns true if the opCode needs one more byte to be fully qualified.
	*/
	inline static bool OpCodeNeedsOneMoreByte(unsigned int firstByte)
	{
		return (firstByte==I486_OPCODE_NEED_SECOND_BYTE);
	}

	/*!
	*/
	inline unsigned long LinearAddressToPhysicalAddress(
	    unsigned int &exceptionType,unsigned int &exceptionCode,unsigned int linearAddr,const Memory &mem) const
	{
		unsigned int pageDirectoryIndex=((linearAddr>>22)&1023);
		unsigned int pageTableIndex=((linearAddr>>12)&1023);
		unsigned int offset=(linearAddr&4095);

		unsigned int pageTableInfo;
		if(4096==state.pageDirectoryCache.length)
		{
			const auto ptr=state.pageDirectoryCache.ptr+(pageDirectoryIndex<<2);
			pageTableInfo=cpputil::GetDword(ptr);
		}
		else
		{
			const unsigned int pageDirectoryPtr=state.GetCR(3)&0xFFFFF000;
			pageTableInfo=mem.FetchDword(pageDirectoryPtr+(pageDirectoryIndex<<2));
		}
		if(0==(pageTableInfo&1))
		{
			exceptionType=EXCEPTION_PF;
			exceptionCode=0;
			return 0;
		}

		const unsigned int pageTablePtr=(pageTableInfo&0xFFFFF000);
		unsigned int pageInfo=mem.FetchDword(pageTablePtr+(pageTableIndex<<2));
		if(0==(pageInfo&1))
		{
			exceptionType=EXCEPTION_PF;
			exceptionCode=0;
			return 0;
		}

		auto physicalAddr=(pageInfo&0xFFFFF000)+offset;
		exceptionType=EXCEPTION_NONE;
		return physicalAddr;
	}

	/*! Convert physicall address to the first-matching linear address.
	    For debugging purpose only.
	*/
	unsigned int PhysicalAddressToLinearAddress(unsigned physAddr,const Memory &mem) const;

	/*!
	*/
	inline unsigned long LinearAddressToPhysicalAddress(unsigned int linearAddr,const Memory &mem)
	{
		unsigned int exceptionType,exceptionCode;
		auto physicalAddr=LinearAddressToPhysicalAddress(exceptionType,exceptionCode,linearAddr,mem);
		if(EXCEPTION_NONE!=exceptionType)
		{
			RaiseException(exceptionType,exceptionCode);
			return 0;
		}
		return physicalAddr;
	}


	/*! Push a value.
	*/
	void Push(Memory &mem,unsigned int operandSize,unsigned int value);

	/*! Pop a value.
	*/
	unsigned int Pop(Memory &mem,unsigned int operandSize);


	/*! Fetch a byte. 
	*/
	inline unsigned int FetchByte(unsigned int addressSize,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
	{
		offset&=AddressMask((unsigned char)addressSize);
		auto addr=seg.baseLinearAddr+offset;

		if(seg.limit<offset)
		{
			RaiseException(EXCEPTION_GP,0);
			return 0;
		}

		if(&seg==&state.SS() && 
		   nullptr!=state.SSESPWindow.ptr &&
		   true==state.SSESPWindow.IsLinearAddressInRange(addr))
		{
			return state.SSESPWindow.ptr[addr&(MemoryAccess::MEMORY_WINDOW_SIZE-1)];
		}

		if(true==PagingEnabled())
		{
			addr=LinearAddressToPhysicalAddress(addr,mem);
		}
		return mem.FetchByte(addr);
	}

	/*! Fetch a dword.
	*/
	inline unsigned int FetchWord(unsigned int addressSize,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
	{
		offset&=AddressMask((unsigned char)addressSize);
		auto addr=seg.baseLinearAddr+offset;

		if(seg.limit<offset+1)
		{
			RaiseException(EXCEPTION_GP,0);
			return 0;
		}

		if(&seg==&state.SS() && 
		   nullptr!=state.SSESPWindow.ptr &&
		   true==state.SSESPWindow.IsLinearAddressInRange(addr))
		{
			unsigned int low12bits=(addr&(MemoryAccess::MEMORY_WINDOW_SIZE-1));
			if(low12bits<=MemoryAccess::MEMORY_WINDOW_SIZE-2)
			{
				return cpputil::GetWord(state.SSESPWindow.ptr+low12bits);
			}
		}

		if(true==PagingEnabled())
		{
			addr=LinearAddressToPhysicalAddress(addr,mem);
			if(0xFFC<(addr&0xfff)) // May hit the page boundary
			{
				return FetchByte(addressSize,seg,offset,mem)|(FetchByte(addressSize,seg,offset+1,mem)<<8);
			}
		}
		return mem.FetchWord(addr);
	}

	/*! Fetch a dword.
	*/
	inline unsigned int FetchDword(unsigned int addressSize,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
	{
		offset&=AddressMask((unsigned char)addressSize);
		auto addr=seg.baseLinearAddr+offset;

		if(seg.limit<offset+3)
		{
			RaiseException(EXCEPTION_GP,0);
			return 0;
		}

		if(&seg==&state.SS() && 
		   nullptr!=state.SSESPWindow.ptr &&
		   true==state.SSESPWindow.IsLinearAddressInRange(addr))
		{
			unsigned int low12bits=(addr&(MemoryAccess::MEMORY_WINDOW_SIZE-1));
			if(low12bits<=MemoryAccess::MEMORY_WINDOW_SIZE-4)
			{
				return cpputil::GetDword(state.SSESPWindow.ptr+low12bits);
			}
		}

		if(true==PagingEnabled())
		{
			addr=LinearAddressToPhysicalAddress(addr,mem);
			if(0xFF8<(addr&0xfff)) // May hit the page boundary
			{
				return 
				     FetchByte(addressSize,seg,offset,mem)
				   |(FetchByte(addressSize,seg,offset+1,mem)<<8)
				   |(FetchByte(addressSize,seg,offset+2,mem)<<16)
				   |(FetchByte(addressSize,seg,offset+3,mem)<<24);
			}
		}
		return mem.FetchDword(addr);
	}
	/*! Fetch a byte, word, or dword.
	    Function name is left as FetchWordOrDword temporarily for the time being.
	    Will be unified to FetchByteWordOrDword in the future.
	*/
	inline unsigned int FetchWordOrDword(unsigned int operandSize,unsigned int addressSize,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
	{
		switch(operandSize)
		{
		case 8:
			return FetchByte(addressSize,seg,offset,mem);
		case 16:
			return FetchWord(addressSize,seg,offset,mem);
		default:
		case 32:
			return FetchDword(addressSize,seg,offset,mem);
		}
	}
	inline unsigned int FetchByteWordOrDword(unsigned int operandSize,unsigned int addressSize,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
	{
		return FetchWordOrDword(operandSize,addressSize,seg,offset,mem);
	}

	/*! Fetch a byte by linear address.
	*/
	inline unsigned int FetchByteByLinearAddress(const Memory &mem,unsigned int linearAddr)
	{
		if(true==PagingEnabled())
		{
			linearAddr=LinearAddressToPhysicalAddress(linearAddr,mem);
		}
		return mem.FetchByte(linearAddr);
	}

	/*! Returns const memory window from SEG:OFFSET.
	    This may raise page fault depending on SEG:OFFSET.
	*/
	inline MemoryAccess::ConstMemoryWindow GetConstMemoryWindow(unsigned int addressSize,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
	{
		offset&=AddressMask((unsigned char)addressSize);
		auto linearAddr=seg.baseLinearAddr+offset;
		return GetConstMemoryWindowFromLinearAddress(linearAddr,mem);
	}

	/*! Returns const memory window from a linear address.
	    This may raise page fault depending on SEG:OFFSET.
	*/
	inline MemoryAccess::ConstMemoryWindow GetConstMemoryWindowFromLinearAddress(unsigned int linearAddr,const Memory &mem)
	{
		auto physAddr=linearAddr;
		if(true==PagingEnabled())
		{
			physAddr=LinearAddressToPhysicalAddress(linearAddr,mem);
		}
		auto memWin=mem.GetConstMemoryWindow(physAddr);
		memWin.linearBaseAddr=(linearAddr&(~0xfff));
		return memWin;
	}

	/*! Returns const memory window from SEG:OFFSET.
	    It will not change the state of the CPU including exceptions.
	*/
	inline MemoryAccess::ConstMemoryWindow DebugGetConstMemoryWindow(unsigned int addressSize,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const
	{
		offset&=AddressMask((unsigned char)addressSize);
		auto linearAddr=seg.baseLinearAddr+offset;
		return DebugGetConstMemoryWindowFromLinearAddress(linearAddr,mem);
	}

	/*! Returns const memory window from a linear address.
	    It will not change the state of the CPU including exceptions.
	*/
	inline MemoryAccess::ConstMemoryWindow DebugGetConstMemoryWindowFromLinearAddress(unsigned int linearAddr,const Memory &mem) const
	{
		auto physAddr=linearAddr;
		if(true==PagingEnabled())
		{
			unsigned int type,code;
			physAddr=LinearAddressToPhysicalAddress(type,code,linearAddr,mem);
		}
		auto memWin=mem.GetConstMemoryWindow(physAddr);
		memWin.linearBaseAddr=(linearAddr&(~0xfff));
		return memWin;
	}

	/*! Store a byte.
	*/
	inline void StoreByte(Memory &mem,int addressSize,SegmentRegister seg,unsigned int offset,unsigned char data);

	/*! Store a word.
	*/
	inline void StoreWord(Memory &mem,int addressSize,SegmentRegister seg,unsigned int offset,unsigned int data);

	/*! Store a dword.
	*/
	inline void StoreDword(Memory &mem,int addressSize,SegmentRegister seg,unsigned int offset,unsigned int data);



	/*! Store a word or dword.  Operand size must be 16 or 32.
	*/
	inline void StoreWordOrDword(Memory &mem,unsigned int operandSize,int addressSize,SegmentRegister seg,unsigned int offset,unsigned int data);

	/*! Fetch a byte for debugger.  It won't change exception status.
	*/
	unsigned int DebugFetchByte(unsigned int addressSize,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const;
	/*! Fetch a dword.  It won't change exception status.
	*/
	unsigned int DebugFetchWord(unsigned int addressSize,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const;
	/*! Fetch a dword for debugging.  It won't change exception status.
	*/
	unsigned int DebugFetchDword(unsigned int addressSize,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const;
	unsigned int DebugFetchWordOrDword(unsigned int operandSize,unsigned int addressSize,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const;
	unsigned int DebugFetchByteWordOrDword(unsigned int operandSize,unsigned int addressSize,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const;
	unsigned int DebugFetchByteByLinearAddress(const Memory &mem,unsigned int linearAddr) const;
	void DebugStoreByte(Memory &mem,int addressSize,SegmentRegister seg,unsigned int offset,unsigned char data);
	void DebugStoreWord(Memory &mem,int addressSize,SegmentRegister seg,unsigned int offset,unsigned int data);
	void DebugStoreDword(Memory &mem,int addressSize,SegmentRegister seg,unsigned int offset,unsigned int data);


	/*! Fetch a C-string from the given address
	*/
	std::string DebugFetchString(int addressSize,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const;




	/*! Fetch a byte from CS:[EIP+offset].
	*/
	inline unsigned int FetchInstructionByte(unsigned int offset,const Memory &mem)
	{
		return FetchByte(state.CS().addressSize,state.CS(),state.EIP+offset,mem);
	}

	/*! Fetch an instruction.
	    This function may raise page fault.
	*/
	inline void FetchInstruction(MemoryAccess::ConstMemoryWindow &memWin,Instruction &inst,Operand &op1,Operand &op2,const Memory &mem)
	{
		return FetchInstruction(memWin,inst,op1,op2,state.CS(),state.EIP,mem);
	}
	/*! Fetch an instruction.
	    It will not affect the CPU state.
	*/
	inline void DebugFetchInstruction(MemoryAccess::ConstMemoryWindow &memWin,Instruction &inst,Operand &op1,Operand &op2,const Memory &mem) const
	{
		return DebugFetchInstruction(memWin,inst,op1,op2,state.CS(),state.EIP,mem);
	}


	/*! Raise an exception. 
	*/
	void RaiseException(int type,int code)
	{
		state.exception=true;
		state.exceptionType=type;
		state.exceptionCode=code;
	}

	enum
	{
		INT_DIVISION_BY_ZERO=0,
		INT_DEBUG_EXCEPTION=1,
		INT_NMI=2,
		INT_BREAKPOINT=3,
		INT_INTO_OVERFLOW=4,
		INT_BOUND_RANGE_EXCEEDED=5,
		INT_INVALID_OPCODE=6,
		INT_DEVICE_NOT_AVAILABLE=7,
		INT_DOUBLE_FAULT=8,
		INT_INVALID_TASK_STATE_SEGMENT=10,
		INT_SEGMENT_NOT_PRESENT=11,
		INT_STACK_FAULT=12,
		INT_GENERAL_PROTECTION=13,
		INT_PAGE_FAULT=14,
		INT_FLOATING_POINT_ERROR=16,
		INT_ALIGNMENT_CHECK=17
	};

	/*! Jump to an appropriate exception handler.
	    Also clear state.exception.
	*/
	void HandleException(bool wasReadOp,Memory &mem);

	/*! 80386 and 80486 apparently accepts REPNE in place for REP is used for INS,MOVS,OUTS,LODS,STOS.
	    This inline function just make REPNE work as REP.
	*/
	inline unsigned int REPNEtoREP(unsigned int prefix)
	{
		// If prefix is 0xF2, it should be taken as 0xF3.
		// If prefix is 0xF0, it shouldn't be taken as REP or REPNE.
		return prefix|1;
	}


	/*! Check for REP.  Execute a string operation if the return value is true. 
	    It returns true if no REP prefix.
	*/
	bool REPCheck(unsigned int &clocksForRep,unsigned int instPrefix,unsigned int addressSize);

	/*! Check for REPE or REPNE.
	    It returns true if the operation should be continued to the next iteration.
	    This check must be after performing a string operation because [1] pp.26-246
	    indicates that REPE and REPNE must be do-while loop.  (Counter should be checked before the operation)
        Therefore, REPCheck is used before the operation, and REPEorNECheck after.
	*/
	bool REPEorNECheck(unsigned int &clocksForRep,unsigned int instPrefix,unsigned int addressSize);



private:
	/*! Fetch an 8-bit operand.  Returns the number of bytes fetched.
	    It pushes inst.operandLen and this->numBytes by 1 byte.
	*/
	inline void FetchOperand8(Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem);
	/*! Peek an 8-bit operand.  Returns the number of bytes fetched.
	    It does not push inst.operandLen and this->numBytes by 1 byte.
	*/
	inline void PeekOperand8(unsigned int &operand,const Instruction &inst,const MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem);
	/*! Fetch an 16-bit operand  Returns the number of bytes fetched..
	*/
	inline void FetchOperand16(Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem);
	/*! Fetch an 32-bit operand.  Returns the number of bytes fetched.
	*/
	inline void FetchOperand32(Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem);
	/*! Fetch an 16- or 32-bit operand.  Length fetched depends on inst.operandSize.
	    Returns the number of bytes fetched.
	*/
	inline unsigned int FetchOperand16or32(Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem);


	/*! Fetch an 8-bit operand.  Returns the number of bytes fetched.
	    It pushes inst.operandLen and this->numBytes by 1 byte.
	*/
	inline void DebugFetchOperand8(Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const;
	/*! Peek an 8-bit operand.  Returns the number of bytes fetched.
	    It does not push inst.operandLen and this->numBytes by 1 byte.
	*/
	inline void DebugPeekOperand8(unsigned int &operand,const Instruction &inst,const MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const;
	/*! Fetch an 16-bit operand  Returns the number of bytes fetched..
	*/
	inline void DebugFetchOperand16(Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const;
	/*! Fetch an 32-bit operand.  Returns the number of bytes fetched.
	*/
	inline void DebugFetchOperand32(Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const;
	/*! Fetch an 16- or 32-bit operand.  Length fetched depends on inst.operandSize.
	    Returns the number of bytes fetched.
	*/
	inline unsigned int DebugFetchOperand16or32(Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const;



	/*! Fetch an operand defined by the RM byte.
	    Returns the number of bytes fetched.
	*/
	template <class CPUCLASS,class FUNCCLASS>
	inline static unsigned int FetchOperandRM(CPUCLASS &cpu,Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem);


	/*! Fetch an 8-bit operand.
	    It pushes inst.operandLen and this->numBytes by 1 byte.
	*/
	inline void FetchImm8(Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem);
	/*! Fetch an 16-bit operand.
	*/
	inline void FetchImm16(Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem);
	/*! Fetch an 32-bit operand.
	*/
	inline void FetchImm32(Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem);
	/*! Fetch an 16- or 32-bit operand.  Length fetched depends on inst.operandSize.
	    Returns the number of bytes fetched.
	*/
	inline unsigned int FetchImm16or32(Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem);

	/*! Fetch an 8-bit operand.
	    It pushes inst.operandLen and this->numBytes by 1 byte.
	*/
	inline void DebugFetchImm8(Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const;
	/*! Fetch an 16-bit operand.
	*/
	inline void DebugFetchImm16(Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const;
	/*! Fetch an 32-bit operand.
	*/
	inline void DebugFetchImm32(Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const;
	/*! Fetch an 16- or 32-bit operand.  Length fetched depends on inst.operandSize.
	    Returns the number of bytes fetched.
	*/
	inline unsigned int DebugFetchImm16or32(Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const;



private:
	/*! Made public for disassembly test.
	    Fetch operand(s) for the instruction.
	The following instruction:
	    I486_OPCODE_MOV_TO_CR://        0x220F,
	    I486_OPCODE_MOV_FROM_CR://      0x200F,
	    I486_OPCODE_MOV_FROM_DR://      0x210F,
	    I486_OPCODE_MOV_TO_DR://        0x230F,
	    I486_OPCODE_MOV_FROM_TR://      0x240F,
	    I486_OPCODE_MOV_TO_TR://        0x260F,
	always set 32 to inst.operandSize regardless of the preceding operand-size override, or default operand size.

	    Made a template function to make const-version (for debugging) and non-const version (for real)
	    with minimum code duplicate.
	*/
	template <class CPUCLASS,class FUNCCLASS>
	static void FetchOperand(CPUCLASS &cpu,Instruction &inst,Operand &op1,Operand &op2,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,int offset,const Memory &mem);

public:
	/*! Fetch an instruction from specific segment and offset.
	*/
	inline void FetchInstruction(MemoryAccess::ConstMemoryWindow &memWin,Instruction &inst,Operand &op1,Operand &op2,const SegmentRegister &CS,unsigned int offset,const Memory &mem)
	{
		const unsigned int operandSize=*CSOperandSizePointer[Return0InRealMode1InProtectedMode()];
		const unsigned int addressSize=*CSAddressSizePointer[Return0InRealMode1InProtectedMode()];
		return FetchInstruction(memWin,inst,op1,op2,CS,offset,mem,operandSize,addressSize);
	}
	/*! Fetch an instruction from specific segment and offset.
	    It will not affect the CPU state including exceptions.
	*/
	inline void DebugFetchInstruction(MemoryAccess::ConstMemoryWindow &memWin,Instruction &inst,Operand &op1,Operand &op2,const SegmentRegister &CS,unsigned int offset,const Memory &mem) const
	{
		const unsigned int operandSize=*CSOperandSizePointer[Return0InRealMode1InProtectedMode()];
		const unsigned int addressSize=*CSAddressSizePointer[Return0InRealMode1InProtectedMode()];
		return DebugFetchInstruction(memWin,inst,op1,op2,CS,offset,mem,operandSize,addressSize);
	}
private:
	class DebugFetchInstructionFunctions;
	class RealFetchInstructionFunctions;
	template <class CPUCLASS,class FUNCCLASS>
	class FetchInstructionClass;
private:
	/* Set in the constructor. CS******SizePointer[0] points to sixteen, and
	   CS******SizePointer[1] points to CS.******Size.
	*/
	unsigned int *CSOperandSizePointer[2];
	unsigned int *CSAddressSizePointer[2];

public:
	/*! Fetch an instruction from specific segment and offset with given default operand size and address size.
	*/
	void FetchInstruction(
	    MemoryAccess::ConstMemoryWindow &memWin,
	    Instruction &inst,Operand &op1,Operand &op2,
	    const SegmentRegister &CS,unsigned int offset,const Memory &mem,unsigned int defOperSize,unsigned int defAddrSize);

	/*! Fetch an instruction from specific segment and offset with given default operand size and address size.
	    It will not affect the CPU status including exceptions.
	*/
	void DebugFetchInstruction(
	    MemoryAccess::ConstMemoryWindow &memWin,
	    Instruction &inst,Operand &op1,Operand &op2,
	    const SegmentRegister &CS,unsigned int offset,const Memory &mem,unsigned int defOperSize,unsigned int defAddrSize) const;

private:
	inline unsigned int FetchInstructionByte(MemoryAccess::ConstPointer &ptr,unsigned int addressSize,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
	{
		if(0<ptr.length)
		{
			return ptr.FetchByte();
		}
		else
		{
			return FetchByte(addressSize,seg,offset,mem);
		}
	}
	inline unsigned int PeekInstructionByte(const MemoryAccess::ConstPointer &ptr,unsigned int addressSize,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
	{
		if(0<ptr.length)
		{
			return ptr.PeekByte();
		}
		else
		{
			return FetchByte(addressSize,seg,offset,mem);
		}
	}
	inline void FetchInstructionTwoBytes(unsigned char buf[2],MemoryAccess::ConstPointer &ptr,unsigned int addressSize,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
	{
		if(2<=ptr.length)
		{
			ptr.FetchTwoBytes(buf);
		}
		else
		{
			// See comment in FetchInstructionFourBytes for why ptr.length needs to be cleared.
			ptr.length=0;
			buf[0]=FetchByte(addressSize,seg,offset  ,mem);
			buf[1]=FetchByte(addressSize,seg,offset+1,mem);
		}
	}
	inline void FetchInstructionFourBytes(unsigned char buf[4],MemoryAccess::ConstPointer &ptr,unsigned int addressSize,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
	{
		if(4<=ptr.length)
		{
			ptr.FetchFourBytes(buf);
		}
		else
		{
			// ptr.length should be cleared.
			// Or, the following instruction:
			//   000C:0000BFFA 66C705F85700001300        MOV     WORD PTR [000057F8H],0013H
			// was interpreted as:
			//   000C:0000BFFA 66C705F85700001300        MOV     WORD PTR [000057F8H],57F8H
			// Why?  At CS:EIP==000C:BFFAH, ConstPointer has 7 more bytes to go, but it runs out
			// before reading 000057F8.  So, ConstPointer was leaving 3 bytes when the
			// address offset was fetched.  Then, subsequent FetchInstructionTwoBytes saw
			// ConstPointer having 3 more bytes, and took from the ConstPointer.
			// This problem can be avoided by clearing ptr.length.
			// It happens only at the 4KB border, therefore little performance penalty.
			ptr.length=0;
			buf[0]=FetchByte(addressSize,seg,offset  ,mem);
			buf[1]=FetchByte(addressSize,seg,offset+1,mem);
			buf[2]=FetchByte(addressSize,seg,offset+2,mem);
			buf[3]=FetchByte(addressSize,seg,offset+3,mem);
		}
	}

public:
	/*! Make a disassembly.
	*/
	std::string Disassemble(const Instruction &inst,const Operand &op1,const Operand &op2,SegmentRegister seg,unsigned int offset,const Memory &mem,const class i486SymbolTable &symTable,const std::map <unsigned int,std::string> &ioTable) const;

	/*! Make a data line for disassembly.
	    When it reaches chopOff, the rest will be shown as :.
	*/
	std::string DisassembleData(unsigned int addressSize,SegmentRegister seg,unsigned int offset,const Memory &mem,unsigned int unitBytes,unsigned int segBytes,unsigned int repeat,unsigned int chopOff) const;

	/*! Get 8-bit register name from MODR_M. */
	static std::string Get8BitRegisterNameFromMODR_M(unsigned char MOD_RM);
	/*! Get 16-bit register name from MODR_M. */
	static std::string Get16BitRegisterNameFromMODR_M(unsigned char MOD_RM);
	/*! Get 32-bit register name from MODR_M. */
	static std::string Get32BitRegisterNameFromMODR_M(unsigned char MOD_RM);
	/*! Get 32-bit register name from MODR_M. */
	static std::string Get16or32BitRegisterNameFromMODR_M(int dataSize,unsigned char MOD_RM);


	/*! Run one instruction and returns number of clocks. */
	unsigned int RunOneInstruction(Memory &mem,InOut &io);


	/*! Move source operand to destination operand.  
	    If the destination operand is a segment register, it updates baseLinearAddr.
	    If the destination operand is SS, it sets holdIRQ flag.
	*/
	void Move(Memory &mem,int addressMode,int segmentOverride,const Operand &dst,const Operand &src);


	/*! Decrement a value.  It also sets OF SF ZF AF PF according to the result.
	    operandSize needs to be 16 or 32.
	*/
	void DecrementWordOrDword(unsigned int operandSize,unsigned int &value);
	void DecrementDword(unsigned int &value);
	void DecrementWord(unsigned int &value);
	void DecrementByte(unsigned int &value);
	void DecrementWithMask(unsigned int &value,unsigned int mask,unsigned int signBit);


	/*! Increment a value.  It also sets OF SF ZF AF PF according to the result.
	    operandSize needs to be 16 or 32.
	*/
	void IncrementWordOrDword(unsigned int operandSize,unsigned int &value);
	void IncrementDword(unsigned int &value);
	void IncrementWord(unsigned int &value);
	void IncrementByte(unsigned int &value);
	void IncrementWithMask(unsigned int &value,unsigned int mask,unsigned int signBit);


	/*! Add a value.  OF,SF,ZF,AF,CF, and PF flags are set accoring to the result.
	    operandSize needs to be 16 or 32.
	*/ 
	void AddWordOrDword(int operandSize,unsigned int &value1,unsigned int value2);
	void AddDword(unsigned int &value1,unsigned int value2);
	void AddWord(unsigned int &value1,unsigned int value2);
	void AddByte(unsigned int &value1,unsigned int value2);
	/*! Bitwise-And a value.  It also clears CF and OF, and sets SF,ZF, and PF according to the result.
	    operandSize needs to be 16 or 32.
	*/ 
	void AndWordOrDword(int operandSize,unsigned int &value1,unsigned int value2);
	void AndDword(unsigned int &value1,unsigned int value2);
	void AndWord(unsigned int &value1,unsigned int value2);
	void AndByte(unsigned int &value1,unsigned int value2);
	/*! Add a value + carry flag.  OF,SF,ZF,AF,CF, and PF flags are set accoring to the result.
	    operandSize needs to be 16 or 32.
	*/ 
	void AdcWordOrDword(int operandSize,unsigned int &value1,unsigned int value2);
	void AdcDword(unsigned int &value1,unsigned int value2);
	void AdcWord(unsigned int &value1,unsigned int value2);
	void AdcByte(unsigned int &value1,unsigned int value2);
	/*! Subtract (a value + carry flag).  OF,SF,ZF,AF,CF, and PF flags are set accoring to the result.
	    operandSize needs to be 16 or 32.
	*/ 
	void SbbWordOrDword(int operandSize,unsigned int &value1,unsigned int value2);
	void SbbDword(unsigned int &value1,unsigned int value2);
	void SbbWord(unsigned int &value1,unsigned int value2);
	void SbbByte(unsigned int &value1,unsigned int value2);
	/*! Bitwise-Or a value.  It also clears CF and OF, and sets SF,ZF, and PF according to the result.
	    operandSize needs to be 16 or 32.
	*/ 
	void OrWordOrDword(int operandSize,unsigned int &value1,unsigned int value2);
	void OrDword(unsigned int &value1,unsigned int value2);
	void OrWord(unsigned int &value1,unsigned int value2);
	void OrByte(unsigned int &value1,unsigned int value2);
	/*! Subtract a value.  OF,SF,ZF,AF,CF, and PF flags are set accoring to the result.
	    operandSize needs to be 8, 16, or 32.
	*/ 
	void SubByteWordOrDword(int operandSize,unsigned int &value1,unsigned int value2);
	void SubWordOrDword(int operandSize,unsigned int &value1,unsigned int value2);
	void SubDword(unsigned int &value1,unsigned int value2);
	void SubWord(unsigned int &value1,unsigned int value2);
	void SubByte(unsigned int &value1,unsigned int value2);
	/*! Bitwise-Xor a value.  It also clears CF and OF, and sets SF,ZF, and PF according to the result.
	    operandSize needs to be 16 or 32.
	*/ 
	void XorWordOrDword(int operandSize,unsigned int &value1,unsigned int value2);
	void XorDword(unsigned int &value1,unsigned int value2);
	void XorWord(unsigned int &value1,unsigned int value2);
	void XorByte(unsigned int &value1,unsigned int value2);

private:
	template <unsigned int valueMask,unsigned int countMask,unsigned int bitLength,unsigned int signBit>
	inline void RolTemplate(unsigned int &value,unsigned int ctr);
public:
	/*! ROL a value and set OF and CF flags accoring to the result.
	    OF is only set if ctr==1.
	    operandSize needs to be 16 or 32.
	*/ 
	void RolByteWordOrDword(int operandSize,unsigned int &value,unsigned int ctr);
	void RolDword(unsigned int &value,unsigned int ctr);
	void RolWord(unsigned int &value,unsigned int ctr);
	void RolByte(unsigned int &value,unsigned int ctr);

private:
	template <unsigned int bitCount,unsigned int allBits,unsigned int signBit>
	inline void RorTemplate(unsigned int &value,unsigned int ctr);
public:
	/*! ROR a value and set OF and CF flags accoring to the result.
	    OF is only set if ctr==1.
	    operandSize needs to be 16 or 32.
	*/ 
	void RorByteWordOrDword(int operandSize,unsigned int &value,unsigned int ctr);
	void RorDword(unsigned int &value,unsigned int ctr);
	void RorWord(unsigned int &value,unsigned int ctr);
	void RorByte(unsigned int &value,unsigned int ctr);

	/*! RCL a value and set OF and CF flags accoring to the result.
	    OF is only set if ctr==1.
	    operandSize needs to be 16 or 32.
	*/ 
	void RclWordOrDword(int operandSize,unsigned int &value,unsigned int ctr);
	void RclDword(unsigned int &value,unsigned int ctr);
	void RclWord(unsigned int &value,unsigned int ctr);
	void RclByte(unsigned int &value,unsigned int ctr);

	/*! RCR a value and set OF and CF flags accoring to the result.
	    OF is only set if ctr==1.
	    operandSize needs to be 16 or 32.
	*/ 
	void RcrWordOrDword(int operandSize,unsigned int &value,unsigned int ctr);
	void RcrDword(unsigned int &value,unsigned int ctr);
	void RcrWord(unsigned int &value,unsigned int ctr);
	void RcrByte(unsigned int &value,unsigned int ctr);

	/*! SAR a value and set OF and CF flags accoring to the result.
	    OF is only set if ctr==1.
	    operandSize needs to be 16 or 32.
	*/ 
	void SarByteWordOrDword(int operandSize,unsigned int &value,unsigned int ctr);
	void SarDword(unsigned int &value,unsigned int ctr);
	void SarWord(unsigned int &value,unsigned int ctr);
	void SarByte(unsigned int &value,unsigned int ctr);

	/*! SHL a value and set OF and CF flags accoring to the result.
	    OF is only set if ctr==1.
	    operandSize needs to be 16 or 32.
	*/ 
private:
	template <unsigned int bitCount,unsigned int allBits,unsigned int signBit>
	void ShlTemplate(unsigned int &value,unsigned int ctr);
public:
	void ShlWordOrDword(int operandSize,unsigned int &value,unsigned int ctr);
	void ShlDword(unsigned int &value,unsigned int ctr);
	void ShlWord(unsigned int &value,unsigned int ctr);
	void ShlByte(unsigned int &value,unsigned int ctr);
	/*! SHR a value and set OF and CF flags accoring to the result.
	    OF is only set if ctr==1.
	    operandSize needs to be 16 or 32.
	*/ 
private:
	template <unsigned int bitCount,unsigned int allBits,unsigned int signBit>
	inline void ShrTemplate(unsigned int &value,unsigned int ctr);
public:
	void ShrWordOrDword(int operandSize,unsigned int &value,unsigned int ctr);
	void ShrDword(unsigned int &value,unsigned int ctr);
	void ShrWord(unsigned int &value,unsigned int ctr);
	void ShrByte(unsigned int &value,unsigned int ctr);


	/*! Evaluates an operand.
	    destinationBytes is non zero if the operand that receives the value has a known size.
	    If the destination size depends on the source size, destinationBytes should be zero.

		When operandType is OPER_ADDR, destinationBytes parameter defines the number of bytes fetched from the memory.
		It is significant when FWORD PTR (4 bytes in Real Mode, 6 bytes in Protected Mode) needs to be
		fetched.  First 2 or 4 bytes from FWORD PTR will give offset, and the last 2 bytes give segment.
		In which case returned OperandValue will return correct value for GetAsDword, GetAsWord, and GetFwordSegment.
	*/
	OperandValue EvaluateOperand(
	    const Memory &mem,int addressSize,int segmentOverride,const Operand &op,int destinationBytes);

	/*! Evaluate operand as an 8-bit operand.
	*/
	OperandValue EvaluateOperand8(
	    const Memory &mem,int addressSize,int segmentOverride,const Operand &op);


	/*! Extract segment register and address offset from the OPER_ADDR type operand.
	    It doesn't check the operand type.  Using it for a different operand type would crash the
	    program.

	    offset will always be calculated from 32-bit registers.
	    For example, even if base is REG_DI the base value is taken from EDI.
	    offset must be masked appropriately depending on the address size.
	*/
	inline const SegmentRegister *ExtractSegmentAndOffset(unsigned int &offset,const Operand &op,unsigned int segmentOverride) const
	{
		offset=
		    state.NULL_and_reg32[op.baseReg&15]+
		   ((state.NULL_and_reg32[op.indexReg&15])<<op.indexShift)+
		   op.offset;

		sregIndexToSregPtrTable[NUM_SEGMENT_REGISTERS]=baseRegisterToDefaultSegment[op.baseReg];
		auto sregIndex=segPrefixToSregIndex[segmentOverride];
		return sregIndexToSregPtrTable[sregIndex];
	}


	/*! Stores value to the destination described by the operand.
	    If the destination is memory, the number of bytes stored depends on numByte member of OperandValue.
	    If the destination is a register, the number of bytes stored depends on the size of the register.
	*/
	void StoreOperandValue(const Operand &dst,Memory &mem,int addressSize,int segmentOverride,const OperandValue &value);

	/*! Store value to an 8-bit operand.
	*/
	void StoreOperandValue8(const Operand &dst,Memory &mem,int addressSize,int segmentOverride,const OperandValue &value);

	/*! Returns override-segment for the prefix.  Returns default DS.
	*/
	inline const SegmentRegister &SegmentOverrideDefaultDS(int segOverridePrefix)
	{
		sregIndexToSregPtrTable[NUM_SEGMENT_REGISTERS]=&state.sreg[REG_DS-REG_SEGMENT_REG_BASE];
		auto sregIndex=segPrefixToSregIndex[segOverridePrefix];
		return *sregIndexToSregPtrTable[sregIndex];
	}

	static int StrToReg(const std::string &regName);

	/*! Test I/O-read exception, and RaiseException and HandleException if needed.
	    Returns true if an exception was raised.
	*/
	inline bool TakeIOReadException(unsigned int ioport,unsigned int accessSize,Memory &mem);

	/*! Test I/O-write exception, and RaiseException and HandleException if needed.
	    Returns true if an exception was raised.
	*/
	inline bool TakeIOWriteException(unsigned int ioport,unsigned int accessSize,Memory &mem);

	/*! Return true if I/O access is permitted in I/O Map of TSS.
	    It could raise exception.
	*/
	bool TestIOMapPermission(const SegmentRegister &TR,unsigned int ioMin,unsigned int accessSize,const Memory &mem);

	/*! Return true if I/O access is permitted in I/O Map of TSS.
	    It will not raise an exception.  It is for debugging purpose.
	*/
	bool DebugTestIOMapPermission(const SegmentRegister &TR,unsigned int ioMin,unsigned int accessSize,const Memory &mem) const;
};


#include "i486debug.h"

inline void i486DX::Interrupt(unsigned int INTNum,Memory &mem,unsigned int numInstBytes)
{
	if(nullptr!=debuggerPtr)
	{
		debuggerPtr->Interrupt(*this,INTNum,mem,numInstBytes);
	}

	state.halt=false;

	if(IsInRealMode())
	{
		Push(mem,16,state.EFLAGS&0xFFFF);
		Push(mem,16,state.CS().value);
		Push(mem,16,state.EIP+numInstBytes);

		auto intVecAddr=(INTNum&0xFF)*4;
		auto destIP=mem.FetchWord(intVecAddr);
		auto destCS=mem.FetchWord(intVecAddr+2);
		if(true==enableCallStack)
		{
			PushCallStack(
			    true,INTNum,GetAX(), // Is an interrupt
			    state.GetCR(0),
			    state.CS().value,state.EIP,numInstBytes,
			    destCS,destIP,
			    mem);
		}
		LoadSegmentRegisterRealMode(state.CS(),destCS);
		state.EIP=destIP;
		SetIF(false);
		SetTF(false);
	}
	else
	{
		auto desc=GetInterruptDescriptor(INTNum,mem);
		if(FarPointer::NO_SEG!=desc.SEG)
		{
			auto type=desc.GetType();
			std::string errMsg;
			const unsigned int gateOperandSize=32;
			// https://wiki.osdev.org/Interrupt_Descriptor_Table
			switch(type)
			{
			default:
				if(true==state.exception && EXCEPTION_ND==state.exceptionType)
				{
					Abort("Infinite NO_PRESENT exception.");
				}
				else
				{
					unsigned int Ibit=2;
					unsigned int EXTbit=0; // 1 if external interrupt source.
					RaiseException(EXCEPTION_GP,INTNum*8+Ibit+EXTbit); // EXT -> [1] 9-8 Error Code
					HandleException(false,mem);  // <- This will shoot INT 0BH
				}
				return;
			case 0b0110:
				Abort("286 16-bit INT gate not supported");
				break;
			case 0b0111:
				Abort("286 16-bit Trap gate not supported");
				break;
			case 0b0101: //"386 32-bit Task";
			case 0b1110: //"386 32-bit INT";
			case 0b1111: //"386 32-bit Trap";
				break;
			}

			if(true==enableCallStack)
			{
				PushCallStack(
				    true,INTNum,GetAX(), // Is an interrupt
				    state.GetCR(0),
				    state.CS().value,state.EIP,numInstBytes,
				    desc.SEG,desc.OFFSET,
				    mem);
			}

			if(0==(state.EFLAGS&EFLAGS_VIRTUAL86))
			{
				SegmentRegister newCS;
				LoadSegmentRegister(newCS,desc.SEG,mem);
				Push(mem,gateOperandSize,state.EFLAGS);
				Push(mem,gateOperandSize,state.CS().value);
				Push(mem,gateOperandSize,state.EIP+numInstBytes);
				SetIPorEIP(gateOperandSize,desc.OFFSET);
				state.CS()=newCS;
				SetIF(false);
				SetTF(false);
			}
			else // Interrupt from Virtual86 mode
			{
				// INT instruction of [1].
				auto TempEFLAGS=state.EFLAGS;
				auto TempSS=state.SS();
				auto TempESP=state.ESP();
				state.EFLAGS&=~(EFLAGS_VIRTUAL86|EFLAGS_TRAP);
				// if(fromInterruptGate)
				{
					state.EFLAGS&=~EFLAGS_INT_ENABLE;
				}
				// Is TR always 32-bit address size?
				LoadSegmentRegister(state.SS(),FetchWord(32,state.TR,TSS_OFFSET_SS0,mem),mem);
				state.ESP()=FetchDword(32,state.TR,TSS_OFFSET_ESP0,mem);
				Push(mem,32,state.GS().value);
				Push(mem,32,state.FS().value);
				Push(mem,32,state.DS().value);
				Push(mem,32,state.ES().value);
				Push(mem,32,TempSS.value);
				Push(mem,32,TempESP);
				Push(mem,32,TempEFLAGS);
				Push(mem,32,state.CS().value);
				Push(mem,32,state.EIP+numInstBytes);

				SetIPorEIP(gateOperandSize,desc.OFFSET);
				LoadSegmentRegister(state.CS(),desc.SEG,mem);
			}
		}
		else
		{
			RaiseException(EXCEPTION_GP,INTNum*8); // What's +EXT?  ([1] pp.26-170)
		}
	}
};

inline void i486DX::StoreByte(Memory &mem,int addressSize,SegmentRegister seg,unsigned int offset,unsigned char byteData)
{
	offset&=AddressMask((unsigned char)addressSize);
	auto linearAddr=seg.baseLinearAddr+offset;

	if(&seg==&state.SS() && 
	   nullptr!=state.SSESPWindow.ptr &&
	   true==state.SSESPWindow.IsLinearAddressInRange(linearAddr))
	{
		state.SSESPWindow.ptr[linearAddr&(MemoryAccess::MEMORY_WINDOW_SIZE-1)]=byteData;
		return;
	}

	auto physicalAddr=linearAddr;
	if(true==PagingEnabled())
	{
		physicalAddr=LinearAddressToPhysicalAddress(linearAddr,mem);
	}
	return mem.StoreByte(physicalAddr,byteData);
}

inline void i486DX::StoreWord(Memory &mem,int addressSize,SegmentRegister seg,unsigned int offset,unsigned int data)
{
	offset&=AddressMask((unsigned char)addressSize);
	auto linearAddr=seg.baseLinearAddr+offset;

	if(&seg==&state.SS() && 
	   nullptr!=state.SSESPWindow.ptr &&
	   true==state.SSESPWindow.IsLinearAddressInRange(linearAddr))
	{
		unsigned int low12bits=(linearAddr&(MemoryAccess::MEMORY_WINDOW_SIZE-1));
		if(low12bits<=MemoryAccess::MEMORY_WINDOW_SIZE-2)
		{
			cpputil::PutWord(state.SSESPWindow.ptr+low12bits,data);
			return;
		}
	}

	auto physicalAddr=linearAddr;
	if(true==PagingEnabled())
	{
		physicalAddr=LinearAddressToPhysicalAddress(linearAddr,mem);
		if(0xFFE<(physicalAddr&0xfff)) // May hit the page boundary
		{
			StoreByte(mem,addressSize,seg,offset  , data    &255);// May hit the page boundary. Don't use StoreWord
			StoreByte(mem,addressSize,seg,offset+1,(data>>8)&255);// May hit the page boundary. Don't use StoreWord
			return;
		}
	}
	mem.StoreWord(physicalAddr,data);
}
inline void i486DX::StoreDword(Memory &mem,int addressSize,SegmentRegister seg,unsigned int offset,unsigned int data)
{
	offset&=AddressMask((unsigned char)addressSize);
	auto linearAddr=seg.baseLinearAddr+offset;

	if(&seg==&state.SS() && 
	   nullptr!=state.SSESPWindow.ptr &&
	   true==state.SSESPWindow.IsLinearAddressInRange(linearAddr))
	{
		unsigned int low12bits=(linearAddr&(MemoryAccess::MEMORY_WINDOW_SIZE-1));
		if(low12bits<=MemoryAccess::MEMORY_WINDOW_SIZE-4)
		{
			cpputil::PutDword(state.SSESPWindow.ptr+low12bits,data);
			return;
		}
	}

	auto physicalAddr=linearAddr;
	if(true==PagingEnabled())
	{
		physicalAddr=LinearAddressToPhysicalAddress(linearAddr,mem);
		if(0xFFC<(physicalAddr&0xfff)) // May hit the page boundary
		{
			StoreByte(mem,addressSize,seg,offset  , data     &255);
			StoreByte(mem,addressSize,seg,offset+1,(data>> 8)&255);// May hit the page boundary. Don't use StoreDword
			StoreByte(mem,addressSize,seg,offset+2,(data>>16)&255);// May hit the page boundary. Don't use StoreDword
			StoreByte(mem,addressSize,seg,offset+3,(data>>24)&255);// May hit the page boundary. Don't use StoreDword
			return;
		}
	}
	mem.StoreDword(physicalAddr,data);
}

inline void i486DX::StoreWordOrDword(Memory &mem,unsigned int operandSize,int addressSize,SegmentRegister seg,unsigned int offset,unsigned int data)
{
	switch(operandSize)
	{
	case 8:
		StoreByte(mem,addressSize,seg,offset,data);
		break;
	case 16:
		StoreWord(mem,addressSize,seg,offset,data);
		break;
	default:
	case 32:
		StoreDword(mem,addressSize,seg,offset,data);
		break;
	}
}

inline void i486DX::IOOut8(InOut &io,unsigned int ioport,unsigned int data)
{
	if(nullptr!=debuggerPtr)
	{
		debuggerPtr->IOWrite(*this,ioport,data,1);
	}
	io.Out8(ioport,data);
}
inline void i486DX::IOOut16(InOut &io,unsigned int ioport,unsigned int data)
{
	if(nullptr!=debuggerPtr)
	{
		debuggerPtr->IOWrite(*this,ioport,data,2);
	}
	io.Out16(ioport,data);
}
inline void i486DX::IOOut32(InOut &io,unsigned int ioport,unsigned int data)
{
	if(nullptr!=debuggerPtr)
	{
		debuggerPtr->IOWrite(*this,ioport,data,4);
	}
	io.Out32(ioport,data);
}
inline unsigned int i486DX::IOIn8(InOut &io,unsigned int ioport)
{
	auto data=io.In8(ioport);
	if(nullptr!=debuggerPtr)
	{
		debuggerPtr->IORead(*this,ioport,data,1);
	}
	return data;
}
inline unsigned int i486DX::IOIn16(InOut &io,unsigned int ioport)
{
	auto data=io.In16(ioport);
	if(nullptr!=debuggerPtr)
	{
		debuggerPtr->IORead(*this,ioport,data,2);
	}
	return data;
}
inline unsigned int i486DX::IOIn32(InOut &io,unsigned int ioport)
{
	auto data=io.In32(ioport);
	if(nullptr!=debuggerPtr)
	{
		debuggerPtr->IORead(*this,ioport,data,4);
	}
	return data;
}

inline unsigned int i486DX::GetRegisterValue(int reg) const
{
	switch(reg)
	{
	case REG_AL:
		return state.EAX()&255;
	case REG_CL:
		return state.ECX()&255;
	case REG_DL:
		return state.EDX()&255;
	case REG_BL:
		return state.EBX()&255;
	case REG_AH:
		return (state.EAX()>>8)&255;
	case REG_CH:
		return (state.ECX()>>8)&255;
	case REG_DH:
		return (state.EDX()>>8)&255;
	case REG_BH:
		return (state.EBX()>>8)&255;

	case REG_AX:
		return state.EAX()&65535;
	case REG_CX:
		return state.ECX()&65535;
	case REG_DX:
		return state.EDX()&65535;
	case REG_BX:
		return state.EBX()&65535;
	case REG_SP:
		return state.ESP()&65535;
	case REG_BP:
		return state.EBP()&65535;
	case REG_SI:
		return state.ESI()&65535;
	case REG_DI:
		return state.EDI()&65535;

	case REG_EAX:
		return state.EAX();
	case REG_ECX:
		return state.ECX();
	case REG_EDX:
		return state.EDX();
	case REG_EBX:
		return state.EBX();
	case REG_ESP:
		return state.ESP();
	case REG_EBP:
		return state.EBP();
	case REG_ESI:
		return state.ESI();
	case REG_EDI:
		return state.EDI();

	case REG_EIP:
		return state.EIP;
	case REG_EFLAGS:
		return state.EFLAGS;

	case REG_ES:
		return state.ES().value;
	case REG_CS:
		return state.CS().value;
	case REG_SS:
		return state.SS().value;
	case REG_DS:
		return state.DS().value;
	case REG_FS:
		return state.FS().value;
	case REG_GS:
		return state.GS().value;

	//case REG_GDT:
	//case REG_LDT:
	//case REG_TR:
	//case REG_IDTR:

	case REG_CR0:
		return state.GetCR(0);
	case REG_CR1:
		return state.GetCR(1);
	case REG_CR2:
		return state.GetCR(2);
	case REG_CR3:
		return state.GetCR(3);

	case REG_DR0:
	case REG_DR1:
	case REG_DR2:
	case REG_DR3:
	case REG_DR4:
	case REG_DR5:
	case REG_DR6:
	case REG_DR7:
		return state.DR[reg-REG_DR0];

	case REG_TEST0:
	case REG_TEST1:
	case REG_TEST2:
	case REG_TEST3:
	case REG_TEST4:
	case REG_TEST5:
	case REG_TEST6:
	case REG_TEST7:
		return state.TEST[reg-REG_TEST0];
	}
	return 0;
}

inline unsigned int i486DX::GetRegisterValue8(int reg) const
{
	unsigned int regIdx=reg-REG_AL;
	unsigned int shift=(regIdx<<1)&8;
	return ((state.reg32()[regIdx&3]>>shift)&255);
}

inline void i486DX::SetRegisterValue(unsigned int reg,unsigned int value)
{
	switch(reg)
	{
	case REG_AL:
		SetAL(value);
		break;
	case REG_CL:
		SetCL(value);
		break;
	case REG_DL:
		SetDL(value);
		break;
	case REG_BL:
		SetBL(value);
		break;
	case REG_AH:
		SetAH(value);
		break;
	case REG_CH:
		SetCH(value);
		break;
	case REG_DH:
		SetDH(value);
		break;
	case REG_BH:
		SetBH(value);
		break;

	case REG_AX:
		SetAX(value);
		break;
	case REG_CX:
		SetCX(value);
		break;
	case REG_DX:
		SetDX(value);
		break;
	case REG_BX:
		SetBX(value);
		break;
	case REG_SP:
		SetSP(value);
		break;
	case REG_BP:
		SetBP(value);
		break;
	case REG_SI:
		SetSI(value);
		break;
	case REG_DI:
		SetDI(value);
		break;

	case REG_EAX:
		SetEAX(value);
		break;
	case REG_ECX:
		SetECX(value);
		break;
	case REG_EDX:
		SetEDX(value);
		break;
	case REG_EBX:
		SetEBX(value);
		break;
	case REG_ESP:
		SetESP(value);
		break;
	case REG_EBP:
		SetEBP(value);
		break;
	case REG_ESI:
		SetESI(value);
		break;
	case REG_EDI:
		SetEDI(value);
		break;

	default:
		Abort("SetRegisterValue function is not suppose to be used for this register.");
		break;
	}
}

inline void i486DX::SetRegisterValue8(unsigned int reg,unsigned char value)
{
	static const unsigned int highLowMask[2]=
	{
		0xFFFFFF00,
		0xFFFF00FF,
	};
	unsigned int regIdx=reg-REG_AL;
	unsigned int highLow=regIdx>>2;
	state.reg32()[regIdx&3]&=highLowMask[highLow];
	state.reg32()[regIdx&3]|=(value<<(highLow<<3));
}

inline bool i486DX::TakeIOReadException(unsigned int ioport,unsigned int accessSize,Memory &mem)
{
	if(0!=(state.EFLAGS&EFLAGS_VIRTUAL86))
	{
		if(true!=TestIOMapPermission(state.TR,ioport,accessSize,mem))
		{
			RaiseException(EXCEPTION_GP,0);
			HandleException(true,mem);
			return true;
		}
	}
	return false;
}

inline bool i486DX::TakeIOWriteException(unsigned int ioport,unsigned int accessSize,Memory &mem)
{
	if(0!=(state.EFLAGS&EFLAGS_VIRTUAL86))
	{
		if(true!=TestIOMapPermission(state.TR,ioport,accessSize,mem))
		{
			RaiseException(EXCEPTION_GP,0);
			HandleException(false,mem);
			return true;
		}
	}
	return false;
}

/* } */
#endif
