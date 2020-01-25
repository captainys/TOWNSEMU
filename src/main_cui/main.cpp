#include <iostream>
#include <fstream>

#include "towns.h"
#include "cpputil.h"


void RunUntil(FMTowns &towns,unsigned int CS,unsigned int EIP)
{
	unsigned int prevCS=0x7fffffff;
	unsigned int prevEIP=0x7fffffff;
	for(;;)
	{
		auto inst=towns.FetchInstruction();

		if(towns.cpu.state.CS().value!=prevCS || towns.cpu.state.EIP!=prevEIP)
		{
			auto disasm=towns.cpu.Disassemble(inst,towns.cpu.state.CS(),towns.cpu.state.EIP,towns.mem);
			std::cout << disasm << std::endl;
		}
		prevCS=towns.cpu.state.CS().value;
		prevEIP=towns.cpu.state.EIP;

		towns.RunOneInstruction();
		if(true==towns.CheckAbort())
		{
			break;;
		}
		if(towns.cpu.state.CS().value==CS && towns.cpu.state.EIP==EIP)
		{
			break;
		}
	}
}


int main(int ac,char *av[])
{
	if(sizeof(void *)<8)
	{
		printf("This requires minimum 64-bit CPU.\n");
		return 0;
	}


	if(ac<2)
	{
		printf("Usage:\n");
		printf("main_cui rom_directory_name\n");
		return 1;
	}

	FMTowns towns;
	if(true!=towns.LoadROMImages(av[1]))
	{
		return 1;
	}

	printf("Loaded ROM Images.\n");

	towns.Reset();
	towns.physMem.takeJISCodeLog=true;
	towns.io.EnableLog();

	printf("Virtual Machine Reset.\n");

	unsigned int eightBytesCSEIP[8]=
	{
		towns.FetchByteCS_EIP(0),
		towns.FetchByteCS_EIP(1),
		towns.FetchByteCS_EIP(2),
		towns.FetchByteCS_EIP(3),
		towns.FetchByteCS_EIP(4),
		towns.FetchByteCS_EIP(5),
		towns.FetchByteCS_EIP(6),
		towns.FetchByteCS_EIP(7)
	};
	for(auto b : eightBytesCSEIP)
	{
		std::cout << cpputil::Uitox(b) << std::endl;
	}


	std::string cmd;
	// Path Bit2 of ResetReason is on.
	// 0010:24D0 for Ready to Turn Off
	// 0010:21A0 Checking HIRES bit.
	// 0010:2227 Checking VideoOutControl Register 4
	// 0010:2297 After banging 0000 into IO:[0474H] 0x400 times.
	// 0010:239D After massively banging Video Out Control Registers.
	// 0010:20FC CRTC Init Loop
	// 0010:2165 After initializing Digital Palette Registers
	// 0010:2404 Reading VRAMSize IO (0471H)
	// 0010:241B REP STOSD VRAM Clear
	// 0010:241D After VRAM Clear

	// FC00:12AE XCHG if Bit2 of ResetReason is clear.
	RunUntil(towns,0xFC00,0x12AE);

	std::cout << "Kanji Count:" << towns.physMem.JISCodeLog.size() << std::endl;
	{
		std::vector <unsigned char> jisTxt;
		jisTxt.push_back(0x1B);
		jisTxt.push_back(0x24);
		jisTxt.push_back(0x42);
		jisTxt.insert(jisTxt.end(),towns.physMem.JISCodeLog.begin(),towns.physMem.JISCodeLog.end());
		jisTxt.push_back(0x1B);
		jisTxt.push_back(0x28);
		jisTxt.push_back(0x42);
		// cpputil::WriteBinaryFile("kanjilog.txt",jisTxt.size(),jisTxt.data());
	}

	std::cout << ">";
	std::getline(std::cin,cmd);

	for(;;)
	{
		auto inst=towns.FetchInstruction();

		towns.cpu.PrintState();
		towns.PrintStack(32);

		auto disasm=towns.cpu.Disassemble(inst,towns.cpu.state.CS(),towns.cpu.state.EIP,towns.mem);

		std::string cmd;

		std::cout << disasm << std::endl;
		std::cout << ">";
		std::getline(std::cin,cmd);
	
		if(true!=towns.CheckAbort())
		{
			auto clocksPassed=towns.RunOneInstruction();
			std::cout << clocksPassed << " clocks passed." << std::endl;
			towns.CheckAbort();
		}
	}

	return 0;
}
