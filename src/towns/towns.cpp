#include <iostream>

#include "cpputil.h"
#include "towns.h"
#include "townsdef.h"
#include "townsmap.h"
#include "render.h"
#include "outside_world.h"



void FMTowns::State::PowerOn(void)
{
	Reset();
	townsTime=0;
	nextFastDevicePollingTime=FAST_DEVICE_POLLING_INTERVAL;
	freq=FREQUENCY_DEFAULT;
	resetReason=0;
}
void FMTowns::State::Reset(void)
{
	clockBalance=0;
	// resetReason should survive Reset.
}


////////////////////////////////////////////////////////////


FMTowns::Variable::Variable()
{
	freeRunTimerShift=0;
	Reset();
}

void FMTowns::Variable::Reset(void)
{
	// freeRunTimerShift should survive Reset.
	nextRenderingTime=0;
	disassemblePointer.SEG=0;
	disassemblePointer.OFFSET=0;
}


////////////////////////////////////////////////////////////


FMTowns::FMTowns() : 
	physMem(&cpu),
	crtc(this),
	pic(this),
	dmac(this),
	fdc(this,&pic,&dmac),
	rtc(this),
	sound(this),
	timer(this,&pic)
{
	townsType=TOWNSTYPE_2_MX;

	debugger.ioLabel=FMTownsIOMap();

	abort=false;
	allDevices.push_back(&pic);
	allDevices.push_back(&dmac);
	allDevices.push_back(&ioRAM);
	allDevices.push_back(&physMem);
	allDevices.push_back(&crtc);
	allDevices.push_back(&fdc);
	allDevices.push_back(&cdrom);
	allDevices.push_back(&rtc);
	allDevices.push_back(&sound);
	allDevices.push_back(&timer);

	fastDevices.push_back(&sound);
	fastDevices.push_back(&timer);

	physMem.SetMainRAMSize(4*1024*1024);

	physMem.SetVRAMSize(1024*1024);
	physMem.SetCVRAMSize(32768);
	physMem.SetSpriteRAMSize(512*1024);
	physMem.SetWaveRAMSize(64*1024);

	io.AddDevice(&ioRAM,0x3000,0x3FFF);

	mainRAMAccess.SetPhysicalMemoryPointer(&physMem);
	mainRAMAccess.SetCPUPointer(&cpu);
	mem.AddAccess(&mainRAMAccess,0x00000000,0x000BFFFF);
	mem.AddAccess(&mainRAMAccess,0x000F0000,0x000F7FFF);

	mainRAMorFMRVRAMAccess.SetPhysicalMemoryPointer(&physMem);
	mainRAMorFMRVRAMAccess.SetCPUPointer(&cpu);
	mem.AddAccess(&mainRAMorFMRVRAMAccess,0x000C0000,0x000CFFFF);

	dicROMandDicRAMAccess.SetPhysicalMemoryPointer(&physMem);
	dicROMandDicRAMAccess.SetCPUPointer(&cpu);
	mem.AddAccess(&dicROMandDicRAMAccess,0x000D0000,0x000EFFFF);
	mem.AddAccess(&dicROMandDicRAMAccess,0xC2080000,0xC20FFFFF);
	mem.AddAccess(&dicROMandDicRAMAccess,0xC2140000,0xC2141FFF);

	mainRAMorSysROMAccess.SetPhysicalMemoryPointer(&physMem);
	mainRAMorSysROMAccess.SetCPUPointer(&cpu);
	mem.AddAccess(&mainRAMorSysROMAccess,0x000F8000,0x000FFFFF);

	if(0x00100000<physMem.state.RAM.size())
	{
		mem.AddAccess(&mainRAMAccess,0x00100000,(unsigned int)physMem.state.RAM.size()-1);
	}

	VRAMAccess.SetPhysicalMemoryPointer(&physMem);
	VRAMAccess.SetCPUPointer(&cpu);
	mem.AddAccess(&VRAMAccess,0x80000000,0x8007FFFF);
	mem.AddAccess(&VRAMAccess,0x80100000,0x8017FFFF);
	mem.AddAccess(&VRAMAccess,0x82000000,0x83FFFFFF); // For IIMX High Resolution Access.

	spriteRAMAccess.SetPhysicalMemoryPointer(&physMem);
	spriteRAMAccess.SetCPUPointer(&cpu);
	mem.AddAccess(&spriteRAMAccess,0x81000000,0x8101FFFF);

	osROMAccess.SetPhysicalMemoryPointer(&physMem);
	osROMAccess.SetCPUPointer(&cpu);
	mem.AddAccess(&osROMAccess,0xC2000000,0xC207FFFF);

	fontROMAccess.SetPhysicalMemoryPointer(&physMem);
	fontROMAccess.SetCPUPointer(&cpu);
	mem.AddAccess(&fontROMAccess,0xC2100000,0xC213FFFF);

	waveRAMAccess.SetPhysicalMemoryPointer(&physMem);
	waveRAMAccess.SetCPUPointer(&cpu);
	mem.AddAccess(&waveRAMAccess,0xC2200000,0xC2200FFF);

	sysROMAccess.SetPhysicalMemoryPointer(&physMem);
	sysROMAccess.SetCPUPointer(&cpu);
	mem.AddAccess(&sysROMAccess,0xFFFC0000,0xFFFFFFFF);

	// Free-run counter since FM TOWNS 2UG [2] pp.801
	// Didn't it exist since the first model FM TOWNS 2?
	// I vaguely rember I used something similar when I wrote my first flight simulator 
	// submitted to Japan National High School Students' Programming Contest.
	// FM TOWNS 2UG didn't exist then.
	// I'm positive that I was using the second-generation FM TOWNS then.
	// I'll check if I can find the source code from my old backups.

	// Do range I/O mapping first, then do single I/O mapping.
	// Range I/O mapping may wipe single I/O mapping.
	io.AddDevice(this,TOWNSIO_FREERUN_TIMER_LOW/*0x26*/,TOWNSIO_MACHINE_ID_HIGH/*0x31*/);
	io.AddDevice(&crtc,TOWNSIO_MX_HIRES/*0x470*/,TOWNSIO_MX_IMGOUT_ADDR_D3/*0x477*/);
	io.AddDevice(&keyboard,TOWNSIO_KEYBOARD_DATA/*0x600*/,TOWNSIO_KEYBOARD_IRQ/*0x604*/);
	io.AddDevice(&fdc,TOWNSIO_FDC_STATUS_COMMAND/*0x200*/,TOWNSIO_FDC_DRIVE_SWITCH/*0x20E*/);


	io.AddDevice(&crtc,TOWNSIO_CRTC_ADDRESS);//             0x440,
	io.AddDevice(&crtc,TOWNSIO_CRTC_DATA_LOW);//            0x442,
	io.AddDevice(&crtc,TOWNSIO_CRTC_DATA_HIGH);//           0x443,


	io.AddDevice(this,TOWNSIO_FMR_RESOLUTION); // 0x400


	io.AddDevice(&pic,TOWNSIO_PIC_PRIMARY_ICW1);//          0x00
	io.AddDevice(&pic,TOWNSIO_PIC_PRIMARY_ICW2_3_4_OCW);//  0x02
	io.AddDevice(&pic,TOWNSIO_PIC_SECONDARY_ICW1);//        0x10
	io.AddDevice(&pic,TOWNSIO_PIC_SECONDARY_ICW2_3_4_OCW);//0x12


	io.AddDevice(&dmac,TOWNSIO_DMAC_INITIALIZE);//          0xA0,
	io.AddDevice(&dmac,TOWNSIO_DMAC_CHANNEL);//             0xA1,
	io.AddDevice(&dmac,TOWNSIO_DMAC_COUNT_LOW);//           0xA2,
	io.AddDevice(&dmac,TOWNSIO_DMAC_COUNT_HIGH);//          0xA3,
	io.AddDevice(&dmac,TOWNSIO_DMAC_ADDRESS_LOWEST);//      0xA4,
	io.AddDevice(&dmac,TOWNSIO_DMAC_ADDRESS_MIDLOW);//      0xA5,
	io.AddDevice(&dmac,TOWNSIO_DMAC_ADDRESS_MIDHIGH);//     0xA6,
	io.AddDevice(&dmac,TOWNSIO_DMAC_ADDRESS_HIGHEST);//     0xA7,
	io.AddDevice(&dmac,TOWNSIO_DMAC_DEVICE_CONTROL_LOW);//  0xA8,
	io.AddDevice(&dmac,TOWNSIO_DMAC_DEVICE_CONTROL_HIGH);// 0xA9,
	io.AddDevice(&dmac,TOWNSIO_DMAC_MODE_CONTROL);//        0xAA,
	io.AddDevice(&dmac,TOWNSIO_DMAC_STATUS);//              0xAB,
	io.AddDevice(&dmac,TOWNSIO_DMAC_TEMPORARY_REG_LOW);//   0xAC,
	io.AddDevice(&dmac,TOWNSIO_DMAC_TEMPORARY_REG_HIGH);//  0xAD,
	io.AddDevice(&dmac,TOWNSIO_DMAC_REQUEST);//             0xAE,
	io.AddDevice(&dmac,TOWNSIO_DMAC_MASK);//                0xAF,


	io.AddDevice(this,TOWNSIO_RESET_REASON);
	io.AddDevice(&physMem,TOWNSIO_FMR_VRAM_OR_MAINRAM);
	io.AddDevice(&physMem,TOWNSIO_SYSROM_DICROM);
	io.AddDevice(&physMem,TOWNSIO_MEMSIZE);
	io.AddDevice(&physMem,TOWNSIO_FMR_VRAMMASK);
	io.AddDevice(&physMem,TOWNSIO_FMR_VRAMDISPLAYMODE);
	io.AddDevice(&physMem,TOWNSIO_FMR_VRAMPAGESEL);
	io.AddDevice(&physMem,TOWNSIO_TVRAM_WRITE);


	io.AddDevice(&cdrom,TOWNSIO_CDROM_MASTER_CTRL_STATUS);
	io.AddDevice(&cdrom,TOWNSIO_CDROM_COMMAND_STATUS);
	io.AddDevice(&cdrom,TOWNSIO_CDROM_PARAMETER_DATA);
	io.AddDevice(&cdrom,TOWNSIO_CDROM_TRANSFER_CTRL);
	io.AddDevice(&cdrom,TOWNSIO_CDROM_SUBCODE_STATUS);
	io.AddDevice(&cdrom,TOWNSIO_CDROM_SUBCODE_DATA);


	io.AddDevice(&rtc,TOWNSIO_RTC_DATA);//                 0x70,
	io.AddDevice(&rtc,TOWNSIO_RTC_COMMAND);//              0x80,


	io.AddDevice(&sound,TOWNSIO_SOUND_MUTE);//              0x4D5, // [2] pp.18,
	io.AddDevice(&sound,TOWNSIO_SOUND_STATUS_ADDRESS0);//   0x4D8, // [2] pp.18,
	io.AddDevice(&sound,TOWNSIO_SOUND_DATA0);//             0x4DA, // [2] pp.18,
	io.AddDevice(&sound,TOWNSIO_SOUND_ADDRESS1);//          0x4DC, // [2] pp.18,
	io.AddDevice(&sound,TOWNSIO_SOUND_DATA1);//             0x4DE, // [2] pp.18,
	io.AddDevice(&sound,TOWNSIO_SOUND_INT_REASON);//        0x4E9, // [2] pp.19,
	io.AddDevice(&sound,TOWNSIO_SOUND_PCM_INT_MASK);//      0x4EA, // [2] pp.19,
	io.AddDevice(&sound,TOWNSIO_SOUND_PCM_INT);//           0x4EB, // [2] pp.19,
	io.AddDevice(&sound,TOWNSIO_SOUND_PCM_ENV);//           0x4F0, // [2] pp.19,
	io.AddDevice(&sound,TOWNSIO_SOUND_PCM_PAN);//           0x4F1, // [2] pp.19,
	io.AddDevice(&sound,TOWNSIO_SOUND_PCM_FDL);//           0x4F2, // [2] pp.19,
	io.AddDevice(&sound,TOWNSIO_SOUND_PCM_FDH);//           0x4F3, // [2] pp.19,
	io.AddDevice(&sound,TOWNSIO_SOUND_PCM_LSL);//           0x4F4, // [2] pp.19,
	io.AddDevice(&sound,TOWNSIO_SOUND_PCM_LSH);//           0x4F5, // [2] pp.19,
	io.AddDevice(&sound,TOWNSIO_SOUND_PCM_ST);//            0x4F6, // [2] pp.19,
	io.AddDevice(&sound,TOWNSIO_SOUND_PCM_CTRL);//          0x4F7, // [2] pp.19,
	io.AddDevice(&sound,TOWNSIO_SOUND_PCM_CH_ON_OFF);//     0x4F8, // [2] pp.19,


	io.AddDevice(&timer,TOWNSIO_TIMER0_COUNT_LOW);//         0x40,
	io.AddDevice(&timer,TOWNSIO_TIMER0_COUNT_HIGH);//        0x41,
	io.AddDevice(&timer,TOWNSIO_TIMER1_COUNT_LOW);//         0x42,
	io.AddDevice(&timer,TOWNSIO_TIMER1_COUNT_HIGH);//        0x43,
	io.AddDevice(&timer,TOWNSIO_TIMER2_COUNT_LOW);//         0x44,
	io.AddDevice(&timer,TOWNSIO_TIMER2_COUNT_HIGH);//        0x45,
	io.AddDevice(&timer,TOWNSIO_TIMER_0_1_2_CTRL);//         0x46,
	io.AddDevice(&timer,TOWNSIO_TIMER3_COUNT_LOW);//         0x50,
	io.AddDevice(&timer,TOWNSIO_TIMER3_COUNT_HIGH);//        0x51,
	io.AddDevice(&timer,TOWNSIO_TIMER4_COUNT_LOW);//         0x52,
	io.AddDevice(&timer,TOWNSIO_TIMER4_COUNT_HIGH);//        0x53,
	io.AddDevice(&timer,TOWNSIO_TIMER5_COUNT_LOW);//         0x54,
	io.AddDevice(&timer,TOWNSIO_TIMER5_COUNT_HIGH);//        0x55,
	io.AddDevice(&timer,TOWNSIO_TIMER_3_4_5_CTRL);//         0x56,
	io.AddDevice(&timer,TOWNSIO_TIMER_INT_CTRL_INT_REASON);

	PowerOn();
}

bool FMTowns::CheckAbort(void) const
{
	bool ab=false;
	if(true==abort)
	{
		std::cout << "FMTowns:" << abortReason << std::endl;
		ab=true;
	}
	if(true==cpu.abort)
	{
		std::cout << cpu.DeviceName() << ':' << cpu.abortReason << std::endl;
		ab=true;
	}
	if(true==physMem.abort)
	{
		std::cout << physMem.DeviceName() << ':' <<  physMem.abortReason << std::endl;
		ab=true;
	}
	for(auto devPtr : allDevices)
	{
		if(true==devPtr->abort)
		{
			std::cout << devPtr->DeviceName() << ':' <<  devPtr->abortReason << std::endl;
			ab=true;
		}
	}
	return ab;
}

bool FMTowns::LoadROMImages(const char dirName[])
{
	if(true!=physMem.LoadROMImages(dirName))
	{
		abort=true;
		abortReason="Unable to load ROM images.";
		return false;
	}
	return true;
}

void FMTowns::PowerOn(void)
{
	state.PowerOn();
	cpu.PowerOn();
	for(auto devPtr : allDevices)
	{
		devPtr->PowerOn();
	}
}
void FMTowns::Reset(void)
{
	var.Reset();
	state.Reset();
	cpu.Reset();
	for(auto devPtr : allDevices)
	{
		devPtr->Reset();
	}

	var.disassemblePointer.SEG=cpu.state.CS().value;
	var.disassemblePointer.OFFSET=cpu.state.EIP;
}

unsigned int FMTowns::RunOneInstruction(void)
{
	auto clocksPassed=cpu.RunOneInstruction(mem,io);
	state.clockBalance+=clocksPassed*1000;

	// Since last update, clockBalance*1000/freq nano seconds have passed.
	// Eg.  66MHz ->  66 clocks passed means 1 micro second.
	//                clockBalance is 66000.
	//                clockBalance/freq=1000.  1000 nano seconds.
	state.townsTime+=(state.clockBalance/state.freq);
	state.clockBalance%=state.freq;

	var.disassemblePointer.SEG=cpu.state.CS().value;
	var.disassemblePointer.OFFSET=cpu.state.EIP;

	return clocksPassed;
}

void FMTowns::RunScheduledTasks(void)
{
	for(auto devPtr : allDevices)
	{
		if(devPtr->commonState.scheduleTime<=state.townsTime)
		{
			devPtr->RunScheduledTask(state.townsTime);
		}
	}
}

void FMTowns::RunFastDevicePolling(void)
{
	if(state.nextFastDevicePollingTime<state.townsTime)
	{
		for(auto devPtr : fastDevices)
		{
			devPtr->RunScheduledTask(state.townsTime);
		}
		state.nextFastDevicePollingTime=state.townsTime+FAST_DEVICE_POLLING_INTERVAL;
	}
}

bool FMTowns::CheckRenderingTimer(TownsRender &render,Outside_World &world)
{
	if(var.nextRenderingTime<=state.townsTime)
	{
		render.BuildImage(crtc,physMem);
		world.Render(render.GetImage());
		var.nextRenderingTime=state.townsTime+TOWNS_RENDERING_FREQUENCY;
		return true;
	}
	return false;
}

void FMTowns::ForceRender(class TownsRender &render,class Outside_World &world)
{
	render.BuildImage(crtc,physMem);
	world.Render(render.GetImage());
}

////////////////////////////////////////////////////////////


unsigned int FMTowns::FetchByteCS_EIP(int offset) const
{
	return cpu.FetchInstructionByte(offset,mem);
}

i486DX::Instruction FMTowns::FetchInstruction(void) const
{
	return cpu.FetchInstruction(mem);
}
std::vector <std::string> FMTowns::GetStackText(unsigned int numBytes) const
{
	std::vector <std::string> text;
	for(unsigned int offsetHigh=0; offsetHigh<numBytes; offsetHigh+=16)
	{
		auto addressSize=cpu.GetStackAddressingSize();
		std::string line;
		line="SS+"+cpputil::Uitox(offsetHigh)+":";
		for(unsigned int offsetLow=0; offsetLow<16 && offsetHigh+offsetLow<numBytes; ++offsetLow)
		{
			line+=cpputil::Ubtox(cpu.FetchByte(addressSize,cpu.state.SS(),cpu.state.ESP()+offsetHigh+offsetLow,mem));
			line.push_back(' ');
		}
		text.push_back(line);
	}
	return text;
}
void FMTowns::PrintStack(unsigned int numBytes) const
{
	for(auto s : GetStackText(numBytes))
	{
		std::cout << s << std::endl;
	}
}
void FMTowns::PrintDisassembly(void) const
{
	auto inst=FetchInstruction();
	auto disasm=cpu.Disassemble(inst,cpu.state.CS(),cpu.state.EIP,mem);
	std::cout << disasm << std::endl;
}

std::vector <std::string> FMTowns::GetRealModeIntVectorsText(void) const
{
	std::vector <std::string> text;
	for(int i=0; i<256; i+=4)
	{
		std::string str;
		for(int j=0; j<4; ++j)
		{
			if(0<j)
			{
				str+=" | ";
			}
			auto ij=i+j;
			str+=cpputil::Ubtox(ij)+" ";
			unsigned int offset=physMem.state.RAM[ij*4]|((unsigned int)physMem.state.RAM[ij*4+1]<<8);
			unsigned int seg=physMem.state.RAM[ij*4+2]|((unsigned int)physMem.state.RAM[ij*4+3]<<8);
			str+=cpputil::Ustox(seg)+":"+cpputil::Ustox(offset);
		}
		text.push_back(str);
	}
	return text;
}
void FMTowns::DumpRealModeIntVectors(void) const
{
	for(auto s : GetRealModeIntVectorsText())
	{
		std::cout << s << std::endl;
	}
}
std::vector <std::string> FMTowns::GetCallStackText(void) const
{
	return debugger.GetCallStackText(cpu);
}
void FMTowns::PrintCallStack(void) const
{
	for(auto str : GetCallStackText())
	{
		std::cout << str << std::endl;
	}
}
void FMTowns::PrintPIC(void) const
{
	for(auto str : pic.GetStateText())
	{
		std::cout << str << std::endl;
	}
}

void FMTowns::PrintDMAC(void) const
{
	for(auto str : dmac.GetStateText())
	{
		std::cout << str << std::endl;
	}
}

void FMTowns::PrintFDC(void) const
{
	for(auto str : fdc.GetStatusText())
	{
		std::cout << str << std::endl;
	}
}

void FMTowns::PrintTimer(void) const
{
	for(auto str : timer.GetStatusText())
	{
		std::cout << str << std::endl;
	}
}

void FMTowns::PrintSound(void) const
{
	for(auto str : sound.state.ym2612.GetStatusText())
	{
		std::cout << str << std::endl;
	}
}

void FMTowns::PrintStatus(void) const
{
	std::cout << "Towns TIME (Nano-Seconds): " << state.townsTime << std::endl;
	cpu.PrintState();
	PrintStack(32);
	if(""!=debugger.externalBreakReason)
	{
		std::cout << debugger.externalBreakReason << std::endl;
	}
	PrintDisassembly();
}
