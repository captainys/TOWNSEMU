#include <iostream>
#include <fstream>

#include "towns.h"
#include "cpputil.h"


void RunUntil(FMTowns &towns,unsigned int CS,unsigned int EIP,bool silent)
{
	unsigned int prevCS=0x7fffffff;
	unsigned int prevEIP=0x7fffffff;
	for(;;)
	{
		auto inst=towns.FetchInstruction();

		if(true!=silent)
		{
			if(towns.cpu.state.CS().value!=prevCS || towns.cpu.state.EIP!=prevEIP)
			{
				auto disasm=towns.cpu.Disassemble(inst,towns.cpu.state.CS(),towns.cpu.state.EIP,towns.mem);
				std::cout << disasm << std::endl;
			}
			prevCS=towns.cpu.state.CS().value;
			prevEIP=towns.cpu.state.EIP;
		}

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

	towns.var.freeRunTimerShift=9;
	printf("**** For Testing Purpose Free-Running Timer is scaled up by 512.\n");

	printf("Loaded ROM Images.\n");

	towns.Reset();
	towns.physMem.takeJISCodeLog=true;
	towns.io.EnableLog();
	towns.cpu.enableCallStack=true;

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

	// Path Bit2 of ResetReason is off. (Before fixing SUB DH,Imm8);
	// FC00:12AE XCHG if Bit2 of ResetReason is clear.
	// FC00:135C CALL    WORD PTR CS:[BX+1187H]
	// FC00:12BB Decides later SS and SP
	// FC00:1346 RETF
	// 0010:1E30 Wait Keyboard Ready
	// 0010:1DDA Keyboard Initialized, and the waited 5ms
	// 0010:1DED After waiting 5ms again (Keyboard)
	// 0010:1D56 After waiting 5ms again (Keyboard)
	// 0010:1D5B Checking Boot Key Combination
	// 0010:1DB0 Key combination stored in EBX
	// 0010:1DDA After waiting 5ms again (Keyboard)
	// 0010:1E25 End of Keyboard things.
	// 0010:1DA6 Keyboard initialization again?
	// 0010:1DAB End of Keyboard things.
	// FC00:2B60 Prob: FDC reset
	// 0010:15EE REP MOVSB  Drawing FM TOWNS Logo?
	// 0010:0c87 After drawing FM TOWNS logo.
	// 0010:03aa Probably after printing "Memory Size=0000MB" (in Japanese)
	// 0010:0531 Memory Test
	// 0010:0541 REPE SCASD
	// 0010:0543 End of REPE SCASD
	// 0010:041E JECXZ
	// 0010:0492 After all RAM check?
	// FC00:078A Call wait with C350H 50000us Twice
	// FC00:0794 After wait
	// FC00:05A3 After INT vector is set.
	// 0010:197D MOVSX
	// Memory Card IO 490H, 491H [2] pp.786
	// FC00:0C00 Prob After Checking Memory Card  <- Coming to this address multiple times
	// FC00:0C68 Must be after checking 11 types of devices?  Or re-trying ROM card 11 times?
	// FC00:0CA2 RET
	RunUntil(towns,0x0010,0x1DB0,false);
	towns.cpu.PrintState();

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
		cpputil::WriteBinaryFile("kanjilog.txt",jisTxt.size(),jisTxt.data());
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
