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
	static const char *const Reg8[8];
	static const char *const Reg16[8];
	static const char *const Reg32[8];
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

	REG_TOTAL_NUMBER_OF_REGISTERS
	};
	enum
	{
		REG_8BIT_REG_BASE=REG_AL,
		REG_16BIT_REG_BASE=REG_AX,
		REG_32BIT_REG_BASE=REG_EAX,
		REG_SEGMENT_REG_BASE=REG_ES,
	};
	static const char *const RegToStr[REG_TOTAL_NUMBER_OF_REGISTERS];

	class SegmentRegister
	{
	public:
		unsigned short value;
		unsigned int baseLinearAddr;
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
	class State
	{
	public:
		unsigned int EAX,EBX,ECX,EDX;
		unsigned int ESI,EDI,EBP,ESP;
		unsigned int EIP;
		unsigned int EFLAGS;   // bit 1=Always 1 ([1] pp.2-14)
		SegmentRegister CS,DS,ES,SS,FS,GS;
		SystemAddressRegister GDT,LDT;
		SystemSegmentRegister TR,IDTR;
		unsigned int CR0,CR1,CR2,CR3;
		unsigned int DR0,DR1,DR2,DR3,DR4,DR5,DR6,DR7;

		// [1] pp.26-211 in the description of the MOV instruction
		// "Loading to SS register inhibits all interrupts until after the execution of the next instruction"
		bool holdIRQ;
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
		INST_PREFIX_REPNE=0xF2, // REPNE/REPNZ
		INST_PREFIX_LOCK= 0xF0, // LOCK

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

	public:
		/*! Returns Unsigned Imm8 (last byte in the operand) after decoding. */
		unsigned int GetUimm8(void) const;

		/*! Returns Unsigned Imm16 (last 2 byte in the operand) after decoding. */
		unsigned int GetUimm16(void) const;

		/*! Returns Unsigned Imm32 (last 4 byte in the operand) after decoding. */
		unsigned int GetUimm32(void) const;
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
		/*! Decode operand and returns the number of bytes.
		*/
		void DecodeMODR_MForRegister(int dataSize,unsigned char MODR_M);
		/*! Decode operand and returns the number of bytes.
		*/
		void DecodeMODR_MForSegmentRegister(unsigned char MODR_M);
		/*! Decode operand and returns the number of bytes.
		*/
		void MakeByRegisterNumber(int dataSize,int regNum);
		/*! Make Immediate. */
		void MakeImm8(unsigned int imm);
		/*! Make Immediate. */
		void MakeImm16(unsigned int imm);
		/*! Make Immediate. */
		void MakeImm32(unsigned int imm);

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
		static void GetSizeQualifierToDisassembly(
			std::string &op1Qual,std::string &op2Qual,const Operand &op1,const Operand &op2);

		static std::string GetSegmentQualifierToDisassembly(int segOverride,const Operand &op);

		/*! Returns the size in number of bytes if the size can be determined.
		    Returns zero otherwise.
		*/
		unsigned int GetSize(void) const;
	};
	class OperandValue
	{
	public:
		unsigned int numBytes;
		unsigned char byteData[10];
	};


	State state;

	inline void SetDX(unsigned int value)
	{
		state.EDX&=0xffff0000;
		state.EDX|=value;
	}
	inline void SetDL(unsigned int value)
	{
		state.EDX&=0xffffff00;
		state.EDX|=value;
	}
	inline void SetDH(unsigned int value)
	{
		state.EDX&=0xffff00ff;
		state.EDX|=(value<<8);
	}

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
	    How the segment linear base address is set depends on the CPU mode,
	    and in the protected mode, it needs to look at GDT and LDT.
	    Therefore it needs a reference to memory.
	*/
	void LoadSegmentRegister(SegmentRegister &reg,unsigned int value,const Memory &mem);

	/*! Loads a segment register in real mode.
	*/
	void LoadSegmentRegisterRealMode(SegmentRegister &reg,unsigned int value);

	inline bool IsInRealMode(void) const
	{
		return (0==(state.CR0&CR0_PROTECTION_ENABLE));
	}

	/*! Returns a register value. 
	*/
	unsigned int GetRegisterValue(int reg) const;

	/*! Returns a register size in number of bytes. 
	*/
	static unsigned int GetRegisterSize(int reg);

	/*! Returns true if in 16-bit addressing mode.
	*/
	inline bool AddressingMode16Bit(void) const
	{
		if(true==IsInRealMode())
		{
			return true;
		}
		else
		{
			Abort("Protected mode not supported yet.");
		}
		return false;
	}

	/*! Returns true if Paging is enabled.
	*/
	inline bool PagingEnabled(void) const
	{
		return 0!=(state.CR0&CR0_PAGING_ENABLED);
	}

	/*! Returns true if the opCode needs one more byte to be fully qualified.
	*/
	bool OpCodeNeedsOneMoreByte(unsigned int firstByte) const;

	/*!
	*/
	unsigned long LinearAddressToPhysicalAddress(unsigned int linearAddr,const Memory &mem) const
	{
		Abort("Paging not supported yet.");
		return linearAddr;
	}

	/*! Fetch a byte. 
	*/
	inline unsigned int FetchByte(SegmentRegister seg,unsigned int offset,const Memory &mem) const
	{
		if(true==AddressingMode16Bit())
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

	/*! Store a byte.
	*/
	inline void StoreByte(Memory &mem,SegmentRegister seg,unsigned int offset,unsigned char byteData)
	{
		if(true==AddressingMode16Bit())
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

	/*! Fetch a byte from CS:[EIP+offset].
	*/
	inline unsigned int FetchInstructionByte(unsigned int offset,const Memory &mem) const
	{
		return FetchByte(state.CS,state.EIP+offset,mem);
	}

	/*! Fetch an instruction.
	*/
	inline Instruction FetchInstruction(const Memory &mem) const
	{
		return FetchInstruction(state.CS,state.EIP,mem);
	}
private:
	/*! Fetch an 8-bit operand.
	*/
	unsigned int FetchOperand8(Instruction &inst,SegmentRegister seg,unsigned int offset,const Memory &mem) const;
	/*! Fetch an 16-bit operand.
	*/
	unsigned int FetchOperand16(Instruction &inst,SegmentRegister seg,unsigned int offset,const Memory &mem) const;
	/*! Fetch an 32-bit operand.
	*/
	unsigned int FetchOperand32(Instruction &inst,SegmentRegister seg,unsigned int offset,const Memory &mem) const;
	/*! Fetch an operand defined by the RM byte.
	*/
	void FetchOperandRM(Instruction &inst,SegmentRegister seg,unsigned int offset,const Memory &mem) const;
	/*! Fetch operand(s) for the instruction.
	*/
	void FetchOperand(Instruction &inst,SegmentRegister seg,int offset,const Memory &mem) const;

public:
	/*! Fetch an instruction from specific segment and offset.
	*/
	Instruction FetchInstruction(const SegmentRegister seg,unsigned int offset,const Memory &mem) const;


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


	/*! Evaluates an operand.
	    destinationBytes is non zero if the operand that receives the value has a known size.
	    If the destination size depends on the source size, destinationBytes should be zero.
	*/
	OperandValue EvaluateOperand(
	    const Memory &mem,int addressSize,int segmentOverride,const Operand &op,int destinationBytes) const;


	/*!
	*/
	void StoreOperandValue(const Operand &dst,Memory &mem,int addressSize,int segmentOverride,OperandValue value);

};

/* } */
#endif
