#include "towns.h"
#include "townsdef.h"
#include "tbiosid.h"


void FMTowns::GetTBIOSIdentifierStrings(std::string s[4],unsigned int biosPhysicalBaseAddr) const
{
	for(int i=0; i<4; ++i)
	{
		s[i]="";
		for(int j=0; j<8; ++j)
		{
			auto c=(unsigned char)mem.FetchByte(biosPhysicalBaseAddr+i*8+j);
			if(0==c)
			{
				break;
			}
			s[i].push_back(c);
		}
	}
}

/* Physical Memory Dump
0001F750                         56 33 31 4C 32 32 41 00|        V31L22A
0001F760 38 39 2F 30 33 2F 30 38 74 6F 77 6E 73 00 00 00|89/03/08towns
0001F770 74 62 69 6F 73 00 00 00 80 02 00 00 10 01 00 00|tbios

Towns OS V1.1 L30, TBIOS V31L23A
0001F3D0             56 33 31 4C 32 33 41 00 39 30 2F 30|    V31L23A 90/0
0001F3E0 39 2F 32 31 74 6F 77 6E 73 00 00 00 74 62 69 6F|9/21towns   tbio
0001F3F0 73 00 00 00 80 02 00 00 10 01 00 00 00 00 00 00|s

Towns OS V2.1 L20, TBIOS V31L31
00100000 56 33 31 4C 33 31 00 00 39 32 2F 31 30 2F 31 36|V31L31  92/10/16
00100010 74 6F 77 6E 73 00 00 00 74 62 69 6F 73 00 00 00|towns   tbios
*/
unsigned int FMTowns::IdentifyTBIOS(unsigned int biosPhysicalBaseAddr) const
{
	std::string s[4];
	GetTBIOSIdentifierStrings(s,biosPhysicalBaseAddr);
	if("V31L22A"==s[0] && "towns"==s[2] && "tbios"==s[3])
	{
		return TBIOS_V31L22A;
	}
	if("V31L23A"==s[0] && "towns"==s[2] && "tbios"==s[3])
	{
		return TBIOS_V31L23A;
	}
	if("V31L31"==s[0] && "towns"==s[2] && "tbios"==s[3])
	{
		return TBIOS_V31L31;
	}
	return TBIOS_UNKNOWN;
}
const char *FMTowns::TBIOSIDENTtoString(unsigned int tbios) const
{
	switch(tbios)
	{
	default:
	case TBIOS_UNKNOWN:
		return "TBIOS_UNKNOWN";
	case TBIOS_V31L22A:
		return "TBIOS_V31L22A";
	case TBIOS_V31L23A:
		return "TBIOS_V31L23A";
	case TBIOS_V31L31:
		return "TBIOS_V31L31";
	}
}

static inline int ClampStep(int d)
{
	if(d<-4)
	{
		d=-4+(d+4)/4;
		if(d<-64)
		{
			return -64;
		}
		return d;
	}
	else if(4<d)
	{
		d=4+(d-4)/4;
		if(64<d)
		{
			return 64;
		}
		return d;
	}
	return d;
}

bool FMTowns::ControlMouse(int hostMouseX,int hostMouseY,unsigned int tbiosid)
{
	int mx,my;
	if(true==GetMouseCoordinate(mx,my,tbiosid))
	{
		auto dx=ClampStep(hostMouseX-mx);
		auto dy=ClampStep(hostMouseY-my);
		for(auto &p : gameport.state.ports)
		{
			if(p.device==TownsGamePort::MOUSE)
			{
				p.mouseMotion.Set(-dx,-dy);
			}
		}
		return true;
	}
	return false;
}

bool FMTowns::GetMouseCoordinate(int &mx,int &my,unsigned int tbiosid) const
{
	switch(tbiosid)
	{
	case TBIOS_V31L22A:
		// 0110:0000FA88 8A6F44                    MOV     CH,[EDI+44H]
		// 0110:0000FA8B 8B5752                    MOV     EDX,[EDI+52H]
		// 0110:0000FA8E 0FA4D310                  SHLD    EBX,EDX,10H
		// 0110:0000FA92 886D1D                    MOV     [EBP+1DH],CH
		// 0110:0000FA95 66895518                  MOV     [EBP+18H],DX
		// 0110:0000FA99 66895D14                  MOV     [EBP+14H],BX
		// 0110:0000FA9D C3                        RET
		mx=(int)mem.FetchWord(state.MOS_work_physicalAddr+0x52);
		my=(int)mem.FetchWord(state.MOS_work_physicalAddr+0x54);
		return true;
	case TBIOS_V31L23A:
		// 0110:000103B8 8A6F44                    MOV     CH,[EDI+44H]
		// 0110:000103BB 8B5756                    MOV     EDX,[EDI+56H]
		// 0110:000103BE 0FA4D310                  SHLD    EBX,EDX,10H
		// 0110:000103C2 886D1D                    MOV     [EBP+1DH],CH
		// 0110:000103C5 66895518                  MOV     [EBP+18H],DX
		// 0110:000103C9 66895D14                  MOV     [EBP+14H],BX
		// 0110:000103CD C3                        RET
		mx=(int)mem.FetchWord(state.MOS_work_physicalAddr+0x56);
		my=(int)mem.FetchWord(state.MOS_work_physicalAddr+0x58);
		return true;
	case TBIOS_V31L31:
		// 0110:00011D50 268A2D28050000            MOV     CH,ES:[00000528H]
		// 0110:00011D57 268B1510050000            MOV     EDX,ES:[00000510H]
		// 0110:00011D5E 0FA4D310                  SHLD    EBX,EDX,10H
		// 0110:00011D62 886D1D                    MOV     [EBP+1DH],CH
		// 0110:00011D65 66895518                  MOV     [EBP+18H],DX
		// 0110:00011D69 66895D14                  MOV     [EBP+14H],BX
		mx=(int)mem.FetchWord(state.TBIOS_physicalAddr+0x510);
		my=(int)mem.FetchWord(state.TBIOS_physicalAddr+0x512);
		return true;
	}
	return false;
}
