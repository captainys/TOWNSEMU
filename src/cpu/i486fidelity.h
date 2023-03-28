#ifndef I486FIDELITY_IS_INCLUDED
#define I486FIDELITY_IS_INCLUDED
/* { */


// Low Fidelity Mode (Planned)  Minimum exception handling.  Faster execution of FM TOWNS native apps.
// Default Fidelity Mode        Exception handling required for running Fractal Engine.
// High Fidelity Mode (Planned) Exception handling required for running Windows 3.1

class i486DXLowFidelity
{
public:
	inline static void SaveESPHighWord(uint8_t,uint32_t){}
	inline static void RestoreESPHighWord(uint8_t,uint32_t &){}

	inline static void Sync_CS_RPL_to_DPL(class i486DX &){}

	constexpr bool UDException_MOV_TO_CS(class i486DX &,uint32_t reg,Memory &mem,uint32_t instNumBytes) const{return false;}

	constexpr bool IOPLException(class i486DX &,uint32_t exceptionType,Memory &,uint32_t instNumBytes) const{return false;}

	constexpr bool IOPLExceptionInVM86Mode(class i486DX &cpu,uint32_t exceptionType,Memory &mem,uint32_t instNumBytes) const{return false;}

	constexpr bool HandleExceptionIfAny(class i486DX &,Memory &,uint32_t instNumBytes) const{return false;}

	constexpr bool PageLevelException(class i486DX &cpu,bool write,uint32_t linearAddr,uint32_t pageIndex,uint32_t pageInfo) const{return false;}

	inline static void SetPageFlags(class i486DX &cpu,uint32_t linearAddr,Memory &mem,uint32_t flags){};

	constexpr bool SegmentWriteException(class i486DX &cpu,const i486DX::SegmentRegister &reg,uint32_t offset) const{return false;}

	constexpr bool SegmentReadException(class i486DX &cpu,const i486DX::SegmentRegister &seg,uint32_t offset) const{return false;}

	constexpr bool LockNotAllowed(class i486DX &cpu,Memory &mem,const i486DX::Instruction &inst,const i486DX::Operand &op1) const{return false;}

	// This is not performance critical, but unless it returns true, state-file saved in the older version Tsugaru
	// may not work.
	constexpr bool VERRTypeCheck(const i486DX &cpu,const i486DX::SegmentRegister &reg) const{return true;}
	constexpr bool VERWTypeCheck(const i486DX &cpu,const i486DX::SegmentRegister &reg) const{return true;}

	inline void PageFaultCheckAfterEnter(class i486DX &cpu,Memory &mem) const{}

	inline void CheckRETFtoOuterLevel(i486DX &cpu,Memory &mem,uint32_t operandSize,uint32_t prevDPL){};

	// LoadSegmentRegister
	class LoadSegmentRegisterVariables
	{
	public:
		uint32_t limit=0;
	};
	class LoadSegmentRegisterFlags
	{
	public:
	};
	inline static void SetLimit(LoadSegmentRegisterVariables &,uint32_t limit){};
	inline static void SetLoadSegmentRegisterFlags(LoadSegmentRegisterFlags &flags,const i486DX &cpu,const i486DX::SegmentRegister &reg){};
	inline constexpr bool CheckSelectorBeyondLimit(class i486DX &cpu,LoadSegmentRegisterVariables &var,uint32_t selector)const{return false;}
	inline constexpr bool CheckSelectorBeyondLimit(const class i486DX &cpu,LoadSegmentRegisterVariables &var,uint32_t selector)const{return false;}
	inline constexpr bool LoadNullSelector(i486DX &cpu,i486DX::SegmentRegister &reg,uint32_t selector)const{return false;}
	inline constexpr bool LoadNullSelector(const i486DX &cpu,const i486DX::SegmentRegister &reg,uint32_t selector)const{return false;}
	inline constexpr bool DescriptorException(const LoadSegmentRegisterFlags flags,i486DX &cpu,uint32_t selector,const uint8_t *desc)const{return false;}
	inline constexpr bool DescriptorException(const LoadSegmentRegisterFlags flags,const i486DX &cpu,uint32_t selector,const uint8_t *desc)const{return false;}
	inline constexpr bool TakeIOReadException(i486DX &cpu,unsigned int ioport,unsigned int accessSize,Memory &mem,unsigned int numInstBytes)const{return false;}
	inline constexpr bool TakeIOWriteException(i486DX &cpu,unsigned int ioport,unsigned int accessSize,Memory &mem,unsigned int numInstBytes)const{return false;}


	// If low-fidelity, don't care if it is readable or writable.
	inline static void ClearSegmentRegisterAttribBytes(uint16_t &attribBytes){};

	// Protect EFLAGS IOPL bits.
	class EFLAGS
	{
	};
	inline static void SaveEFLAGS(EFLAGS &,const i486DX &cpu){};
	inline static void RestoreIOPLBits(i486DX &cpu,const EFLAGS &){};
	inline static void RestoreIF(i486DX &cpu,const EFLAGS &){};
};

class i486DXDefaultFidelity : public i486DXLowFidelity
{
public:
	static inline bool SegmentReadException(class i486DX &cpu,const i486DX::SegmentRegister &seg,uint32_t offset)
	{
		if(seg.limit<offset) // Needed to run Fractal Engine Demo and other Psygnosis games.
		{
			cpu.RaiseException(i486DX::EXCEPTION_GP,0);
			return true;
		}
		return false;
	}

	// Default fidelity level does not consider Protected Mode && IOPL<CPL since no known Towns native app uses 0<CPL protected mode.
	static inline bool TakeIOReadException(i486DX &cpu,unsigned int ioport,unsigned int accessSize,Memory &mem,unsigned int numInstBytes)
	{
		if(0!=(cpu.state.EFLAGS&i486DX::EFLAGS_VIRTUAL86))
		{
			if(true!=cpu.TestIOMapPermission(cpu.state.TR,ioport,accessSize,mem))
			{
				cpu.RaiseException(i486DX::EXCEPTION_GP,0);
				cpu.HandleException(true,mem,numInstBytes);
				return true;
			}
		}
		return false;
	}

	// Default fidelity level does not consider Protected Mode && IOPL<CPL since no known Towns native app uses 0<CPL protected mode.
	static inline bool TakeIOWriteException(i486DX &cpu,unsigned int ioport,unsigned int accessSize,Memory &mem,unsigned int numInstBytes)
	{
		if(0!=(cpu.state.EFLAGS&i486DX::EFLAGS_VIRTUAL86))
		{
			if(true!=cpu.TestIOMapPermission(cpu.state.TR,ioport,accessSize,mem))
			{
				cpu.RaiseException(i486DX::EXCEPTION_GP,0);
				cpu.HandleException(false,mem,numInstBytes);
				return true;
			}
		}
		return false;
	}
};

class i486DXHighFidelity : public i486DXDefaultFidelity
{
public:
	uint32_t ESPHigh;
	inline void SaveESPHighWord(uint8_t addressSize,uint32_t ESP)
	{
		if(16==addressSize)
		{
			ESPHigh=(ESP&0xFFFF0000);
		}
	}
	inline void RestoreESPHighWord(uint8_t addressSize,uint32_t &ESP)
	{
		if(16==addressSize)
		{
			ESP&=0xFFFF;ESP|=ESPHigh;
		}
	}

	inline static void Sync_CS_RPL_to_DPL(class i486DX &cpu)
	{
		cpu.state.CS().value&=~3;
		cpu.state.CS().value|=(cpu.state.CS().DPL&3);
	}

	inline bool UDException_MOV_TO_CS(class i486DX &cpu,uint32_t reg,Memory &mem,uint32_t instNumBytes)
	{
		if(reg==i486DX::REG_CS)
		{
			cpu.RaiseException(i486DX::EXCEPTION_UD,0);
			cpu.HandleException(true,mem,instNumBytes);
			return true;
		}
		return false;
	}

	inline static bool IOPLException(class i486DX &cpu,uint32_t exceptionType,Memory &mem,uint32_t instNumBytes)
	{
		if(true!=cpu.IsInRealMode() && cpu.GetIOPL()<cpu.state.CS().DPL)
		{
			cpu.RaiseException(exceptionType,0);
			cpu.HandleException(false,mem,instNumBytes);
			return true;
		}
		return false;
	}
	inline static bool IOPLExceptionInVM86Mode(class i486DX &cpu,uint32_t exceptionType,Memory &mem,uint32_t instNumBytes)
	{
		if(true!=cpu.IsInRealMode() && cpu.GetVM() && cpu.GetIOPL()<3)
		{
			cpu.RaiseException(exceptionType,0);
			cpu.HandleException(false,mem,instNumBytes);
			return true;
		}
		return false;
	}

	inline static bool HandleExceptionIfAny(class i486DX &cpu,Memory &mem,uint32_t instNumBytes)
	{
		if(true==cpu.state.exception)
		{
			cpu.HandleException(true,mem,instNumBytes);
			return true;
		}
		return false;
	}

	inline static bool PageLevelException(class i486DX &cpu,bool write,uint32_t linearAddr,uint32_t pageIndex,uint32_t pageInfo)
	{
		auto raise=[&]
		{
			uint32_t code=i486DX::PFFLAG_PAGE_LEVEL;
			if(true==write)
			{
				code|=i486DX::PFFLAG_WRITE;
			}
			if(3==cpu.state.CS().DPL)
			{
				code|=i486DX::PFFLAG_USER_MODE;
			}
			cpu.RaiseException(i486DX::EXCEPTION_PF,code);
			cpu.state.exceptionLinearAddr=linearAddr;
		};

		// Intel 80386 Programmer's Reference Manual 1986 pp.129 Table 6-5 tells R/W restriction.
		uint8_t URUR=((pageIndex<<1)&0x0C)|((pageInfo>>1)&0x03);
		// Read only if user level and URUR==1010,1011,1110.
		// User mode access allowed if URUR==1010,1011,1110,1111.

		// Intel 80386 Programmer's Reference Manual 1986 pp. 127
		// All pages are read/write in supervisor mode.  RW flag has meaning only if it is running in the user mode.
		// The current level (U/S=1) is related to CPL.  If CPL is 0,1, or 2, the processor is executing at supervisor level.
		// If CPL is 3, the processor is executing at user level.
		if(3==cpu.state.CS().DPL && true==write && (0b1010==URUR || 0b1011==URUR || 0b1110==URUR)) // Read-Only Page.
		{
			raise();
			return true;
		}
		if(3==cpu.state.CS().DPL && 0b1010!=URUR && 0b1011!=URUR && 0b1110!=URUR && 0b1111!=URUR) // System Page.
		{
			raise();
			return true;
		}
		return false;
	}

	inline static void SetPageFlags(class i486DX &cpu,uint32_t linearAddr,Memory &mem,uint32_t flags)
	{
		uint32_t pageDirectoryIndex=((linearAddr>>22)&1023);
		uint32_t pageTableIndex=((linearAddr>>12)&1023);

		auto pageDirectoryPtr=cpu.state.GetCR(3)&0xFFFFF000;
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

		pageTableInfo|=flags;
		pageInfo|=flags;
		mem.StoreDword(pageDirectoryPtr+(pageDirectoryIndex<<2),pageTableInfo);
		mem.StoreDword(pageTablePtr+(pageTableIndex<<2),pageInfo);
	}


	static inline bool SegmentReadException(class i486DX &cpu,const i486DX::SegmentRegister &seg,uint32_t offset)
	{
		auto raise=[&]
		{
			if(&seg==&cpu.state.SS())
			{
				cpu.RaiseException(i486DX::EXCEPTION_SS,0);
			}
			else
			{
				cpu.RaiseException(i486DX::EXCEPTION_GP,0);
			}
		};

		uint32_t type=seg.GetType();
		if(true!=cpu.IsInRealMode() && true!=cpu.GetVM() && (i486DX::SEGTYPE_CODE_NONCONFORMING_EXECONLY==type || i486DX::SEGTYPE_CODE_CONFORMING_EXECONLY==type))
		{
			raise();
			return true;
		}
		if(seg.limit<offset)
		{
			raise();
			return true;
		}
		return false;
	}
	inline static bool SegmentWriteException(class i486DX &cpu,const i486DX::SegmentRegister &seg,uint32_t offset)
	{
		auto raise=[&]
		{
			if(&seg==&cpu.state.SS())
			{
				cpu.RaiseException(i486DX::EXCEPTION_SS,0);
			}
			else
			{
				cpu.RaiseException(i486DX::EXCEPTION_GP,0);
			}
		};

		uint32_t type=seg.GetType();
		if(true!=cpu.IsInRealMode() && true!=cpu.GetVM() && i486DX::SEGTYPE_DATA_NORMAL_RW!=type && i486DX::SEGTYPE_DATA_EXPAND_DOWN_RW!=type)
		{
			raise();
			return true;
		}
		if(seg.limit<offset)
		{
			raise();
			return true;
		}
		return false;
	}

	static inline bool LockNotAllowed(class i486DX &cpu,Memory &mem,const i486DX::Instruction &inst,const i486DX::Operand &op1)
	{
		if(i486DX::INST_PREFIX_LOCK==inst.instPrefix)
		{
			if(i486DX::OPER_ADDR==op1.operandType)
			{
				switch(inst.opCode)
				{
				case I486_OPCODE_INC_DEC_R_M8: //                     0xFE, // INC(REG=0),DEC(REG=1)
				case I486_OPCODE_BTC_RM_R: //   0x0FBB,
				case I486_OPCODE_BTS_RM_R: //   0x0FAB,
				case I486_OPCODE_BTR_RM_R: //   0x0FB3,
				case I486_OPCODE_ADC_RM8_FROM_R8: // 0x10,
				case I486_OPCODE_ADC_RM_FROM_R: //   0x11,
				case I486_OPCODE_ADD_RM8_FROM_R8: // 0x00,
				case I486_OPCODE_ADD_RM_FROM_R: //   0x01,
				case I486_OPCODE_AND_RM8_FROM_R8: // 0x20,
				case I486_OPCODE_AND_RM_FROM_R: //   0x21,
				case I486_OPCODE_OR_RM8_FROM_R8: //   0x08,
				case I486_OPCODE_OR_RM_FROM_R: //     0x09,
				case I486_OPCODE_SBB_RM8_FROM_R8: // 0x18,
				case I486_OPCODE_SBB_RM_FROM_R: //   0x19,
				case I486_OPCODE_SUB_RM8_FROM_R8: // 0x28,
				case I486_OPCODE_SUB_RM_FROM_R: //   0x29,
				case I486_OPCODE_XCHG_RM8_R8: //      0x86,
				case I486_OPCODE_XCHG_RM_R: //        0x87,
				case I486_OPCODE_XOR_RM8_FROM_R8: //  0x30,
				case I486_OPCODE_XOR_RM_FROM_R: //    0x31,
					return false;
				case I486_OPCODE_INC_DEC_CALL_CALLF_JMP_JMPF_PUSH: // =0xFF, // INC(REG=0),DEC(REG=1),CALL(REG=2),CALLF(REG=3),JMP(REG=4),JMPF(REG=5),PUSH(REG=6)
					if(0==inst.GetREG() || 1==inst.GetREG())
					{
						return false;
					}
					break;
				case I486_OPCODE_F6_TEST_NOT_NEG_MUL_IMUL_DIV_IDIV: // =0xF6,
				case I486_OPCODE_F7_TEST_NOT_NEG_MUL_IMUL_DIV_IDIV:   // 0xF7,
					if(2==inst.GetREG() || 3==inst.GetREG())
					{
						return false;
					}
					break;
				case I486_OPCODE_BT_BTS_BTR_BTC_RM_I8:   // 0xFBA, // BT(REG=4),BTS(REG=5),BTR(REG=6),BTC(REG=7)
					if(5==inst.GetREG() || 6==inst.GetREG() || 7==inst.GetREG())
					{
						return false;
					}
					break;
				case I486_OPCODE_BINARYOP_RM8_FROM_I8: // =0x80, // AND(REG=4), OR(REG=1), or XOR(REG=6) depends on the REG field of MODR/M
				case I486_OPCODE_BINARYOP_RM8_FROM_I8_ALIAS:   // 0x82, 
				case I486_OPCODE_BINARYOP_RM_FROM_SXI8:   // 0x83,
					if(7!=inst.GetREG())
					{
						return false;
					}
					break;
				}
			}
			cpu.RaiseException(i486DX::EXCEPTION_UD,0);
			cpu.HandleException(true,mem,inst.numBytes);
			return true;
		}
		return false;
	}

	static inline bool VERRTypeCheck(const i486DX &cpu,const i486DX::SegmentRegister &reg)
	{
		// https://wiki.osdev.org/Descriptors
		uint32_t type=reg.GetType();
		if(type<8)
		{
			return false; // Looks like inaccessible for system segments.
		}
		else if(i486DX::SEGTYPE_CODE_NONCONFORMING_EXECONLY==type || i486DX::SEGTYPE_CODE_CONFORMING_EXECONLY==type)
		{
			return false; // Not readable.
		}
		else if((i486DX::SEGTYPE_CODE_NONCONFORMING_READABLE==type ||
		         i486DX::SEGTYPE_DATA_NORMAL_RW==type)
		         && reg.DPL<cpu.state.CS().DPL) // Is it the condition?  Weird.
		{
			return false; // Not readable.
		}
		// Readable.
		return true;
	}
	static inline bool VERWTypeCheck(const i486DX &cpu,const i486DX::SegmentRegister &reg)
	{
		// https://wiki.osdev.org/Descriptors
		uint32_t type=reg.GetType();
		if(type<8)
		{
			return false; // Looks like inaccessible for system segments.
		}
		else if(i486DX::SEGTYPE_DATA_NORMAL_RW!=type && i486DX::SEGTYPE_DATA_EXPAND_DOWN_RW!=type)
		{
			return false; // Unwritable.
		}
		else if(reg.DPL<cpu.state.CS().DPL)
		{
			return false; // Unwritable.
		}
		// Writable.
		return true;
	}

	inline static void PageFaultCheckAfterEnter(class i486DX &cpu,Memory &mem)
	{
		if(16==cpu.GetStackAddressingSize())
		{
			cpu.LinearAddressToPhysicalAddressWrite(cpu.state.SS().baseLinearAddr+cpu.state.SP(),mem);
		}
		else
		{
			cpu.LinearAddressToPhysicalAddressWrite(cpu.state.SS().baseLinearAddr+cpu.state.ESP(),mem);
		}
	}


	class LoadSegmentRegisterFlags
	{
	public:
		bool needsDataOrReadableCode=false,loadingStackSegment=false,loadingCodeSegment=false;
	};
	inline static void SetLimit(LoadSegmentRegisterVariables &var,uint32_t limit)
	{
		var.limit=limit;
	}
	inline static void SetLoadSegmentRegisterFlags(LoadSegmentRegisterFlags &flags,const i486DX &cpu,const i486DX::SegmentRegister &reg)
	{
		if(&reg==&cpu.state.DS() || &reg==&cpu.state.ES() || &reg==&cpu.state.FS() || &reg==&cpu.state.GS())
		{
			// GP exception if the segment is data or readable code.
			flags.needsDataOrReadableCode=true;
		}
		else if(&reg==&cpu.state.SS())
		{
			flags.loadingStackSegment=true;
		}
		else if(&reg==&cpu.state.CS())
		{
			flags.loadingCodeSegment=true;
		}
	}
	inline bool LoadNullSelector(i486DX &cpu,i486DX::SegmentRegister &reg,uint32_t selector)
	{
		// INTEL 80386 Programmer's Reference Manual 1986 pp.346
		// IF DS,ES,FS,or GS is loaded with a null selector THEN load segment register with selector, clear descriptor valid flag.
		if(0==selector)
		{
			if(&reg==&cpu.state.SS())
			{
				cpu.RaiseException(i486DX::EXCEPTION_GP,0);
				return true;
			}
			else
			{
				reg.value=0;
				reg.limit=0;
				return true;
			}
		}
		return false;
	}
	inline bool LoadNullSelector(const i486DX &cpu,const i486DX::SegmentRegister &reg,uint32_t selector)
	{
		// Do nothing from const i486DX &.
		return false;
	}
	inline bool DescriptorException(const LoadSegmentRegisterFlags flags,i486DX &cpu,uint32_t selector,const uint8_t *desc)
	{
		if(nullptr==desc || 0==(desc[5]&0x80)) // Segment not present
		{
			if(true!=flags.loadingStackSegment)
			{
				cpu.RaiseException(i486DX::EXCEPTION_ND,selector&~3);
			}
			else
			{
				cpu.RaiseException(i486DX::EXCEPTION_SS,selector&~3);
			}
			return true;
		}

		if(true==flags.needsDataOrReadableCode)
		{
			// pp.108 of Intel 80386 Programmer's Reference Manual 1986 tells there is
			// no way of distinguishing writable-data and readable-code segments.
			// No way of distinguishing data and code?  Very poor description.
			// 
			// Better clues:
			// https://redirect.cs.umbc.edu/courses/undergraduate/CMPE310/Fall07/cpatel2/slides/html_versions/chap17_lect15_segmentation.html
			//   1000A Data Read-Only
			//   1001A Data Read/Write
			//   1010A Stack Read-Only
			//   1011A Stack Read/Write
			//   1100A Code Execute-Only
			//   1101A Code Readable
			//   1110A Code Execute-Only Conforming
			//   1111A Code Readable Conforming
			//
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
			// So it was a mis-print in Intel 80386 programmer's reference. WTF.

			if(0==(desc[5]&0x10)) // If system, GP.
			{
				cpu.RaiseException(i486DX::EXCEPTION_GP,selector&~3);
				return true;
			}
			else if(0!=(desc[5]&8) && 0==(desc[5]&2)) // If Code and Unreadable, GP.
			{
				cpu.RaiseException(i486DX::EXCEPTION_GP,selector&~3); // If cpu is const i486DX &, it does nothing.
				return true;
			}
			else
			{
				auto RPL=selector&3;
				auto CPL=cpu.state.CS().DPL;
				auto DPL=(desc[5]>>5)&3;
				if(DPL<CPL && DPL<RPL && (0==(desc[5]&8) || 0==(desc[5]&4))) // If Data or Non-Conforming Code and DPL<RPL,CPL
				{
					cpu.RaiseException(i486DX::EXCEPTION_GP,selector&~3);
					return true;
				}
			}
		}

		if(true==flags.loadingStackSegment)
		{
			// Needs to be:
			//   Writable data segment
			//   DPL must be equal to CPL
			if(0==(desc[5]&0x10)) // If system, GP.
			{
				cpu.RaiseException(i486DX::EXCEPTION_GP,selector&~3);
				return true;
			}
			if(0!=(desc[5]&8)) // If code, GP.
			{
				cpu.RaiseException(i486DX::EXCEPTION_GP,selector&~3);
				return true;
			}
			if(0==(desc[5]&2)) // If readable data, GP.
			{
				cpu.RaiseException(i486DX::EXCEPTION_GP,selector&~3);
				return true;
			}
			auto CPL=cpu.state.CS().DPL;
			auto DPL=(desc[5]>>5)&3;
			if(CPL!=DPL)
			{
				cpu.RaiseException(i486DX::EXCEPTION_GP,selector&~3);
				return true;
			}
		}
		return false;
	}
	inline bool DescriptorException(const LoadSegmentRegisterFlags flags,const i486DX &cpu,uint32_t selector,const uint8_t *desc)
	{
		return false;
	}
	inline static bool CheckSelectorBeyondLimit(class i486DX &cpu,LoadSegmentRegisterVariables &var,uint32_t selector)
	{
		if(var.limit<=(selector&0xfff8))
		{
			cpu.RaiseException(i486DX::EXCEPTION_GP,selector&~3);
			return true;
		}
		return false;
	}
	inline static bool CheckSelectorBeyondLimit(const class i486DX &cpu,LoadSegmentRegisterVariables &var,uint32_t selector)
	{
		return false;
	}

	inline static void ClearSegmentRegisterAttribBytes(uint16_t &attribBytes)
	{
		attribBytes=0;
	}
	inline static void CheckRETFtoOuterLevel(i486DX &cpu,Memory &mem,uint32_t operandSize,uint32_t prevDPL)
	{
		if(0==cpu.GetVM() && true!=cpu.IsInRealMode() && cpu.state.CS().DPL>prevDPL)
		{
			uint32_t newSP,newSS;
			if(16==operandSize)
			{
				// (*)
				newSP=cpu.Pop16(mem);
				newSS=cpu.Pop16(mem);
			}
			else
			{
				// (*)
				newSP=cpu.Pop32(mem);
				newSS=cpu.Pop32(mem);
			}
			// (*) 80386 Programmer's Reference Manual 1986 says increment eSP by 4 if operandSize is 16 or 8 if 32.
			//     However, looks like these 4 and 8 seem to be talking about the size for CS and eIP.
			//     Also same description exists for SAME-PRIVILEGE-LEVEL.
			//     Therefore, I suppose I am not supposed to increment eSP here.

			// Imm is already added.
			cpu.LoadSegmentRegister(cpu.state.SS(),newSS,mem);
			cpu.state.ESP()=newSP;
		}
	}



	// High fidelity level considers Protected Mode && IOPL<CPL for Windows 3.1 protected-mode interrupt handlers.
	static inline bool TakeIOReadException(i486DX &cpu,unsigned int ioport,unsigned int accessSize,Memory &mem,unsigned int numInstBytes)
	{
		if(true!=cpu.IsInRealMode() && (0!=(cpu.state.EFLAGS&i486DX::EFLAGS_VIRTUAL86) || cpu.GetIOPL()<cpu.state.CS().DPL))
		{
			if(true!=cpu.TestIOMapPermission(cpu.state.TR,ioport,accessSize,mem))
			{
				cpu.RaiseException(i486DX::EXCEPTION_GP,0);
				cpu.HandleException(true,mem,numInstBytes);
				return true;
			}
		}
		return false;
	}

	// High fidelity level considers Protected Mode && IOPL<CPL for Windows 3.1 protected-mode interrupt handlers.
	static inline bool TakeIOWriteException(i486DX &cpu,unsigned int ioport,unsigned int accessSize,Memory &mem,unsigned int numInstBytes)
	{
		if(true!=cpu.IsInRealMode() && (0!=(cpu.state.EFLAGS&i486DX::EFLAGS_VIRTUAL86) || cpu.GetIOPL()<cpu.state.CS().DPL))
		{
			if(true!=cpu.TestIOMapPermission(cpu.state.TR,ioport,accessSize,mem))
			{
				cpu.RaiseException(i486DX::EXCEPTION_GP,0);
				cpu.HandleException(false,mem,numInstBytes);
				return true;
			}
		}
		return false;
	}



	class EFLAGS
	{
	public:
		uint32_t eflags;
	};
	inline static void SaveEFLAGS(EFLAGS &eflags,const i486DX &cpu)
	{
		if(true!=cpu.IsInRealMode() && 0!=cpu.state.CS().DPL)
		{
			eflags.eflags=cpu.state.EFLAGS;
		}
	};
	inline static void RestoreIOPLBits(i486DX &cpu,const EFLAGS &eflags)
	{
		if(true!=cpu.IsInRealMode() && 0!=cpu.state.CS().DPL)
		{
			cpu.state.EFLAGS&=~i486DX::EFLAGS_IOPL;
			cpu.state.EFLAGS|=(eflags.eflags&i486DX::EFLAGS_IOPL);
		}
	};
	inline static void RestoreIF(i486DX &cpu,const EFLAGS &eflags)
	{
		if(true!=cpu.IsInRealMode() && cpu.GetIOPL()<cpu.state.CS().DPL)
		{
			cpu.state.EFLAGS&=~i486DX::EFLAGS_INT_ENABLE;
			cpu.state.EFLAGS|=(eflags.eflags&i486DX::EFLAGS_INT_ENABLE);
		}
	}
};

/* } */
#endif
