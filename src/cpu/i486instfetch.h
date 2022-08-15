class i486DX::BurstModeFetchInstructionFunctions
{
public:
	/* This class does not check remaining bytes in the MemoryAccess::ConstPointer.

	   After prefixes, if the remaining length is greater than or equal to MAX_INSTRUCTION_LENGTH (actually MAX_INSTRUCTION_LENGTH-2),
	   it is safe not to check for the length.

	   FetchInstruction needs one if statement, but for the majority of the cases, the check passes and BurstMode will be used.
	   Then, non-burst mode was anyway checking for remaining length for each operand, therefore virtually zero penalty.

	   I really want to do for prefixes as well, however, there may be indefinite prefixes, which will bust the 4K boundary.
	   It should cause CPU exception.  However, I don't want to waste host CPU time for checking this.

	*/
	typedef i486DX CPUCLASS;

	inline static void GetConstMemoryWindow(
		CPUCLASS &cpu,
		MemoryAccess::ConstMemoryWindow &memWin,
		unsigned int codeAddressSize,
		const i486DX::SegmentRegister &CS,
		unsigned int offset,
		Memory &mem)
	{
		memWin=cpu.GetConstMemoryWindow(codeAddressSize,CS,offset,mem);
	}
	inline static unsigned int FetchInstructionByte(
		CPUCLASS &cpu,
		MemoryAccess::ConstPointer &ptr,
		unsigned int,
		const SegmentRegister &,
		unsigned int,const Memory &)
	{
		return ptr.FetchByte();
	}
	inline static unsigned int PeekInstructionByte(
		CPUCLASS &cpu,
		MemoryAccess::ConstPointer &ptr,
		unsigned int,
		const SegmentRegister &,
		unsigned int,const Memory &)
	{
		return ptr.PeekByte();
	}
	inline static void FetchInstructionTwoBytes(
		unsigned char dat[2],
		CPUCLASS &cpu,
		MemoryAccess::ConstPointer &ptr,
		unsigned int,
		const SegmentRegister &,
		unsigned int,const Memory &)
	{
		ptr.FetchTwoBytes(dat);
	}
	inline static void FetchInstructionFourBytes(
		unsigned char dat[4],
		CPUCLASS &cpu,
		MemoryAccess::ConstPointer &ptr,
		unsigned int ,
		const SegmentRegister &,
		unsigned int,const Memory &)
	{
		ptr.FetchFourBytes(dat);
	}

	inline static void FetchOperand8(CPUCLASS &cpu,Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
	{
		inst.operand[inst.operandLen++]=ptr.FetchByte();
		++inst.numBytes;
	}
	inline static void PeekOperand8(CPUCLASS &cpu,unsigned int &operand,const Instruction &inst,const MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
	{
		operand=ptr.PeekByte();
	}
	inline static void FetchOperand16(CPUCLASS &cpu,Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
	{
		ptr.FetchTwoBytes(inst.operand+inst.operandLen);
		inst.operandLen+=2;
		inst.numBytes+=2;
	}
	inline static void FetchOperand32(CPUCLASS &cpu,Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
	{
		ptr.FetchFourBytes(inst.operand+inst.operandLen);
		inst.operandLen+=4;
		inst.numBytes+=4;
	}
	inline static unsigned int FetchOperand16or32(CPUCLASS &cpu,Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
	{
		if(16==inst.operandSize)
		{
			ptr.FetchTwoBytes(inst.operand+inst.operandLen);
			inst.operandLen+=2;
			inst.numBytes+=2;
			return 2;
		}
		else // if(32==inst.operandSize)
		{
			ptr.FetchFourBytes(inst.operand+inst.operandLen);
			inst.operandLen+=4;
			inst.numBytes+=4;
			return 4;
		}
	}
	inline static void FetchImm8(CPUCLASS &cpu,Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
	{
		inst.imm[0]=ptr.FetchByte();
		++inst.numBytes;
	}
	inline static void FetchImm16(CPUCLASS &cpu,Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
	{
		ptr.FetchTwoBytes(inst.imm);
		inst.numBytes+=2;
	}
	inline static void FetchImm32(CPUCLASS &cpu,Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
	{
		ptr.FetchFourBytes(inst.imm);
		inst.numBytes+=4;
	}
	inline static unsigned int FetchImm16or32(CPUCLASS &cpu,Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
	{
		if(16==inst.operandSize)
		{
			ptr.FetchTwoBytes(inst.imm);
			inst.numBytes+=2;
			return 2;
		}
		else // if(32==inst.operandSize)
		{
			ptr.FetchFourBytes(inst.imm);
			inst.numBytes+=4;
			return 4;
		}
	}
};

class i486DX::DebugFetchInstructionFunctions
{
public:
	typedef const i486DX CPUCLASS;

	inline static void GetConstMemoryWindow(
		CPUCLASS &cpu,
		MemoryAccess::ConstMemoryWindow &memWin,
		unsigned int codeAddressSize,
		const i486DX::SegmentRegister &CS,
		unsigned int offset,
		const Memory &mem)
	{
		memWin=cpu.DebugGetConstMemoryWindow(codeAddressSize,CS,offset,mem);
	}
	inline static unsigned int FetchInstructionByte(
		CPUCLASS &cpu,
		MemoryAccess::ConstPointer &,
		unsigned int addressSize,
		const SegmentRegister &seg,
		unsigned int offset,const Memory &mem)
	{
		return cpu.DebugFetchByte(addressSize,seg,offset,mem);
	}
	inline static unsigned int PeekInstructionByte(
		CPUCLASS &cpu,
		MemoryAccess::ConstPointer &,
		unsigned int addressSize,
		const SegmentRegister &seg,
		unsigned int offset,const Memory &mem)
	{
		return cpu.DebugFetchByte(addressSize,seg,offset,mem);
	}
	inline static void FetchInstructionTwoBytes(
		unsigned char dat[2],
		CPUCLASS &cpu,
		MemoryAccess::ConstPointer &ptr,
		unsigned int addressSize,
		const SegmentRegister &seg,
		unsigned int offset,const Memory &mem)
	{
		cpputil::PutWord(dat,cpu.DebugFetchWord(addressSize,seg,offset,mem));
	}
	inline static void FetchInstructionFourBytes(
		unsigned char dat[4],
		CPUCLASS &cpu,
		MemoryAccess::ConstPointer &ptr,
		unsigned int addressSize,
		const SegmentRegister &seg,
		unsigned int offset,const Memory &mem)
	{
		cpputil::PutDword(dat,cpu.DebugFetchDword(addressSize,seg,offset,mem));
	}

	inline static void FetchOperand8(CPUCLASS &cpu,Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
	{
		cpu.DebugFetchOperand8(inst,ptr,seg,offset,mem);
	}
	inline static void PeekOperand8(CPUCLASS &cpu,unsigned int &operand,const Instruction &inst,const MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
	{
		cpu.DebugPeekOperand8(operand,inst,ptr,seg,offset,mem);
	}
	inline static void FetchOperand16(CPUCLASS &cpu,Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
	{
		cpu.DebugFetchOperand16(inst,ptr,seg,offset,mem);
	}
	inline static void FetchOperand32(CPUCLASS &cpu,Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
	{
		cpu.DebugFetchOperand32(inst,ptr,seg,offset,mem);
	}
	inline static unsigned int FetchOperand16or32(CPUCLASS &cpu,Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
	{
		return cpu.DebugFetchOperand16or32(inst,ptr,seg,offset,mem);
	}
	inline static void FetchImm8(CPUCLASS &cpu,Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
	{
		cpu.DebugFetchImm8(inst,ptr,seg,offset,mem);
	}
	inline static void FetchImm16(CPUCLASS &cpu,Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
	{
		cpu.DebugFetchImm16(inst,ptr,seg,offset,mem);
	}
	inline static void FetchImm32(CPUCLASS &cpu,Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
	{
		cpu.DebugFetchImm32(inst,ptr,seg,offset,mem);
	}
	inline static unsigned int FetchImm16or32(CPUCLASS &cpu,Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
	{
		return cpu.DebugFetchImm16or32(inst,ptr,seg,offset,mem);
	}
};

class i486DX::RealFetchInstructionFunctions
{
public:
	typedef i486DX CPUCLASS;

	inline static void GetConstMemoryWindow(
		CPUCLASS &cpu,
		MemoryAccess::ConstMemoryWindow &memWin,
		unsigned int codeAddressSize,
		const i486DX::SegmentRegister &CS,
		unsigned int offset,
		Memory &mem)
	{
		memWin=cpu.GetConstMemoryWindow(codeAddressSize,CS,offset,mem);
	}
	inline static unsigned int FetchInstructionByte(
		CPUCLASS &cpu,
		MemoryAccess::ConstPointer &ptr,
		unsigned int addressSize,
		const SegmentRegister &seg,
		unsigned int offset,Memory &mem)
	{
		return cpu.FetchInstructionByte(ptr,addressSize,seg,offset,mem);
	}
	inline static unsigned int PeekInstructionByte(
		CPUCLASS &cpu,
		MemoryAccess::ConstPointer &ptr,
		unsigned int addressSize,
		const SegmentRegister &seg,
		unsigned int offset,Memory &mem)
	{
		return cpu.PeekInstructionByte(ptr,addressSize,seg,offset,mem);
	}
	inline static void FetchInstructionTwoBytes(
		unsigned char dat[2],
		CPUCLASS &cpu,
		MemoryAccess::ConstPointer &ptr,
		unsigned int addressSize,
		const SegmentRegister &seg,
		unsigned int offset,Memory &mem)
	{
		cpu.FetchInstructionTwoBytes(dat,ptr,addressSize,seg,offset,mem);
	}
	inline static void FetchInstructionFourBytes(
		unsigned char dat[4],
		CPUCLASS &cpu,
		MemoryAccess::ConstPointer &ptr,
		unsigned int addressSize,
		const SegmentRegister &seg,
		unsigned int offset,Memory &mem)
	{
		cpu.FetchInstructionFourBytes(dat,ptr,addressSize,seg,offset,mem);
	}

	inline static void FetchOperand8(CPUCLASS &cpu,Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,Memory &mem)
	{
		cpu.FetchOperand8(inst,ptr,seg,offset,mem);
	}
	inline static void PeekOperand8(CPUCLASS &cpu,unsigned int &operand,const Instruction &inst,const MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,Memory &mem)
	{
		cpu.PeekOperand8(operand,inst,ptr,seg,offset,mem);
	}
	inline static void FetchOperand16(CPUCLASS &cpu,Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,Memory &mem)
	{
		cpu.FetchOperand16(inst,ptr,seg,offset,mem);
	}
	inline static void FetchOperand32(CPUCLASS &cpu,Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,Memory &mem)
	{
		cpu.FetchOperand32(inst,ptr,seg,offset,mem);
	}
	inline static unsigned int FetchOperand16or32(CPUCLASS &cpu,Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,Memory &mem)
	{
		return cpu.FetchOperand16or32(inst,ptr,seg,offset,mem);
	}
	inline static void FetchImm8(CPUCLASS &cpu,Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,Memory &mem)
	{
		cpu.FetchImm8(inst,ptr,seg,offset,mem);
	}
	inline static void FetchImm16(CPUCLASS &cpu,Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,Memory &mem)
	{
		cpu.FetchImm16(inst,ptr,seg,offset,mem);
	}
	inline static void FetchImm32(CPUCLASS &cpu,Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,Memory &mem)
	{
		cpu.FetchImm32(inst,ptr,seg,offset,mem);
	}
	inline static unsigned int FetchImm16or32(CPUCLASS &cpu,Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,Memory &mem)
	{
		return cpu.FetchImm16or32(inst,ptr,seg,offset,mem);
	}
};

template <class CPUCLASS,class MEMCLASS,class FUNCCLASS,class BURSTMODEFUNCCLASS>
class i486DX::FetchInstructionClass
{
public:
	static inline void FetchInstruction(
	   CPUCLASS &cpu,
	   MemoryAccess::ConstMemoryWindow &memWin,
	   InstructionAndOperand &instOp,
	   const SegmentRegister &CS,unsigned int offset,MEMCLASS &mem,unsigned int defOperSize,unsigned int defAddrSize)
	{
		auto &inst=instOp.inst;
		auto &op1=instOp.op1;
		auto &op2=instOp.op2;

		inst.Clear();
		inst.operandSize=defOperSize;
		inst.addressSize=defAddrSize;
		inst.codeAddressSize=defAddrSize;

		// Question: Do prefixes need to be in the specific order INST_PREFIX->ADDRSIZE_OVERRIDE->OPSIZE_OVERRIDE->SEG_OVERRIDE?

		auto CSEIPLinear=CS.baseLinearAddr+offset;
		if(nullptr==memWin.ptr || true!=memWin.IsLinearAddressInRange(CSEIPLinear))
		{
			FUNCCLASS::GetConstMemoryWindow(cpu,memWin,inst.codeAddressSize,CS,offset,mem);
		}
		auto ptr=memWin.GetReadAccessPointer(CSEIPLinear);

	#ifdef VERIFY_MEMORY_WINDOW
		MemoryAccess::ConstMemoryWindow testMemWin;
		FUNCCLASS::GetConstMemoryWindow(cpu,testMemWin,inst.codeAddressSize,CS,offset,mem);
		if(testMemWin.linearBaseAddr!=memWin.linearBaseAddr || testMemWin.ptr!=memWin.ptr)
		{
			std::cout << "Memory Window Test failed!" << std::endl;
			std::cout << "Retained Memory Window Linear Addr: " << cpputil::Uitox(memWin.linearBaseAddr) << std::endl;
			std::cout << "CS:EIP Memory Window Linear Addr:   " << cpputil::Uitox(testMemWin.linearBaseAddr) << std::endl;
			std::cout << "Pointer Diff " << cpputil::Uitox(memWin.ptr-testMemWin.ptr) << std::endl;
			Abort("Memory Cache Failed.");
			return;
		}
	#endif

		// Multi-byte instruction (0x0F) and pre-fixes are handled in FetchOperand.

		inst.opCode=FUNCCLASS::FetchInstructionByte(cpu,ptr,inst.codeAddressSize,CS,offset+inst.numBytes++,mem);
		if(MAX_INSTRUCTION_LENGTH<=ptr.length)
		{
			CPUCLASS::template FetchOperand<CPUCLASS,MEMCLASS,BURSTMODEFUNCCLASS>(cpu,instOp,ptr,CS,offset+inst.numBytes,mem,defOperSize,defAddrSize);
		}
		else
		{
			CPUCLASS::template FetchOperand<CPUCLASS,MEMCLASS,FUNCCLASS>(cpu,instOp,ptr,CS,offset+inst.numBytes,mem,defOperSize,defAddrSize);
		}
	}
};
