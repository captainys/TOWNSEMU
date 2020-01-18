#ifndef I486_IS_INCLUDED
#define I486_IS_INCLUDED
/* { */

#include "cpu.h"
#include "ramrom.h"
#include "inout.h"

// References
// [1]  i486 Processor Programmers Reference Manual

class i486DX : public CPU
{
public:
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
		unsigned short selector;
		unsigned int linearBaseAddr;
		unsigned short limit;
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
		unsigned int DR0,DR1,DR2,DR3,R4,DR5,DR6,DR7;
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

		CR0_PAGING=               0x80000000,  // [1] Page 4-6
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

	/*! Loads a segment register.
	    How the segment linear base address is set depends on the CPU mode,
	    and in the protected mode, it needs to look at GDT and LDT.
	    Therefore it needs a reference to memory.
	*/
	void LoadSegmentRegister(SegmentRegister &reg,unsigned int value,const Memory &mem);

	/*! Loads a segment register in real mode.
	*/
	void LoadSegmentRegisterRealMode(SegmentRegister &reg,unsigned int value);


	unsigned long long RunOneInstruction(Memory &mem,InOut &io);
};

/* } */
#endif
