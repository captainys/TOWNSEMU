#ifndef I486LOADSEGREG_IS_INCLUDED
#define I486LOADSEGREG_IS_INCLUDED
/* { */

// Must be included from i486???.cpp only.

template <class CPUCLASS,class FIDELITY>
class i486DXCommon::LoadSegmentRegisterTemplate
{
public:
	unsigned char rawDescBuf[8];
	const unsigned char *rawDesc;

	FIDELITY fidelity;
	class FIDELITY::LoadSegmentRegisterFlags fidelityFlags;

	// For mutable i486DXCommon >>
	static inline unsigned int FetchByteByLinearAddress(i486DXFidelityLayer<FIDELITY> &cpu,Memory &mem,unsigned int linearAddr)
	{
		return cpu.FetchByteByLinearAddress(mem,linearAddr);
	}
	static inline MemoryAccess::ConstMemoryWindow GetConstMemoryWindowFromLinearAddress(i486DXFidelityLayer<FIDELITY> &cpu,unsigned int linearAddr,Memory &mem)
	{
		return cpu.GetConstMemoryWindowFromLinearAddress(linearAddr,mem);
	}
	static inline const unsigned char *LoadFromDescriptorCache(i486DXCommon &cpu,uint16_t selectorValue)
	{
		auto index=(selectorValue>>DESCRIPTOR_TO_INDEX_SHIFT);
		if(cpu.state.descriptorCacheValidCounter<=cpu.state.descriptorCacheValid[index])
		{
			return cpu.state.descriptorCache[index];
		}
		return nullptr;
	}
	static inline void StoreToDescriptorCache(i486DXCommon &cpu,uint16_t selectorValue,const unsigned char *descPtr)
	{
		auto index=(selectorValue>>DESCRIPTOR_TO_INDEX_SHIFT);
		cpu.state.descriptorCache[index]=descPtr;
		cpu.state.descriptorCacheValid[index]=cpu.state.descriptorCacheValidCounter;
	}
	static inline void SetAccessedFlag(uint8_t rawDesc[],i486DXCommon &cpu)
	{
		rawDesc[5]|=1;
	}
	// For mutable i486DXCommon <<

	// For constant i486DXCommon >>
	static inline unsigned int FetchByteByLinearAddress(const i486DXCommon &cpu,const Memory &mem,unsigned int linearAddr)
	{
		return cpu.DebugFetchByteByLinearAddress(mem,linearAddr);
	}
	static inline MemoryAccess::ConstMemoryWindow GetConstMemoryWindowFromLinearAddress(const i486DXCommon &cpu,unsigned int linearAddr,const Memory &mem)
	{
		return cpu.DebugGetConstMemoryWindowFromLinearAddress(linearAddr,mem);
	}
	static inline const unsigned char *LoadFromDescriptorCache(const i486DXCommon &,uint16_t)
	{
		return nullptr;
	}
	static inline void StoreToDescriptorCache(const i486DXCommon &,uint16_t selectorValue,const unsigned char *)
	{
	}
	// For constant i486DXCommon <<



	inline void LoadProtectedModeDescriptor(CPUCLASS &cpu,unsigned int value,const Memory &mem)
	{
		value=cpu.LOW16BITS(value);

		typename FIDELITY::LoadSegmentRegisterVariables fidelityVar;

		rawDesc=LoadFromDescriptorCache(cpu,value);
		if(nullptr!=rawDesc)
		{
			return;
		}

		auto RPL=(value&3);
		auto TI=(0!=(value&4));

		unsigned int DTLinearBaseAddr=0;
		if(0==TI)
		{
			DTLinearBaseAddr=cpu.state.GDTR.linearBaseAddr;
			fidelity.SetLimit(fidelityVar,cpu.state.GDTR.limit);
		}
		else
		{
			DTLinearBaseAddr=cpu.state.LDTR.linearBaseAddr;
			fidelity.SetLimit(fidelityVar,cpu.state.LDTR.limit);
		}
		DTLinearBaseAddr+=(value&0xfff8); // Use upper 13 bits.

		if(true==fidelity.CheckSelectorBeyondLimit(cpu,fidelityVar,value))
		{
			return;
		}

		auto memWin=GetConstMemoryWindowFromLinearAddress(cpu,DTLinearBaseAddr,mem);
		if(nullptr!=memWin.ptr && (DTLinearBaseAddr&(MemoryAccess::MEMORY_WINDOW_SIZE-1))<=(MemoryAccess::MEMORY_WINDOW_SIZE-8))
		{
			rawDesc=memWin.ptr+(DTLinearBaseAddr&(MemoryAccess::MEMORY_WINDOW_SIZE-1));
			StoreToDescriptorCache(cpu,value,rawDesc);
		}
		else
		{
			rawDesc=rawDescBuf;
			rawDescBuf[0]=(unsigned char)FetchByteByLinearAddress(cpu,mem,DTLinearBaseAddr);
			rawDescBuf[1]=(unsigned char)FetchByteByLinearAddress(cpu,mem,DTLinearBaseAddr+1);
			rawDescBuf[2]=(unsigned char)FetchByteByLinearAddress(cpu,mem,DTLinearBaseAddr+2);
			rawDescBuf[3]=(unsigned char)FetchByteByLinearAddress(cpu,mem,DTLinearBaseAddr+3);
			rawDescBuf[4]=(unsigned char)FetchByteByLinearAddress(cpu,mem,DTLinearBaseAddr+4);
			rawDescBuf[5]=(unsigned char)FetchByteByLinearAddress(cpu,mem,DTLinearBaseAddr+5);
			rawDescBuf[6]=(unsigned char)FetchByteByLinearAddress(cpu,mem,DTLinearBaseAddr+6);
			rawDescBuf[7]=(unsigned char)FetchByteByLinearAddress(cpu,mem,DTLinearBaseAddr+7);
		}
	}

	inline unsigned int LoadSegmentRegister(CPUCLASS &cpu,SegmentRegister &reg,unsigned int value,const Memory &mem,bool isInRealMode)
	{
		value=cpu.LOW16BITS(value);
		if(true==isInRealMode || 0!=(i486DXCommon::EFLAGS_VIRTUAL86&cpu.state.EFLAGS))
		{
			reg.value=value;
			reg.baseLinearAddr=(value<<4);
			reg.addressSize=16;
			reg.operandSize=16;
			// reg.limit=0xffff;   Surprisingly, reg.limit isn't affected!?  According to https://wiki.osdev.org/Unreal_Mode
			reg.limit=std::max<unsigned int>(reg.limit,0xffff);
			reg.DPL=(0!=(i486DXCommon::EFLAGS_VIRTUAL86&cpu.state.EFLAGS) ? 3 : 0);
			fidelity.ClearSegmentRegisterAttribBytes(reg.attribBytes);
			return 0xFFFFFFFF;
		}
		else
		{
			if(true==fidelity.LoadNullSelector(cpu,reg,value))
			{
				return 0;
			}
			LoadProtectedModeDescriptor(cpu,value,mem);
			if(nullptr==rawDesc || true==fidelity.DescriptorException(fidelityFlags,cpu,value,rawDesc))
			{
				rawDesc=nullptr;
				return 0;
			}

			// Sample GDT from WRHIGH.ASM
			//	DB		0FFH,0FFH	; Segment Limit (0-15)
			//	DB		0,0,010H		; Base Address 0-23
			//	DB		10010010B	; P=1, DPL=00, S=1, TYPE=0010
			//	DB		11000111B	; G=1, DB=1, (Unused)=0, A=0, LIMIT 16-19=0011
			//	DB		0			; Base Address 24-31

		#ifdef YS_LITTLE_ENDIAN
			uint32_t segLimit,segBase;
			uint8_t *segLimitPtr=(uint8_t *)&segLimit,*segBasePtr=(uint8_t *)&segBase;
			cpputil::CopyWord(segLimitPtr,rawDesc+0);
			segLimitPtr[2]=rawDesc[6]&0x0F;
			segLimitPtr[3]=0;
			cpputil::CopyWord(segBasePtr,rawDesc+2);
			segBasePtr[2]=rawDesc[4];
			segBasePtr[3]=rawDesc[7];
		#else
			unsigned int segLimit=cpputil::GetWord(rawDesc+0)|((rawDesc[6]&0x0F)<<16);
			unsigned int segBase=cpputil::GetWord(rawDesc+2)|(rawDesc[4]<<16)|(rawDesc[7]<<24);
		#endif

			if((0x80&rawDesc[6])==0) // G==0
			{
				reg.limit=segLimit;
			}
			else
			{
				reg.limit=(segLimit+1)*4096-1;
			}
			reg.baseLinearAddr=segBase;
			reg.value=value;
			reg.DPL=(rawDesc[5]>>5)&3;
			reg.attribBytes=cpputil::GetWord(rawDesc+5);

			if((0x40&rawDesc[6])==0) // D==0
			{
				reg.addressSize=16;
				reg.operandSize=16;
			}
			else
			{
				reg.addressSize=32;
				reg.operandSize=32;
			}
			return cpputil::GetDword(rawDesc+4);
		}
	}

	inline i486DXCommon::FarPointer GetCallGate(CPUCLASS &cpu,unsigned int value,const Memory &mem)
	{
		LoadProtectedModeDescriptor(cpu,value,mem);

		// i486 Programmer's Reference Manual pp.6-11 Figure 6-5 Call Gate
		// What is "COUNT" used for?
		i486DXCommon::FarPointer ptr;
		ptr.SEG=(rawDesc[2]|(rawDesc[3]<<8));
		ptr.OFFSET=(rawDesc[0]|(rawDesc[1]<<8)|(rawDesc[6]<<16)|(rawDesc[7]<<24));
		return ptr;
	}
};

/* } */
#endif
