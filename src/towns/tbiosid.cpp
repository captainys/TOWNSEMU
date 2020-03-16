/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
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
Towns OS V1.1 L10, V1.1 L20, TBIOS V31L22A
0001F750                         56 33 31 4C 32 32 41 00|        V31L22A
0001F760 38 39 2F 30 33 2F 30 38 74 6F 77 6E 73 00 00 00|89/03/08towns
0001F770 74 62 69 6F 73 00 00 00 80 02 00 00 10 01 00 00|tbios

Towns OS V1.1 L30, TBIOS V31L23A
0001F3D0             56 33 31 4C 32 33 41 00 39 30 2F 30|    V31L23A 90/0
0001F3E0 39 2F 32 31 74 6F 77 6E 73 00 00 00 74 62 69 6F|9/21towns   tbio
0001F3F0 73 00 00 00 80 02 00 00 10 01 00 00 00 00 00 00|s

Towns OS V2.1 L10B, TBIOS V31L31_91
00100000 56 33 31 4C 33 31 00 00 39 31 2F 31 30 2F 30 35|V31L31  91/10/05
00100010 74 6F 77 6E 73 00 00 00 74 62 69 6F 73 00 00 00|towns   tbios

Towns OS V2.1 L20, TBIOS V31L31_92
00100000 56 33 31 4C 33 31 00 00 39 32 2F 31 30 2F 31 36|V31L31  92/10/16
00100010 74 6F 77 6E 73 00 00 00 74 62 69 6F 73 00 00 00|towns   tbios

Towns OS V2.1 L31, TBIOS V31L35
00100000 56 33 31 4C 33 35 00 00 39 33 2F 31 30 2F 31 35|V31L35  93/10/15
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
		auto year=s[1];
		year.resize(2);
		if(year=="91")
		{
			return TBIOS_V31L31_91;
		}
		else if(year=="92")
		{
			return TBIOS_V31L31_92;
		}
	}
	if("V31L35"==s[0] && "towns"==s[2] && "tbios"==s[3])
	{
		return TBIOS_V31L35;
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
	case TBIOS_V31L31_91:
		return "TBIOS_V31L31_91";
	case TBIOS_V31L31_92:
		return "TBIOS_V31L31_92";
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
	case TBIOS_V31L31_91:
		// 0110:00011064 648A2D1C050000            MOV     CH,FS:[0000051CH]
		// 0110:0001106B 648B156C050000            MOV     EDX,FS:[0000056CH]
		// 0110:00011072 0FA4D310                  SHLD    EBX,EDX,10H
		// 0110:00011076 886D1D                    MOV     [EBP+1DH],CH
		// 0110:00011079 66895518                  MOV     [EBP+18H],DX
		// 0110:0001107D 66895D14                  MOV     [EBP+14H],BX
		// 0110:00011081 C3                        RET
		mx=(int)mem.FetchWord(state.TBIOS_physicalAddr+0x56C);
		my=(int)mem.FetchWord(state.TBIOS_physicalAddr+0x56E);
		return true;
	case TBIOS_V31L31_92:
		// 0110:00011D50 268A2D28050000            MOV     CH,ES:[00000528H]
		// 0110:00011D57 268B1510050000            MOV     EDX,ES:[00000510H]
		// 0110:00011D5E 0FA4D310                  SHLD    EBX,EDX,10H
		// 0110:00011D62 886D1D                    MOV     [EBP+1DH],CH
		// 0110:00011D65 66895518                  MOV     [EBP+18H],DX
		// 0110:00011D69 66895D14                  MOV     [EBP+14H],BX
		mx=(int)mem.FetchWord(state.TBIOS_physicalAddr+0x510);
		my=(int)mem.FetchWord(state.TBIOS_physicalAddr+0x512);
		return true;
	case TBIOS_V31L35:
		// 0110:00014B2B BF804A0100                MOV     EDI,00014A80H

		// 0110:00014C94 8A6F1C                    MOV     CH,[EDI+1CH]
		// 0110:00014C97 8B570C                    MOV     EDX,[EDI+0CH]
		// 0110:00014C9A 0FA4D310                  SHLD    EBX,EDX,10H
		// 0110:00014C9E 886D1D                    MOV     [EBP+1DH],CH
		// 0110:00014CA1 66895518                  MOV     [EBP+18H],DX
		// 0110:00014CA5 66895D14                  MOV     [EBP+14H],BX
		// 0110:00014CA9 C3                        RET
		mx=(int)mem.FetchWord(state.TBIOS_physicalAddr+0x14A80+0x0C);
		my=(int)mem.FetchWord(state.TBIOS_physicalAddr+0x14A80+0x0E);
		return true;
	}
	return false;
}
