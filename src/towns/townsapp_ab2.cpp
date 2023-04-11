#include <iostream>
#include "towns.h"

// Afterburner II Application-Specific Customization
// Contribution from BCC.

void FMTownsCommon::AB2_Identify(void)
{
	i486DX::SegmentRegister CS,DS;
	unsigned int exceptionType,exceptionCode;
	cpu.DebugLoadSegmentRegister(DS,0x0014,mem,false);
	cpu.DebugLoadSegmentRegister(CS,0x000C,mem,false);

	if(0x66 == cpu.DebugFetchByte(32, CS, 0xeeea, mem) &&
	   0xa3 == cpu.DebugFetchByte(32, CS, 0xeeeb, mem) &&
	   0x44 == cpu.DebugFetchByte(32, CS, 0xeeec, mem) &&
	   0x2f == cpu.DebugFetchByte(32, CS, 0xeeed, mem)) // for After Burner 1.01
	{
		std::cout << "Afterburner II Ver. 1.01" << std::endl;
		cpu.DebugStoreDword(mem, 32, CS, 0xeeea, 0x90909090);
		cpu.DebugStoreWord(mem, 32, CS, 0xeeee, 0x9090);
		state.appSpecific_ThrottlePtr=cpu.DebugLinearAddressToPhysicalAddress(exceptionType,exceptionCode,DS.baseLinearAddr+0x82f44,mem);
	}
	else if(0x66 == cpu.DebugFetchByte(32, CS, 0xef4e, mem) &&
	        0xa3 == cpu.DebugFetchByte(32, CS, 0xef4f, mem) &&
	        0xa8 == cpu.DebugFetchByte(32, CS, 0xef50, mem) &&
	        0x2f == cpu.DebugFetchByte(32, CS, 0xef51, mem)) // for After Burner 1.02
	{
		std::cout << "Afterburner II Ver. 1.02" << std::endl;
		cpu.DebugStoreDword(mem, 32, CS, 0xef4e, 0x90909090);
		cpu.DebugStoreWord(mem, 32, CS, 0xef52, 0x9090);
		state.appSpecific_ThrottlePtr=cpu.DebugLinearAddressToPhysicalAddress(exceptionType,exceptionCode,DS.baseLinearAddr+0x82fa8,mem);
	}
	else if(0x66 == cpu.DebugFetchByte(32, CS, 0xf6f6, mem) &&
	        0xa3 == cpu.DebugFetchByte(32, CS, 0xf6f7, mem) &&
	        0xe4 == cpu.DebugFetchByte(32, CS, 0xf6f8, mem) &&
	        0x5d == cpu.DebugFetchByte(32, CS, 0xf6f9, mem)) // for After Burner 1.03
	{
		std::cout << "Afterburner II Ver. 1.03" << std::endl;
		cpu.DebugStoreDword(mem, 32, CS, 0xf6f6, 0x90909090);
		cpu.DebugStoreWord(mem, 32, CS, 0xf6fa, 0x9090);
		state.appSpecific_ThrottlePtr=cpu.DebugLinearAddressToPhysicalAddress(exceptionType,exceptionCode,DS.baseLinearAddr+0x85de4,mem);
	}
}

void FMTownsCommon::AB2_Throttle(unsigned int inputThr)
{
	if(0!=state.appSpecific_ThrottlePtr)
	{
		switch (inputThr)
		{
		case 2: // High
			mem.StoreByte(state.appSpecific_ThrottlePtr,1);
			break;
		case 0: // Low
			mem.StoreByte(state.appSpecific_ThrottlePtr,2);
			break;
		default: // Normal
			mem.StoreByte(state.appSpecific_ThrottlePtr,3);
		}
	}
}
