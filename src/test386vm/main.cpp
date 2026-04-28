#include "vmbase.h"
#include "i486.h"
#include "ramrom.h"
#include "cpputil.h"
#include <memory>

#define IO_POST 0x190
#define IO_LPT0 0x3BC
#define IO_LPT1 0x378
#define IO_LPT2 0x278


void WaitEnter(void)
{
	std::cout << std::endl;
	std::cout << ">";
	std::string str;
	std::getline(std::cin,str);
}

class TesterVM : public VMBase, public Device
{
public:
	int POSTCODE=0;

	const char *DeviceName(void) const override
	{
		return "TEST386VM";
	}

	TesterVM() : Device(this)
	{
	}
	virtual void IOWriteByte(unsigned int ioport,unsigned int data) override
	{
		if(IO_POST==ioport)
		{
			POSTCODE=data;
			std::cout << std::endl;
			std::cout << "POST ";
			std::cout << cpputil::Ubtox(data);
			std::cout << std::endl;
		}
		if(IO_LPT0==ioport || IO_LPT1==ioport || IO_LPT2==ioport)
		{
			std::cout << char(data);
		}
	}
	virtual unsigned int IOReadByte(unsigned int ioport) override
	{
		return 0xFF;
	}
};

class TestVMMemory : public Memory
{
public:
	uint8_t ROM[65536];
	uint8_t RAM[0x1000000];
};

Memory::Memory()
{
}

void Memory::CleanUp(void)
{
}
unsigned int Memory::FetchByte(unsigned int physAddr) const
{
	const TestVMMemory *realThis=(TestVMMemory *)this;
	if(physAddr<=0x000F0000)
	{
		return realThis->RAM[physAddr];
	}
	else if(physAddr<0x00100000 || 0xFFFF0000<=physAddr)
	{
		return realThis->ROM[(physAddr&0xFFFF)];
	}
	return 0xFF;
}

unsigned int Memory::FetchWord(unsigned int physAddr) const
{
	return FetchByte(physAddr)|(FetchByte(physAddr+1)<<8);
}

unsigned int Memory::FetchDword(unsigned int physAddr) const
{
	return FetchWord(physAddr)|(FetchWord(physAddr+2)<<16);
}

MemoryAccess::ConstMemoryWindow Memory::GetConstMemoryWindow(unsigned int physAddr) const
{
	MemoryAccess::ConstMemoryWindow window;
	window.ptr=nullptr;
	return window;
}
MemoryAccess::MemoryWindow Memory::GetMemoryWindow(unsigned int physAddr)
{
	MemoryAccess::MemoryWindow window;
	window.ptr=nullptr;
	return window;
}

void Memory::StoreByte(unsigned int physAddr,unsigned char data)
{
	TestVMMemory *realThis=(TestVMMemory *)this;
	if(physAddr<=0x000F0000)
	{
		realThis->RAM[physAddr]=data;
	}
}

unsigned int Memory::FetchByteDMA(unsigned int physAddr) const
{
	return FetchByte(physAddr);
}

void Memory::StoreByteDMA(unsigned int physAddr,unsigned char data)
{
	StoreByte(physAddr,data);
}

void Memory::StoreWord(unsigned int physAddr,unsigned int data)
{
	StoreByte(physAddr  ,data);
	StoreByte(physAddr+1,data>>8);
}

void Memory::StoreDword(unsigned int physAddr,unsigned int data)
{
	StoreWord(physAddr,data);
	StoreWord(physAddr+2,data>>16);
}


int main(int ac,char *av[])
{
	TesterVM vm;
	std::unique_ptr <TestVMMemory> memPtr(new TestVMMemory);
	std::unique_ptr <i486DXHighFidelity> cpuPtr(new i486DXHighFidelity(&vm));
	std::unique_ptr <i486Debugger> debuggerPtr(new i486Debugger(&vm));
	std::unique_ptr <InOut> io(new InOut);

	auto &cpu=*cpuPtr;

	io->AddDevice(&vm,IO_POST);
	io->AddDevice(&vm,IO_LPT0);
	io->AddDevice(&vm,IO_LPT1);
	io->AddDevice(&vm,IO_LPT2);


	printf("This VM is for runnint TEST386.\n");
	printf("test386.bin was built from the source https://github.com/barotto/test386.asm.git\n");

	if(ac<2)
	{
		printf("Specify ROM file.\n");
		return 0;
	}

	auto rom=cpputil::ReadBinaryFile(av[1]);
	if(65536!=rom.size())
	{
		printf("ROM size error.\n");
		return 0;
	}
	for(int i=0; i<65536; ++i)
	{
		memPtr->ROM[i]=rom[i];
	}

	int ctr=0,noMove=0;
	bool triggered=false;
	uint32_t EIP_Error=0xD58F;
	uint16_t prevCS=0;
	uint32_t prevEIP=0;
	while(true!=cpu.state.halt)
	{
		if(true==cpu.state.halt || true==vm.CheckAbort() || cpu.GetEIP()==EIP_Error || /*1369681<ctr+100 ||*/ true==triggered)
		{
			std::cout << std::endl;

			if(true==cpu.state.halt)
			{
				std::cout << "Halt" << std::endl;
			}
			else if(true==vm.CheckAbort())
			{
				std::cout << "Abort" << std::endl;
			}
			else if(cpu.GetEIP()==EIP_Error)
			{
				std::cout << "Error!" << std::endl;
			}

			std::cout << ctr;
			std::cout << " POST CODE: 0x" << cpputil::Ubtox(vm.POSTCODE) << std::endl;

			for(auto str : cpu.GetStateText())
			{
				std::cout << str << std::endl;
			}

			for(int i=0; i<0x10; ++i)
			{
				std::cout << cpputil::Ubtox(cpu.DebugFetchByte(cpu.GetStackAddressingSize(),cpu.state.SS(),cpu.state.ESP()+i,*memPtr));
				std::cout << " ";
			}
			std::cout << std::endl;

			i486DXCommon::InstructionAndOperand instOp;
			MemoryAccess::ConstMemoryWindow emptyMemWin;

			cpu.DebugFetchInstruction(emptyMemWin,instOp,cpu.state.CS(),cpu.GetEIP(),*memPtr);
			auto &inst=instOp.inst;
			auto &op1=instOp.op1;
			auto &op2=instOp.op2;
			auto disasm=cpu.Disassemble(inst,op1,op2,cpu.state.CS(),cpu.GetEIP(),*memPtr,debuggerPtr->GetSymTable(),debuggerPtr->GetIOTable());
			std::cout << disasm << std::endl;
		}

		if(true==vm.CheckAbort())
		{
			std::cout << vm.vmAbortReason << std::endl;
		}

		if(true==cpu.state.halt || true==vm.CheckAbort() || cpu.GetEIP()==EIP_Error)
		{
			break;
		}

		// if(99999==ctr%100000)
		// {
		// 	std::cout << ctr << " " << cpputil::Ustox(cpu.state.CS().value) << ":" << cpputil::Uitox(cpu.GetEIP()) << std::endl;
		// }

		prevEIP=cpu.GetEIP();
		prevCS=cpu.state.CS().value;

		auto EIP=cpu.GetEIP();
		auto clocksPassed=cpu.RunOneInstruction(*memPtr,*io);
		if(EIP==cpu.GetEIP())
		{
			++noMove;
		}
		else
		{
			noMove=0;
		}
		++ctr;
		if(true==cpu.state.exception)
		{
			cpu.Abort("Unhandled Exception.");
		}
	}

	std::cout << std::endl;

	std::cout << "POST CODE: 0x" << cpputil::Ubtox(vm.POSTCODE) << std::endl;
	std::cout << "Prev CS:EIP: " << cpputil::Ustox(prevCS) << ":" << cpputil::Uitox(prevEIP) << std::endl;

	if(true==cpu.state.halt && 0xFF==vm.POSTCODE)
	{
		std::cout << "Test Successful!" << std::endl;
		return 0;
	}

	std::cout << "Test Failed!" << std::endl;
	return 1;
}
