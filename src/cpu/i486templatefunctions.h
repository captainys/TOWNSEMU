#ifndef I486TEMPLATEFUNCTIONS_IS_INCLUDED
#define I486TEMPLATEFUNCTIONS_IS_INCLUDED
/* { */

#include <string.h> // for memcpy

template <class FIDELITY>
inline void i486DXFidelityLayer <FIDELITY>::Interrupt(unsigned int INTNum,Memory &mem,unsigned int numInstBytesForReturn,unsigned int numInstBytesForCallStack,bool SWI)
{
	FIDELITY fidelity;

	if(nullptr!=debuggerPtr)
	{
		debuggerPtr->Interrupt(*this,INTNum,mem,numInstBytesForReturn,SWI);
	}

	state.halt=false;

	if(IsInRealMode())
	{
		Push(mem,16,cpputil::LowWord(state.EFLAGS),state.CS().value,state.EIP+numInstBytesForReturn);
		// Equivalent:
		// Push(mem,16,state.EFLAGS&0xFFFF);
		// Push(mem,16,state.CS().value);
		// Push(mem,16,state.EIP+numInstBytesForReturn);

		auto intVecAddr=(INTNum&0xFF)*4;
		uint32_t CSIP=mem.FetchDword(state.IDTR.linearBaseAddr+intVecAddr); // Experiment on real FM TOWNS 2MX confirmed that IDTR is valid even in the real mode.
		auto destIP=cpputil::LowWord(CSIP);
		auto destCS=cpputil::HighWord(CSIP);
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

		state.EFLAGS&=(~(EFLAGS_INT_ENABLE|EFLAGS_TRAP));
		// SetIF(false);
		// SetTF(false);
	}
	else
	{
		auto desc=GetInterruptDescriptor(INTNum,mem);
		if(FarPointer::NO_SEG!=desc.SEG)
		{
			const auto type=desc.GetType();
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
			case DESCTYPE_286_INT_GATE: // 0b0110:
				gateOperandSize=16;
				isINTGate=true;
				break;
			case DESCTYPE_286_TRAP_GATE: // 0b0111:
				// I have no idea if it is the correct way of handling an 80286 gate.
				gateOperandSize=16;
				isINTGate=false;
				break;
			case DESCTYPE_TASK_GATE: // 0b0101: //"386 32-bit Task";
				Abort("INT to 386 32-bit Task gate not supported");
				return; // If abort, must return.  The trailing lines may cause seg fault. (Happened while booting OSASK for TOWNS)
				break;
			case DESCTYPE_386_INT_GATE: // 0b1110: //"386 32-bit INT";
				break;
			case DESCTYPE_386_TRAP_GATE: // 0b1111: //"386 32-bit Trap";
				isINTGate=false;
				break;
			case DESCTYPE_AVAILABLE_386_TSS://               9,
			case DESCTYPE_BUSY_386_TSS://                 0x0B,
				Abort("INT-TO-TSS not supported");
				return; // If abort, must return.  The trailing lines may cause seg fault.
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

			// There are three things:
			//   CPL,
			//   Gate's DPL, and
			//   nextCS's DPL.

			// If Software Interrupt && gateDPL<CPL, shoot GP exception.
			if(true==SWI && gateDPL<CPL)
			{
				RaiseException(EXCEPTION_GP,INTNum*8); // What's +EXT?  ([1] pp.26-170)
				HandleException(false,mem,numInstBytesForCallStack);
				return;
			}

			if(0==(state.EFLAGS&EFLAGS_VIRTUAL86))
			{
				SegmentRegister newCS;
				LoadSegmentRegister(newCS,desc.SEG,mem);

				if(newCS.DPL<CPL)
				{
					state.CS().DPL=newCS.DPL; // Raise privilege level so that it can load SS.

					auto TempSS=state.SS().value;
					auto TempESP=state.ESP();
					if(DESCTYPE_AVAILABLE_286_TSS==state.TR.GetType() ||
					   DESCTYPE_BUSY_286_TSS==state.TR.GetType())
					{
						LoadSegmentRegister(state.SS(),FetchWord(32,state.TR,TSS286_OFFSET_SS0+newCS.DPL*4,mem),mem);
						state.ESP()=FetchWord(32,state.TR,TSS286_OFFSET_SP0+newCS.DPL*4,mem);
					}
					else
					{
						// Assume 386 TSS.
						LoadSegmentRegister(state.SS(),FetchWord(32,state.TR,TSS_OFFSET_SS0+newCS.DPL*8,mem),mem);
						state.ESP()=FetchDword(32,state.TR,TSS_OFFSET_ESP0+newCS.DPL*8,mem);
					}
					Push(mem,gateOperandSize,TempSS,TempESP);
				}
				else if(CPL<newCS.DPL)
				{
					Abort("Interrupt to lower-privilege level should raise exception.");
				}


				Push(mem,gateOperandSize,state.EFLAGS,state.CS().value,state.EIP+numInstBytesForReturn);
				// Equivalent >>
				// Push(mem,gateOperandSize,state.EFLAGS);
				// Push(mem,gateOperandSize,state.CS().value);
				// Push(mem,gateOperandSize,state.EIP+numInstBytesForReturn);

				SetIPorEIP(gateOperandSize,desc.OFFSET);
				state.CS()=newCS;
				// Intel 80386 Programmer's Reference Manual 1986 pp.307
				fidelity.Sync_CS_RPL_to_DPL(*this);

				if(true==isINTGate)
				{
					state.EFLAGS&=(~(EFLAGS_INT_ENABLE|EFLAGS_NESTED|EFLAGS_TRAP));
				}
				else
				{
					state.EFLAGS&=(~(EFLAGS_NESTED|EFLAGS_TRAP));
				}
			}
			else // Interrupt from Virtual86 mode
			{
				{
					state.CS().DPL=0;
					// Just in case, set CPL to zero so that SS can be loaded.
					// VM86 monitor is supposed to be ring 0.

					// INT instruction of [1].
					auto TempEFLAGS=state.EFLAGS;
					auto TempSS=state.SS().value;
					auto TempESP=state.ESP();
					state.EFLAGS&=~(EFLAGS_VIRTUAL86|EFLAGS_TRAP);

					state.mode=state.RecalculateMode();

					// if(fromInterruptGate)
					{
						state.EFLAGS&=~EFLAGS_INT_ENABLE;
					}
					// Is TR always 32-bit address size?
					state.CS().DPL=0; // Change to CPL=0 before loading SS.
					LoadSegmentRegister(state.SS(),FetchWord(32,state.TR,TSS_OFFSET_SS0,mem),mem);
					state.ESP()=FetchDword(32,state.TR,TSS_OFFSET_ESP0,mem);

					Push(mem,32,state.GS().value,state.FS().value,state.DS().value);
					Push(mem,32,state.ES().value,TempSS,TempESP);
					Push(mem,32,TempEFLAGS,state.CS().value,state.EIP+numInstBytesForReturn);
					// Equivalent>>
					// Push32(mem,state.GS().value);
					// Push32(mem,state.FS().value);
					// Push32(mem,state.DS().value);
					// Push32(mem,state.ES().value);
					// Push32(mem,TempSS);
					// Push32(mem,TempESP);
					// Push32(mem,TempEFLAGS);
					// Push32(mem,state.CS().value);
					// Push32(mem,state.EIP+numInstBytesForReturn);

					// Need to clear DS,ES,FS,GS.  Or, PUSH FS -> POP FS will shoot GP(0).
					NullifySegmentRegister(state.DS());
					NullifySegmentRegister(state.ES());
					NullifySegmentRegister(state.FS());
					NullifySegmentRegister(state.GS());

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

template <class FIDELITY>
void i486DXFidelityLayer <FIDELITY>::HandleException(bool,Memory &mem,unsigned int numInstBytesForCallStack)
{
	if(nullptr!=debuggerPtr)
	{
		debuggerPtr->HandleException(*this,mem,numInstBytesForCallStack);
		FIDELITY::OnHandleException(*this,debuggerPtr);
	}

	// Only some of the exceptions push error code onto the stack.
	// See Section 9.9 of i486 Programmer's Reference Manual for the information.

	// Clear exception flag before Interrupt, so that Push16/Push32 works again.

	switch(state.exceptionType)
	{
	case EXCEPTION_PF:
		state.exception=false;
		Interrupt(INT_PAGE_FAULT,mem,0,numInstBytesForCallStack,false);
		Push(mem,32,state.exceptionCode);
		SetCR(2,state.exceptionLinearAddr);
		break;
	case EXCEPTION_GP:
		state.exception=false;
		Interrupt(INT_GENERAL_PROTECTION,mem,0,numInstBytesForCallStack,false);
		if(true!=IsInRealMode()) // As HIMEM.SYS's expectation.
		{
			Push(mem,32,state.exceptionCode);
		}
		break;
	case EXCEPTION_ND:
		state.exception=false;
		Interrupt(INT_SEGMENT_NOT_PRESENT,mem,0,numInstBytesForCallStack,false);
		if(true!=IsInRealMode())
		{
			Push(mem,32,state.exceptionCode);
		}
		break;
	case EXCEPTION_UD:
		state.exception=false;
		Interrupt(INT_INVALID_OPCODE,mem,0,numInstBytesForCallStack,false);
		break;
	case EXCEPTION_SS:
		state.exception=false;
		Interrupt(INT_STACK_FAULT,mem,0,numInstBytesForCallStack,false);
		if(true!=IsInRealMode()) // As HIMEM.SYS's expectation.
		{
			Push(mem,32,state.exceptionCode);
		}
		break;
	default:
		Abort("Undefined exception.");
		break;
	}
}

template <class FIDELITY>
const unsigned char *i486DXFidelityLayer <FIDELITY>::GetSegmentDescriptor(unsigned char buf[8],unsigned int selector,const Memory &mem) const
{
	LoadSegmentRegisterTemplate<const i486DXFidelityLayer <FIDELITY>,FIDELITY> loader;
	loader.LoadProtectedModeDescriptor(*this,selector,mem);
	if(nullptr!=loader.rawDesc)
	{
		memcpy(buf,loader.rawDesc,8);
		return buf;
	}
	return nullptr;
}

template <class FIDELITY>
unsigned int i486DXFidelityLayer <FIDELITY>::DebugLoadSegmentRegister(SegmentRegister &reg,unsigned int value,const Memory &mem,bool isInRealMode) const
{
	LoadSegmentRegisterTemplate<const i486DXFidelityLayer <FIDELITY>,FIDELITY> loader;
	return loader.LoadSegmentRegister(*this,reg,value,mem,false,state.mode);
}

template <class FIDELITY>
unsigned int i486DXFidelityLayer <FIDELITY>::LoadSegmentRegister(SegmentRegister &reg,unsigned int value,const Memory &mem)
{
	FIDELITY fidelity;

	if(&reg==&state.SS())
	{
		state.holdIRQ=true;
	}
	LoadSegmentRegisterTemplate<i486DXFidelityLayer <FIDELITY>,FIDELITY> loader;

	fidelity.SetLoadSegmentRegisterFlags(loader.fidelityFlags,*this,reg);

	auto ret=loader.LoadSegmentRegister(*this,reg,value,mem,false,state.mode);

	return ret;
}

template <class FIDELITY>
unsigned int i486DXFidelityLayer <FIDELITY>::LoadSegmentRegister(SegmentRegister &reg,unsigned int value,const Memory &mem,bool,unsigned int mode)
{
	FIDELITY fidelity;

	if(&reg==&state.SS())
	{
		state.holdIRQ=true;
	}
	LoadSegmentRegisterTemplate<i486DXCommon,FIDELITY> loader;

	fidelity.SetLoadSegmentRegisterFlags(loader.fidelityFlags,*this,reg);

	auto ret=loader.LoadSegmentRegister(*this,reg,value,mem,false,mode);

	return ret;
}

template <class FIDELITY>
void i486DXFidelityLayer <FIDELITY>::LoadTaskRegister(unsigned int value,const Memory &mem)
{
	LoadSegmentRegister(state.TR,value,mem);
}

template <class FIDELITY>
i486DXCommon::FarPointer i486DXFidelityLayer <FIDELITY>::GetCallGate(unsigned int &paramWordCount,unsigned int selector,const Memory &mem)
{
	LoadSegmentRegisterTemplate<i486DXFidelityLayer <FIDELITY>,FIDELITY> loader;
	return loader.GetCallGate(paramWordCount,*this,selector,mem);
}

template <class FIDELITY>
i486DXCommon::FarPointer i486DXFidelityLayer <FIDELITY>::DebugGetCallGate(unsigned int &paramWordCount,unsigned int selector,const Memory &mem) const
{
	LoadSegmentRegisterTemplate<const i486DXFidelityLayer <FIDELITY>,FIDELITY> loader;
	return loader.GetCallGate(paramWordCount,*this,selector,mem);
}

template <class FIDELITY>
void i486DXFidelityLayer <FIDELITY>::StoreOperandValue(
    const Operand &dst,Memory &mem,int addressSize,int segmentOverride,const OperandValue &value)
{
	switch(dst.operandType)
	{
	case OPER_UNDEFINED:
		Abort("Tried to evaluate an undefined operand.");
		break;
	case OPER_ADDR:
		{
			unsigned int offset;
			const SegmentRegister &seg=*ExtractSegmentAndOffset(offset,dst,segmentOverride);

			switch(value.numBytes)
			{
			case 1:
				StoreByte(mem,addressSize,seg,offset,value.byteData[0]);
				break;
			case 2:
				StoreWord(mem,addressSize,seg,offset,cpputil::GetWord(value.byteData));// cpputil::GetWord is faster than using value.GetAsWord.
				break;
			case 4:
				StoreDword(mem,addressSize,seg,offset,cpputil::GetDword(value.byteData));// cpputil::GetWord is faster than using value.GetAsDword.
				break;
			default:
				for(unsigned int i=0; i<value.numBytes; ++i)
				{
					StoreByte(mem,addressSize,seg,offset+i,value.byteData[i]);
				}
				break;
			}
		}
		break;
	case OPER_FARADDR:
		Abort("Tried to evaluate FAR ADDRESS.");
		break;
	case OPER_REG32:
		{
			state.NULL_and_reg32[dst.reg]=cpputil::GetDword(value.byteData);
		}
		break;
	case OPER_REG16:
		{
			SET_INT_LOW_WORD(state.NULL_and_reg32[dst.reg&15],cpputil::GetWord(value.byteData));
		}
		break;
	case OPER_REG8:
		SetRegisterValue8(dst.reg,value.byteData[0]);
		break;
	case OPER_SREG:
		LoadSegmentRegister(state.sreg[dst.reg-REG_SEGMENT_REG_BASE],cpputil::GetWord(value.byteData),mem);
		break;

	case OPER_CR0:
	case OPER_CR1:
	case OPER_CR2:
	case OPER_CR3:
		SetCR(dst.operandType-OPER_CR0,cpputil::GetDword(value.byteData));
		break;
	case OPER_DR0:
	case OPER_DR1:
	case OPER_DR2:
	case OPER_DR3:
	case OPER_DR4:
	case OPER_DR5:
	case OPER_DR6:
	case OPER_DR7:
		state.DR[dst.operandType-OPER_DR0]=cpputil::GetDword(value.byteData);
		break;
	case OPER_TEST0:
	case OPER_TEST1:
	case OPER_TEST2:
	case OPER_TEST3:
	case OPER_TEST4:
	case OPER_TEST5:
	case OPER_TEST6:
	case OPER_TEST7:
		state.TEST[dst.operandType-OPER_TEST0]=cpputil::GetDword(value.byteData);
		break;
	default:
		#ifdef _WIN32
			__assume(0);
		#elif defined(__clang__) || defined(__GNUC__)
			__builtin_unreachable();
		#else
			break;
		#endif
	}
}

template <class FIDELITY>
inline unsigned char *i486DXFidelityLayer<FIDELITY>::GetStackAccessPointer(Memory &mem,uint32_t linearAddr,const unsigned int numBytes)
{
	if((linearAddr&(MemoryAccess::MEMORY_WINDOW_SIZE-1))<=(MemoryAccess::MEMORY_WINDOW_SIZE-numBytes))
	{
		if(nullptr==state.SSESPWindow.ptr || true!=state.SSESPWindow.IsLinearAddressInRange(linearAddr))
		{
			auto physAddr=linearAddr;
			if(true==PagingEnabled())
			{
				physAddr=LinearAddressToPhysicalAddressWrite(linearAddr,mem); // Assume write-operation for stack.
			}
			state.SSESPWindow=mem.GetMemoryWindow(physAddr);
			state.SSESPWindow.UpdateLinearBaseAddress(linearAddr);
		}
		// The second conidition in the next line is automatic.
		if(nullptr!=state.SSESPWindow.ptr /*&& true==state.SSESPWindow.IsLinearAddressInRange(linearAddr)*/)
		{
			return state.SSESPWindow.ptr+(linearAddr&(MemoryAccess::MEMORY_WINDOW_SIZE-1));
		}
	}
	return nullptr;
}

template <class FIDELITY>
void i486DXFidelityLayer <FIDELITY>::Push16(Memory &mem,unsigned int value)
{
	FIDELITY fidelity;

	NUM_BYTES_MASK;
	auto addressSize=GetStackAddressingSize();
	auto &ESP=state.ESP();

	class FIDELITY::SavedESP savedESP;
	fidelity.SaveESP(savedESP,addressSize,ESP);

	const unsigned int bytesToStore=2;
	ESP-=bytesToStore;

	fidelity.RestoreESPHighWord(addressSize,ESP,savedESP);

	unsigned int linearAddr=state.SS().baseLinearAddr+(ESP&(numBytesMask[addressSize>>3]));
	auto accessPtr=GetStackAccessPointer(mem,linearAddr,bytesToStore);

	if(fidelity.CheckExceptionInHighFidelityMode(*this))
	{
		return;
	}

	if(nullptr!=accessPtr)
	{
		cpputil::PutWord(accessPtr,value);
		return;
	}

	// When addressSize==16, ESP will be ANDed with 0xFFFF in StoreWord/StoreDword.
	// Also ESP crossing 16-bit boundary would be an exception if addressSize==16.
	// I cannot check it here, but to run a valid application, it shouldn't happen.
	StoreWord(mem,addressSize,state.SS(),ESP,value);
}

template <class FIDELITY>
void i486DXFidelityLayer <FIDELITY>::Push32(Memory &mem,unsigned int value)
{
	FIDELITY fidelity;

	NUM_BYTES_MASK;
	auto addressSize=GetStackAddressingSize();
	auto &ESP=state.ESP();

	class FIDELITY::SavedESP savedESP;
	fidelity.SaveESP(savedESP,addressSize,ESP);

	const unsigned int bytesToStore=4;
	ESP-=bytesToStore;

	fidelity.RestoreESPHighWord(addressSize,ESP,savedESP);

	unsigned int linearAddr=state.SS().baseLinearAddr+(ESP&(numBytesMask[addressSize>>3]));
	auto accessPtr=GetStackAccessPointer(mem,linearAddr,bytesToStore);

	if(fidelity.CheckExceptionInHighFidelityMode(*this))
	{
		return;
	}

	if(nullptr!=accessPtr)
	{
		cpputil::PutDword(accessPtr,value);
		return;
	}
	StoreDword(mem,addressSize,state.SS(),ESP,value);
}

template <class FIDELITY>
unsigned int i486DXFidelityLayer <FIDELITY>::Pop16(Memory &mem)
{
	FIDELITY fidelity;

	NUM_BYTES_MASK;
	unsigned int value;
	auto addressSize=GetStackAddressingSize();
	auto &ESP=state.ESP();
	const unsigned int bytesToPop=2;

	class FIDELITY::SavedESP savedESP;
	fidelity.SaveESP(savedESP,addressSize,ESP);

	unsigned int linearAddr=state.SS().baseLinearAddr+(ESP&(numBytesMask[addressSize>>3]));
	auto accessPtr=GetStackAccessPointer(mem,linearAddr,bytesToPop);
	if(nullptr!=accessPtr)
	{
		ESP+=bytesToPop;
		fidelity.RestoreESPHighWord(addressSize,ESP,savedESP);
		return cpputil::GetWord(accessPtr);
	}

	// When addressSize==16, ESP will be &ed with 0xFFFF in StoreWord/StoreDword.
	// Also ESP crossing 16-bit boundary would be an exception if addressSize==16.
	// I cannot check it here, but to run a valid application, it shouldn't happen.
	value=FetchWord(addressSize,state.SS(),ESP,mem);
	ESP+=2;
	fidelity.RestoreESPHighWord(addressSize,ESP,savedESP);
	return value;
}

template <class FIDELITY>
unsigned int i486DXFidelityLayer <FIDELITY>::Pop32(Memory &mem)
{
	FIDELITY fidelity;

	NUM_BYTES_MASK;
	unsigned int value;
	auto addressSize=GetStackAddressingSize();
	auto &ESP=state.ESP();
	const unsigned int bytesToPop=4;

	class FIDELITY::SavedESP savedESP;
	fidelity.SaveESP(savedESP,addressSize,ESP);

	unsigned int linearAddr=state.SS().baseLinearAddr+(ESP&(numBytesMask[addressSize>>3]));
	auto accessPtr=GetStackAccessPointer(mem,linearAddr,bytesToPop);
	if(nullptr!=accessPtr)
	{
		ESP+=bytesToPop;
		fidelity.RestoreESPHighWord(addressSize,ESP,savedESP);
		return cpputil::GetDword(accessPtr);
	}

	value=FetchDword(addressSize,state.SS(),ESP,mem);
	ESP+=4;
	fidelity.RestoreESPHighWord(addressSize,ESP,savedESP);
	return value;
}

template <class FIDELITY>
void i486DXFidelityLayer <FIDELITY>::Push(Memory &mem,unsigned int operandSize,uint32_t firstPush,uint32_t secondPush)
{
	FIDELITY fidelity;

	NUM_BYTES_MASK;
	auto addressSize=GetStackAddressingSize();
	auto &ESP=state.ESP();

	class FIDELITY::SavedESP savedESP;
	fidelity.SaveESP(savedESP,addressSize,ESP);

	const unsigned int bytesToStore=(operandSize>>3)*2;
	ESP-=bytesToStore;

	fidelity.RestoreESPHighWord(addressSize,ESP,savedESP);

	unsigned int linearAddr=state.SS().baseLinearAddr+(ESP&(numBytesMask[addressSize>>3]));
	auto accessPtr=GetStackAccessPointer(mem,linearAddr,bytesToStore);
	if(nullptr!=accessPtr)
	{
		if(16==operandSize)
		{
			cpputil::PutWord(accessPtr+2,firstPush);
			cpputil::PutWord(accessPtr  ,secondPush);
		}
		else // if(32==operandSize)
		{
			cpputil::PutDword(accessPtr+4,firstPush);
			cpputil::PutDword(accessPtr  ,secondPush);
		}
		return;
	}

	if(16==operandSize)
	{
		StoreWord(mem,addressSize,state.SS(),ESP+2,firstPush);
		StoreWord(mem,addressSize,state.SS(),ESP  ,secondPush);
	}
	else // if(32==operandSize)
	{
		StoreDword(mem,addressSize,state.SS(),ESP+4,firstPush);
		StoreDword(mem,addressSize,state.SS(),ESP  ,secondPush);
	}
}

template <class FIDELITY>
void i486DXFidelityLayer <FIDELITY>::Pop(uint32_t &firstPop,uint32_t &secondPop,Memory &mem,unsigned int operandSize)
{
	FIDELITY fidelity;

	NUM_BYTES_MASK;
	auto addressSize=GetStackAddressingSize();
	auto &ESP=state.ESP();
	const unsigned int bytesToPop=(operandSize>>3)*2;

	class FIDELITY::SavedESP savedESP;
	fidelity.SaveESP(savedESP,addressSize,ESP);

	unsigned int linearAddr=state.SS().baseLinearAddr+(ESP&(numBytesMask[addressSize>>3]));
	auto accessPtr=GetStackAccessPointer(mem,linearAddr,bytesToPop);
	if(nullptr!=accessPtr)
	{
		if(16==operandSize)
		{
			firstPop=cpputil::GetWord(accessPtr);
			secondPop=cpputil::GetWord(accessPtr+2);
		}
		else // if(32==operandSize)
		{
			firstPop=cpputil::GetDword(accessPtr);
			secondPop=cpputil::GetDword(accessPtr+4);
		}
		ESP+=bytesToPop;
		fidelity.RestoreESPHighWord(addressSize,ESP,savedESP);
		return;
	}

	if(16==operandSize)
	{
		firstPop=FetchWord(addressSize,state.SS(),ESP,mem);
		secondPop=FetchWord(addressSize,state.SS(),ESP+2,mem);
		ESP+=4;
	}
	else // if(32==operandSize)
	{
		firstPop=FetchDword(addressSize,state.SS(),ESP,mem);
		secondPop=FetchDword(addressSize,state.SS(),ESP+4,mem);
		ESP+=8;
	}
	fidelity.RestoreESPHighWord(addressSize,ESP,savedESP);
}

template <class FIDELITY>
void i486DXFidelityLayer <FIDELITY>::Push(Memory &mem,unsigned int operandSize,uint32_t firstPush,uint32_t secondPush,uint32_t thirdPush)
{
	FIDELITY fidelity;

	NUM_BYTES_MASK;
	auto addressSize=GetStackAddressingSize();
	auto &ESP=state.ESP();

	class FIDELITY::SavedESP savedESP;
	fidelity.SaveESP(savedESP,addressSize,ESP);

	const unsigned int bytesToStore=(operandSize>>3)*3;
	ESP-=bytesToStore;

	fidelity.RestoreESPHighWord(addressSize,ESP,savedESP);

	unsigned int linearAddr=state.SS().baseLinearAddr+(ESP&(numBytesMask[addressSize>>3]));
	auto accessPtr=GetStackAccessPointer(mem,linearAddr,bytesToStore);
	if(nullptr!=accessPtr)
	{
		if(16==operandSize)
		{
			cpputil::PutWord(accessPtr+4,firstPush);
			cpputil::PutWord(accessPtr+2,secondPush);
			cpputil::PutWord(accessPtr  ,thirdPush);
		}
		else // if(32==operandSize)
		{
			cpputil::PutDword(accessPtr+8,firstPush);
			cpputil::PutDword(accessPtr+4,secondPush);
			cpputil::PutDword(accessPtr  ,thirdPush);
		}
		return;
	}

	if(16==operandSize)
	{
		StoreWord(mem,addressSize,state.SS(),ESP+4,firstPush);
		StoreWord(mem,addressSize,state.SS(),ESP+2,secondPush);
		StoreWord(mem,addressSize,state.SS(),ESP  ,thirdPush);
	}
	else // if(32==operandSize)
	{
		StoreDword(mem,addressSize,state.SS(),ESP+8,firstPush);
		StoreDword(mem,addressSize,state.SS(),ESP+4,secondPush);
		StoreDword(mem,addressSize,state.SS(),ESP  ,thirdPush);
	}
}

template <class FIDELITY>
void i486DXFidelityLayer <FIDELITY>::Pop(uint32_t &firstPop,uint32_t &secondPop,uint32_t &thirdPop,Memory &mem,unsigned int operandSize)
{
	FIDELITY fidelity;

	NUM_BYTES_MASK;
	auto addressSize=GetStackAddressingSize();
	auto &ESP=state.ESP();
	const unsigned int bytesToPop=(operandSize>>3)*3;

	class FIDELITY::SavedESP savedESP;
	fidelity.SaveESP(savedESP,addressSize,ESP);

	unsigned int linearAddr=state.SS().baseLinearAddr+(ESP&(numBytesMask[addressSize>>3]));
	auto accessPtr=GetStackAccessPointer(mem,linearAddr,bytesToPop);
	if(nullptr!=accessPtr)
	{
		if(16==operandSize)
		{
			firstPop=cpputil::GetWord(accessPtr);
			secondPop=cpputil::GetWord(accessPtr+2);
			thirdPop=cpputil::GetWord(accessPtr+4);
		}
		else // if(32==operandSize)
		{
			firstPop=cpputil::GetDword(accessPtr);
			secondPop=cpputil::GetDword(accessPtr+4);
			thirdPop=cpputil::GetDword(accessPtr+8);
		}
		ESP+=bytesToPop;
		fidelity.RestoreESPHighWord(addressSize,ESP,savedESP);
		return;
	}

	if(16==operandSize)
	{
		firstPop=FetchWord(addressSize,state.SS(),ESP,mem);
		secondPop=FetchWord(addressSize,state.SS(),ESP+2,mem);
		thirdPop=FetchWord(addressSize,state.SS(),ESP+4,mem);
		ESP+=6;
	}
	else // if(32==operandSize)
	{
		firstPop=FetchDword(addressSize,state.SS(),ESP,mem);
		secondPop=FetchDword(addressSize,state.SS(),ESP+4,mem);
		thirdPop=FetchDword(addressSize,state.SS(),ESP+8,mem);
		ESP+=12;
	}
	fidelity.RestoreESPHighWord(addressSize,ESP,savedESP);
}

template <class FIDELITY>
inline unsigned long i486DXFidelityLayer<FIDELITY>::LinearAddressToPhysicalAddressRead(unsigned int linearAddr,Memory &mem)
{
	FIDELITY fidelity;

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

	fidelity.SetPageFlags(*this,linearAddr,mem,PAGEINFO_FLAG_A,pageInfo.dir,pageInfo.table);

	auto offset=(linearAddr&4095);
	auto physicalAddr=(pageInfo.table&0xFFFFF000)+offset;
	return physicalAddr;
}

template <class FIDELITY>
inline unsigned long i486DXFidelityLayer<FIDELITY>::LinearAddressToPhysicalAddressWrite(unsigned int linearAddr,Memory &mem)
{
	FIDELITY fidelity;

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

	fidelity.SetPageFlags(*this,linearAddr,mem,PAGEINFO_FLAG_A|PAGEINFO_FLAG_D,pageInfo.dir,pageInfo.table);

	auto offset=(linearAddr&4095);
	auto physicalAddr=(pageInfo.table&0xFFFFF000)+offset;
	return physicalAddr;
}

template <class FIDELITY>
i486DXCommon::InterruptDescriptor i486DXFidelityLayer<FIDELITY>::GetInterruptDescriptor(unsigned int INTNum,Memory &mem)
{
	InterruptDescriptor desc;
	if(8*INTNum<state.IDTR.limit)
	{
		auto DTLinearBaseAddr=state.IDTR.linearBaseAddr;
		DTLinearBaseAddr+=(8*INTNum);

		const unsigned char *rawDesc;
		unsigned char rawDescBuf[8];
		auto memWin=GetConstMemoryWindowFromLinearAddress(DTLinearBaseAddr,mem);
		if(nullptr!=memWin.ptr && (DTLinearBaseAddr&(MemoryAccess::MEMORY_WINDOW_SIZE-1))<=(MemoryAccess::MEMORY_WINDOW_SIZE-8))
		{
			rawDesc=memWin.ptr+(DTLinearBaseAddr&(MemoryAccess::MEMORY_WINDOW_SIZE-1));
		}
		else
		{
			rawDesc=rawDescBuf;
			rawDescBuf[0]=(unsigned char)FetchByteByLinearAddress(mem,DTLinearBaseAddr);
			rawDescBuf[1]=(unsigned char)FetchByteByLinearAddress(mem,DTLinearBaseAddr+1);
			rawDescBuf[2]=(unsigned char)FetchByteByLinearAddress(mem,DTLinearBaseAddr+2);
			rawDescBuf[3]=(unsigned char)FetchByteByLinearAddress(mem,DTLinearBaseAddr+3);
			rawDescBuf[4]=(unsigned char)FetchByteByLinearAddress(mem,DTLinearBaseAddr+4);
			rawDescBuf[5]=(unsigned char)FetchByteByLinearAddress(mem,DTLinearBaseAddr+5);
			rawDescBuf[6]=(unsigned char)FetchByteByLinearAddress(mem,DTLinearBaseAddr+6);
			rawDescBuf[7]=(unsigned char)FetchByteByLinearAddress(mem,DTLinearBaseAddr+7);
		}

		desc.SEG=cpputil::GetWord(rawDesc+2);
		desc.OFFSET=cpputil::GetWord(rawDesc+0)|(cpputil::GetWord(rawDesc+6)<<16);
		desc.flags=cpputil::GetWord(rawDesc+4);
	}
	else
	{
		desc.SEG=FarPointer::NO_SEG;
		desc.OFFSET=0;
	}
	return desc;
}

template <class FIDELITY>
i486DXCommon::OperandValue i486DXFidelityLayer<FIDELITY>::EvaluateOperand(
    Memory &mem,int addressSize,int segmentOverride,const Operand &op,int destinationBytes)
{
	i486DXCommon::OperandValue value;
	value.numBytes=0;
	switch(op.operandType)
	{
	case OPER_UNDEFINED:
		Abort("Tried to evaluate an undefined operand.");
		break;
	case OPER_ADDR:
		{
			value.numBytes=destinationBytes;

			unsigned int offset;
			const SegmentRegister &seg=*ExtractSegmentAndOffset(offset,op,segmentOverride);

			switch(value.numBytes)
			{
			case 1:
				value.byteData[0]=FetchByte(addressSize,seg,offset,mem);
				break;
			case 2:
				value.SetWord(FetchWord(addressSize,seg,offset,mem));
				break;
			case 4:
				value.SetDword(FetchDword(addressSize,seg,offset,mem));
				break;
			default:
				for(unsigned int i=0; i<value.numBytes; ++i)
				{
					value.byteData[i]=FetchByte(addressSize,seg,offset+i,mem);
				}
				break;
			}
		}
		break;
	case OPER_FARADDR:
		Abort("Tried to evaluate FAR ADDRESS.");
		break;
	case OPER_REG32:
		{
			unsigned int reg=state.NULL_and_reg32[op.reg];
			value.numBytes=4;
			cpputil::PutDword(value.byteData,reg);
		}
		break;
	case OPER_REG16:
		{
			unsigned int reg=state.NULL_and_reg32[op.reg&15];
			value.numBytes=2;
			cpputil::PutWord(value.byteData,reg);
		}
		break;
	case OPER_REG8:
		{
			value.numBytes=1;
			value.byteData[0]=GetRegisterValue8(op.reg);
		}
		break;
	case OPER_SREG:
		value.numBytes=2;
		cpputil::PutWord(value.byteData,state.sreg[op.reg-REG_SEGMENT_REG_BASE].value);
		break;

	case OPER_CR0:
	case OPER_CR1:
	case OPER_CR2:
	case OPER_CR3:
		value.numBytes=4;
		cpputil::PutDword(value.byteData,state.GetCR(op.operandType-OPER_CR0));
		break;

	case OPER_DR0:
	case OPER_DR1:
	case OPER_DR2:
	case OPER_DR3:
	case OPER_DR4:
	case OPER_DR5:
	case OPER_DR6:
	case OPER_DR7:
		value.numBytes=4;
		cpputil::PutDword(value.byteData,state.DR[op.operandType-OPER_DR0]);
		break;
	case OPER_TEST0:
	case OPER_TEST1:
	case OPER_TEST2:
	case OPER_TEST3:
	case OPER_TEST4:
	case OPER_TEST5:
	case OPER_TEST6:
	case OPER_TEST7:
		value.numBytes=4;
		cpputil::PutDword(value.byteData,state.TEST[op.operandType-OPER_TEST0]);
		break;
	default:
		#ifdef _WIN32
			__assume(0);
		#elif defined(__clang__) || defined(__GNUC__)
			__builtin_unreachable();
		#else
			break;
		#endif
	}
	return value;
}

template <class FIDELITY>
uint8_t i486DXFidelityLayer<FIDELITY>::EvaluateOperandRegOrMem8(Memory &mem,int addressSize,int segmentOverride,const Operand &op)
{
	if(OPER_ADDR==op.operandType)
	{
		unsigned int offset;
		const SegmentRegister &seg=*ExtractSegmentAndOffset(offset,op,segmentOverride);
		if(16==addressSize)
		{
			offset=cpputil::LowWord(offset);
		}
		return FetchByte(addressSize,seg,offset,mem);
	}
	else
	{
		return GetRegisterValue8(op.reg);
	}
	return 0;
}

template <class FIDELITY>
uint16_t i486DXFidelityLayer<FIDELITY>::EvaluateOperandRegOrMem16(Memory &mem,int addressSize,int segmentOverride,const Operand &op)
{
	i486DXCommon::OperandValue value;
	value.numBytes=0;
	if(OPER_REG32==op.operandType)
	{
		return state.NULL_and_reg32[op.reg];
	}
	else if(OPER_REG16==op.operandType)
	{
		return INT_LOW_WORD(state.NULL_and_reg32[op.reg&15]);
	}
	else
	{
		unsigned int offset;
		const SegmentRegister &seg=*ExtractSegmentAndOffset(offset,op,segmentOverride);
		return FetchWord(addressSize,seg,offset,mem);
	}
}

template <class FIDELITY>
uint32_t i486DXFidelityLayer<FIDELITY>::EvaluateOperandRegOrMem32(Memory &mem,int addressSize,int segmentOverride,const Operand &op)
{
	i486DXCommon::OperandValue value;
	value.numBytes=0;
	if(OPER_REG32==op.operandType)
	{
		return state.NULL_and_reg32[op.reg];
	}
	else if(OPER_REG16==op.operandType)
	{
		return INT_LOW_WORD(state.NULL_and_reg32[op.reg&15]);
	}
	else
	{
		unsigned int offset;
		const SegmentRegister &seg=*ExtractSegmentAndOffset(offset,op,segmentOverride);
		return FetchDword(addressSize,seg,offset,mem);
	}
}

template <class FIDELITY>
i486DXCommon::OperandValue i486DXFidelityLayer<FIDELITY>::EvaluateOperandReg16OrReg32OrMem(
	    Memory &mem,int addressSize,int segmentOverride,const Operand &op,int destinationBytes)
{
	i486DXCommon::OperandValue value;
	value.numBytes=0;
	if(OPER_REG32==op.operandType)
	{
		unsigned int reg=state.NULL_and_reg32[op.reg];
		value.numBytes=4;
		cpputil::PutDword(value.byteData,reg);
	}
	else if(OPER_REG16==op.operandType)
	{
		unsigned int reg=state.NULL_and_reg32[op.reg&15];
		value.numBytes=2;
		cpputil::PutWord(value.byteData,reg);
	}
	else
	{
		value.numBytes=destinationBytes;

		unsigned int offset;
		const SegmentRegister &seg=*ExtractSegmentAndOffset(offset,op,segmentOverride);
		switch(value.numBytes)
		{
		case 1:
			value.byteData[0]=FetchByte(addressSize,seg,offset,mem);
			break;
		case 2:
			value.SetWord(FetchWord(addressSize,seg,offset,mem));
			break;
		case 4:
			value.SetDword(FetchDword(addressSize,seg,offset,mem));
			break;
		default:
			for(unsigned int i=0; i<value.numBytes; ++i)
			{
				value.byteData[i]=FetchByte(addressSize,seg,offset+i,mem);
			}
			break;
		}
	}
	return value;
}

template <class FIDELITY>
i486DXCommon::OperandValue i486DXFidelityLayer<FIDELITY>::EvaluateOperand64(
	    Memory &mem,int addressSize,int segmentOverride,const Operand &op)
{
	i486DXCommon::OperandValue value;
	value.numBytes=0;
	switch(op.operandType)
	{
	default:
		Abort("Tried to evaluate 64-bit from an inappropriate operandType.");
		break;
	case OPER_ADDR:
		{
			value.numBytes=8;

			unsigned int offset;
			const SegmentRegister &seg=*ExtractSegmentAndOffset(offset,op,segmentOverride);

			cpputil::PutDword(value.byteData  ,FetchDword(addressSize,seg,offset,mem));
			cpputil::PutDword(value.byteData+4,FetchDword(addressSize,seg,offset+4,mem));
		}
		break;
	}
	return value;
}

template <class FIDELITY>
i486DXCommon::OperandValue i486DXFidelityLayer<FIDELITY>::EvaluateOperand80(
	    Memory &mem,int addressSize,int segmentOverride,const Operand &op)
{
	i486DXCommon::OperandValue value;
	value.numBytes=0;
	switch(op.operandType)
	{
	default:
		Abort("Tried to evaluate 80-bit from an inappropriate operandType.");
		break;
	case OPER_ADDR:
		{
			value.numBytes=8;

			unsigned int offset;
			const SegmentRegister &seg=*ExtractSegmentAndOffset(offset,op,segmentOverride);

			cpputil::PutDword(value.byteData  ,FetchDword(addressSize,seg,offset,mem));
			cpputil::PutDword(value.byteData+4,FetchDword(addressSize,seg,offset+4,mem));
			cpputil::PutWord(value.byteData+8,FetchWord(addressSize,seg,offset+8,mem));
		}
		break;
	}
	return value;
}

template <class FIDELITY>
bool i486DXFidelityLayer<FIDELITY>::TestIOMapPermission(const SegmentRegister &TR,unsigned int ioMin,unsigned int accessSize,Memory &mem)
{
	unsigned int IOMapOffset0=FetchWord(32,TR,0x66,mem);
	for(auto ioport=ioMin; ioport<ioMin+accessSize; ++ioport)
	{
		unsigned int IOMapOffset=IOMapOffset0+(ioport>>3);
		unsigned int IOMapBit=(1<<(ioport&7));
		if(TR.limit<IOMapOffset)
		{
			return false;
		}
		if(0!=(FetchByte(32,TR,IOMapOffset,mem)&IOMapBit))
		{
			return false;
		}
	}
	return true;
}

template <class FIDELITY>
void i486DXFidelityLayer<FIDELITY>::FetchInstruction(
   MemoryAccess::ConstMemoryWindow &memWin,
   InstructionAndOperand &instOp,
   const SegmentRegister &CS,unsigned int offset,Memory &mem,unsigned int defOperSize,unsigned int defAddrSize)
{
	FetchInstructionClass<i486DXFidelityLayer<FIDELITY>,Memory,RealFetchInstructionFunctions,BurstModeFetchInstructionFunctions>::FetchInstruction(
	    *this,memWin,instOp,CS,offset,mem,defOperSize,defAddrSize);
}

template <class FIDELITY>
inline void i486DXFidelityLayer<FIDELITY>::FetchOperand8(Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,Memory &mem)
{
	inst.operand[inst.operandLen++]=FetchInstructionByte(ptr,inst.codeAddressSize,seg,offset,mem);
	++inst.numBytes;
}
template <class FIDELITY>
inline void i486DXFidelityLayer<FIDELITY>::PeekOperand8(unsigned int &operand,const Instruction &inst,const MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,Memory &mem)
{
	operand=PeekInstructionByte(ptr,inst.codeAddressSize,seg,offset,mem);
}
template <class FIDELITY>
inline void i486DXFidelityLayer<FIDELITY>::FetchOperand16(Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,Memory &mem)
{
	FetchInstructionTwoBytes(inst.operand+inst.operandLen,ptr,inst.codeAddressSize,seg,offset,mem);
	offset+=2;
	inst.operandLen+=2;
	inst.numBytes+=2;
}
template <class FIDELITY>
inline void i486DXFidelityLayer<FIDELITY>::FetchOperand32(Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,Memory &mem)
{
	FetchInstructionFourBytes(inst.operand+inst.operandLen,ptr,inst.codeAddressSize,seg,offset,mem);
	offset+=4;
	inst.operandLen+=4;
	inst.numBytes+=4;
}

template <class FIDELITY>
inline unsigned int i486DXFidelityLayer<FIDELITY>::FetchOperand16or32(Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,Memory &mem)
{
	if(16==inst.operandSize)
	{
		FetchOperand16(inst,ptr,seg,offset,mem);
		return 2;
	}
	else // if(32==inst.operandSize)
	{
		FetchOperand32(inst,ptr,seg,offset,mem);
		return 4;
	}
}

template <class FIDELITY>
inline void i486DXFidelityLayer<FIDELITY>::FetchImm8(Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,Memory &mem)
{
	inst.imm[0]=FetchInstructionByte(ptr,inst.codeAddressSize,seg,offset,mem);
	++inst.numBytes;
}
template <class FIDELITY>
inline void i486DXFidelityLayer<FIDELITY>::FetchImm16(Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,Memory &mem)
{
	FetchInstructionTwoBytes(inst.imm,ptr,inst.codeAddressSize,seg,offset,mem);
	inst.numBytes+=2;
}
template <class FIDELITY>
inline void i486DXFidelityLayer<FIDELITY>::FetchImm32(Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,Memory &mem)
{
	FetchInstructionFourBytes(inst.imm,ptr,inst.codeAddressSize,seg,offset,mem);
	inst.numBytes+=4;
}
template <class FIDELITY>
inline unsigned int i486DXFidelityLayer<FIDELITY>::FetchImm16or32(Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,Memory &mem)
{
	if(16==inst.operandSize)
	{
		FetchImm16(inst,ptr,seg,offset,mem);
		return 2;
	}
	else // if(32==inst.operandSize)
	{
		FetchImm32(inst,ptr,seg,offset,mem);
		return 4;
	}
}

template <class FIDELITY>
void i486DXFidelityLayer<FIDELITY>::StoreOperandValueRegOrMem8(const Operand &dst,Memory &mem,int addressSize,int segmentOverride,uint8_t value)
{
	if(OPER_REG8==dst.operandType)
	{
		SetRegisterValue8(dst.reg,value);
	}
	else
	{
		unsigned int offset;
		const SegmentRegister &seg=*ExtractSegmentAndOffset(offset,dst,segmentOverride);
		StoreByte(mem,addressSize,seg,offset,value);
	}
}
template <class FIDELITY>
void i486DXFidelityLayer<FIDELITY>::StoreOperandValueRegOrMem16(const Operand &dst,Memory &mem,int addressSize,int segmentOverride,uint16_t value)
{
	if(OPER_REG32==dst.operandType)
	{
		state.NULL_and_reg32[dst.reg]=value;
	}
	else if(OPER_REG16==dst.operandType)
	{
		SET_INT_LOW_WORD(state.NULL_and_reg32[dst.reg&15],value);
	}
	else
	{
		unsigned int offset;
		const SegmentRegister &seg=*ExtractSegmentAndOffset(offset,dst,segmentOverride);
		StoreWord(mem,addressSize,seg,offset,value);
	}
}
template <class FIDELITY>
void i486DXFidelityLayer<FIDELITY>::StoreOperandValueRegOrMem32(const Operand &dst,Memory &mem,int addressSize,int segmentOverride,uint32_t value)
{
	if(OPER_REG32==dst.operandType)
	{
		state.NULL_and_reg32[dst.reg]=value;
	}
	else if(OPER_REG16==dst.operandType)
	{
		SET_INT_LOW_WORD(state.NULL_and_reg32[dst.reg&15],value);
	}
	else
	{
		unsigned int offset;
		const SegmentRegister &seg=*ExtractSegmentAndOffset(offset,dst,segmentOverride);
		StoreDword(mem,addressSize,seg,offset,value);
	}
}

template <class FIDELITY>
void i486DXFidelityLayer<FIDELITY>::StoreOperandValueReg16OrReg32OrMem(
    const Operand &dst,Memory &mem,int addressSize,int segmentOverride,const OperandValue &value)
{
	if(OPER_REG32==dst.operandType)
	{
		state.NULL_and_reg32[dst.reg]=cpputil::GetDword(value.byteData);
	}
	else if(OPER_REG16==dst.operandType)
	{
		SET_INT_LOW_WORD(state.NULL_and_reg32[dst.reg&15],cpputil::GetWord(value.byteData));
	}
	else
	{
		unsigned int offset;
		const SegmentRegister &seg=*ExtractSegmentAndOffset(offset,dst,segmentOverride);
		switch(value.numBytes)
		{
		case 1:
			StoreByte(mem,addressSize,seg,offset,value.byteData[0]);
			break;
		case 2:
			StoreWord(mem,addressSize,seg,offset,cpputil::GetWord(value.byteData));// cpputil::GetWord is faster than using value.GetAsWord.
			break;
		case 4:
			StoreDword(mem,addressSize,seg,offset,cpputil::GetDword(value.byteData));// cpputil::GetWord is faster than using value.GetAsDword.
			break;
		default:
			for(unsigned int i=0; i<value.numBytes; ++i)
			{
				StoreByte(mem,addressSize,seg,offset+i,value.byteData[i]);
			}
			break;
		}
	}
}

template <class FIDELITY>
void i486DXFidelityLayer<FIDELITY>::StoreOperandValue64(
    const Operand &dst,Memory &mem,int addressSize,int segmentOverride,const OperandValue &value)
{
	switch(dst.operandType)
	{
	default:
		Abort("Tried to store 64-bit value to a non-address operand.");
		break;
	case OPER_ADDR:
		{
			unsigned int offset;
			const SegmentRegister &seg=*ExtractSegmentAndOffset(offset,dst,segmentOverride);
			StoreDword(mem,addressSize,seg,offset,  cpputil::GetDword(value.byteData));
			StoreDword(mem,addressSize,seg,offset+4,cpputil::GetDword(value.byteData+4));
		}
		break;
	}
}

template <class FIDELITY>
void i486DXFidelityLayer<FIDELITY>::StoreOperandValue80(
    const Operand &dst,Memory &mem,int addressSize,int segmentOverride,const OperandValue &value)
{
	switch(dst.operandType)
	{
	default:
		Abort("Tried to store 64-bit value to a non-address operand.");
		break;
	case OPER_ADDR:
		{
			unsigned int offset;
			const SegmentRegister &seg=*ExtractSegmentAndOffset(offset,dst,segmentOverride);
			StoreDword(mem,addressSize,seg,offset,  cpputil::GetDword(value.byteData));
			StoreDword(mem,addressSize,seg,offset+4,cpputil::GetDword(value.byteData+4));
			StoreWord(mem,addressSize,seg,offset+8,cpputil::GetWord(value.byteData+8));
		}
		break;
	}
}

/* } */
#endif
