#ifndef I486TEMPLATEFUNCTIONS_IS_INCLUDED
#define I486TEMPLATEFUNCTIONS_IS_INCLUDED
/* { */

template <class FIDELITY>
inline void i486DXFidelityLayer <FIDELITY>::Interrupt(unsigned int INTNum,Memory &mem,unsigned int numInstBytesForReturn,unsigned int numInstBytesForCallStack,bool SWI)
{
	FIDELITY fidelity;

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
				Abort("386 16-bit INT gate not supported");
				break;
			case DESCTYPE_386_INT_GATE: // 0b1110: //"386 32-bit INT";
				break;
			case DESCTYPE_386_TRAP_GATE: // 0b1111: //"386 32-bit Trap";
				isINTGate=false;
				break;
			case DESCTYPE_AVAILABLE_386_TSS://               9,
			case DESCTYPE_BUSY_386_TSS://                 0x0B,
				Abort("INT-TO-TSS not supported");
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

					auto TempSS=state.SS();
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
					Push(mem,gateOperandSize,TempSS.value);
					Push(mem,gateOperandSize,TempESP);
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

template <class FIDELITY>
void i486DXFidelityLayer <FIDELITY>::HandleException(bool,Memory &mem,unsigned int numInstBytesForCallStack)
{
	if(nullptr!=debuggerPtr)
	{
		debuggerPtr->HandleException(*this,mem,numInstBytesForCallStack);
	}

	// Only some of the exceptions push error code onto the stack.
	// See Section 9.9 of i486 Programmer's Reference Manual for the information.
	switch(state.exceptionType)
	{
	case EXCEPTION_PF:
		Interrupt(INT_PAGE_FAULT,mem,0,numInstBytesForCallStack,false);
		Push(mem,32,state.exceptionCode);
		SetCR(2,state.exceptionLinearAddr);
		break;
	case EXCEPTION_GP:
		Interrupt(INT_GENERAL_PROTECTION,mem,0,numInstBytesForCallStack,false);
		if(true!=IsInRealMode()) // As HIMEM.SYS's expectation.
		{
			Push(mem,32,state.exceptionCode);
		}
		break;
	case EXCEPTION_ND:
		Interrupt(INT_SEGMENT_NOT_PRESENT,mem,0,numInstBytesForCallStack,false);
		if(true!=IsInRealMode())
		{
			Push(mem,32,state.exceptionCode);
		}
		break;
	case EXCEPTION_UD:
		Interrupt(INT_INVALID_OPCODE,mem,0,numInstBytesForCallStack,false);
		break;
	case EXCEPTION_SS:
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
	state.exception=false;
}

/* } */
#endif
