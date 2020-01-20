#include <iostream>

#include "towns.h"
#include "cpputil.h"



int main(int ac,char *av[])
{
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


	for(;;)
	{
		auto inst=towns.FetchInstruction();

		towns.cpu.PrintState();

		auto disasm=towns.cpu.Disassemble(inst,towns.cpu.state.CS,towns.cpu.state.EIP,towns.mem);

		std::cout << disasm << std::endl;
		std::cout << ">";
		std::string cmd;
		std::cin >> cmd;

		if(true!=towns.CheckAbort())
		{
			auto clocksPassed=towns.RunOneInstruction();
			std::cout << clocksPassed << " clocks passed." << std::endl;
			towns.CheckAbort();
		}
	}

	return 0;
}
