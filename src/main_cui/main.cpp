#include <iostream>

#include "towns.h"
#include "cpputil.h"


void RunUntil(FMTowns &towns,unsigned int CS,unsigned int EIP)
{
	for(;;)
	{
		auto inst=towns.FetchInstruction();
		auto disasm=towns.cpu.Disassemble(inst,towns.cpu.state.CS(),towns.cpu.state.EIP,towns.mem);
		std::cout << disasm << std::endl;

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
	RunUntil(towns,0x0010,0x117F);
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
