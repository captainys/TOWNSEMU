#ifndef I486_IS_INCLUDED
#define I486_IS_INCLUDED
/* { */

#include "cpu.h"
#include "ramrom.h"
#include "inout.h"

// References
// [1]  i486 Processor Programmers Reference Manual

// Todo:
//   Move function should raise protection fault / real-mode exception

class i486DX : public CPU
{
public:
	static const char *const Reg8Str[8];
	static const char *const Reg16Str[8];
	static const char *const Reg32Str[8];
	static const char *const Sreg[8];

	enum
	{
		REG_NONE,

		REG_AL,
		REG_CL,
		REG_DL,
		REG_BL,
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

		REG_EAX,
		REG_ECX,
		REG_EDX,
		REG_EBX,
		REG_ESP,
		REG_EBP,
		REG_ESI,
		REG_EDI,

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
		REG_TR0,
		REG_TR1,
		REG_TR2,
		REG_TR3,
		REG_TR4,
		REG_TR5,
		REG_TR6,
		REG_TR7,
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
		REG_TR_REG_BASE=REG_TR0,
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
	class SystemAddressRegister
	{
	public:
		unsigned int linearBaseAddr;
		unsigned short limit;
	};
	class SystemSegmentRegister
	{
	public:
		unsigned int linearBaseAddr;
		unsigned short limit;
		unsigned short selector;
		unsigned short attrib;
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
			NO_SEG=0xFFFF0000,
			SEG_REGISTER=0x00010000,
			LINEAR_ADDR=0x00020000,
			PHYS_ADDR=0x00030000,
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
	};

	class FPUState
	{
	public:
		void FNINIT(void);
		bool ExceptionPending(void) const;
		unsigned int GetStatusWord(void) const;
	};

	class State
	{
	public:
		unsigned int reg32[8];

		inline unsigned int EAX(void) const
		{
			return reg32[REG_EAX-REG_EAX];
		}
		inline unsigned int EBX(void) const
		{
			return reg32[REG_EBX-REG_EAX];
		}
		inline unsigned int ECX(void) const
		{
			return reg32[REG_ECX-REG_EAX];
		}
		inline unsigned int EDX(void) const
		{
			return reg32[REG_EDX-REG_EAX];
		}
		inline unsigned int ESI(void) const
		{
			return reg32[REG_ESI-REG_EAX];
		}
		inline unsigned int EDI(void) const
		{
			return reg32[REG_EDI-REG_EAX];
		}
		inline unsigned int EBP(void) const
		{
			return reg32[REG_EBP-REG_EAX];
		}
		inline unsigned int ESP(void) const
		{
			return reg32[REG_ESP-REG_EAX];
		}
		inline unsigned int &EAX(void)
		{
			return reg32[REG_EAX-REG_EAX];
		}
		inline unsigned int &EBX(void)
		{
			return reg32[REG_EBX-REG_EAX];
		}
		inline unsigned int &ECX(void)
		{
			return reg32[REG_ECX-REG_EAX];
		}
		inline unsigned int &EDX(void)
		{
			return reg32[REG_EDX-REG_EAX];
		}
		inline unsigned int &ESI(void)
		{
			return reg32[REG_ESI-REG_EAX];
		}
		inline unsigned int &EDI(void)
		{
			return reg32[REG_EDI-REG_EAX];
		}
		inline unsigned int &EBP(void)
		{
			return reg32[REG_EBP-REG_EAX];
		}
		inline unsigned int &ESP(void)
		{
			return reg32[REG_ESP-REG_EAX];
		}

		unsigned int EIP;
		unsigned int EFLAGS;   // bit 1=Always 1 ([1] pp.2-14)
		SegmentRegister sreg[6];
		SystemAddressRegister GDTR,IDTR,LDTR;
		SystemSegmentRegister TR[8];
		unsigned int CR[4];
		unsigned int DR[8];

		const SegmentRegister &ES(void) const
		{
			return sreg[REG_ES-REG_SEGMENT_REG_BASE];
		}
		const SegmentRegister &CS(void) const
		{
			return sreg[REG_CS-REG_SEGMENT_REG_BASE];
		}
		const SegmentRegister &SS(void) const
		{
			return sreg[REG_SS-REG_SEGMENT_REG_BASE];
		}
		const SegmentRegister &DS(void) const
		{
			return sreg[REG_DS-REG_SEGMENT_REG_BASE];
		}
		const SegmentRegister &FS(void) const
		{
			return sreg[REG_FS-REG_SEGMENT_REG_BASE];
		}
		const SegmentRegister &GS(void) const
		{
			return sreg[REG_GS-REG_SEGMENT_REG_BASE];
		}
		SegmentRegister &ES(void)
		{
			return sreg[REG_ES-REG_SEGMENT_REG_BASE];
		}
		SegmentRegister &CS(void)
		{
			return sreg[REG_CS-REG_SEGMENT_REG_BASE];
		}
		SegmentRegister &SS(void)
		{
			return sreg[REG_SS-REG_SEGMENT_REG_BASE];
		}
		SegmentRegister &DS(void)
		{
			return sreg[REG_DS-REG_SEGMENT_REG_BASE];
		}
		SegmentRegister &FS(void)
		{
			return sreg[REG_FS-REG_SEGMENT_REG_BASE];
		}
		SegmentRegister &GS(void)
		{
			return sreg[REG_GS-REG_SEGMENT_REG_BASE];
		}

		/*! Returns Segment-Register.  REG must be REG_CS,REG_DS,REG_ES,REG_FS,REG_FS,REG_SS.
		*/
		SegmentRegister GetSegmentRegister(int reg) const
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

		FPUState fpuState;

		bool halt;

		// [1] pp.26-211 in the description of the MOV instruction
		// "Loading to SS register inhibits all interrupts until after the execution of the next instruction"
		bool holdIRQ;

		// True when an instruction raised an exception.
		bool exception;
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
		RESET_IDTRBASE=      0,
		RESET_IDTRLIMIT=0x03FF,
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
		EXCEPTION_GP,
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

		unsigned int opCode;
		unsigned int operandLen;
		unsigned char operand[12];  // Is 8 bytes maximum?

		inline void Clear(void)
		{
			numBytes=0;
			instPrefix=0;
			segOverride=0;
			operandLen=0;
		}

		/*! Decode operands.
		    I think data size is operand size, but I am not 100% sure yet.
		*/
		void DecodeOperand(int addressSize,int operandSize,class Operand &op1,class Operand &op2) const;

		std::string Disassemble(SegmentRegister reg,unsigned int offset) const;
	private:
		/* operandSize is 8, 16, or 32 */
		std::string DisassembleTypicalOneOperand(std::string inst,const Operand &op,int operandSize) const;
		std::string DisassembleTypicalRM8_I8(std::string inst,const Operand &op1,const Operand &op2) const;
		std::string DisassembleTypicalRM_I8(std::string inst,const Operand &op1,const Operand &op2) const;
		std::string DisassembleTypicalTwoOperands(std::string inst,const Operand &op1,const Operand &op2) const;


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
		unsigned int GetUimm8(void) const;

		/*! Returns Unsigned Imm16 (last 2 byte in the operand) after decoding. */
		unsigned int GetUimm16(void) const;

		/*! Returns Unsigned Imm32 (last 4 byte in the operand) after decoding. */
		unsigned int GetUimm32(void) const;

		/*! Returns Unsigned Imm16 or Imm32 after decoding. */
		unsigned int GetUimm16or32(unsigned int operandSize) const;

		/*! Returns Signed Imm8 (last byte in the operand) after decoding. */
		int GetSimm8(void) const;

		/*! Returns Signed Imm16 (last 2 byte in the operand) after decoding. */
		int GetSimm16(void) const;

		/*! Returns Signed Imm32 (last 4 byte in the operand) after decoding. */
		int GetSimm32(void) const;

		/*! Returns Signed Imm16 or Imm32 after decoding. */
		int GetSimm16or32(unsigned int operandSize) const;
	};


	enum
	{
		OPER_UNDEFINED,
		OPER_ADDR,    // BaseReg+IndexReg*IndexScaling+Offset
		OPER_FARADDR,
		OPER_REG,
		OPER_IMM8,
		OPER_IMM16,
		OPER_IMM32,
	};
	class Operand
	{
	public:
		int operandType;

		// For OPER_REG operand type
		int reg;
		// For OPER_IMM* operand type
		int imm;
		// For OPER_ADDR and OPER_FARADDR operand type
		int baseReg,indexReg,indexScaling,offset;
		int offsetBits;
		// For OPER_FARADDR operand type.  NOT USED in any other operand types.
		int seg;

		inline Operand(){}

		/*! Constructor for decoding on construction.
		*/
		Operand(int addressSize,int dataSize,const unsigned char operand[]);

		/*! operandType=OPER_UNDEFINED
		*/
		void Clear(void);

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
		/*! Decode operand for a TR register.
		*/
		void DecodeMODR_MForTRRegister(unsigned char MODR_M);
		/*! Decode operand and returns the number of bytes.
		*/
		void MakeByRegisterNumber(int dataSize,int regNum);
		/*! Make Immediate. */
		void MakeImm8(const Instruction &inst);
		/*! Make Immediate. */
		void MakeImm16(const Instruction &inst);
		/*! Make Immediate. */
		void MakeImm32(const Instruction &inst);
		/*! Make Immediate 16 or 32.  Size depends on operandSize. */
		void MakeImm8or16or32(const Instruction &inst,unsigned int operandSize);

		/*! Make Simple Offset from segment base.  Like for MOV(A0H,A1H,A2H,A3H)
		    Number of bytes evaluated will be inst.addressSize/8.
		*/
		void MakeSimpleAddressOffset(const Instruction &inst);

		/*! Sign-Extend imm operand.  If it is applied to non-imm operand, it returns false.
		    Current operand type must be OPER_IMM8 or OPER_IMM16.
		    newOperaType must be OPER_IMM16 or OPER_IMM32.
		*/
		bool SignExtendImm(int newOperaType);

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
		/*! Returns size qualifier if one of the operands is address.
		*/
		static void GetSizeQualifierToDisassembly(
			std::string &op1Qual,std::string &op2Qual,const Operand &op1,const Operand &op2);

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
		unsigned int numBytes;
		unsigned char byteData[10];

		/*! Returns a value as a unsigned dword.
		    It won't evaluate beyond numBytes.
		*/
		inline unsigned int GetAsDword(void) const
		{
			unsigned int dword=0;
			switch(numBytes)
			{
			default:
			case 4:
				dword|=(byteData[3]<<24);
			case 3:
				dword|=(byteData[2]<<16);
			case 2:
				dword|=(byteData[1]<<8);
			case 1:
				dword|= byteData[0];
			case 0:
				break;
			}
			return dword;
		}
		/*! Returns a value as a unsigned wword.
		    It won't evaluate beyond numBytes.
		*/
		inline unsigned int GetAsWord(void) const
		{
			unsigned int word=0;
			switch(numBytes)
			{
			default:
			case 4:
			case 3:
			case 2:
				word|=(byteData[1]<<8);
			case 1:
				word|= byteData[0];
			case 0:
				break;
			}
			return word;
		}

		/*! Returns a value as a signed byte, word, or dword.
		*/
		inline int GetAsSignedDword(void) const
		{
			unsigned long long int uValue=GetAsDword();
			switch(numBytes)
			{
			default:
			case 4:
				if(0x80000000&uValue)
				{
					uValue-=0x80000000;
					uValue-=0x80000000;
				}
				break;
			case 3:
			case 2:
				if(0x8000&uValue)
				{
					uValue-=0x10000;
				}
				break;
			case 1:
				if(0x80&uValue)
				{
					uValue-=0x100;
				}
				break;
			}
			return (int)uValue;
		}

		/*! SetDword does not change numBytes.
		    It won't update beyond numBytes.
		    If it needs to be made 4-byte long, use MakeDword instead.
		*/
		inline void SetDword(unsigned int dword)
		{
			switch(numBytes)
			{
			default:
			case 4:
				byteData[3]=((dword>>24)&255);
			case 3:
				byteData[2]=((dword>>16)&255);
			case 2:
				byteData[1]=((dword>>8)&255);
			case 1:
				byteData[0]=(dword&255);
			case 0:
				break;
			}
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
			byteData[0]=(dword&255);
			byteData[1]=((dword>>8)&255);
			byteData[2]=((dword>>16)&255);
			byteData[3]=((dword>>24)&255);
		}
		/*! MakeDword makes a 4-byte long OperandValue.
		    It updates numByte to 4.
		*/
		inline void MakeWord(unsigned int word)
		{
			numBytes=2;
			byteData[0]=(word&255);
			byteData[1]=((word>>8)&255);
		}
		/*! Makes a word or dword value.  The size depends on the operandSize. */
		inline void MakeWordOrDword(unsigned int operandSize,unsigned int value)
		{
			if(16==operandSize)
			{
				MakeWord(value);
			}
			else
			{
				MakeDword(value);
			}
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
		bool isInterrupt;
		unsigned int fromCS,fromEIP;
		unsigned int callOpCodeLength;
		unsigned int procCS,procEIP;
	};
	bool enableCallStack;
	std::vector <CallStack> callStack;
	class i486Debugger *debuggerPtr;


	/*! Make a call-stack entry.
	*/
	CallStack MakeCallStack(
	    bool isInterrupt,
	    unsigned int fromCS,unsigned int fromEIP,unsigned int callOpCodeLength,
	    unsigned int procCS,unsigned int procEIP);
	/*! Make a call-stack entry, and push to the call stack.
	*/
	void PushCallStack(
	    bool isInterrupt,
	    unsigned int fromCS,unsigned int fromEIP,unsigned int callOpCodeLength,
	    unsigned int procCS,unsigned int procEIP);
	/*! Pop an entry from call stack.
	*/
	void PopCallStack(void);

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
		state.EAX()|=value;
	}
	inline void SetAL(unsigned int value)
	{
		state.EAX()&=0xffffff00;
		state.EAX()|=value;
	}
	inline void SetAH(unsigned int value)
	{
		state.EAX()&=0xffff00ff;
		state.EAX()|=(value<<8);
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
		state.ECX()|=value;
	}
	inline void SetCL(unsigned int value)
	{
		state.ECX()&=0xffffff00;
		state.ECX()|=value;
	}
	inline void SetCH(unsigned int value)
	{
		state.ECX()&=0xffff00ff;
		state.ECX()|=(value<<8);
	}
	inline unsigned int GetCXorECX(unsigned int bits) const
	{
		if(16==bits)
		{
			return GetCX();
		}
		else
		{
			return GetECX();
		}
	}
	inline void SetCXorECX(unsigned int bits,unsigned int newECX)
	{
		if(16==bits)
		{
			SetCX(newECX);
		}
		else
		{
			SetECX(newECX);
		}
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
		state.EDX()|=value;
	}
	inline void SetDL(unsigned int value)
	{
		state.EDX()&=0xffffff00;
		state.EDX()|=value;
	}
	inline void SetDH(unsigned int value)
	{
		state.EDX()&=0xffff00ff;
		state.EDX()|=(value<<8);
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
	inline void SetIF(bool flag)
	{
		SetEFLAGSBit(flag,EFLAGS_INT_ENABLE);
	}
	inline void SetOverflowFlag(bool flag)
	{
		SetEFLAGSBit(flag,EFLAGS_OVERFLOW);
	}

	inline void SetSignFlag(bool flag)
	{
		SetEFLAGSBit(flag,EFLAGS_SIGN);
	}

	inline void SetZeroFlag(bool flag)
	{
		SetEFLAGSBit(flag,EFLAGS_ZERO);
	}

	inline void SetAuxCarryFlag(bool flag)
	{
		SetEFLAGSBit(flag,EFLAGS_AUX_CARRY);
	}

	inline void SetParityFlag(bool flag)
	{
		SetEFLAGSBit(flag,EFLAGS_PARITY);
	}

	inline bool CheckParity(unsigned char lowByte)
	{
		int n=0;
		for(int i=0; i<8; ++i)
		{
			if(0!=(lowByte&1))
			{
				++n;
			}
			lowByte>>=1;
		}
		return 0==(n&1);
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
			state.EDI()=(state.EDI()&0xffff0000|(DI&0xffff));
		}
		else
		{
			auto EDI=state.EDI();
			if(true==GetDF())
			{
				EDI-=operandSize/8;
			}
			else
			{
				EDI+=operandSize/8;
			}
			state.EDI()=EDI;
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
			state.ESI()=(state.ESI()&0xffff0000|(SI&0xffff));
		}
		else
		{
			auto ESI=state.ESI();
			if(true==GetDF())
			{
				ESI-=operandSize/8;
			}
			else
			{
				ESI+=operandSize/8;
			}
			state.ESI()=ESI;
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
	i486DX();

	/*! Resets the CPU.
	*/
	void Reset(void);

	/*! Returns the state text.
	*/
	std::vector <std::string> GetStateText(void) const;

	/*! Print state.
	*/
	void PrintState(void) const;

	/*! Loads a segment register.
	    If reg is SS, it raise holdIRQ flag.
	    How the segment linear base address is set depends on the CPU mode,
	    and in the protected mode, it needs to look at GDT and LDT.
	    Therefore it needs a reference to memory.
	*/
	void LoadSegmentRegister(SegmentRegister &reg,unsigned int value,const Memory &mem);

	/*! Loads a segment register in real mode.
	    If reg is SS, it raise holdIRQ flag.
	*/
	void LoadSegmentRegisterRealMode(SegmentRegister &reg,unsigned int value);

	/*! Loads a segment register.
	    It does not rely on the current CPU state, instead isInRealMode is given as a parameter.
	    Even if reg==SS, it does not update holdIRQ flag.
	*/
	inline void LoadSegmentRegisterQuiet(SegmentRegister &reg,unsigned int value,const Memory &mem,bool isInRealMode) const;

	/*! Loads limit and linear base address to a descriptor table register.
	    How many bytes are loaded depends on operand size.  [1] 26-194.
	*/
	void LoadDescriptorTableRegister(SystemAddressRegister &reg,int operandSize,const unsigned char byteData[]);

	/*! Make an OperandValue from a descriptor table register.
	*/
	OperandValue DescriptorTableToOperandValue(const SystemAddressRegister &reg,int operandSize) const;

	inline bool IsInRealMode(void) const
	{
		return (0==(state.CR[0]&CR0_PROTECTION_ENABLE));
	}

	/*! Returns a register value. 
	*/
	unsigned int GetRegisterValue(int reg) const;

	/*! Returns a register size in number of bytes. 
	*/
	static unsigned int GetRegisterSize(int reg);

	/*! Returns true if in 16-bit addressing mode.
	*/
	inline bool AddressingMode16Bit(int addressSize) const
	{
		if(true==IsInRealMode())
		{
			return true;
		}
		else
		{
			return (16==addressSize);
		}
		return false;
	}


	/*! Returns the addressing size (16 or 32) of the stack segment.
	*/
	unsigned int GetStackAddressingSize(void) const;

	/*! Returns true if Paging is enabled.
	*/
	inline bool PagingEnabled(void) const
	{
		return 0!=(state.CR[0]&CR0_PAGING_ENABLED);
	}

	/*! Returns true if the opCode needs one more byte to be fully qualified.
	*/
	bool OpCodeNeedsOneMoreByte(unsigned int firstByte) const;

	/*!
	*/
	inline unsigned long LinearAddressToPhysicalAddress(unsigned int linearAddr,const Memory &mem) const
	{
		Abort("Paging not supported yet.");
		return linearAddr;
	}


	/*! Push a value.
	*/
	void Push(Memory &mem,unsigned int operandSize,unsigned int value);

	/*! Pop a value.
	*/
	unsigned int Pop(Memory &mem,unsigned int operandSize);


	/*! Fetch a byte. 
	*/
	inline unsigned int FetchByte(unsigned int addressSize,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const
	{
		if(true==AddressingMode16Bit(addressSize))
		{
			offset&=0xffff;
		}
		auto addr=seg.baseLinearAddr+offset;
		if(true==PagingEnabled())
		{
			addr=LinearAddressToPhysicalAddress(addr,mem);
		}
		return mem.FetchByte(addr);
	}

	/*! Fetch a dword.
	*/
	inline unsigned int FetchWord(unsigned int addressSize,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const
	{
		if(true==AddressingMode16Bit(addressSize))
		{
			offset&=0xffff;
		}
		auto addr=seg.baseLinearAddr+offset;
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
	inline unsigned int FetchDword(unsigned int addressSize,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const
	{
		if(true==AddressingMode16Bit(addressSize))
		{
			offset&=0xffff;
		}
		auto addr=seg.baseLinearAddr+offset;
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
	inline unsigned int FetchWordOrDword(unsigned int operandSize,unsigned int addressSize,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const
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
	inline unsigned int FetchByteWordOrDword(unsigned int operandSize,unsigned int addressSize,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const
	{
		return FetchWordOrDword(operandSize,addressSize,seg,offset,mem);
	}

	/*! Fetch a byte by linear address.
	*/
	inline unsigned int FetchByteByLinearAddress(const Memory &mem,unsigned int linearAddr) const
	{
		if(true==PagingEnabled())
		{
			linearAddr=LinearAddressToPhysicalAddress(linearAddr,mem);
		}
		return mem.FetchByte(linearAddr);
	}

	/*! Store a byte.
	*/
	inline void StoreByte(Memory &mem,int addressSize,SegmentRegister seg,unsigned int offset,unsigned char byteData)
	{
		if(true==AddressingMode16Bit(addressSize))
		{
			offset&=0xffff;
		}
		auto addr=seg.baseLinearAddr+offset;
		if(true==PagingEnabled())
		{
			addr=LinearAddressToPhysicalAddress(addr,mem);
		}
		return mem.StoreByte(addr,byteData);
	}

	/*! Store a word or dword.  Operand size must be 16 or 32.
	*/
	inline void StoreWordOrDword(Memory &mem,unsigned int operandSize,int addressSize,SegmentRegister seg,unsigned int offset,unsigned int data)
	{
		if(true==AddressingMode16Bit(addressSize))
		{
			offset&=0xffff;
		}
		auto addr=seg.baseLinearAddr+offset;
		if(true==PagingEnabled())
		{
			addr=LinearAddressToPhysicalAddress(addr,mem);
			if(0xFF8<(addr&0xfff)) // May hit the page boundary
			{
				if(16==operandSize)
				{
					mem.StoreByte(addr,data&255);
					mem.StoreByte(addr+1,(data>>8)&255);
				}
				else
				{
					mem.StoreByte(addr,data&255);
					mem.StoreByte(addr+1,(data>>8)&255);
					mem.StoreByte(addr+2,(data>>16)&255);
					mem.StoreByte(addr+3,(data>>24)&255);
				}
				return;
			}
		}
		if(16==operandSize)
		{
			mem.StoreWord(addr,data);
		}
		else
		{
			mem.StoreDword(addr,data);
		}
	}

	/*! Fetch a byte from CS:[EIP+offset].
	*/
	inline unsigned int FetchInstructionByte(unsigned int offset,const Memory &mem) const
	{
		return FetchByte(state.CS().addressSize,state.CS(),state.EIP+offset,mem);
	}

	/*! Fetch an instruction.
	*/
	inline Instruction FetchInstruction(const Memory &mem) const
	{
		return FetchInstruction(state.CS(),state.EIP,mem);
	}


	/*! Raise an exception. 
	*/
	void RaiseException(int exceptionType,int exception){exception=true;};// Right now it's just a placeholder


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
	unsigned int FetchOperand8(Instruction &inst,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const;
	/*! Peek an 8-bit operand.  Returns the number of bytes fetched.
	    It does not push inst.operandLen and this->numBytes by 1 byte.
	*/
	unsigned int PeekOperand8(unsigned int &operand,const Instruction &inst,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const;
	/*! Fetch an 16-bit operand  Returns the number of bytes fetched..
	*/
	unsigned int FetchOperand16(Instruction &inst,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const;
	/*! Fetch an 32-bit operand.  Returns the number of bytes fetched.
	*/
	unsigned int FetchOperand32(Instruction &inst,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const;
	/*! Fetch an 16- or 32-bit operand.  Length fetched depends on inst.operandSize.
	    Returns the number of bytes fetched.
	*/
	unsigned int FetchOperand16or32(Instruction &inst,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const;
	/*! Fetch an operand defined by the RM byte.
	    Returns the number of bytes fetched.
	*/
	unsigned int FetchOperandRM(Instruction &inst,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const;
	/*! Fetch operand(s) for the instruction.
	The following instruction:
	    I486_OPCODE_MOV_TO_CR://        0x220F,
	    I486_OPCODE_MOV_FROM_CR://      0x200F,
	    I486_OPCODE_MOV_FROM_DR://      0x210F,
	    I486_OPCODE_MOV_TO_DR://        0x230F,
	    I486_OPCODE_MOV_FROM_TR://      0x240F,
	    I486_OPCODE_MOV_TO_TR://        0x260F,
	always set 32 to inst.operandSize regardless of the preceding operand-size override, or default operand size.
	*/
	void FetchOperand(Instruction &inst,const SegmentRegister &seg,int offset,const Memory &mem) const;

public:
	/*! Fetch an instruction from specific segment and offset.
	*/
	inline Instruction FetchInstruction(const SegmentRegister &CS,unsigned int offset,const Memory &mem) const
	{
		if(true==IsInRealMode())
		{
			return FetchInstruction(CS,offset,mem,16,16);
		}
		else
		{
			// Default operandSize and addressSize depends on the D flag of the segment descriptor.
			return FetchInstruction(CS,offset,mem,CS.operandSize,CS.addressSize);
		}
	}

	/*! Fetch an instruction from specific segment and offset with given default operand size and address size.
	*/
	Instruction FetchInstruction(const SegmentRegister &CS,unsigned int offset,const Memory &mem,unsigned int defOperSize,unsigned int defAddrSize) const;


public:
	/*! Make a disassembly.
	*/
	std::string Disassemble(const Instruction &inst,SegmentRegister seg,unsigned int offset,const Memory &mem) const;


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


	/*! Increment a value.  It also sets OF SF ZF AF PF according to the result.
	    operandSize needs to be 16 or 32.
	*/
	void IncrementWordOrDword(unsigned int operandSize,unsigned int &value);
	void IncrementDword(unsigned int &value);
	void IncrementWord(unsigned int &value);
	void IncrementByte(unsigned int &value);


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

	/*! ROL a value and set OF and CF flags accoring to the result.
	    OF is only set if ctr==1.
	    operandSize needs to be 16 or 32.
	*/ 
	void RolByteWordOrDword(int operandSize,unsigned int &value,unsigned int ctr);
	void RolDword(unsigned int &value,unsigned int ctr);
	void RolWord(unsigned int &value,unsigned int ctr);
	void RolByte(unsigned int &value,unsigned int ctr);

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

	/*! SHL a value and set OF and CF flags accoring to the result.
	    OF is only set if ctr==1.
	    operandSize needs to be 16 or 32.
	*/ 
	void ShlWordOrDword(int operandSize,unsigned int &value,unsigned int ctr);
	void ShlDword(unsigned int &value,unsigned int ctr);
	void ShlWord(unsigned int &value,unsigned int ctr);
	void ShlByte(unsigned int &value,unsigned int ctr);
	/*! SHR a value and set OF and CF flags accoring to the result.
	    OF is only set if ctr==1.
	    operandSize needs to be 16 or 32.
	*/ 
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
	    const Memory &mem,int addressSize,int segmentOverride,const Operand &op,int destinationBytes) const;


	/*! Stores value to the destination described by the operand.
	    If the destination is memory, the number of bytes stored depends on numByte member of OperandValue.
	    If the destination is a register, the number of bytes stored depends on the size of the register.
	*/
	void StoreOperandValue(const Operand &dst,Memory &mem,int addressSize,int segmentOverride,OperandValue value);
};


#include "i486debug.h"

inline void i486DX::Interrupt(unsigned int intNum,Memory &mem,unsigned int numInstBytes)
{
	if(IsInRealMode())
	{
		Push(mem,16,state.EFLAGS&0xFFFF);
		Push(mem,16,state.CS().value);
		Push(mem,16,state.EIP+numInstBytes);

		auto intVecAddr=(intNum&0xFF)*4;
		auto destIP=mem.FetchWord(intVecAddr);
		auto destCS=mem.FetchWord(intVecAddr+2);
		if(true==enableCallStack)
		{
			PushCallStack(
			    true, // Is an interrupt
			    state.CS().value,state.EIP,numInstBytes,
			    destCS,destIP);
		}
		LoadSegmentRegisterRealMode(state.CS(),destCS);
		state.EIP=destIP;
	}
	else
	{
		Abort("Protected-Mode Interrupt not supported yet.");
	}
};

inline void i486DX::IOOut8(InOut &io,unsigned int ioport,unsigned int data)
{
	if(nullptr!=debuggerPtr)
	{
		debuggerPtr->IOWriteByte(*this,ioport,data);
	}
	io.Out8(ioport,data);
}
inline void i486DX::IOOut16(InOut &io,unsigned int ioport,unsigned int data)
{
	if(nullptr!=debuggerPtr)
	{
		debuggerPtr->IOWriteWord(*this,ioport,data);
	}
	io.Out16(ioport,data);
}
inline void i486DX::IOOut32(InOut &io,unsigned int ioport,unsigned int data)
{
	if(nullptr!=debuggerPtr)
	{
		debuggerPtr->IOWriteDword(*this,ioport,data);
	}
	io.Out32(ioport,data);
}
inline unsigned int i486DX::IOIn8(InOut &io,unsigned int ioport)
{
	auto data=io.In8(ioport);
	if(nullptr!=debuggerPtr)
	{
		debuggerPtr->IOReadByte(*this,ioport,data);
	}
	return data;
}
inline unsigned int i486DX::IOIn16(InOut &io,unsigned int ioport)
{
	auto data=io.In16(ioport);
	if(nullptr!=debuggerPtr)
	{
		debuggerPtr->IOReadWord(*this,ioport,data);
	}
	return data;
}
inline unsigned int i486DX::IOIn32(InOut &io,unsigned int ioport)
{
	auto data=io.In32(ioport);
	if(nullptr!=debuggerPtr)
	{
		debuggerPtr->IOReadDword(*this,ioport,data);
	}
	return data;
}



/* } */
#endif
