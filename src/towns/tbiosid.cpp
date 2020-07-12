/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include <iostream>

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

Towns OS V1.1 L30, TBIOS V31L31_90 (Free Software Collection 4)
0001F3D0             56 33 31 4C 33 31 00 00 39 30 2F 31|    V31L31  90/1
0001F3E0 31 2F 32 31 74 6F 77 6E 73 00 00 00 74 62 69 6F|1/21towns   tbio
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

Towns OS V2.1 L50 TBIOS V31L35_94 (Free Software Collection 11)
00100000 56 33 31 4C 33 35 00 00 39 34 2F 31 32 2F 30 33|V31L35  94/12/03
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
		if("90"==year)
		{
			return TBIOS_V31L31_90;
		}
		else if(year=="91")
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
		auto year=s[1];
		year.resize(2);
		if("94"==year)
		{
			return TBIOS_V31L35_94;
		}
		return TBIOS_V31L35;
	}

	return TBIOS_UNKNOWN;
}

unsigned int FMTowns::FindTBIOSMouseInfoOffset(unsigned int tbiosVersion,unsigned int biosPhysicalBaseAddr) const
{
	switch(tbiosVersion)
	{
	case TBIOS_V31L35:
		{
			// 0110:00014B2B BF804A0100                MOV     EDI,00014A80H

			// 0110:00014C94 8A6F1C                    MOV     CH,[EDI+1CH]
			// 0110:00014C97 8B570C                    MOV     EDX,[EDI+0CH]
			// 0110:00014C9A 0FA4D310                  SHLD    EBX,EDX,10H
			// 0110:00014C9E 886D1D                    MOV     [EBP+1DH],CH
			// 0110:00014CA1 66895518                  MOV     [EBP+18H],DX
			// 0110:00014CA5 66895D14                  MOV     [EBP+14H],BX
			// 0110:00014CA9 C3                        RET

			const int dist=0x14C94-0x14B2B;
			const unsigned char rdposCode[]=
			{
				0x8A,0x6F,0x1C,0x8B,0x57,0x0C,0x0F,0xA4,0xD3,0x10,0x88,0x6D,0x1D,0x66,0x89,0x55,0x18,0x66,0x89,0x5D,0x14,0xC3,
			};
			for(unsigned int ptr=0; ptr<0x30000; ++ptr)
			{
				if(0xBF==mem.FetchByte(biosPhysicalBaseAddr+ptr))
				{
					bool match=true;
					for(int j=0; j<sizeof(rdposCode); ++j)
					{
						if(rdposCode[j]!=mem.FetchByte(biosPhysicalBaseAddr+ptr+dist+j))
						{
							match=false;
							break;
						}
					}
					if(true==match)
					{
						return mem.FetchDword(biosPhysicalBaseAddr+ptr+1);
					}
				}
			}
		}
		break;
	case TBIOS_V31L35_94:
		{
			// 0110:000162C8 BF30620100                MOV     EDI,00016230H

			// 0110:0001643C 8A6F1C                    MOV     CH,[EDI+1CH]
			// 0110:0001643F 8B570C                    MOV     EDX,[EDI+0CH]
			// 0110:00016442 0FA4D310                  SHLD    EBX,EDX,10H
			// 0110:00016446 886D1D                    MOV     [EBP+1DH],CH
			// 0110:00016449 66895518                  MOV     [EBP+18H],DX
			// 0110:0001644D 66895D14                  MOV     [EBP+14H],BX
			// 0110:00016451 C3                        RET

			const int dist=0x1643C-0x162C8;
			const unsigned char rdposCode[]=
			{
				0x8A,0x6F,0x1C,0x8B,0x57,0x0C,0x0F,0xA4,0xD3,0x10,0x88,0x6D,0x1D,0x66,0x89,0x55,0x18,0x66,0x89,0x5D,0x14,0xC3,
			};
			for(unsigned int ptr=0; ptr<0x30000; ++ptr)
			{
				if(0xBF==mem.FetchByte(biosPhysicalBaseAddr+ptr))
				{
					bool match=true;
					for(int j=0; j<sizeof(rdposCode); ++j)
					{
						if(rdposCode[j]!=mem.FetchByte(biosPhysicalBaseAddr+ptr+dist+j))
						{
							match=false;
							break;
						}
					}
					if(true==match)
					{
						return mem.FetchDword(biosPhysicalBaseAddr+ptr+1);
					}
				}
			}
		}
		break;
	}
	return 0;
}

void FMTowns::OnCRTC_HST_Write(void)
{
	std::cout << "Write to CRTC-HST register." << std::endl;
	if(0!=(cpu.state.GetCR(0)&i486DX::CR0_PROTECTION_ENABLE))
	{
		switch(state.appSpecificSetting)
		{
		case TOWNS_APPSPECIFIC_WINGCOMMANDER1:
			{
				i486DX::SegmentRegister DS;
				unsigned int exceptionType,exceptionCode;
				cpu.LoadSegmentRegisterQuiet(DS,0x0014,mem,false);

				state.appSpecific_MousePtrX=cpu.LinearAddressToPhysicalAddress(exceptionType,exceptionCode,DS.baseLinearAddr+0x6EEDC,mem);
				state.appSpecific_MousePtrY=cpu.LinearAddressToPhysicalAddress(exceptionType,exceptionCode,DS.baseLinearAddr+0x6EEDE,mem);

				std::cout << "  MousePointerX Physical Base=" << cpputil::Uitox(state.appSpecific_MousePtrX) << std::endl;
				std::cout << "  MousePointerY Physical Base=" << cpputil::Uitox(state.appSpecific_MousePtrY) << std::endl;
			}
			break;
		case TOWNS_APPSPECIFIC_LEMMINGS:
			{
				i486DX::SegmentRegister DS;
				unsigned int exceptionType,exceptionCode;
				cpu.LoadSegmentRegisterQuiet(DS,0x0014,mem,false);

				state.appSpecific_MousePtrX=cpu.LinearAddressToPhysicalAddress(exceptionType,exceptionCode,DS.baseLinearAddr+0x9122,mem);
				state.appSpecific_MousePtrY=cpu.LinearAddressToPhysicalAddress(exceptionType,exceptionCode,DS.baseLinearAddr+0x9124,mem);

				std::cout << "  MousePointerX Physical Base=" << cpputil::Uitox(state.appSpecific_MousePtrX) << std::endl;
				std::cout << "  MousePointerY Physical Base=" << cpputil::Uitox(state.appSpecific_MousePtrY) << std::endl;
			}
			break;
		case TOWNS_APPSPECIFIC_STRIKECOMMANDER:
			{
				i486DX::SegmentRegister DS;
				unsigned int exceptionType,exceptionCode;
				cpu.LoadSegmentRegisterQuiet(DS,0x0014,mem,false);

				state.appSpecific_MousePtrX=cpu.LinearAddressToPhysicalAddress(exceptionType,exceptionCode,DS.baseLinearAddr+0xC1330,mem);
				state.appSpecific_MousePtrY=cpu.LinearAddressToPhysicalAddress(exceptionType,exceptionCode,DS.baseLinearAddr+0xC1332,mem);

				std::cout << "  MousePointerX Physical Base=" << cpputil::Uitox(state.appSpecific_MousePtrX) << std::endl;
				std::cout << "  MousePointerY Physical Base=" << cpputil::Uitox(state.appSpecific_MousePtrY) << std::endl;
			}
			break;
		}
	}
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
	case TBIOS_V31L31_90:
		return "TBIOS_V31L31_90";
	case TBIOS_V31L31_91:
		return "TBIOS_V31L31_91";
	case TBIOS_V31L31_92:
		return "TBIOS_V31L31_92";
	case TBIOS_V31L35:
		return "TBIOS_V31L35";
	case TBIOS_V31L35_94:
		return "TBIOS_V31L35_94";
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
	int slowDownRange=0;
	var.lastKnownMouseX=hostMouseX;
	var.lastKnownMouseY=hostMouseY;
	if(true==GetMouseCoordinate(mx,my,tbiosid) && true==var.mouseIntegration)
	{
		Vec2i zoom;
		unsigned int VRAMSize;
		if(true==crtc.InSinglePageMode())
		{
			zoom=crtc.GetPageZoom(0);
			VRAMSize=0x80000;
		}
		else
		{
			zoom=crtc.GetPageZoom(state.mouseDisplayPage);
			VRAMSize=0x40000;
		}
		if(0<zoom.x())
		{
			hostMouseX/=zoom.x();
		}
		if(0<zoom.y())
		{
			hostMouseY/=zoom.y();
		}

		// 2020/07/08
		// Lemmings uses double-buffering.  In TBIOS, mouse pointer is influenced by the VRAM offset.
		// However, in Lemmings the mouse pointer is distance from top-left corner of the monitor
		// regardless of the VRAM offset.  Therefore, the transformation needs to be skipped.
		// Also internally-stored X coordinate looks to be half of the actual coordinate.
		bool considerVRAMOffset=true;
		switch(state.appSpecificSetting)
		{
		case TOWNS_APPSPECIFIC_LEMMINGS:
			considerVRAMOffset=false;
			hostMouseX*=zoom.x();
			hostMouseX/=2;
			slowDownRange=4;
			break;
		}

		// 2020/06/13
		// SuperDAISENRYAKU uses mouse with VRAM offset=3BC00H.
		// This offset makes towns mouse cursor appear 32 pixels down from the Windows mouse cursor.
		// VRAM offset needs to be taken into account.
		if(true==considerVRAMOffset)
		{
			auto VRAMoffset=crtc.GetPageVRAMAddressOffset(state.mouseDisplayPage);
			auto bytesPerLine=crtc.GetPageBytesPerLine(state.mouseDisplayPage);
			if(0!=bytesPerLine)
			{
				unsigned int VRAMHeight=VRAMSize/bytesPerLine;
				hostMouseY+=VRAMoffset/bytesPerLine;
				hostMouseY%=VRAMHeight;
			}
			// At this time it only takes vertical displacement into account.
		}

		auto dx=ClampStep(hostMouseX-mx);
		auto dy=ClampStep(hostMouseY-my);
		if(-slowDownRange<=dx && dx<=slowDownRange)
		{
			if(dx<0)
			{
				dx=-1;
			}
			else if(0<dx)
			{
				dx=1;
			}
		}
		if(-slowDownRange<=dy && dy<=slowDownRange)
		{
			if(dy<0)
			{
				dy=-1;
			}
			else if(0<dy)
			{
				dy=1;
			}
		}
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

void FMTowns::DontControlMouse(void)
{
	for(auto &p : gameport.state.ports)
	{
		if(p.device==TownsGamePort::MOUSE)
		{
			p.mouseMotion.Set(0,0);
		}
	}
}

void FMTowns::SetMouseButtonState(bool lButton,bool rButton)
{
	if(TOWNS_APPSPECIFIC_SUPERDAISEN==state.appSpecificSetting)
	{
		if(true==lButton && 0==var.frequencyBackup)
		{
			var.frequencyBackup=state.freq;
			state.freq=2;
		}
		if(true!=lButton && 0!=var.frequencyBackup)
		{
			state.freq=var.frequencyBackup;
			var.frequencyBackup=0;
		}
	}

	if(TownsEventLog::MODE_RECORDING==eventLog.mode && true==var.mouseIntegration)
	{
		int mx=0,my=0;
		GetMouseCoordinate(mx,my,state.tbiosVersion);
		for(auto &p : gameport.state.ports)
		{
			if(p.device==TownsGamePort::MOUSE)
			{
				if(p.button[0]!=lButton)
				{
					if(true==lButton)
					{
						eventLog.LogLeftButtonDown(state.townsTime,mx,my);
						if(true==var.debugBreakOnLButtonDown)
						{
							debugger.ExternalBreak("Left Button Down");
						}
					}
					else
					{
						eventLog.LogLeftButtonUp(state.townsTime,mx,my);
						if(true==var.debugBreakOnLButtonUp)
						{
							debugger.ExternalBreak("Left Button Up");
						}
					}
				}
				if(p.button[1]!=rButton)
				{
					if(true==rButton)
					{
						eventLog.LogRightButtonDown(state.townsTime,mx,my);
					}
					else
					{
						eventLog.LogRightButtonUp(state.townsTime,mx,my);
					}
				}
				p.button[0]=lButton;
				p.button[1]=rButton;
			}
		}
	}
	else
	{
		for(auto &p : gameport.state.ports)
		{
			if(p.device==TownsGamePort::MOUSE)
			{
				if(true==var.debugBreakOnLButtonDown && p.button[0]!=lButton && true==lButton)
				{
					debugger.ExternalBreak("Left Button Down");
				}
				if(true==var.debugBreakOnLButtonUp && p.button[0]!=lButton && true!=lButton)
				{
					debugger.ExternalBreak("Left Button Up");
				}
				p.button[0]=lButton;
				p.button[1]=rButton;
			}
		}
	}
}

void FMTowns::SetGamePadState(int port,bool Abutton,bool Bbutton,bool left,bool right,bool up,bool down,bool run,bool pause)
{
	auto &p=gameport.state.ports[port&1];
	p.button[0]=Abutton;
	p.button[1]=Bbutton;
	p.left =left;
	p.right=right;
	p.up   =up;
	p.down =down;
	p.run  =run;
	p.pause=pause;
}

bool FMTowns::GetMouseCoordinate(int &mx,int &my,unsigned int tbiosid) const
{
	if(true==state.mouseBIOSActive)
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
		case TBIOS_V31L31_90:
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
			mx=(int)mem.FetchWord(state.TBIOS_physicalAddr+state.TBIOS_mouseInfoOffset+0x0C);
			my=(int)mem.FetchWord(state.TBIOS_physicalAddr+state.TBIOS_mouseInfoOffset+0x0E);
			return true;
		case TBIOS_V31L35_94:
			// 0110:000162C8 BF30620100                MOV     EDI,00016230H

			// 0110:0001643C 8A6F1C                    MOV     CH,[EDI+1CH]
			// 0110:0001643F 8B570C                    MOV     EDX,[EDI+0CH]
			// 0110:00016442 0FA4D310                  SHLD    EBX,EDX,10H
			// 0110:00016446 886D1D                    MOV     [EBP+1DH],CH
			// 0110:00016449 66895518                  MOV     [EBP+18H],DX
			// 0110:0001644D 66895D14                  MOV     [EBP+14H],BX
			// 0110:00016451 C3                        RET
			mx=(int)mem.FetchWord(state.TBIOS_physicalAddr+state.TBIOS_mouseInfoOffset+0x0C);
			my=(int)mem.FetchWord(state.TBIOS_physicalAddr+state.TBIOS_mouseInfoOffset+0x0E);
			return true;
		}
	}
	else
	{
		switch(state.appSpecificSetting)
		{
		case TOWNS_APPSPECIFIC_WINGCOMMANDER1:
		case TOWNS_APPSPECIFIC_STRIKECOMMANDER:
			{
				auto debugStop=debugger.stop; // FetchWord may break due to MEMR.
				mx=(int)mem.FetchWord(state.appSpecific_MousePtrX);
				my=(int)mem.FetchWord(state.appSpecific_MousePtrY);
				debugger.stop=debugStop;
			}
			return true;
		case TOWNS_APPSPECIFIC_LEMMINGS:
			{
				auto debugStop=debugger.stop; // FetchWord may break due to MEMR.
				mx=(int)mem.FetchWord(state.appSpecific_MousePtrX);
				my=(int)mem.FetchWord(state.appSpecific_MousePtrY);
				debugger.stop=debugStop;
			}
			return true;
		}
	}
	return false;
}
