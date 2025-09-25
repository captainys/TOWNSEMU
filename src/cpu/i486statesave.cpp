#include "i486.h"


void i486DXCommon::SegmentProperty::Serialize(std::vector <unsigned char> &data) const
{
	PushUint32(data,baseLinearAddr);
	PushUint32(data,operandSize);
	PushUint32(data,addressSize);
	PushUint32(data,limit);
	PushUint32(data,DPL);
	PushUint32(data,attribBytes);
}

bool i486DXCommon::SegmentProperty::Deserialize(const unsigned char *&data,unsigned int version)
{
	baseLinearAddr=ReadUint32(data);
	operandSize=ReadUint32(data);
	addressSize=ReadUint32(data);
	limit=ReadUint32(data);
	DPL=ReadUint32(data);
	if(2<=version)
	{
		attribBytes=ReadUint32(data);
	}
	else
	{
		attribBytes=0;
	}
	return true;
}

void i486DXCommon::SegmentRegister::Serialize(std::vector <unsigned char> &data) const
{
	SegmentProperty::Serialize(data);
	PushUint16(data,value);
}

bool i486DXCommon::SegmentRegister::Deserialize(const unsigned char *&data,unsigned int version)
{
	SegmentProperty::Deserialize(data,version);
	value=ReadUint16(data);
	return true;
}

void i486DXCommon::SystemAddressRegister::Serialize(std::vector <unsigned char> &data) const
{
	PushUint32(data,linearBaseAddr);
	PushUint16(data,limit);
}

bool i486DXCommon::SystemAddressRegister::Deserialize(const unsigned char *&data)
{
	linearBaseAddr=ReadUint32(data);
	limit=ReadUint16(data);
	return true;
}

void i486DXCommon::SystemAddressRegisterAndSelector::Serialize(std::vector <unsigned char> &data) const
{
	SystemAddressRegister::Serialize(data);
	PushUint16(data,selector);
}

bool i486DXCommon::SystemAddressRegisterAndSelector::Deserialize(const unsigned char *&data)
{
	SystemAddressRegister::Deserialize(data);
	selector=ReadUint16(data);
	return true;
}

void i486DXCommon::TaskRegister::Serialize(std::vector <unsigned char> &data) const
{
	SegmentRegister::Serialize(data);
	PushUint32(data,attrib);
}

bool i486DXCommon::TaskRegister::Deserialize(const unsigned char *&data,unsigned int version)
{
	SegmentRegister::Deserialize(data,version);
	attrib=ReadUint32(data);
	return true;
}

void i486DXCommon::State::Serialize(std::vector <unsigned char> &data) const
{
	for(auto x : NULL_and_reg32)
	{
		PushUint32(data,x);
	}

	PushUint32(data,EIP);
	PushUint32(data,EFLAGS);   // bit 1=Always 1 ([1] pp.2-14)

	for(auto x : sreg)
	{
		x.Serialize(data);
	}
	GDTR.Serialize(data);
	IDTR.Serialize(data);
	LDTR.Serialize(data);
	TR.Serialize(data);

	for(auto x : CR)
	{
		PushUint32(data,x);
	}
	for(auto x : DR)
	{
		PushUint32(data,x);
	}
	for(auto x : TEST)
	{
		PushUint32(data,x);
	}

	// Version 0 not support fpuState
	// FPUState fpuState;

	PushBool(data,halt);
	PushBool(data,holdIRQ);
	PushBool(data,exception);
	PushUint32(data,exceptionCode);
	PushUint32(data,exceptionType);
	PushUint32(data,exceptionLinearAddr); // For EXCEPTION_PF

	// Version 1:
	PushBool(data,fpuState.enabled);
	PushUint16(data,fpuState.statusWord);
	PushUint16(data,fpuState.controlWord);
	PushUint16(data,fpuState.tagWord); // I'm still not sure if this tag word is separate from tag bits of the stack.
	for(auto s : fpuState.stack)
	{
		auto data64=((uint64_t *)&s.value);
		PushUint64(data,*data64);
		PushUint16(data,s.tag);
	}
}
bool i486DXCommon::State::Deserialize(const unsigned char *&data,uint32_t version)
{
	for(auto &x : NULL_and_reg32)
	{
		x=ReadUint32(data);
	}

	EIP=ReadUint32(data);
	EFLAGS=ReadUint32(data);   // bit 1=Always 1 ([1] pp.2-14)

	for(auto &x : sreg)
	{
		x.Deserialize(data,version);
	}
	GDTR.Deserialize(data);
	IDTR.Deserialize(data);
	LDTR.Deserialize(data);
	TR.Deserialize(data,version);

	for(auto &x : CR)
	{
		x=ReadUint32(data);
	}
	for(auto &x : DR)
	{
		x=ReadUint32(data);
	}
	for(auto &x : TEST)
	{
		x=ReadUint32(data);
	}

	// Version 0 not support fpuState
	// FPUState fpuState;

	halt=ReadBool(data);
	holdIRQ=ReadBool(data);
	exception=ReadBool(data);
	exceptionCode=ReadUint32(data);
	exceptionType=ReadUint32(data);
	exceptionLinearAddr=ReadUint32(data); // For EXCEPTION_PF


	// Version 1:
	if(1<=version)
	{
		fpuState.enabled=ReadBool(data);
		fpuState.statusWord=ReadUint16(data);
		fpuState.controlWord=ReadUint16(data);
		fpuState.tagWord=ReadUint16(data); // I'm still not sure if this tag word is separate from tag bits of the stack.
		for(auto &s : fpuState.stack)
		{
			uint64_t data64=ReadUint64(data);
			*((uint64_t *)&s.value)=data64;
			s.tag=ReadUint16(data);
		}
	}
	else
	{
		fpuState.enabled=false;
		fpuState.Reset();
	}


	mode=RecalculateMode();
	CSEIPWindow.CleanUp();   // This must be cleared on state-load.
	SSESPWindow.CleanUp();         // This must be cleared on state-load.
	return true;
}

/* virtual */ uint32_t i486DXCommon::SerializeVersion(void) const
{
	return 2;
	// Version 0 doesn't include FPU state.
	// Version 1 includes FPU state.
	// Version 2 includes segment attrib bytes.
}
/* virtual */ void i486DXCommon::SpecificSerialize(std::vector <unsigned char> &data,std::string) const
{
	state.Serialize(data);
}
/* virtual */ bool i486DXCommon::SpecificDeserialize(const unsigned char *&data,std::string,uint32_t version)
{
	ClearPageTableCache();  // Need to clear on load state.
	ClearDescriptorCache(); // Need to clear on load state.
	callStack.clear();
	return state.Deserialize(data,version);
}
