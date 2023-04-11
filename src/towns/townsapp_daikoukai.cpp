// Daikoukaijidai for FM TOWNS
// Keyboard Shortcuts
#include "towns.h"


// Y for Yes (when dialog is open)
// N for No (when dialog is open)
// Left Right for course change
// Space or Full-Key-side Enter for course set
// F1 to F8 Click on cruise-menu button (when on the ocean).
// ESC or Break  Right-Click (Can navigate without mouse click!)
// 0 to 9  (when keypad is open)  number
// Numpad +  +100
// Numpad -  +10
// Numpad *  +1000
// Numpad /  +1
// Up        +1
// Down      -1
// Numpad .  AC
// Numpad Enter  Enter on the keypad
// Q/A +-1000
// W/S +-100
// E/D +-10
// R/F +-1


static void PushBack_MouseClick(TownsEventLog &eventLog,int x,int y)
{
	TownsEventLog::Event e;
	e.eventType=TownsEventLog::EVT_LBUTTONDOWN;
	e.t=std::chrono::milliseconds(75);
	e.mos.Set(x,y);
	e.mosTolerance=2;
	eventLog.AddEvent(e);

	e.eventType=TownsEventLog::EVT_LBUTTONUP;
	e.t=std::chrono::milliseconds(75);
	e.mos.Set(x,y);
	e.mosTolerance=2;
	eventLog.AddEvent(e);
}

void FMTownsCommon::Daikoukai_ApplyPatchesCacheAddr(void)
{
	auto &cpu=CPU();
	if(true!=cpu.IsInRealMode())
	{
		if(
			0xC7==cpu.DebugFetchByte(32,cpu.state.CS(),0x488E2,mem) &&
			0x47==cpu.DebugFetchByte(32,cpu.state.CS(),0x488E3,mem) &&
			0x07==cpu.DebugFetchByte(32,cpu.state.CS(),0x488E4,mem) &&
			0x00==cpu.DebugFetchByte(32,cpu.state.CS(),0x488E5,mem) &&
			0x00==cpu.DebugFetchByte(32,cpu.state.CS(),0x488E6,mem) &&
			0x00==cpu.DebugFetchByte(32,cpu.state.CS(),0x488E7,mem) &&
			0x00==cpu.DebugFetchByte(32,cpu.state.CS(),0x488E8,mem))
		{
			cpu.DebugStoreByte(mem,32,cpu.state.CS(),0x488E2,0x8B);
			cpu.DebugStoreByte(mem,32,cpu.state.CS(),0x488E3,0x7F);
			cpu.DebugStoreByte(mem,32,cpu.state.CS(),0x488E4,0x07);
			cpu.DebugStoreByte(mem,32,cpu.state.CS(),0x488E5,0xC6);
			cpu.DebugStoreByte(mem,32,cpu.state.CS(),0x488E6,0x07);
			cpu.DebugStoreByte(mem,32,cpu.state.CS(),0x488E7,0xFF);
			cpu.DebugStoreByte(mem,32,cpu.state.CS(),0x488E8,0x90);
			std::cout << "Applied Daikoukaijidai Pointer-Destruction Prevention Patch." << std::endl;
		}

		if(
			0x80==cpu.DebugFetchByte(32,cpu.state.CS(),0x29777,mem) &&
			0x3D==cpu.DebugFetchByte(32,cpu.state.CS(),0x29778,mem) &&
			0xAD==cpu.DebugFetchByte(32,cpu.state.CS(),0x29779,mem) &&
			0x25==cpu.DebugFetchByte(32,cpu.state.CS(),0x2977A,mem) &&
			0x00==cpu.DebugFetchByte(32,cpu.state.CS(),0x2977B,mem) &&
			0x00==cpu.DebugFetchByte(32,cpu.state.CS(),0x2977C,mem) &&
			0x00==cpu.DebugFetchByte(32,cpu.state.CS(),0x2977D,mem) &&
			0x75==cpu.DebugFetchByte(32,cpu.state.CS(),0x2977E,mem) &&
			0x06==cpu.DebugFetchByte(32,cpu.state.CS(),0x2977F,mem) &&
			0x43==cpu.DebugFetchByte(32,cpu.state.CS(),0x29780,mem) &&
			0xE9==cpu.DebugFetchByte(32,cpu.state.CS(),0x29781,mem) &&
			0xA7==cpu.DebugFetchByte(32,cpu.state.CS(),0x29782,mem) &&
			0xFE==cpu.DebugFetchByte(32,cpu.state.CS(),0x29783,mem) &&
			0xFF==cpu.DebugFetchByte(32,cpu.state.CS(),0x29784,mem) &&
			0xFF==cpu.DebugFetchByte(32,cpu.state.CS(),0x29785,mem))
		{
			int patch[]=
			{
				0x0029630,0x0F,0x83,0x46,0x01,0x00,0x00,-1,
				0x002964B,0x0F,0x85,0x1D,0x01,0x00,0x00,-1,
				0x002971C,0x74,0x50,-1,
				0x0029746,0xEB,0x34,-1,
				0x0029760,0x75,0x0C,-1,
				0x0029755,0xEB,0x16,-1,
				0x002976B,0xEB,0x01,-1,

				0x002976D,0x58,-1,
				0x002976E,0x43,-1,
				0x002976F,0x80,0x3D,0xAD,0x25,0x00,0x00,0x00,-1,
				0x0029776,0x0F,0x84,0xB1,0xFE,0xFF,0xFF,-1,
				0x002977C,0xFE,0x05,0xB1,0x25,0x00,0x00,-1,
				0x0029782,0x6A,0x08,-1,   // PUSH wait_count
				0x0029784,0xE8,0x5B,0xC5,0xFF,0xFF,-1,
				0x0029789,0x58,-1,
				0x002978A,0x80,0x3D,0xAD,0x25,0x00,0x00,0x00,-1,
				0x0029791,0x0F,0x84,0x28,0xFE,0xFF,0xFF,-1,

				0x0029797,0x90,-1,
				0x0029798,0x90,-1,
				0x0029799,0x90,-1,-1,
			};
			int i=0;
			while(patch[i]!=-1)
			{
				auto addr=patch[i++];
				while(patch[i]!=-1)
				{
					cpu.DebugStoreByte(mem,32,cpu.state.CS(),addr++,patch[i++]);
				}
				++i;
			}
			std::cout << "Applied Daikoukaijidai Consistent-Wait Patch." << std::endl;
		}
		if(
			0x8B==cpu.DebugFetchByte(32,cpu.state.CS(),0x25CE4,mem) &&
			0x54==cpu.DebugFetchByte(32,cpu.state.CS(),0x25CE5,mem) &&
			0x24==cpu.DebugFetchByte(32,cpu.state.CS(),0x25CE6,mem) &&
			0x04==cpu.DebugFetchByte(32,cpu.state.CS(),0x25CE7,mem) &&
			0x2B==cpu.DebugFetchByte(32,cpu.state.CS(),0x25CE8,mem) &&
			0xC9==cpu.DebugFetchByte(32,cpu.state.CS(),0x25CE9,mem) &&
			0x3B==cpu.DebugFetchByte(32,cpu.state.CS(),0x25CEA,mem) &&
			0xD1==cpu.DebugFetchByte(32,cpu.state.CS(),0x25CEB,mem) &&
			0x76==cpu.DebugFetchByte(32,cpu.state.CS(),0x25CEC,mem) &&
			0x10==cpu.DebugFetchByte(32,cpu.state.CS(),0x25CED,mem) &&
			0x2B==cpu.DebugFetchByte(32,cpu.state.CS(),0x25CEE,mem) &&
			0xC0==cpu.DebugFetchByte(32,cpu.state.CS(),0x25CEF,mem))
		{
			unsigned char patch[]=
			{
				0x8B,0x44,0x24,0x04,
				0x23,0xC0,
				0x74,0x0C,

				0xB9,0x58,0x1B,0x00,0x00, // MOV ECX,count   2710h->10000  1b58h->7000

				0xE6,0x6C,
				0xE2,0xFC,

				0x48,
				0x75,0xF4,

				0x66,0xB8,0x58,0x1B,

				0xC3,
			};
			uint32_t addr=0x00025CE4;
			for(auto b : patch)
			{
				cpu.DebugStoreByte(mem,32,cpu.state.CS(),addr++,b);
			}
			std::cout << "Applied Daikoukaijidai No Busy-Wait Patch." << std::endl;
		}
		if(
			0xE8==cpu.DebugFetchByte(32,cpu.state.CS(),0x2B244,mem) &&
			0xB7==cpu.DebugFetchByte(32,cpu.state.CS(),0x2B245,mem) &&
			0xAA==cpu.DebugFetchByte(32,cpu.state.CS(),0x2B246,mem) &&
			0xFF==cpu.DebugFetchByte(32,cpu.state.CS(),0x2B247,mem) &&
			0xFF==cpu.DebugFetchByte(32,cpu.state.CS(),0x2B248,mem))
		{
			cpu.DebugStoreByte(mem,32,cpu.state.CS(),0x2B244,0x90);
			cpu.DebugStoreByte(mem,32,cpu.state.CS(),0x2B245,0x90);
			cpu.DebugStoreByte(mem,32,cpu.state.CS(),0x2B246,0x90);
			cpu.DebugStoreByte(mem,32,cpu.state.CS(),0x2B247,0x90);
			cpu.DebugStoreByte(mem,32,cpu.state.CS(),0x2B248,0x90);
			std::cout << "Applied Daikoukaijidai load/unload button repeat no-delay patch." << std::endl;
		}
		if(
			0x68==cpu.DebugFetchByte(32,cpu.state.CS(),0x48D26,mem) &&
			0xDE==cpu.DebugFetchByte(32,cpu.state.CS(),0x48D27,mem) &&
			0x01==cpu.DebugFetchByte(32,cpu.state.CS(),0x48D28,mem) &&
			0x00==cpu.DebugFetchByte(32,cpu.state.CS(),0x48D29,mem) &&
			0x00==cpu.DebugFetchByte(32,cpu.state.CS(),0x48D2A,mem) &&
			0x68==cpu.DebugFetchByte(32,cpu.state.CS(),0x48D2B,mem) &&
			0xB0==cpu.DebugFetchByte(32,cpu.state.CS(),0x48D2C,mem) &&
			0x00==cpu.DebugFetchByte(32,cpu.state.CS(),0x48D2D,mem) &&
			0x00==cpu.DebugFetchByte(32,cpu.state.CS(),0x48D2E,mem) &&
			0x00==cpu.DebugFetchByte(32,cpu.state.CS(),0x48D2F,mem) &&
			0x6A==cpu.DebugFetchByte(32,cpu.state.CS(),0x48D30,mem) &&
			0x22==cpu.DebugFetchByte(32,cpu.state.CS(),0x48D31,mem))
		{
			cpu.DebugStoreByte(mem,32,cpu.state.CS(),0x48D31,0x20);
			std::cout << "Applied Daikoukaijidai fix corrupted image after final fleet battle." << std::endl;
		}
		if(
			0xE8==cpu.DebugFetchByte(32,cpu.state.CS(),0x3183F,mem) &&
			0xE0==cpu.DebugFetchByte(32,cpu.state.CS(),0x31840,mem) &&
			0xFD==cpu.DebugFetchByte(32,cpu.state.CS(),0x31841,mem) &&
			0xFF==cpu.DebugFetchByte(32,cpu.state.CS(),0x31842,mem) &&
			0xFF==cpu.DebugFetchByte(32,cpu.state.CS(),0x31843,mem) &&
			0xE8==cpu.DebugFetchByte(32,cpu.state.CS(),0x31844,mem) &&
			0x73==cpu.DebugFetchByte(32,cpu.state.CS(),0x31845,mem) &&
			0x48==cpu.DebugFetchByte(32,cpu.state.CS(),0x31846,mem) &&
			0xFF==cpu.DebugFetchByte(32,cpu.state.CS(),0x31847,mem) &&
			0xFF==cpu.DebugFetchByte(32,cpu.state.CS(),0x31848,mem))
		{
			cpu.DebugStoreByte(mem,32,cpu.state.CS(),0x3183F,0x90);
			cpu.DebugStoreByte(mem,32,cpu.state.CS(),0x31840,0x90);
			cpu.DebugStoreByte(mem,32,cpu.state.CS(),0x31841,0x90);
			cpu.DebugStoreByte(mem,32,cpu.state.CS(),0x31842,0x90);
			cpu.DebugStoreByte(mem,32,cpu.state.CS(),0x31843,0x90);
			std::cout << "Applied Daikoukaijidai fix heap corruption." << std::endl;
		}

		auto linearBase=cpu.state.DS().baseLinearAddr;
		unsigned int exceptionType,exceptionCode; // To discard
		state.appSpecific_Daikoukai_YNDialogXAddr=cpu.DebugLinearAddressToPhysicalAddress(exceptionType,exceptionCode,linearBase+0x1F098,mem);
		state.appSpecific_Daikoukai_YNDialogYAddr=cpu.DebugLinearAddressToPhysicalAddress(exceptionType,exceptionCode,linearBase+0x1F09C,mem);
		state.appSpecific_Daikoukai_DentakuDialogXAddr=cpu.DebugLinearAddressToPhysicalAddress(exceptionType,exceptionCode,linearBase+0x1F0AC,mem);
		state.appSpecific_Daikoukai_DentakuDialogYAddr=cpu.DebugLinearAddressToPhysicalAddress(exceptionType,exceptionCode,linearBase+0x1F0B0,mem);

		std::cout << "Cached YN Dialog Location Addr:" << 
		             cpputil::Itox(state.appSpecific_Daikoukai_YNDialogXAddr) << " " <<
		             cpputil::Itox(state.appSpecific_Daikoukai_YNDialogYAddr) << std::endl;
		std::cout << "Cached Dentaku Dialog Location Addr:" << 
		             cpputil::Itox(state.appSpecific_Daikoukai_DentakuDialogXAddr) << " " <<
		             cpputil::Itox(state.appSpecific_Daikoukai_DentakuDialogYAddr) << std::endl;

		std::cout << "Enabled Shortcut Keys:" << std::endl;
		std::cout << "Y/N keys for Yes/No (When Yes/No dialog is open.)" << std::endl;
		std::cout << "Numpad Keys for entering number (When Numpad Dialog is open.)" << std::endl;
		std::cout << "Numpad* or Q for add 1000" << std::endl;
		std::cout << "Numpad/ or A for subtract 1000" << std::endl;
		std::cout << "Numpad+ or W for add 100" << std::endl;
		std::cout << "Numpad- or S for subtract 100" << std::endl;
		std::cout << "Up or E for add 10" << std::endl;
		std::cout << "Down or D for subtract 10" << std::endl;
		std::cout << "R for add 1" << std::endl;
		std::cout << "F for subtract 1" << std::endl;
		std::cout << "Enter for decide number (When Numpad Dialog is open.)" << std::endl;
		std::cout << "ESC or Break for Right Click (Cancel or Open Navigation Dialog over the ocean)" << std::endl;
		std::cout << "F1 to F8 for selecting Navigation Dialog button." << std::endl;
		std::cout << "<- -> change heading (when heading dialog is open)" << std::endl;
		std::cout << "Space for deciding heading (when heading dialog is open)" << std::endl;
	}
}
void FMTownsCommon::Daikoukai_RightClick(void)
{
	TownsEventLog::Event e;
	e.eventType=TownsEventLog::EVT_RBUTTONDOWN;
	e.t=std::chrono::milliseconds(75);
	e.mos.Set(320,240);
	e.mosTolerance=640;
	eventLog.AddEvent(e);

	e.eventType=TownsEventLog::EVT_RBUTTONUP;
	e.t=std::chrono::milliseconds(75);
	e.mos.Set(320,240);
	e.mosTolerance=480;
	eventLog.AddEvent(e);
}
void FMTownsCommon::Daikoukai_YKey(void)
{
	if(0!=state.appSpecific_Daikoukai_DentakuDialogXAddr &&
	   0!=state.appSpecific_Daikoukai_DentakuDialogYAddr)
	{
		int x=mem.FetchDword(state.appSpecific_Daikoukai_YNDialogXAddr);
		int y=mem.FetchDword(state.appSpecific_Daikoukai_YNDialogYAddr);
		x&=0xFFFFFFF8;
		PushBack_MouseClick(eventLog,x+20,y+24);
	}
}
void FMTownsCommon::Daikoukai_NKey(void)
{
	if(0!=state.appSpecific_Daikoukai_YNDialogXAddr &&
	   0!=state.appSpecific_Daikoukai_YNDialogYAddr)
	{
		int x=mem.FetchDword(state.appSpecific_Daikoukai_YNDialogXAddr);
		int y=mem.FetchDword(state.appSpecific_Daikoukai_YNDialogYAddr);
		x&=0xFFFFFFF8;
		PushBack_MouseClick(eventLog,x+50,y+24);
	}
}
void FMTownsCommon::Daikoukai_Left(void)
{
	PushBack_MouseClick(eventLog,444,176);
}
void FMTownsCommon::Daikoukai_Right(void)
{
	PushBack_MouseClick(eventLog,474,176);
}
void FMTownsCommon::Daikoukai_CourseSet(void)
{
	PushBack_MouseClick(eventLog,508,176);
}
void FMTownsCommon::Daikoukai_F1(void)
{
	PushBack_MouseClick(eventLog,440,160);
}
void FMTownsCommon::Daikoukai_F2(void)
{
	PushBack_MouseClick(eventLog,496,160);
}
void FMTownsCommon::Daikoukai_F3(void)
{
	PushBack_MouseClick(eventLog,552,160);
}
void FMTownsCommon::Daikoukai_F4(void)
{
	PushBack_MouseClick(eventLog,608,160);
}
void FMTownsCommon::Daikoukai_F5(void)
{
	PushBack_MouseClick(eventLog,440,192);
}
void FMTownsCommon::Daikoukai_F6(void)
{
	PushBack_MouseClick(eventLog,496,192);
}
void FMTownsCommon::Daikoukai_F7(void)
{
	PushBack_MouseClick(eventLog,552,192);
}
void FMTownsCommon::Daikoukai_F8(void)
{
	PushBack_MouseClick(eventLog,608,192);
}
void FMTownsCommon::Daikoukai_DentakuButton(int dx,int dy)
{
	if(0!=state.appSpecific_Daikoukai_DentakuDialogXAddr &&
	   0!=state.appSpecific_Daikoukai_DentakuDialogYAddr)
	{
		int x=mem.FetchDword(state.appSpecific_Daikoukai_DentakuDialogXAddr);
		int y=mem.FetchDword(state.appSpecific_Daikoukai_DentakuDialogYAddr);
		x&=0xFFFFFFF8;
		PushBack_MouseClick(eventLog,x+dx,y+dy);
	}
}
void FMTownsCommon::Daikoukai_Dentaku_Plus1000(void)
{
	Daikoukai_DentakuButton(16,40);
}
void FMTownsCommon::Daikoukai_Dentaku_Plus100(void)
{
	Daikoukai_DentakuButton(40,40);
}
void FMTownsCommon::Daikoukai_Dentaku_Plus10(void)
{
	Daikoukai_DentakuButton(64,40);
}
void FMTownsCommon::Daikoukai_Dentaku_Plus1(void)
{
	Daikoukai_DentakuButton(88,40);
}
void FMTownsCommon::Daikoukai_Dentaku_Minus1000(void)
{
	Daikoukai_DentakuButton(16,72);
}
void FMTownsCommon::Daikoukai_Dentaku_Minus100(void)
{
	Daikoukai_DentakuButton(40,72);
}
void FMTownsCommon::Daikoukai_Dentaku_Minus10(void)
{
	Daikoukai_DentakuButton(64,72);
}
void FMTownsCommon::Daikoukai_Dentaku_Minus1(void)
{
	Daikoukai_DentakuButton(88,72);
}
void FMTownsCommon::Daikoukai_Dentaku_0(void)
{
	Daikoukai_DentakuButton(16,168);
}
void FMTownsCommon::Daikoukai_Dentaku_1(void)
{
	Daikoukai_DentakuButton(16,144);
}
void FMTownsCommon::Daikoukai_Dentaku_2(void)
{
	Daikoukai_DentakuButton(40,144);
}
void FMTownsCommon::Daikoukai_Dentaku_3(void)
{
	Daikoukai_DentakuButton(64,144);
}
void FMTownsCommon::Daikoukai_Dentaku_4(void)
{
	Daikoukai_DentakuButton(16,120);
}
void FMTownsCommon::Daikoukai_Dentaku_5(void)
{
	Daikoukai_DentakuButton(40,120);
}
void FMTownsCommon::Daikoukai_Dentaku_6(void)
{
	Daikoukai_DentakuButton(64,120);
}
void FMTownsCommon::Daikoukai_Dentaku_7(void)
{
	Daikoukai_DentakuButton(16,96);
}
void FMTownsCommon::Daikoukai_Dentaku_8(void)
{
	Daikoukai_DentakuButton(40,96);
}
void FMTownsCommon::Daikoukai_Dentaku_9(void)
{
	Daikoukai_DentakuButton(64,96);
}
void FMTownsCommon::Daikoukai_Dentaku_Max(void)
{
	Daikoukai_DentakuButton(92,96);
}
void FMTownsCommon::Daikoukai_Dentaku_Min(void)
{
	Daikoukai_DentakuButton(92,120);
}
void FMTownsCommon::Daikoukai_Dentaku_AC(void)
{
	Daikoukai_DentakuButton(92,144);
}
void FMTownsCommon::Daikoukai_Dentaku_RET(void)
{
	Daikoukai_DentakuButton(64,168);
}

