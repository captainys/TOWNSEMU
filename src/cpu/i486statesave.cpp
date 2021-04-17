#include "i486.h"


void i486DX::SegmentProperty::Serialize(std::vector <unsigned char> &data) const
{
	PushUint32(data,baseLinearAddr);
	PushUint32(data,operandSize);
	PushUint32(data,addressSize);
	PushUint32(data,limit);
	PushUint32(data,DPL);
}

bool i486DX::SegmentProperty::Deserialize(const unsigned char *&data)
{
	baseLinearAddr=ReadUint32(data);
	operandSize=ReadUint32(data);
	addressSize=ReadUint32(data);
	limit=ReadUint32(data);
	DPL=ReadUint32(data);
	return true;
}

void i486DX::SegmentRegister::Serialize(std::vector <unsigned char> &data) const
{
	SegmentProperty::Serialize(data);
	PushUint16(data,value);
}

bool i486DX::SegmentRegister::Deserialize(const unsigned char *&data)
{
	SegmentProperty::Deserialize(data);
	value=ReadUint16(data);
	return true;
}

void i486DX::SystemAddressRegister::Serialize(std::vector <unsigned char> &data) const
{
	PushUint32(data,linearBaseAddr);
	PushUint16(data,limit);
}

bool i486DX::SystemAddressRegister::Deserialize(const unsigned char *&data)
{
	linearBaseAddr=ReadUint32(data);
	limit=ReadUint16(data);
	return true;
}

void i486DX::SystemAddressRegisterAndSelector::Serialize(std::vector <unsigned char> &data) const
{
	SystemAddressRegister::Serialize(data);
	PushUint16(data,selector);
}

bool i486DX::SystemAddressRegisterAndSelector::Deserialize(const unsigned char *&data)
{
	SystemAddressRegister::Deserialize(data);
	selector=ReadUint16(data);
	return true;
}

void i486DX::TaskRegister::Serialize(std::vector <unsigned char> &data) const
{
	SegmentRegister::Serialize(data);
	PushUint32(data,attrib);
}

bool i486DX::TaskRegister::Deserialize(const unsigned char *&data)
{
	SegmentRegister::Deserialize(data);
	attrib=ReadUint32(data);
	return true;
}

void i486DX::State::Serialize(std::vector <unsigned char> &data) const
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
}
bool i486DX::State::Deserialize(const unsigned char *&data,uint32_t version)
{
	for(auto &x : NULL_and_reg32)
	{
		x=ReadUint32(data);
	}

	EIP=ReadUint32(data);
	EFLAGS=ReadUint32(data);   // bit 1=Always 1 ([1] pp.2-14)

	for(auto &x : sreg)
	{
		x.Deserialize(data);
	}
	GDTR.Deserialize(data);
	IDTR.Deserialize(data);
	LDTR.Deserialize(data);
	TR.Deserialize(data);

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



	CSEIPWindow.CleanUp();   // This must be cleared on state-load.
	SSESPWindow.CleanUp();         // This must be cleared on state-load.
	return true;
}

/* virtual */ uint32_t i486DX::SerializeVersion(void) const
{
	return 0; // Version 0 doesn't include FPU state.
}
/* virtual */ void i486DX::SpecificSerialize(std::vector <unsigned char> &data,std::string) const
{
	state.Serialize(data);
}
/* virtual */ bool i486DX::SpecificDeserialize(const unsigned char *&data,std::string,uint32_t version)
{
	ClearPageTableCache();  // Need to clear on load state.
	ClearDescriptorCache(); // Need to clear on load state.
	return state.Deserialize(data,version);
}
