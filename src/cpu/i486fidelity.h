#ifndef I486FIDELITY_IS_INCLUDED
#define I486FIDELITY_IS_INCLUDED
/* { */

class i486DXLowFidelity
{
public:
	inline static void SaveESPHighWord(uint8_t,uint32_t){}
	inline static void RestoreESPHighWord(uint8_t,uint32_t &){}

	inline static void Sync_SS_CS_RPL_to_DPL(class i486DX &,i486DX::SegmentRegister &,i486DX::SegmentRegister &){}

	constexpr bool UDException_MOV_TO_CS(class i486DX &,uint32_t reg,Memory &mem,uint32_t instNumBytes){return false;}

	constexpr bool IOPLException(class i486DX &,uint32_t exceptionType,Memory &,uint32_t instNumBytes){return false;}

	constexpr bool HandleExceptionIfAny(class i486DX &,Memory &,uint32_t instNumBytes){return false;}

	constexpr bool PageLevelException(class i486DX &cpu,bool write,uint32_t linearAddr,uint32_t pageInfo){return false;}

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

	inline static bool PageLevelException(class i486DX &cpu,bool write,uint32_t linearAddr,uint32_t pageInfo)
	{
		auto raise=[&]
		{
			uint32_t code=i486DX::PFFLAG_PAGE_LEVEL;
			if(true==write)
			{
				code|=i486DX::PFFLAG_WRITE;
			}
			if(0!=cpu.state.CS().DPL)
			{
				code|=i486DX::PFFLAG_USER_MODE;
			}
			cpu.RaiseException(i486DX::EXCEPTION_PF,code);
			cpu.state.exceptionLinearAddr=linearAddr;
		};

		if(true==write && 0==(pageInfo&i486DX::PAGEINFO_FLAG_RW))
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
