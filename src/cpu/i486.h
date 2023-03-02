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

#include <cstdint>
#include <iostream>
#include <map>
#include <string>
#include <type_traits>
#include <math.h>

#include "cpu.h"
#include "ramrom.h"
#include "inout.h"
#include "cpputil.h"
#include "i486inst.h"


// Define the following macro to keep track of Accessed flag (0x20) and Dirty flag (0x40) of page table entries
// #define TSUGARU_I486_UPDATE_PAGE_DA_FLAGS

// Define the following macro to cause UD exception when MOV CS,src
// #define TSUGARU_I486_MORE_EXCEPTION_HANDLING



#ifdef TSUGARU_I486_HIGH_FIDELITY
	#define TSUGARU_I486_FIDELITY_CLASS i486DXHighFidelity
#else
	#define TSUGARU_I486_FIDELITY_CLASS i486DXDefaultFidelity
#endif

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
	static unsigned char opCodeNeedOperandTable[I486_OPCODE_MAX+1];

	enum
	{
		I486_NUM_IOPORT=65536,
		MAX_REP_BUNDLE_COUNT=128,
		MAX_INSTRUCTION_LENGTH=15,  // Intel x86 instruction set 2.3.11 AVX Instruction Length, 80386 Programmer's Reference Manual 14.7 Redundant Prefix

		DESCRIPTOR_CACHE_SIZE=16384, // Even number=GDT  Odd number=LDT
		DESCRIPTOR_TO_INDEX_SHIFT=2, // TI bit will be bit0.

		LINEARADDR_TO_PAGE_SHIFT=12,
		PAGETABLE_CACHE_SIZE=0x00100000,
		PAGEINFO_FLAG_PRESENT=0b000000000001,
		PAGEINFO_FLAG_RW=     0b000000000010,
		PAGEINFO_FLAG_US=     0b000000000100,
		PAGEINFO_FLAG_PWT=    0b000000001000,
		PAGEINFO_FLAG_PCD=    0b000000010000,
		PAGEINFO_FLAG_A=      0b000000100000,
		PAGEINFO_FLAG_D=      0b000001000000,
		PAGEINFO_FLAG_AVAIL=  0b111000000000,

		// https://wiki.osdev.org/Descriptors
		//   0xxxx System Segment
		//   1000A Data Normal         Read-Only
		//   1001A Data Normal         Read/Write
		//   1010A Data Expand-Down    Read-Only
		//   1011A Data Expand-Down    Read/Write
		//   1100A Code Non-Conforming Execute-Only
		//   1101A Code Non-Conforming Readable
		//   1110A Code Conforming     Execute-Only
		//   1111A Code Conforming     Readable
		SEGTYPE_DATA_NORMAL_READONLY=       0b1000, // Data Normal         Read-Only
		SEGTYPE_DATA_NORMAL_RW=             0b1001, // Data Normal         Read/Write
		SEGTYPE_DATA_EXPAND_DOWN_READONLY=  0b1010, // Data Expand-Down    Read-Only
		SEGTYPE_DATA_EXPAND_DOWN_RW=        0b1011, // Data Expand-Down    Read/Write
		SEGTYPE_CODE_NONCONFORMING_EXECONLY=0b1100, // Code Non-Conforming Execute-Only
		SEGTYPE_CODE_NONCONFORMING_READABLE=0b1101, // Code Non-Conforming Readable
		SEGTYPE_CODE_CONFORMING_EXECONLY=   0b1110, // Code Conforming     Execute-Only
		SEGTYPE_CODE_CONFORMING_READABLE=   0b1111, // Code Conforming     Readable

		//                 AVR NIOODITSZ A P C
		//                 CMF0TPL      0 0 1
		EFLAGS_MASK=     0b1110111111111010111,
		EFLAGS_ALWAYS_ON=0b0000000000000000010,
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

	class SegmentProperty
	{
	public:
		uint32_t baseLinearAddr;
		uint32_t operandSize;
		uint32_t addressSize;
		uint32_t limit;
		uint16_t DPL=0;
		uint16_t attribBytes=0;

		void Serialize(std::vector <unsigned char> &data) const;
		bool Deserialize(const unsigned char *&data,unsigned int version);
	};

	class SegmentRegister : public SegmentProperty
	{
	public:
		uint16_t value;

		void Serialize(std::vector <unsigned char> &data) const;
		bool Deserialize(const unsigned char *&data,unsigned int version);
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

		void Serialize(std::vector <unsigned char> &data) const;
		bool Deserialize(const unsigned char *&data);
	};
	class SystemAddressRegisterAndSelector : public SystemAddressRegister
	{
	public:
		unsigned short selector;

		void Serialize(std::vector <unsigned char> &data) const;
		bool Deserialize(const unsigned char *&data);
	};
	enum
	{
		TSS_OFFSET_ESP0=0x04,
		TSS_OFFSET_SS0= 0x08,
		TSS_OFFSET_ESP1=0x0C,
		TSS_OFFSET_SS1= 0x10,
		TSS_OFFSET_ESP2=0x14,
		TSS_OFFSET_SS2= 0x18,
	};
	class TaskRegister : public SegmentRegister
	{
	public:
		unsigned int attrib;  // Should it keep attribute?

		void Serialize(std::vector <unsigned char> &data) const;
		bool Deserialize(const unsigned char *&data,unsigned int version);
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

	class OperandValueBase
	{
	public:
		enum
		{
			MAX_NUM_BYTES=10
		};

		unsigned int numBytes;
		unsigned char byteData[MAX_NUM_BYTES];
	};

	class FPUState
	{
	public:
		enum
		{
			STACK_LEN=8,

			STATUS_IE=    0x01,
			STATUS_DE=    0x02,
			STATUS_ZE=    0x04,
			STATUS_OE=    0x08,
			STATUS_UE=    0x10,
			STATUS_PE=    0x20,
			STATUS_SF=    0x40,
			STATUS_ES=    0x80,
			STATUS_C0=   0x100,
			STATUS_C1=   0x200,
			STATUS_C2=   0x400,
			STATUS_TOP= 0x3800,
			STATUS_C3=  0x4000,
			STATUS_BUSY=0x8000,
		};
		class Stack
		{
		public:
			double value=0.0;
			unsigned char tag=0;
		};

		bool enabled=false;
		uint16_t statusWord=0xFFFF;
		uint16_t controlWord=0xFFFF;
		uint16_t tagWord=0xFFFF;
		int stackPtr=8;
		Stack stack[STACK_LEN],nullRegister;

		/*! Initialize variables regardless of the enabled flag.
		    Does not change enabled flag.
		*/
		void Reset(void);
		/*! Initialize variables only if enabled==true.
		*/
		void FNINIT(void);
		bool ExceptionPending(void) const;
		unsigned int GetStatusWord(void) const;
		unsigned int GetControlWord(void) const;

		void BreakOnNan(i486DX &cpu,double value);

		unsigned int GetRC(void) const;
		double RoundToInteger(double src) const;

		void GetSTAsDouble(class i486DX &cpu,OperandValueBase &value);
		void GetSTAsFloat(class i486DX &cpu,OperandValueBase &value);
		void GetSTAsSignedInt(class i486DX &cpu,OperandValueBase &value);
		void GetSTAs80BitBCD(class i486DX &cpu,OperandValueBase &value);

		static int32_t IntFrom32Bit(const unsigned char byteData[]);
		static int64_t IntFrom64Bit(const unsigned char byteData[]);

		static void DoubleTo80Bit(OperandValueBase &value80,double src);
		static double DoubleFrom80Bit(const OperandValueBase &value80);
		static double DoubleFrom80Bit(const unsigned char value80[]);
		static double DoubleFrom80BitBCD(const unsigned char bcd80[]);

		static double DoubleFrom64Bit(const unsigned char byteData[]);

		static double DoubleFrom32Bit(const unsigned char byteData[]);

		bool Push(class i486DX &cpu,double value);
		void Pop(class i486DX &cpu);
		void Pop(class i486DX &cpu,int level);
		unsigned int NumFilled(void) const;

		FPUState();

		std::vector <std::string> GetStateText(void) const;

		inline Stack &ST(class i486DX &cpu)
		{
			return ST(cpu,0);
		}
		inline Stack &ST(class i486DX &cpu,int i)
		{
			if(stackPtr+i<STACK_LEN)
			{
				return stack[stackPtr+i];
			}
			// Raise exception.
			return nullRegister;
		}
		inline const Stack &ST(class i486DX &cpu) const
		{
			return ST(cpu,0);
		}
		inline const Stack &ST(class i486DX &cpu,int i) const
		{
			if(stackPtr+i<STACK_LEN)
			{
				return stack[stackPtr+i];
			}
			// Raise exception.
			return nullRegister;
		}
		inline void Compare(double ST,double SRC)
		{
			// DOS Extender tests the FPU by checking:
			//   -0.0==0.0
			// While apparently majority of the floating-point implementation
			// simply make it true, it is not guaranteed.
			// FPU should explicitly check this case, or DOS-Extender may fail in
			// some platforms.
			if(-0.0==ST)
			{
				ST=0.0;
			}
			if(-0.0==SRC)
			{
				SRC=0.0;
			}
			statusWord&=~(STATUS_C0|STATUS_C2|STATUS_C3);
			if(true==isnan(ST) || true==isnan(SRC))
			{
				statusWord=(STATUS_C0|STATUS_C2|STATUS_C3);
			}
			else if(ST>SRC)
			{
				// statusWord|=0;
			}
			else if(ST<SRC)
			{
				statusWord|=STATUS_C0;
			}
			else if(ST==SRC)
			{
				statusWord|=STATUS_C3;
			}
		}

		// Returns clocks passed.
		unsigned int F2XM1(i486DX &cpu);
		unsigned int FABS(i486DX &cpu);
		unsigned int FADD_m32real(i486DX &cpu,const unsigned char byteData[]);
		unsigned int FADD64(i486DX &cpu,const unsigned char byteData[]);
		unsigned int FADD_ST_STi(i486DX &cpu,int i);
		unsigned int FADDP_STi_ST(i486DX &cpu,int i);
		unsigned int FBLD(i486DX &cpu,const unsigned char byteData[]);
		unsigned int FCHS(i486DX &cpu);
		unsigned int FCLEX(i486DX &cpu);
		unsigned int FCOM(i486DX &cpu,int i);
		unsigned int FCOMP(i486DX &cpu,int i);
		unsigned int FCOMPP(i486DX &cpu);
		unsigned int FCOM_m32real(i486DX &cpu,const unsigned char byteData[]);
		unsigned int FCOMP_m32real(i486DX &cpu,const unsigned char byteData[]);
		unsigned int FCOM_m64real(i486DX &cpu,const unsigned char byteData[]);
		unsigned int FCOMP_m64real(i486DX &cpu,const unsigned char byteData[]);
		unsigned int FCOS(i486DX &cpu);
		unsigned int FILD_m32int(i486DX &cpu,const unsigned char byteData[]);
		unsigned int FILD_m64int(i486DX &cpu,const unsigned char byteData[]);
		unsigned int FDIV_m32real(i486DX &cpu,const unsigned char byteData[]);
		unsigned int FDIVP_STi_ST(i486DX &cpu,int i);
		unsigned int FDIV_m64real(i486DX &cpu,const unsigned char byteData[]);
		unsigned int FDIVR_m64real(i486DX &cpu,const unsigned char byteData[]);
		unsigned int FDIVR_m32real(i486DX &cpu,const unsigned char byteData[]);
		unsigned int FDIVRP_STi_ST(i486DX &cpu,int i);
		unsigned int FLD32(i486DX &cpu,const unsigned char byteData[]);
		unsigned int FLD64(i486DX &cpu,const unsigned char byteData[]);
		unsigned int FLD80(i486DX &cpu,const unsigned char byteData[]);
		unsigned int FLDCW(i486DX &cpu,uint16_t cw);
		unsigned int FLD_ST(i486DX &cpu,int i);
		unsigned int FLD1(i486DX &cpu);
		unsigned int FLDL2T(i486DX &cpu);
		unsigned int FLDL2E(i486DX &cpu);
		unsigned int FLDLN2(i486DX &cpu);
		unsigned int FLDPI(i486DX &cpu);
		unsigned int FLDZ(i486DX &cpu);
		unsigned int FMULP(i486DX &cpu,int i);
		unsigned int FMUL_ST_STi(i486DX &cpu,int i);
		unsigned int FMUL_m32real(i486DX &cpu,const unsigned char byteData[]);
		unsigned int FMUL_m64real(i486DX &cpu,const unsigned char byteData[]);
		unsigned int FPATAN(i486DX &cpu);
		unsigned int FPREM(i486DX &cpu);
		unsigned int FPTAN(i486DX &cpu);
		unsigned int FRNDINT(i486DX &cpu);
		unsigned int FSCALE(i486DX &cpu);
		unsigned int FSIN(i486DX &cpu);
		unsigned int FSINCOS(i486DX &cpu);
		unsigned int FSQRT(i486DX &cpu);
		unsigned int FSTP_STi(i486DX &cpu,int i);
		unsigned int FSUB_m32real(i486DX &cpu,const unsigned char byteData[]);
		unsigned int FSUB_m64real(i486DX &cpu,const unsigned char byteData[]);
		unsigned int FSUB_ST_STi(i486DX &cpu,int i);
		unsigned int FSUB_STi_ST(i486DX &cpu,int i);
		unsigned int FSUBP_STi_ST(i486DX &cpu,int i);
		unsigned int FSUBR_m32real(i486DX &cpu,const unsigned char byteData[]);
		unsigned int FSUBR_m64real(i486DX &cpu,const unsigned char byteData[]);
		unsigned int FSUBRP_STi_ST(i486DX &cpu,int i);
		unsigned int FTST(i486DX &cpu);
		unsigned int FXAM(i486DX &cpu);
		unsigned int FXCH(i486DX &cpu,int i);
		unsigned int FYL2X(i486DX &cpu);
	};

	class InterruptDescriptor : public FarPointer
	{
	public:
		unsigned short flags;
		inline unsigned int GetType(void) const
		{
			return (flags>>8)&0x1F;
		}
		inline unsigned int GetDPL(void) const
		{
			return (flags>>13)&3;
		}
	};

	class PageTableEntry
	{
	public:
		uint32_t dir,table;
	};
	class PageTableCache
	{
	public:
		PageTableEntry info;
		uint32_t valid=0;
		uint32_t makeIt64Bytes;
	};

	class State
	{
	public:
		unsigned int NULL_and_reg32[9];
	#ifdef YS_LITTLE_ENDIAN
		uint8_t *reg8Ptr[8];
	#endif

		State();

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
		// pageTableCache[i] is valid if pageTableCacheValidCounter<=pageTableCacheValid[i].
		// Page table cache is invalidated by incrementing pageTableCacheValidCounter.
		// When pageTableCacheValidCounter reaches 0xffffffff,
		// pageTableCacheValid and pageTableCacheValidCounter will be reset.
		uint32_t pageTableCacheValidCounter=1;
		PageTableCache pageTableCache[PAGETABLE_CACHE_SIZE];

		// Descriptor cache stores pointers for descriptors, not linear base address, limit, etc.
		// Some programs rewrites descriptor table after loaded by LGDT or LLDT.
		// For example, DOS Extender's malloc extends the limit of data segment (0x14).
		// Therefore, linear base, limit, and other attributes must be checked when
		// a value is set in a segment selector.
		//
		// Descriptor cache is invalidated on Reset, LLDT, LGDT, or when Page Table pointer
		// is set to CR3.
		//
		// Descriptor cache is referred and updated in function:
		//   i486DX::LoadSegmentRegisterTemplate::LoadProtectedModeDescriptor
		unsigned int descriptorCacheValidCounter=1;               // This must be cleared on state-load.
		unsigned int descriptorCacheValid[DESCRIPTOR_CACHE_SIZE]; // This must be cleared on state-load.
		unsigned char const *descriptorCache[DESCRIPTOR_CACHE_SIZE];    // This must be cleared on state-load.

		// Probably INT descriptor cache is very ineffective, and not worth implementing.
		// Majority of INT shot from the protected mode will cause the control down to
		// the real mode.  IDT will most likely be re-loaded when coming back to the
		// protected mode.

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
		uint32_t exceptionLinearAddr=0; // For EXCEPTION_PF

		void Serialize(std::vector <unsigned char> &data) const;
		bool Deserialize(const unsigned char *&data,uint32_t version);
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

		// Not listed as a prefix, but for sure works something like it.
		I486_OPCODE_NEED_SECOND_BYTE=0x0F,

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
	enum
	{
		PFFLAG_PAGE_LEVEL=1,
		PFFLAG_WRITE=2,
		PFFLAG_USER_MODE=4,
	};
	static std::string ExceptionTypeToStr(unsigned int exceptionType);

	class Operand;

	class Instruction
	{
	public:
		// Bytes cleared in Clear() >>
		unsigned char numBytes,fwait;
		unsigned short instPrefix;
		unsigned short segOverride;
		unsigned short operandLen;
		// Bytes cleared in Clear() <<

		unsigned short operandSize;
		unsigned short addressSize;
		unsigned short codeAddressSize; // 2020/03/07 Turned out, I need to keep code-segment address size for fetching instructions.

		unsigned int opCode;
		unsigned char operand[12];  // Is 8 bytes maximum?  Maybe FPU instruction use up to 10 bytes.  I'll see.
		unsigned char imm[4];       // Probably 4 bytes is good enough.

		inline void Clear(void)
		{
			numBytes=0;
			fwait=0;
			instPrefix=0;
			segOverride=0;
			operandLen=0;
		}

		std::string Disassemble(const Operand &op1,const Operand &op2,SegmentRegister reg,unsigned int offset,const class i486SymbolTable &symTable,const std::map <unsigned int,std::string> &ioTable) const;
	private:
		/* operandSize is 8, 16, or 32 */
		std::string DisassembleTypicalOneOperand(std::string inst,const Operand &op,int operandSize,uint32_t CS,uint32_t EIP,const i486SymbolTable &symTable) const;
		std::string DisassembleTypicalOneOperandAndImm(std::string inst,const Operand &op,unsigned int imm,int operandSize,uint32_t CS,uint32_t EIP,const i486SymbolTable &symTable) const;
		std::string DisassembleTypicalRM8_I8(std::string inst,const Operand &op1,unsigned int I8,uint32_t CS,uint32_t EIP,const i486SymbolTable &symTable) const;
		std::string DisassembleTypicalRM_I8(std::string inst,const Operand &op1,unsigned int I8,uint32_t CS,uint32_t EIP,const i486SymbolTable &symTable) const;
		std::string DisassembleTypicalTwoOperands(std::string inst,const Operand &op1,const Operand &op2,uint32_t CS,uint32_t EIP,const i486SymbolTable &symTable) const;
		std::string DisassembleTypicalOneImm(std::string inst,unsigned int imm,int operandSize,uint32_t CS,uint32_t EIP,const i486SymbolTable &symTable) const;

		std::string DisassembleIOLabel(unsigned int CS,unsigned int EIP,const i486SymbolTable &symTable,const std::map <unsigned int,std::string> &ioTable,unsigned int imm) const;
		std::string DisassembleImmAsASCII(unsigned int CS,unsigned int EIP,const i486SymbolTable &symTable,unsigned int imm,unsigned int numBytes) const;

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
		OPER_REG32, // EAX,ECX,EDX,EBX,ESP,EBP,ESI,EDI
		OPER_REG16, // AX,CX,DX,BX,SP,BP,SI,DI
		OPER_REG8,  // AL,CL,DL,BL,AH,CH,DH,BH
		OPER_SREG,  // ES,CS,SS,DS,FS,GS,

		OPER_CR0,
		OPER_CR1,
		OPER_CR2,
		OPER_CR3,
		OPER_DR0,
		OPER_DR1,
		OPER_DR2,
		OPER_DR3,
		OPER_DR4,
		OPER_DR5,
		OPER_DR6,
		OPER_DR7,
		OPER_TEST0,
		OPER_TEST1,
		OPER_TEST2,
		OPER_TEST3,
		OPER_TEST4,
		OPER_TEST5,
		OPER_TEST6,
		OPER_TEST7,
	};
	class Operand
	{
	public:
		unsigned char operandType,indexShift; // Hopefully cleared in Clear instruction with one instruction.

		// For OPER_REG* operand type
		int reg;
		// For OPER_ADDR and OPER_FARADDR operand type
		short  baseReg,indexReg,offsetBits;
		int offset;
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
		std::string Disassemble(uint32_t cs,uint32_t eip,const i486SymbolTable &symTable) const;
	private:
		std::string DisassembleAsAddr(uint32_t cs,uint32_t eip,const i486SymbolTable &symTable) const;
		std::string DisassembleAsFarAddr(uint32_t cs,uint32_t eip,const i486SymbolTable &symTable) const;
		std::string DisassembleAsReg(void) const;

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

	/*! 
	*/
	class InstructionAndOperand
	{
	public:
		Instruction inst;
		Operand op1;
		Operand op2;
	};

	/*! OperandValue class is an evaluated operand value, or a value to be stored to
	    the destination described by the operand.
	    In 80486, operand itself may not know its size if it is an address operand.
	    The size is defined by the instruction an the operand size.
	    This OperandValue is after the operand size is evaluated therefore it knows its size.
	*/
	class OperandValue : public OperandValueBase
	{
	public:
		using OperandValueBase::MAX_NUM_BYTES;
		using OperandValueBase::numBytes;
		using OperandValueBase::byteData;

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
		#ifdef YS_TWOS_COMPLEMENT
			SetDword(*((unsigned int *)&dword));
		#else
			unsigned int uDword;
			// I probably can just say uDword=*((unsigned int *)&dword).
			// But, officially the behavior is undefined by C/C++.
			uDword=(dword&0x7FFFFFFF);
			if(dword<0)
			{
				uDword|=0x80000000;
			}
			SetDword(uDword);
		#endif
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
		// INTNum0,AX0 is the original INT that caused the exception.
		unsigned short INTNum0=0xFFFF,AX0=0xFFFF;
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

#ifdef YS_LITTLE_ENDIAN
	#define INT_LOW_WORD(i) (*((uint16_t *)&(i)))
	#define INT_LOW_BYTE(i) (((unsigned char *)&(i))[0])
	#define INT_SECOND_BYTE(i) (((unsigned char *)&(i))[1])

	#define SET_INT_LOW_WORD(i,value) (*((uint16_t *)&(i)))=(value)
	#define SET_INT_LOW_BYTE(i,value) (((unsigned char *)&(i))[0])=(value)
	#define SET_INT_SECOND_BYTE(i,value) (((unsigned char *)&(i))[1])=(value)

	#define REG_LOW_WORD(reg) INT_LOW_WORD(state.NULL_and_reg32[reg])
	#define REG_LOW_BYTE(reg) INT_LOW_BYTE(state.NULL_and_reg32[reg])
	#define REG_SECOND_BYTE(reg) INT_SECOND_BYTE(state.NULL_and_reg32[reg])

	#define SET_REG_LOW_WORD(reg,value) REG_LOW_WORD(reg)=(value)
	#define SET_REG_LOW_BYTE(reg,value) REG_LOW_BYTE(reg)=(value);
	#define SET_REG_SECOND_BYTE(reg,value) REG_SECOND_BYTE(reg)=(value);
#else
	#define INT_LOW_WORD(i) ((i)&0xFFFF)
	#define INT_LOW_BYTE(i) ((i)&0xFF)
	#define INT_SECOND_BYTE(i) (((i)>>8)&0xFF)

	#define SET_INT_LOW_WORD(i,value) {(i)&=0xFFFF0000;(i)|=((value)&0xFFFF);}
	#define SET_INT_LOW_BYTE(i,value) {(i)&=0xFFFFFF00;(i)|=((value)&0xFF);}
	#define SET_INT_SECOND_BYTE(i,value) {(i)&=0xFFFF00FF;(i)|=(((value)&0xFF)<<8);}

	#define REG_LOW_WORD(reg) INT_LOW_WORD(state.NULL_and_reg32[reg])
	#define REG_LOW_BYTE(reg) INT_LOW_BYTE(state.NULL_and_reg32[reg])
	#define REG_SECOND_BYTE(reg) INT_SECOND_BYTE(state.NULL_and_reg32[reg])

	#define SET_REG_LOW_WORD(reg,value) SET_INT_LOW_WORD(state.NULL_and_reg32[reg],value)
	#define SET_REG_LOW_BYTE(reg,value) SET_INT_LOW_BYTE(state.NULL_and_reg32[reg],value)
	#define SET_REG_SECOND_BYTE(reg,value) SET_INT_SECOND_BYTE(state.NULL_and_reg32[reg],value)
#endif

	// I was hoping the compiler was able to recognize a pattern:
	//    x&=0xFFFF0000;
	//    x|=(y&0xFFFF);
	// and turn into:
	//    *((unsigned short *)&x)=y;
	// .  But apparently not.

	inline unsigned int GetEAX(void) const
	{
		return state.EAX();
	}
	inline unsigned int GetAX(void) const
	{
		return REG_LOW_WORD(REG_EAX);
	}
	inline unsigned int GetAL(void) const
	{
		return REG_LOW_BYTE(REG_EAX);
	}
	inline unsigned int GetAH(void) const
	{
		return REG_SECOND_BYTE(REG_EAX);
	}
	inline void SetEAX(unsigned int value)
	{
		state.EAX()=value;
	}
	inline void SetAX(unsigned int value)
	{
		SET_REG_LOW_WORD(REG_EAX,value);
	}
	inline void SetAL(unsigned int value)
	{
		SET_REG_LOW_BYTE(REG_EAX,value);
	}
	inline void SetAH(unsigned int value)
	{
		SET_REG_SECOND_BYTE(REG_EAX,value);
	}


	inline unsigned int GetEBX(void) const
	{
		return state.EBX();
	}
	inline unsigned int GetBX(void) const
	{
		return REG_LOW_WORD(REG_EBX);
	}
	inline unsigned int GetBL(void) const
	{
		return REG_LOW_BYTE(REG_EBX);
	}
	inline unsigned int GetBH(void) const
	{
		return REG_SECOND_BYTE(REG_EBX);
	}
	inline void SetEBX(unsigned int value)
	{
		state.EBX()=value;
	}
	inline void SetBX(unsigned int value)
	{
		SET_REG_LOW_WORD(REG_EBX,value);
	}
	inline void SetBL(unsigned int value)
	{
		SET_REG_LOW_BYTE(REG_EBX,value);
	}
	inline void SetBH(unsigned int value)
	{
		SET_REG_SECOND_BYTE(REG_EBX,value);
	}


	inline unsigned int GetECX(void) const
	{
		return state.ECX();
	}
	inline unsigned int GetCX(void) const
	{
		return REG_LOW_WORD(REG_ECX);
	}
	inline unsigned int GetCL(void) const
	{
		return REG_LOW_BYTE(REG_ECX);
	}
	inline unsigned int GetCH(void) const
	{
		return REG_SECOND_BYTE(REG_ECX);
	}
	inline void SetECX(unsigned int value)
	{
		state.ECX()=value;
	}
	inline void SetCX(unsigned int value)
	{
		SET_REG_LOW_WORD(REG_ECX,value);
	}
	inline void SetCL(unsigned int value)
	{
		SET_REG_LOW_BYTE(REG_ECX,value);
	}
	inline void SetCH(unsigned int value)
	{
		SET_REG_SECOND_BYTE(REG_ECX,value);
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
		return REG_LOW_WORD(REG_EDX);
	}
	inline unsigned int GetDL(void) const
	{
		return REG_LOW_BYTE(REG_EDX);
	}
	inline unsigned int GetDH(void) const
	{
		return REG_SECOND_BYTE(REG_EDX);
	}
	inline void SetEDX(unsigned int value)
	{
		state.EDX()=value;
	}
	inline void SetDX(unsigned int value)
	{
		SET_REG_LOW_WORD(REG_EDX,value);
	}
	inline void SetDL(unsigned int value)
	{
		SET_REG_LOW_BYTE(REG_EDX,value);
	}
	inline void SetDH(unsigned int value)
	{
		SET_REG_SECOND_BYTE(REG_EDX,value);
	}

	inline unsigned int GetSI(void) const
	{
		return REG_LOW_WORD(REG_ESI);
	}
	inline void SetSI(unsigned int value)
	{
		SET_REG_LOW_WORD(REG_ESI,value);
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
		return REG_LOW_WORD(REG_EDI);
	}
	inline void SetDI(unsigned int value)
	{
		SET_REG_LOW_WORD(REG_EDI,value);
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
		return REG_LOW_WORD(REG_ESP);
	}
	inline void SetSP(unsigned int value)
	{
		SET_REG_LOW_WORD(REG_ESP,value);
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
		return REG_LOW_WORD(REG_EBP);
	}
	inline void SetBP(unsigned int value)
	{
		SET_REG_LOW_WORD(REG_EBP,value);
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
		SET_INT_LOW_WORD(state.EIP,value);
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
			SET_INT_LOW_WORD(state.EFLAGS,value);
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

	inline bool CheckParity(unsigned int lowByte)
	{
		return ParityTable[INT_LOW_BYTE(lowByte)];
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
			SET_INT_LOW_WORD(state.EDI(),DI);
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
			SET_INT_LOW_WORD(state.ESI(),SI);
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
			SET_INT_LOW_WORD(state.EDI(),DI);
			SET_INT_LOW_WORD(state.ESI(),SI);
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
			InvalidatePageTableCache();
			InvalidateDescriptorCache();
			state.SSESPWindow.CleanUp();
			state.CSEIPWindow.CleanUp();
		}
	}
	void ClearPageTableCache(void);
	void InvalidatePageTableCache(void);

	void ClearDescriptorCache(void);
	void InvalidateDescriptorCache(void);


	/*! Issue an interrupt.
	    This function does not check Interrupt-Enabled Flag (IF),
	    nor check mask state of PIC.
	    This function should be called when it is certain that the interrupt 
	    needs to be handled by the CPU.

		numInstbyte should tell how many bytes that EIP should be displaced for the return address.
		For hardware-interrupt, it should be 1.
		For INT, it should be 2.
		For INT3 and INTO, it should be 1.
		For interrupt by exception, it should be 0 to allow re-try.

		numInstBytes is divided into numInstBytesFoReturn and numInstBytesForCallStack.
		The two values should be equal excpet when INT is from an exception.
		The exception handler may correct the exception source and return to the same instruction that caused the exception,
		or may handle what the instruction meant to do and return to the next instruction.
		In that situation, numInstBytesForReturn should be zero, and numInstBytesForCallStack should be the
		number of instruction-bytes that caused the exception.

		numInstBytesForReturn should be the number of instruction bytes that caused an interrupt, or zero if it is from PIC or exception.
		numInstBytesForCallStack should be the number of instruction bytes that caused an interrupt, or zero if it is from PIC.
	*/
	inline void Interrupt(unsigned int intNum,Memory &mem,unsigned int numInstBytesForReturn,unsigned int numInstBytesForCallStack,bool SWI);
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

	/*! Print Page Translation details
	*/
	void PrintPageTranslation(const Memory &mem,uint32_t linearAddr) const;

private:
	template <class CPUCLASS>
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
	InterruptDescriptor GetInterruptDescriptor(unsigned int INTNum,Memory &mem);

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

	/*! Returns page info read from the memory.  It does not look at the page-table cache.
	*/
	inline PageTableEntry ReadPageInfo(unsigned int linearAddr,const Memory &mem) const
	{
		PageTableEntry info;

		uint32_t pageDirectoryIndex=((linearAddr>>22)&1023);
		uint32_t pageTableIndex=((linearAddr>>12)&1023);

		auto pageDirectoryPtr=state.GetCR(3)&0xFFFFF000;
		info.dir=mem.FetchDword(pageDirectoryPtr+(pageDirectoryIndex<<2));
		if(0==(info.dir&1))
		{
			info.table=0;
			return info;
		}

		const unsigned int pageTablePtr=(info.dir&0xFFFFF000);
		info.table=mem.FetchDword(pageTablePtr+(pageTableIndex<<2));
		return info;
	}

	/*! Called from LinearAddressToPhysicalAddressRead if TSUGARU_I486_UPDATE_PAGE_DA_FLAGS is defined.
	*/
	inline void MarkPageAccessed(unsigned int linearAddr,Memory &mem)
	{
		uint32_t pageDirectoryIndex=((linearAddr>>22)&1023);
		uint32_t pageTableIndex=((linearAddr>>12)&1023);

		auto pageDirectoryPtr=state.GetCR(3)&0xFFFFF000;
		auto pageTableInfo=mem.FetchDword(pageDirectoryPtr+(pageDirectoryIndex<<2));
		if(0==(pageTableInfo&1))
		{
			return;
		}

		const unsigned int pageTablePtr=(pageTableInfo&0xFFFFF000);
		unsigned int pageInfo=mem.FetchDword(pageTablePtr+(pageTableIndex<<2));
		if(0==(pageInfo&1))
		{
			return;
		}

		pageTableInfo|=PAGEINFO_FLAG_A;
		pageInfo|=PAGEINFO_FLAG_A;
		mem.StoreDword(pageDirectoryPtr+(pageDirectoryIndex<<2),pageTableInfo);
		mem.StoreDword(pageTablePtr+(pageTableIndex<<2),pageInfo);
	}

	/*! Called from LinearAddressToPhysicalAddressWrite if TSUGARU_I486_UPDATE_PAGE_DA_FLAGS is defined.
	*/
	inline void MarkPageDirty(unsigned int linearAddr,Memory &mem)
	{
		uint32_t pageDirectoryIndex=((linearAddr>>22)&1023);
		uint32_t pageTableIndex=((linearAddr>>12)&1023);

		auto pageDirectoryPtr=state.GetCR(3)&0xFFFFF000;
		auto pageTableInfo=mem.FetchDword(pageDirectoryPtr+(pageDirectoryIndex<<2));
		if(0==(pageTableInfo&1))
		{
			return;
		}

		const unsigned int pageTablePtr=(pageTableInfo&0xFFFFF000);
		unsigned int pageInfo=mem.FetchDword(pageTablePtr+(pageTableIndex<<2));
		if(0==(pageInfo&1))
		{
			return;
		}

		pageTableInfo|=PAGEINFO_FLAG_A|PAGEINFO_FLAG_D;
		pageInfo|=PAGEINFO_FLAG_A|PAGEINFO_FLAG_D;
		mem.StoreDword(pageDirectoryPtr+(pageDirectoryIndex<<2),pageTableInfo);
		mem.StoreDword(pageTablePtr+(pageTableIndex<<2),pageInfo);
	}




	/*!
	*/
	inline unsigned long LinearAddressToPhysicalAddress(
	    unsigned int &exceptionType,unsigned int &exceptionCode,unsigned int linearAddr,const Memory &mem) const
	{
		auto pageIndex=(linearAddr>>LINEARADDR_TO_PAGE_SHIFT);

		PageTableEntry pageInfo;
		if(state.pageTableCache[pageIndex].valid<state.pageTableCacheValidCounter)
		{
			pageInfo=ReadPageInfo(linearAddr,mem);
		}
		else
		{
			pageInfo=state.pageTableCache[pageIndex].info;
		}

		if(0!=(pageInfo.table&PAGEINFO_FLAG_PRESENT))
		{
			auto offset=(linearAddr&4095);
			auto physicalAddr=(pageInfo.table&0xFFFFF000)+offset;
			exceptionType=EXCEPTION_NONE;
			return physicalAddr;
		}
		exceptionType=EXCEPTION_PF;
		exceptionCode=0; //I should have added a flag for read/write
		return 0;
	}

	/*! Convert physicall address to the first-matching linear address.
	    For debugging purpose only.
	*/
	unsigned int PhysicalAddressToLinearAddress(unsigned physAddr,const Memory &mem) const;


	/*! x86 keeps track of accessed page and written page in bit 5 (access) and bit 6 (write) of
	    page-table entries.  Therefore, page-translation for read and write needs to do different
	    things.
	*/
	inline unsigned long LinearAddressToPhysicalAddressRead(unsigned int linearAddr,Memory &mem);
	inline unsigned long LinearAddressToPhysicalAddressWrite(unsigned int linearAddr,Memory &mem);


	/*! Push a value.
	*/
	void Push16(Memory &mem,unsigned int value);
	void Push32(Memory &mem,unsigned int value);
	inline void Push(Memory &mem,unsigned int operandSize,unsigned int value)
	{
		if(16==operandSize)
		{
			Push16(mem,value);
		}
		else
		{
			Push32(mem,value);
		}
	}

	/*! Pop a value.
	*/
	unsigned int Pop16(Memory &mem);
	unsigned int Pop32(Memory &mem);
	inline unsigned int Pop(Memory &mem,unsigned int operandSize)
	{
		if(16==operandSize)
		{
			return Pop16(mem);
		}
		else
		{
			return Pop32(mem);
		}
	}

	/*! Push two values.
	*/
	void Push(Memory &mem,unsigned int operandSize,uint32_t firstPush,uint32_t secondPush);

	/*! Pop two values.
	*/
	void Pop(uint32_t &firstPop,uint32_t &secondPop,Memory &mem,unsigned int operandSize);

	/*! Push three values.
	*/
	void Push(Memory &mem,unsigned int operandSize,uint32_t firstPush,uint32_t secondPush,uint32_t thirdPush);

	/*! Pop three values.
	*/
	void Pop(uint32_t &firstPop,uint32_t &secondPop,uint32_t &thirdPop,Memory &mem,unsigned int operandSize);

private:
	// This function is a helper function for Push and Pop.
	// It checks if the cached SSESPWindow covers the required number of bytes.
	// If not, it tries to re-cache SSESPWindow.
	// If the number of bytes in the stack is accessible from the pointer, it returns the pointer.
	// If not, it returns nullptr.
	inline unsigned char *GetStackAccessPointer(Memory &mem,uint32_t linearAddr,unsigned int numBytes);

public:
	/*! Fetch a byte. 
	    Turned out mem cannot be const.  Fetching byte will update page-table entry A flag.
	*/
	inline unsigned int FetchByte(unsigned int addressSize,const SegmentRegister &seg,unsigned int offset,Memory &mem);

	/*! Fetch a word.
	    Turned out mem cannot be const.  Fetching byte will update page-table entry A flag.
	*/
	inline unsigned int FetchWord(unsigned int addressSize,const SegmentRegister &seg,unsigned int offset,Memory &mem);

	/*! Fetch a dword.
	    Turned out mem cannot be const.  Fetching byte will update page-table entry A flag.
	*/
	inline unsigned int FetchDword(unsigned int addressSize,const SegmentRegister &seg,unsigned int offset,Memory &mem);

	/*! Fetch a byte, word, or dword.
	    Function name is left as FetchWordOrDword temporarily for the time being.
	    Will be unified to FetchByteWordOrDword in the future.
	    Turned out mem cannot be const.  Accessing memory will update page-table entry A flag.
	*/
	inline unsigned int FetchWordOrDword(unsigned int operandSize,unsigned int addressSize,const SegmentRegister &seg,unsigned int offset,Memory &mem)
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
	inline unsigned int FetchByteWordOrDword(unsigned int operandSize,unsigned int addressSize,const SegmentRegister &seg,unsigned int offset,Memory &mem)
	{
		return FetchWordOrDword(operandSize,addressSize,seg,offset,mem);
	}

	/*! Fetch a byte by linear address.
	*/
	inline unsigned int FetchByteByLinearAddress(Memory &mem,unsigned int linearAddr)
	{
		if(true==PagingEnabled())
		{
			linearAddr=LinearAddressToPhysicalAddressRead(linearAddr,mem);
		}
		return mem.FetchByte(linearAddr);
	}

	/*! Returns const memory window from SEG:OFFSET.
	    This may raise page fault depending on SEG:OFFSET.
	    Turned out mem cannot be const.  Accessing memory will update page-table entry A flag.
	*/
	inline MemoryAccess::ConstMemoryWindow GetConstMemoryWindow(unsigned int addressSize,const SegmentRegister &seg,unsigned int offset,Memory &mem)
	{
		offset&=AddressMask((unsigned char)addressSize);
		auto linearAddr=seg.baseLinearAddr+offset;
		return GetConstMemoryWindowFromLinearAddress(linearAddr,mem);
	}

	/*! Returns const memory window from a linear address.
	    This may raise page fault depending on SEG:OFFSET.
	    Turned out mem cannot be const.  Accessing memory will update page-table entry A flag.
	*/
	inline MemoryAccess::ConstMemoryWindow GetConstMemoryWindowFromLinearAddress(unsigned int linearAddr,Memory &mem)
	{
		auto physAddr=linearAddr;
		if(true==PagingEnabled())
		{
			physAddr=LinearAddressToPhysicalAddressRead(linearAddr,mem);
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
	inline void StoreByte(Memory &mem,int addressSize,const SegmentRegister &seg,unsigned int offset,unsigned char data);

	/*! Store a word.
	*/
	inline void StoreWord(Memory &mem,int addressSize,const SegmentRegister &seg,unsigned int offset,unsigned int data);

	/*! Store a dword.
	*/
	inline void StoreDword(Memory &mem,int addressSize,const SegmentRegister &seg,unsigned int offset,unsigned int data);



	/*! Store a word or dword.  Operand size must be 16 or 32.
	*/
	inline void StoreWordOrDword(Memory &mem,unsigned int operandSize,int addressSize,const SegmentRegister &seg,unsigned int offset,unsigned int data);

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
	inline unsigned int FetchInstructionByte(unsigned int offset,Memory &mem)
	{
		return FetchByte(state.CS().addressSize,state.CS(),state.EIP+offset,mem);
	}

	/*! Fetch an instruction.
	    This function may raise page fault.
	*/
	inline void FetchInstruction(MemoryAccess::ConstMemoryWindow &memWin,InstructionAndOperand &instOp,Memory &mem)
	{
		return FetchInstruction(memWin,instOp,state.CS(),state.EIP,mem);
	}
	/*! Fetch an instruction.
	    It will not affect the CPU state.
	*/
	inline void DebugFetchInstruction(MemoryAccess::ConstMemoryWindow &memWin,InstructionAndOperand &instOp,const Memory &mem) const
	{
		return DebugFetchInstruction(memWin,instOp,state.CS(),state.EIP,mem);
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

	    See Interrupt() function for numInstBytes.
	*/
	void HandleException(bool,Memory &mem,unsigned int numInstBytesForCallStack);

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
	inline void FetchOperand8(Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,Memory &mem);
	/*! Peek an 8-bit operand.  Returns the number of bytes fetched.
	    It does not push inst.operandLen and this->numBytes by 1 byte.
	*/
	inline void PeekOperand8(unsigned int &operand,const Instruction &inst,const MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,Memory &mem);
	/*! Fetch an 16-bit operand  Returns the number of bytes fetched..
	*/
	inline void FetchOperand16(Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,Memory &mem);
	/*! Fetch an 32-bit operand.  Returns the number of bytes fetched.
	*/
	inline void FetchOperand32(Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,Memory &mem);
	/*! Fetch an 16- or 32-bit operand.  Length fetched depends on inst.operandSize.
	    Returns the number of bytes fetched.
	*/
	inline unsigned int FetchOperand16or32(Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,Memory &mem);


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



	/*! Fetch an operand defined by the RM byte and decode.
	    Returns the number of bytes fetched.
	*/
	template <class CPUCLASS,class MEMCLASS,class FUNCCLASS>
	inline static unsigned int FetchOperandRMandDecode(
	    Operand &op,int addressSize,int dataSize,
	    CPUCLASS &cpu,Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,MEMCLASS &mem);


	/*! Fetch an 8-bit operand.
	    It pushes inst.operandLen and this->numBytes by 1 byte.
	*/
	inline void FetchImm8(Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,Memory &mem);
	/*! Fetch an 16-bit operand.
	*/
	inline void FetchImm16(Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,Memory &mem);
	/*! Fetch an 32-bit operand.
	*/
	inline void FetchImm32(Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,Memory &mem);
	/*! Fetch an 16- or 32-bit operand.  Length fetched depends on inst.operandSize.
	    Returns the number of bytes fetched.
	*/
	inline unsigned int FetchImm16or32(Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,Memory &mem);

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
	template <class CPUCLASS,class MEMCLASS,class FUNCCLASS>
	static void FetchOperand(CPUCLASS &cpu,InstructionAndOperand &instOp,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,int offset,MEMCLASS &mem,unsigned int defOperSize,unsigned int defAddrSize);

public:
	/*! Fetch an instruction from specific segment and offset.
	*/
	inline void FetchInstruction(MemoryAccess::ConstMemoryWindow &memWin,InstructionAndOperand &instOp,const SegmentRegister &CS,unsigned int offset,Memory &mem)
	{
		const unsigned int operandSize=*CSOperandSizePointer[Return0InRealMode1InProtectedMode()];
		const unsigned int addressSize=*CSAddressSizePointer[Return0InRealMode1InProtectedMode()];
		return FetchInstruction(memWin,instOp,CS,offset,mem,operandSize,addressSize);
	}
	/*! Fetch an instruction from specific segment and offset.
	    It will not affect the CPU state including exceptions.
	*/
	inline void DebugFetchInstruction(MemoryAccess::ConstMemoryWindow &memWin,InstructionAndOperand &instOp,const SegmentRegister &CS,unsigned int offset,const Memory &mem) const
	{
		const unsigned int operandSize=*CSOperandSizePointer[Return0InRealMode1InProtectedMode()];
		const unsigned int addressSize=*CSAddressSizePointer[Return0InRealMode1InProtectedMode()];
		return DebugFetchInstruction(memWin,instOp,CS,offset,mem,operandSize,addressSize);
	}
private:
	class BurstModeFetchInstructionFunctions;
	class DebugFetchInstructionFunctions;
	class RealFetchInstructionFunctions;
	template <class CPUCLASS,class MEMCLASS,class FUNCCLASS,class BURSTMODEFUNCCLASS>
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
	    InstructionAndOperand &instOp,
	    const SegmentRegister &CS,unsigned int offset,Memory &mem,unsigned int defOperSize,unsigned int defAddrSize);

	/*! Fetch an instruction from specific segment and offset with given default operand size and address size.
	    It will not affect the CPU status including exceptions.
	*/
	void DebugFetchInstruction(
	    MemoryAccess::ConstMemoryWindow &memWin,
	    InstructionAndOperand &instOp,
	    const SegmentRegister &CS,unsigned int offset,const Memory &mem,unsigned int defOperSize,unsigned int defAddrSize) const;

private:
	inline unsigned int FetchInstructionByte(MemoryAccess::ConstPointer &ptr,unsigned int addressSize,const SegmentRegister &seg,unsigned int offset,Memory &mem)
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
	inline unsigned int PeekInstructionByte(const MemoryAccess::ConstPointer &ptr,unsigned int addressSize,const SegmentRegister &seg,unsigned int offset,Memory &mem)
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
	inline void FetchInstructionTwoBytes(unsigned char buf[2],MemoryAccess::ConstPointer &ptr,unsigned int addressSize,const SegmentRegister &seg,unsigned int offset,Memory &mem)
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
	inline void FetchInstructionFourBytes(unsigned char buf[4],MemoryAccess::ConstPointer &ptr,unsigned int addressSize,const SegmentRegister &seg,unsigned int offset,Memory &mem)
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
	template <typename T, typename _ = typename std::enable_if<std::is_unsigned<T>::value>::type>
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
	template <typename T, typename _ = typename std::enable_if<std::is_unsigned<T>::value>::type>
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


	/*! Returns a pointer to the operand.
	    Can only be used for OPER_ADDR or OPER_REG.
	    If available, it will reduce operand-type check for evaluate operand -> store operand
	    type instructions.
	    If there is a possibility of crossing the page boundary, it will return nullptr.
	    *** Also, if it was memory, it assumes it will write to the address. ***
	*/
	inline uint8_t *GetOperandPointer(Memory &mem,int addressSize,int segmentOverride,const Operand &op);


	/*! Evaluates an operand.
	    destinationBytes is non zero if the operand that receives the value has a known size.
	    If the destination size depends on the source size, destinationBytes should be zero.

		When operandType is OPER_ADDR, destinationBytes parameter defines the number of bytes fetched from the memory.
		It is significant when FWORD PTR (4 bytes in Real Mode, 6 bytes in Protected Mode) needs to be
		fetched.  First 2 or 4 bytes from FWORD PTR will give offset, and the last 2 bytes give segment.
		In which case returned OperandValue will return correct value for GetAsDword, GetAsWord, and GetFwordSegment.
	*/
	OperandValue EvaluateOperand(
	    Memory &mem,int addressSize,int segmentOverride,const Operand &op,int destinationBytes);

	/*! Evaluate an operand when it is known to be Reg8 or Mem8.
	*/
	uint8_t EvaluateOperandRegOrMem8(Memory &mem,int addressSize,int segmentOverride,const Operand &op);

	/*! Evaluate an operand when it is known to be Reg16 or Mem16.
	*/
	uint16_t EvaluateOperandRegOrMem16(Memory &mem,int addressSize,int segmentOverride,const Operand &op);

	/*! Evaluate an operand when it is known to be Reg32 or Mem32.
	*/
	uint32_t EvaluateOperandRegOrMem32(Memory &mem,int addressSize,int segmentOverride,const Operand &op);

	/*! Evaluate an operand when it is known to be either a register or memory.
	*/
	OperandValue EvaluateOperandReg16OrReg32OrMem(
	    Memory &mem,int addressSize,int segmentOverride,const Operand &op,int destinationBytes);

	/*! Evaluate operand as an 8-bit operand.
	    Use EvaluateOperandRegOrMem8 instead.
	*/
	OperandValue EvaluateOperand8(
	    Memory &mem,int addressSize,int segmentOverride,const Operand &op);

	/*! Evaluate operand as an 64-bit operand (double).
	*/
	OperandValue EvaluateOperand64(
	    Memory &mem,int addressSize,int segmentOverride,const Operand &op);

	/*! Evaluate operand as an 80-bit operand (80-bit floating point).
	*/
	OperandValue EvaluateOperand80(
	    Memory &mem,int addressSize,int segmentOverride,const Operand &op);


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

	/*! Stores value to the destination operand, when the operand is known to be reg8 or mem8. */
	void StoreOperandValueRegOrMem8(const Operand &dst,Memory &mem,int addressSize,int segmentOverride,uint8_t value);

	/*! Stores value to the destination operand, when the operand is known to be reg16 or mem16. */
	void StoreOperandValueRegOrMem16(const Operand &dst,Memory &mem,int addressSize,int segmentOverride,uint16_t value);

	/*! Stores value to the destination operand, when the operand is known to be reg32 or mem32. */
	void StoreOperandValueRegOrMem32(const Operand &dst,Memory &mem,int addressSize,int segmentOverride,uint32_t value);

	/*! Stores value to the destination described by the operand
	    when the operand is known to be reg16, reg32, or mem.
	*/
	void StoreOperandValueReg16OrReg32OrMem(const Operand &dst,Memory &mem,int addressSize,int segmentOverride,const OperandValue &value);

	/*! Store value to an 8-bit operand.
	*/
	void StoreOperandValue8(const Operand &dst,Memory &mem,int addressSize,int segmentOverride,const OperandValue &value);

	/*! Store value to an 64-bit operand.
	*/
	void StoreOperandValue64(const Operand &dst,Memory &mem,int addressSize,int segmentOverride,const OperandValue &value);

	/*! Store value to an 80-bit operand.
	*/
	void StoreOperandValue80(const Operand &dst,Memory &mem,int addressSize,int segmentOverride,const OperandValue &value);

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
	inline bool TakeIOReadException(unsigned int ioport,unsigned int accessSize,Memory &mem,unsigned int numInstBytes);

	/*! Test I/O-write exception, and RaiseException and HandleException if needed.
	    Returns true if an exception was raised.
	*/
	inline bool TakeIOWriteException(unsigned int ioport,unsigned int accessSize,Memory &mem,unsigned int numInstBytes);

	/*! Return true if I/O access is permitted in I/O Map of TSS.
	    It could raise exception.
	*/
	bool TestIOMapPermission(const SegmentRegister &TR,unsigned int ioMin,unsigned int accessSize,Memory &mem);

	/*! Return true if I/O access is permitted in I/O Map of TSS.
	    It will not raise an exception.  It is for debugging purpose.
	*/
	bool DebugTestIOMapPermission(const SegmentRegister &TR,unsigned int ioMin,unsigned int accessSize,const Memory &mem) const;


	virtual uint32_t SerializeVersion(void) const;
	virtual void SpecificSerialize(std::vector <unsigned char> &data,std::string stateFName) const;
	virtual bool SpecificDeserialize(const unsigned char *&data,std::string stateFName,uint32_t version);
};

#include "i486fidelity.h"

inline unsigned long i486DX::LinearAddressToPhysicalAddressRead(unsigned int linearAddr,Memory &mem)
{
	TSUGARU_I486_FIDELITY_CLASS fidelity;

	auto pageIndex=(linearAddr>>LINEARADDR_TO_PAGE_SHIFT);
	PageTableEntry pageInfo;
	if(state.pageTableCache[pageIndex].valid<state.pageTableCacheValidCounter)
	{
		pageInfo=ReadPageInfo(linearAddr,mem);
		if(0==(pageInfo.table&PAGEINFO_FLAG_PRESENT))
		{
			uint32_t code=0;
			if(0!=state.CS().DPL)
			{
				code|=PFFLAG_USER_MODE;
			}
			RaiseException(EXCEPTION_PF,code);
			state.exceptionLinearAddr=linearAddr;
			return 0;
		}
		if(true==fidelity.PageLevelException(*this,false,linearAddr,pageInfo.dir,pageInfo.table))
		{
			return 0;
		}
		state.pageTableCache[pageIndex].info=pageInfo;
		state.pageTableCache[pageIndex].valid=state.pageTableCacheValidCounter;
	}
	else
	{
		pageInfo=state.pageTableCache[pageIndex].info;
		if(true==fidelity.PageLevelException(*this,false,linearAddr,pageInfo.dir,pageInfo.table))
		{
			return 0;
		}
	}

#ifdef TSUGARU_I486_UPDATE_PAGE_DA_FLAGS
	MarkPageAccessed(linearAddr,mem);
#endif

	auto offset=(linearAddr&4095);
	auto physicalAddr=(pageInfo.table&0xFFFFF000)+offset;
	return physicalAddr;
}

inline unsigned long i486DX::LinearAddressToPhysicalAddressWrite(unsigned int linearAddr,Memory &mem)
{
	TSUGARU_I486_FIDELITY_CLASS fidelity;

	auto pageIndex=(linearAddr>>LINEARADDR_TO_PAGE_SHIFT);
	PageTableEntry pageInfo;
	if(state.pageTableCache[pageIndex].valid<state.pageTableCacheValidCounter)
	{
		pageInfo=ReadPageInfo(linearAddr,mem);
		if(0==(pageInfo.table&PAGEINFO_FLAG_PRESENT))
		{
			uint32_t code=PFFLAG_WRITE;
			if(0!=state.CS().DPL)
			{
				code|=PFFLAG_USER_MODE;
			}
			RaiseException(EXCEPTION_PF,code);
			state.exceptionLinearAddr=linearAddr;
			return 0;
		}
		if(true==fidelity.PageLevelException(*this,true,linearAddr,pageInfo.dir,pageInfo.table))
		{
			return 0;
		}
		state.pageTableCache[pageIndex].info=pageInfo;
		state.pageTableCache[pageIndex].valid=state.pageTableCacheValidCounter;
	}
	else
	{
		pageInfo=state.pageTableCache[pageIndex].info;
		if(true==fidelity.PageLevelException(*this,true,linearAddr,pageInfo.dir,pageInfo.table))
		{
			return 0;
		}
	}

#ifdef TSUGARU_I486_UPDATE_PAGE_DA_FLAGS
	MarkPageDirty(linearAddr,mem);
#endif
	auto offset=(linearAddr&4095);
	auto physicalAddr=(pageInfo.table&0xFFFFF000)+offset;
	return physicalAddr;
}


#include "i486debug.h"

inline void i486DX::Interrupt(unsigned int INTNum,Memory &mem,unsigned int numInstBytesForReturn,unsigned int numInstBytesForCallStack,bool SWI)
{
	TSUGARU_I486_FIDELITY_CLASS fidelity;

	if(nullptr!=debuggerPtr)
	{
		debuggerPtr->Interrupt(*this,INTNum,mem,numInstBytesForReturn);
	}

	state.halt=false;

	if(IsInRealMode())
	{
		Push(mem,16,state.EFLAGS&0xFFFF,state.CS().value,state.EIP+numInstBytesForReturn);
		// Equivalent:
		// Push(mem,16,state.EFLAGS&0xFFFF);
		// Push(mem,16,state.CS().value);
		// Push(mem,16,state.EIP+numInstBytesForReturn);

		auto intVecAddr=(INTNum&0xFF)*4;
		uint32_t CSIP=mem.FetchDword(intVecAddr);
		auto destIP=CSIP&0xFFFF;
		auto destCS=(CSIP>>16)&0xFFFF;
		if(true==enableCallStack)
		{
			PushCallStack(
			    true,INTNum,GetAX(), // Is an interrupt
			    state.GetCR(0),
			    state.CS().value,state.EIP,numInstBytesForCallStack,
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
			unsigned int gateOperandSize=32;
			bool isINTGate=true; // false if it is a trap gate.
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
					auto callStackDepth=callStack.size();
					auto AX0=GetAX();

					if(INT_GENERAL_PROTECTION!=INTNum) // Prevent infinite recursion.
					{
						unsigned int Ibit=2;
						unsigned int EXTbit=0; // 1 if external interrupt source.
						RaiseException(EXCEPTION_GP,INTNum*8+Ibit+EXTbit); // EXT -> [1] 9-8 Error Code
						HandleException(false,mem,numInstBytesForCallStack);  // <- This will shoot INT 0BH
					}

					if(true==enableCallStack && callStackDepth<callStack.size()) // Supposed to be true, just in case.
					{
						callStack.back().INTNum0=INTNum;
						callStack.back().AX0=AX0;
					}
				}
				return;
			case 0b0110:
				Abort("286 16-bit INT gate not supported");
				gateOperandSize=16;
				break;
			case 0b0111:
				// I have no idea if it is the correct way of handling an 80286 gate.
				gateOperandSize=16;
				isINTGate=false;
				break;
			case 0b0101: //"386 32-bit Task";
			case 0b1110: //"386 32-bit INT";
				break;
			case 0b1111: //"386 32-bit Trap";
				isINTGate=false;
				break;
			}

			if(true==enableCallStack)
			{
				PushCallStack(
				    true,INTNum,GetAX(), // Is an interrupt
				    state.GetCR(0),
				    state.CS().value,state.EIP,numInstBytesForCallStack,
				    desc.SEG,desc.OFFSET,
				    mem);
			}

			auto gateDPL=desc.GetDPL();
			// Apparently it should be IDT's DPL, not newCS's DPL.
			auto CPL=state.CS().DPL;

			if(0==(state.EFLAGS&EFLAGS_VIRTUAL86))
			{
				SegmentRegister newCS;
				LoadSegmentRegister(newCS,desc.SEG,mem);

				if(newCS.DPL<CPL)
				{
					state.CS().DPL=newCS.DPL; // Raise privilege level so that it can load SS.

					auto TempSS=state.SS();
					auto TempESP=state.ESP();
					LoadSegmentRegister(state.SS(),FetchWord(32,state.TR,TSS_OFFSET_SS0+newCS.DPL*8,mem),mem);
					state.ESP()=FetchDword(32,state.TR,TSS_OFFSET_ESP0+newCS.DPL*8,mem);
					Push(mem,gateOperandSize,TempSS.value);
					Push(mem,gateOperandSize,TempESP);
				}
				// else if(CPL<DPL)
				// {
				//	Interrupt to lower-privilege level should raise exception.
				// }


				Push(mem,gateOperandSize,state.EFLAGS,state.CS().value,state.EIP+numInstBytesForReturn);
				// Equivalent >>
				// Push(mem,gateOperandSize,state.EFLAGS);
				// Push(mem,gateOperandSize,state.CS().value);
				// Push(mem,gateOperandSize,state.EIP+numInstBytesForReturn);

				SetIPorEIP(gateOperandSize,desc.OFFSET);
				state.CS()=newCS;

				fidelity.Sync_SS_CS_RPL_to_DPL(*this,state.CS(),state.CS());

				if(true==isINTGate)
				{
					SetIF(false);
				}
				SetTF(false);
				if(newCS.DPL<CPL)
				{
					state.EFLAGS&=(~EFLAGS_NESTED);
				}
			}
			else // Interrupt from Virtual86 mode
			{
				// Should I raise exception if Software Interrupt && DPL<CPL?
				if(true==SWI && gateDPL<CPL)
				{
					RaiseException(EXCEPTION_GP,INTNum*8); // What's +EXT?  ([1] pp.26-170)
					HandleException(false,mem,numInstBytesForCallStack);
				}
				else
				{
					state.CS().DPL=0;
					// Just in case, set CPL to zero so that SS can be loaded.
					// VM86 monitor is supposed to be ring 0.

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
					state.CS().DPL=0; // Change to CPL=0 before loading SS.
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
					Push(mem,32,state.EIP+numInstBytesForReturn);

					// Need to clear DS,ES,FS,GS.  Or, PUSH FS -> POP FS will shoot GP(0).
					LoadSegmentRegister(state.DS(),0,mem);
					LoadSegmentRegister(state.ES(),0,mem);
					LoadSegmentRegister(state.FS(),0,mem);
					LoadSegmentRegister(state.GS(),0,mem);

					SetIPorEIP(gateOperandSize,desc.OFFSET);
					LoadSegmentRegister(state.CS(),desc.SEG,mem);
				}
			}
		}
		else
		{
			auto callStackDepth=callStack.size();
			auto AX0=GetAX();

			if(INT_GENERAL_PROTECTION!=INTNum) // Prevent infinite recursion.
			{
				RaiseException(EXCEPTION_GP,INTNum*8); // What's +EXT?  ([1] pp.26-170)
				HandleException(false,mem,numInstBytesForCallStack);
			}

			if(true==enableCallStack && callStackDepth<callStack.size()) // Supposed to be true, just in case.
			{
				callStack.back().INTNum0=INTNum;
				callStack.back().AX0=AX0;
			}
		}
	}
};

inline unsigned int i486DX::FetchByte(unsigned int addressSize,const SegmentRegister &seg,unsigned int offset,Memory &mem)
{
	offset&=AddressMask((unsigned char)addressSize);
	auto addr=seg.baseLinearAddr+offset;

	TSUGARU_I486_FIDELITY_CLASS fidelity;
	if(true==fidelity.SegmentReadException(*this,seg,offset))
	{
		return 0;
	}

	if(/* &seg==&state.SS() && */
	   nullptr!=state.SSESPWindow.ptr &&
	   true==state.SSESPWindow.IsLinearAddressInRange(addr))
	{
		return state.SSESPWindow.ptr[addr&(MemoryAccess::MEMORY_WINDOW_SIZE-1)];
	}

	if(true==PagingEnabled())
	{
		addr=LinearAddressToPhysicalAddressRead(addr,mem);
	}
	return mem.FetchByte(addr);
}
inline unsigned int i486DX::FetchWord(unsigned int addressSize,const SegmentRegister &seg,unsigned int offset,Memory &mem)
{
	offset&=AddressMask((unsigned char)addressSize);
	auto addr=seg.baseLinearAddr+offset;

	TSUGARU_I486_FIDELITY_CLASS fidelity;
	if(true==fidelity.SegmentReadException(*this,seg,offset))
	{
		return 0;
	}

	if(/* &seg==&state.SS() && */
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
		addr=LinearAddressToPhysicalAddressRead(addr,mem);
		if(0xFFC<(addr&0xfff)) // May hit the page boundary
		{
			return FetchByte(addressSize,seg,offset,mem)|(FetchByte(addressSize,seg,offset+1,mem)<<8);
		}
	}
	return mem.FetchWord(addr);
}
inline unsigned int i486DX::FetchDword(unsigned int addressSize,const SegmentRegister &seg,unsigned int offset,Memory &mem)
{
	offset&=AddressMask((unsigned char)addressSize);
	auto addr=seg.baseLinearAddr+offset;

	TSUGARU_I486_FIDELITY_CLASS fidelity;
	if(true==fidelity.SegmentReadException(*this,seg,offset))
	{
		return 0;
	}

	if(/* &seg==&state.SS() && */
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
		addr=LinearAddressToPhysicalAddressRead(addr,mem);
		if(0xFF8<(addr&0xfff)) // May hit the page boundary
		{
			return cpputil::MakeUnsignedDword(
			     FetchByte(addressSize,seg,offset,mem),
			     FetchByte(addressSize,seg,offset+1,mem),
			     FetchByte(addressSize,seg,offset+2,mem),
			     FetchByte(addressSize,seg,offset+3,mem));
		}
	}
	return mem.FetchDword(addr);
}

inline void i486DX::StoreByte(Memory &mem,int addressSize,const SegmentRegister &seg,unsigned int offset,unsigned char byteData)
{
	offset&=AddressMask((unsigned char)addressSize);
	auto linearAddr=seg.baseLinearAddr+offset;

	TSUGARU_I486_FIDELITY_CLASS fidelity;
	if(true==fidelity.SegmentWriteException(*this,seg,offset))
	{
		return;
	}

	if(/* &seg==&state.SS() && */
	   nullptr!=state.SSESPWindow.ptr &&
	   true==state.SSESPWindow.IsLinearAddressInRange(linearAddr))
	{
		state.SSESPWindow.ptr[linearAddr&(MemoryAccess::MEMORY_WINDOW_SIZE-1)]=byteData;
		return;
	}

	auto physicalAddr=linearAddr;
	if(true==PagingEnabled())
	{
		physicalAddr=LinearAddressToPhysicalAddressWrite(linearAddr,mem);
	}
	return mem.StoreByte(physicalAddr,byteData);
}

inline void i486DX::StoreWord(Memory &mem,int addressSize,const SegmentRegister &seg,unsigned int offset,unsigned int data)
{
	offset&=AddressMask((unsigned char)addressSize);
	auto linearAddr=seg.baseLinearAddr+offset;

	TSUGARU_I486_FIDELITY_CLASS fidelity;
	if(true==fidelity.SegmentWriteException(*this,seg,offset))
	{
		return;
	}

	if(/* &seg==&state.SS() && */
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
		physicalAddr=LinearAddressToPhysicalAddressWrite(linearAddr,mem);
		if(0xFFE<(physicalAddr&0xfff)) // May hit the page boundary
		{
			StoreByte(mem,addressSize,seg,offset  , data    &255);// May hit the page boundary. Don't use StoreWord
			StoreByte(mem,addressSize,seg,offset+1,(data>>8)&255);// May hit the page boundary. Don't use StoreWord
			return;
		}
	}
	mem.StoreWord(physicalAddr,data);
}
inline void i486DX::StoreDword(Memory &mem,int addressSize,const SegmentRegister &seg,unsigned int offset,unsigned int data)
{
	offset&=AddressMask((unsigned char)addressSize);
	auto linearAddr=seg.baseLinearAddr+offset;

	TSUGARU_I486_FIDELITY_CLASS fidelity;
	if(true==fidelity.SegmentWriteException(*this,seg,offset))
	{
		return;
	}

	if(/* &seg==&state.SS() && */
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
		physicalAddr=LinearAddressToPhysicalAddressWrite(linearAddr,mem);
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

inline void i486DX::StoreWordOrDword(Memory &mem,unsigned int operandSize,int addressSize,const SegmentRegister &seg,unsigned int offset,unsigned int data)
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
	}
	return 0;
}

inline unsigned int i486DX::GetRegisterValue8(int reg) const
{
#ifdef YS_LITTLE_ENDIAN
	return *state.reg8Ptr[reg-REG_AL];
#else
	unsigned int regIdx=reg-REG_AL;
	unsigned int shift=(regIdx<<1)&8;
	return ((state.reg32()[regIdx&3]>>shift)&255);
#endif
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
#ifdef YS_LITTLE_ENDIAN
	*state.reg8Ptr[reg-REG_AL]=value;
#else
	static const unsigned int highLowMask[2]=
	{
		0xFFFFFF00,
		0xFFFF00FF,
	};
	unsigned int regIdx=reg-REG_AL;
	unsigned int highLow=regIdx>>2;
	state.reg32()[regIdx&3]&=highLowMask[highLow];
	state.reg32()[regIdx&3]|=(value<<(highLow<<3));
#endif
}

inline bool i486DX::TakeIOReadException(unsigned int ioport,unsigned int accessSize,Memory &mem,unsigned int numInstBytes)
{
	if(0!=(state.EFLAGS&EFLAGS_VIRTUAL86))
	{
		if(true!=TestIOMapPermission(state.TR,ioport,accessSize,mem))
		{
			RaiseException(EXCEPTION_GP,0);
			HandleException(true,mem,numInstBytes);
			return true;
		}
	}
	return false;
}

inline bool i486DX::TakeIOWriteException(unsigned int ioport,unsigned int accessSize,Memory &mem,unsigned int numInstBytes)
{
	if(0!=(state.EFLAGS&EFLAGS_VIRTUAL86))
	{
		if(true!=TestIOMapPermission(state.TR,ioport,accessSize,mem))
		{
			RaiseException(EXCEPTION_GP,0);
			HandleException(false,mem,numInstBytes);
			return true;
		}
	}
	return false;
}

/* } */
#endif
