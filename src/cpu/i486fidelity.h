#ifndef I486FIDELITY_IS_INCLUDED
#define I486FIDELITY_IS_INCLUDED
/* { */

class i486DXLowFidelity
{
public:
	inline static void SaveESPHighWord(uint8_t,uint32_t){}
	inline static void RestoreESPHighWord(uint8_t,uint32_t &){}

	inline static void Sync_SS_CS_RPL_to_DPL(class i486DX &,i486DX::SegmentRegister &,i486DX::SegmentRegister &){}

	constexpr bool UDException_MOV_TO_CS(class i486DX &,uint32_t reg,Memory &mem,uint32_t instNumBytes) const{return false;}

	constexpr bool IOPLException(class i486DX &,uint32_t exceptionType,Memory &,uint32_t instNumBytes) const{return false;}

	constexpr bool HandleExceptionIfAny(class i486DX &,Memory &,uint32_t instNumBytes) const{return false;}

	constexpr bool PageLevelException(class i486DX &cpu,bool write,uint32_t linearAddr,uint32_t pageIndex,uint32_t pageInfo) const{return false;}

	// LoadSegmentRegister
	class LoadSegmentRegisterVariables
	{
	public:
		uint32_t limit;
	};
	inline static void SetLimit(LoadSegmentRegisterVariables &,uint32_t limit){};
	template <class LoaderClass>
	inline constexpr bool CheckSelectorBeyondLimit(class i486DX &cpu,LoaderClass &loader,LoadSegmentRegisterVariables &var,uint32_t selector){return false;}
	template <class LoaderClass>
	inline constexpr bool CheckSelectorBeyondLimit(const class i486DX &cpu,LoaderClass &loader,LoadSegmentRegisterVariables &var,uint32_t selector){return false;}
};

class i486DXDefaultFidelity : public i486DXLowFidelity
{
public:
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

	inline static void Sync_SS_CS_RPL_to_DPL(class i486DX &cpu,i486DX::SegmentRegister &CS,i486DX::SegmentRegister &setSeg)
	{
		if(&CS==&setSeg)
		{
			cpu.state.CS().value&=0xFFFC;
			cpu.state.CS().value|=cpu.state.CS().DPL;
			cpu.state.SS().value&=0xFFFC;
			cpu.state.SS().value|=cpu.state.CS().DPL;
		}
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

	inline static void SetLimit(LoadSegmentRegisterVariables &var,uint32_t limit)
	{
		var.limit=limit;
	}
	template <class LoaderClass>
	inline static bool CheckSelectorBeyondLimit(class i486DX &cpu,LoaderClass &loader,LoadSegmentRegisterVariables &var,uint32_t selector)
	{
		if(var.limit<=(selector&0xfff8))
		{
			loader.RaiseException(cpu,i486DX::EXCEPTION_GP,selector);
			return true;
		}
		return false;
	}
	using i486DXLowFidelity::CheckSelectorBeyondLimit;
};

/* } */
#endif
