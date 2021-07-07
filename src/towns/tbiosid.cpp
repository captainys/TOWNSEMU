/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include <iostream>
#include <algorithm>
#include <cstdint>

#include "towns.h"
#include "townsdef.h"
#include "tbiosid.h"

// For Wing Commander 2 mouse integration.
// Looks like DS is page aligned.
const unsigned int WC2_EVENTQUEUE_LENGTH=100;
const unsigned int WC2_EVENTQUEUE_BASE_ADDR=  0x3CC;
const unsigned int WC2_EVENTQUEUE_OFFSET=     0x3CC-WC2_EVENTQUEUE_BASE_ADDR;
const unsigned int WC2_EVENTQUEUE_LAST_OFFSET=0x6E4-WC2_EVENTQUEUE_BASE_ADDR;
const unsigned int WC2_EVENTQUEUE_READ_PTR=   0x6EC-WC2_EVENTQUEUE_BASE_ADDR;
const unsigned int WC2_EVENTQUEUE_FILLED=     0x6F4-WC2_EVENTQUEUE_BASE_ADDR;

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

Towns OS V2.1 L20A, TBIOS V31L31_93
00100000 56 33 31 4C 33 31 00 00 39 33 2F 30 31 2F 30 37|V31L31  93/01/07
00100010 74 6F 77 6E 73 00 00 00 74 62 69 6F 73 00 00 00|towns   tbios

Towns OS V2.1 L30/L31/L40, TBIOS V31L35
00100000 56 33 31 4C 33 35 00 00 39 33 2F 31 30 2F 31 35|V31L35  93/10/15
00100010 74 6F 77 6E 73 00 00 00 74 62 69 6F 73 00 00 00|towns   tbios

Towns OS V2.1 L50 TBIOS V31L35 (Free Software Collection 11)
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
		else if(year=="93")
		{
			return TBIOS_V31L31_93;
		}
	}
	if("V31L35"==s[0] && "towns"==s[2] && "tbios"==s[3])
	{
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
			// TownsOS V2.1 L30
			// 0110:00014A8B BFE0490100                MOV     EDI,000149E0H			

			// 0110:00014C00 8A6F1C                    MOV     CH,[EDI+1CH]
			// 0110:00014C03 8B570C                    MOV     EDX,[EDI+0CH]
			// 0110:00014C06 0FA4D310                  SHLD    EBX,EDX,10H
			// 0110:00014C0A 886D1D                    MOV     [EBP+1DH],CH
			// 0110:00014C0D 66895518                  MOV     [EBP+18H],DX
			// 0110:00014C11 66895D14                  MOV     [EBP+14H],BX
			// 0110:00014C15 C3                        RET

			// TownsOS V2.1 L31
			// 0110:00014B2B BF804A0100                MOV     EDI,00014A80H

			// 0110:00014C94 8A6F1C                    MOV     CH,[EDI+1CH]
			// 0110:00014C97 8B570C                    MOV     EDX,[EDI+0CH]
			// 0110:00014C9A 0FA4D310                  SHLD    EBX,EDX,10H
			// 0110:00014C9E 886D1D                    MOV     [EBP+1DH],CH
			// 0110:00014CA1 66895518                  MOV     [EBP+18H],DX
			// 0110:00014CA5 66895D14                  MOV     [EBP+14H],BX
			// 0110:00014CA9 C3                        RET

			// TownsOS V2.1 L40
			// 0110:00016288 BFF0610100                MOV     EDI,000161F0H

			// 0110:000163FC 8A6F1C                    MOV     CH,[EDI+1CH]
			// 0110:000163FF 8B570C                    MOV     EDX,[EDI+0CH]
			// 0110:00016402 0FA4D310                  SHLD    EBX,EDX,10H
			// 0110:00016406 886D1D                    MOV     [EBP+1DH],CH
			// 0110:00016409 66895518                  MOV     [EBP+18H],DX
			// 0110:0001640D 66895D14                  MOV     [EBP+14H],BX
			// 0110:00016411 C3                        RET

			// TownsOS V2.1 L50
			// 0110:000162C8 BF30620100                MOV     EDI,00016230H

			// 0110:0001643C 8A6F1C                    MOV     CH,[EDI+1CH]
			// 0110:0001643F 8B570C                    MOV     EDX,[EDI+0CH]
			// 0110:00016442 0FA4D310                  SHLD    EBX,EDX,10H
			// 0110:00016446 886D1D                    MOV     [EBP+1DH],CH
			// 0110:00016449 66895518                  MOV     [EBP+18H],DX
			// 0110:0001644D 66895D14                  MOV     [EBP+14H],BX
			// 0110:00016451 C3                        RET

			const int distCandidates[]=
			{
				0x14C00-0x14A8B, // V2.1 L30
				0x14C94-0x14B2B, // V2.1 L31
				372, // V2.1 L40  0x163FC-0x16288
				     // V2.1 L50  0x1643C-0x162C8
			};
			const unsigned char rdposCode[]=
			{
				0x8A,0x6F,0x1C,0x8B,0x57,0x0C,0x0F,0xA4,0xD3,0x10,0x88,0x6D,0x1D,0x66,0x89,0x55,0x18,0x66,0x89,0x5D,0x14,0xC3,
			};
			for(unsigned int ptr=0; ptr<0x30000; ++ptr)
			{
				if(0xBF==mem.FetchByte(biosPhysicalBaseAddr+ptr))
				{
					for(int dist : distCandidates)
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

			// None of the above?
			for(unsigned int ptr=0; ptr<0x30000; ++ptr)
			{
				bool match=true;
				for(int j=0; j<sizeof(rdposCode); ++j)
				{
					if(rdposCode[j]!=mem.FetchByte(biosPhysicalBaseAddr+ptr+j))
					{
						match=false;
						break;
					}
				}
				if(true==match)
				{
					while(0<ptr)
					{
						--ptr;
						if(0xBF==mem.FetchByte(biosPhysicalBaseAddr+ptr))
						{
							return mem.FetchDword(biosPhysicalBaseAddr+ptr+1);
						}
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
				cpu.DebugLoadSegmentRegister(DS,0x0014,mem,false);

				state.appSpecific_MousePtrX=cpu.LinearAddressToPhysicalAddress(exceptionType,exceptionCode,DS.baseLinearAddr+0x6EEDC,mem);
				state.appSpecific_MousePtrY=cpu.LinearAddressToPhysicalAddress(exceptionType,exceptionCode,DS.baseLinearAddr+0x6EEDE,mem);

				state.appSpecific_StickPosXPtr=cpu.LinearAddressToPhysicalAddress(exceptionType,exceptionCode,DS.baseLinearAddr+0x0006EF30,mem);
				state.appSpecific_StickPosYPtr=cpu.LinearAddressToPhysicalAddress(exceptionType,exceptionCode,DS.baseLinearAddr+0x0006EF34,mem);

				state.appSpecific_WC_setSpeedPtr=cpu.LinearAddressToPhysicalAddress(exceptionType,exceptionCode,DS.baseLinearAddr+0x00066EF1,mem);
				state.appSpecific_WC_maxSpeedPtr=cpu.LinearAddressToPhysicalAddress(exceptionType,exceptionCode,DS.baseLinearAddr+0x000651BC,mem);

				std::cout << "  MousePointerX Physical Base=" << cpputil::Uitox(state.appSpecific_MousePtrX) << std::endl;
				std::cout << "  MousePointerY Physical Base=" << cpputil::Uitox(state.appSpecific_MousePtrY) << std::endl;
				std::cout << "  StickX Physical Base       =" << cpputil::Uitox(state.appSpecific_StickPosXPtr) << std::endl;
				std::cout << "  StickY Physical Base       =" << cpputil::Uitox(state.appSpecific_StickPosYPtr) << std::endl;
				std::cout << "  Set-Speed Physical Addr    =" << cpputil::Uitox(state.appSpecific_WC_setSpeedPtr) << std::endl;
				std::cout << "  Max-Speed Physical Addr    =" << cpputil::Uitox(state.appSpecific_WC_maxSpeedPtr) << std::endl;
			}
			break;
		case TOWNS_APPSPECIFIC_WINGCOMMANDER2:
			{
				i486DX::SegmentRegister DS;
				unsigned int exceptionType,exceptionCode;
				cpu.DebugLoadSegmentRegister(DS,0x0014,mem,false);

				state.appSpecific_MousePtrX=cpu.LinearAddressToPhysicalAddress(exceptionType,exceptionCode,DS.baseLinearAddr+0x4B060,mem);
				state.appSpecific_MousePtrY=cpu.LinearAddressToPhysicalAddress(exceptionType,exceptionCode,DS.baseLinearAddr+0x4B064,mem);
				state.appSpecific_WC2_EventQueueBaseAddr=cpu.LinearAddressToPhysicalAddress(exceptionType,exceptionCode,DS.baseLinearAddr+WC2_EVENTQUEUE_BASE_ADDR,mem);
				state.appSpecific_HoldMouseIntegration=true;

				state.appSpecific_WC_setSpeedPtr=cpu.LinearAddressToPhysicalAddress(exceptionType,exceptionCode,DS.baseLinearAddr+0x00002C46,mem);
				state.appSpecific_WC_maxSpeedPtr=cpu.LinearAddressToPhysicalAddress(exceptionType,exceptionCode,DS.baseLinearAddr+0x00003C53,mem);

				std::cout << "  MousePointerX Physical Base=" << cpputil::Uitox(state.appSpecific_MousePtrX) << std::endl;
				std::cout << "  MousePointerY Physical Base=" << cpputil::Uitox(state.appSpecific_MousePtrY) << std::endl;
				std::cout << "  Set-Speed Physical Addr    =" << cpputil::Uitox(state.appSpecific_WC_setSpeedPtr) << std::endl;
				std::cout << "  Max-Speed Physical Addr    =" << cpputil::Uitox(state.appSpecific_WC_maxSpeedPtr) << std::endl;
				std::cout << "  Event Queue Base Physical Base=" << cpputil::Uitox(state.appSpecific_WC2_EventQueueBaseAddr) << std::endl;

				i486DX::SegmentRegister CS;
				cpu.DebugLoadSegmentRegister(CS,0x0014,mem,false);
				if(0x74==cpu.DebugFetchByte(32,CS,0x8F634,mem) &&
				   0x72==cpu.DebugFetchByte(32,CS,0x8F635,mem) &&
				   0x22==cpu.DebugFetchByte(32,CS,0x8F636,mem) &&
				   0xC0==cpu.DebugFetchByte(32,CS,0x8F637,mem) &&
				   0x75==cpu.DebugFetchByte(32,CS,0x8F638,mem) &&
				   0x6E==cpu.DebugFetchByte(32,CS,0x8F639,mem))
				{
					// Wing Commander 2 doesn't allow moving X and Y of turret simultaneously.
					// Which is extremely unreasonable and destroying the game balance.
					// This MOD allows turret aim X and Y move simultaneously.
					cpu.DebugStoreByte(mem,32,CS,0x8F639,0);
					std::cout << "  Allow turret X and Y move simultaneously." << std::endl;

					// From:
					// 000C:0008F634 7472                      JE      0008F6A8         ; Jump if no Y motion
					// 000C:0008F636 22C0                      AND     AL,AL
					// 000C:0008F638 756E                      JNE     0008F6A8         ; AL=X motion flag.  Jump if non zero X motion.
					// 000C:0008F63A 8B3514AB0400              MOV     ESI,[0004AB14H]  ; DS:[0004AB14H] is turret pitch.

					// To:
					// 000C:0008F634 7472                      JE      0008F6A8         ; Jump if no Y motion
					// 000C:0008F636 22C0                      AND     AL,AL
					// 000C:0008F638 7500                      JNE     0008F63A         ; AL=X motion flag.  Jump if non zero X motion.
					// 000C:0008F63A 8B3514AB0400              MOV     ESI,[0004AB14H]  ; DS:[0004AB14H] is turret pitch.
				}
			}
			break;
		case TOWNS_APPSPECIFIC_LEMMINGS:
			{
				i486DX::SegmentRegister DS;
				unsigned int exceptionType,exceptionCode;
				cpu.DebugLoadSegmentRegister(DS,0x0014,mem,false);

				state.appSpecific_MousePtrX=cpu.LinearAddressToPhysicalAddress(exceptionType,exceptionCode,DS.baseLinearAddr+0x9122,mem);
				state.appSpecific_MousePtrY=cpu.LinearAddressToPhysicalAddress(exceptionType,exceptionCode,DS.baseLinearAddr+0x9124,mem);

				std::cout << "  MousePointerX Physical Base=" << cpputil::Uitox(state.appSpecific_MousePtrX) << std::endl;
				std::cout << "  MousePointerY Physical Base=" << cpputil::Uitox(state.appSpecific_MousePtrY) << std::endl;
			}
			break;
		case TOWNS_APPSPECIFIC_LEMMINGS2:
			// CS=009C(LIN:00423000)  DS=005C(LIN:00411000)  ES=005C(LIN:00411000)
			// FS=005C(LIN:00411000)  GS=0014(LIN:00400000)  SS=005C(LIN:00411000)
			// 009C:00003C41 660305002E0000            ADD     AX,[00002E00H] ; (Prob)DS:[2E00H]=X
			// 009C:00003C98 66A3FE2D0000              MOV     [00002DFEH],AX ; (Prob)DS:[2DFEH]=Y
			{
				i486DX::SegmentRegister DS;
				unsigned int exceptionType,exceptionCode;
				cpu.DebugLoadSegmentRegister(DS,0x005C,mem,false);

				state.appSpecific_MousePtrX=cpu.LinearAddressToPhysicalAddress(exceptionType,exceptionCode,DS.baseLinearAddr+0x2DFE,mem);
				state.appSpecific_MousePtrY=cpu.LinearAddressToPhysicalAddress(exceptionType,exceptionCode,DS.baseLinearAddr+0x2E00,mem);

				std::cout << "  MousePointerX Physical Base=" << cpputil::Uitox(state.appSpecific_MousePtrX) << std::endl;
				std::cout << "  MousePointerY Physical Base=" << cpputil::Uitox(state.appSpecific_MousePtrY) << std::endl;
			}
			break;
		case TOWNS_APPSPECIFIC_OPERATIONWOLF:
			{
				i486DX::SegmentRegister DS;
				unsigned int exceptionType,exceptionCode;
				cpu.DebugLoadSegmentRegister(DS,0x0014,mem,false);

				state.appSpecific_MousePtrX=cpu.LinearAddressToPhysicalAddress(exceptionType,exceptionCode,DS.baseLinearAddr+0x5E420,mem);
				state.appSpecific_MousePtrY=cpu.LinearAddressToPhysicalAddress(exceptionType,exceptionCode,DS.baseLinearAddr+0x5E422,mem);

				std::cout << "  MousePointerX Physical Base=" << cpputil::Uitox(state.appSpecific_MousePtrX) << std::endl;
				std::cout << "  MousePointerY Physical Base=" << cpputil::Uitox(state.appSpecific_MousePtrY) << std::endl;
			}
			break;
		case TOWNS_APPSPECIFIC_STRIKECOMMANDER:
			{
				i486DX::SegmentRegister DS;
				unsigned int exceptionType,exceptionCode;
				cpu.DebugLoadSegmentRegister(DS,0x0118,mem,false);

				state.appSpecific_MousePtrX=cpu.LinearAddressToPhysicalAddress(exceptionType,exceptionCode,DS.baseLinearAddr+0x15E60+0x0C,mem);
				state.appSpecific_MousePtrY=cpu.LinearAddressToPhysicalAddress(exceptionType,exceptionCode,DS.baseLinearAddr+0x15E60+0x0E,mem);

				std::cout << "  MousePointerX Physical Base=" << cpputil::Uitox(state.appSpecific_MousePtrX) << std::endl;
				std::cout << "  MousePointerY Physical Base=" << cpputil::Uitox(state.appSpecific_MousePtrY) << std::endl;
			}
			break;
		case TOWNS_APPSPECIFIC_ULTIMAUNDERWORLD:
			{
				i486DX::SegmentRegister DS;
				unsigned int exceptionType,exceptionCode;
				cpu.DebugLoadSegmentRegister(DS,0x014,mem,false);

				state.appSpecific_MousePtrX=cpu.LinearAddressToPhysicalAddress(exceptionType,exceptionCode,DS.baseLinearAddr+0x55CDC,mem);
				state.appSpecific_MousePtrY=cpu.LinearAddressToPhysicalAddress(exceptionType,exceptionCode,DS.baseLinearAddr+0x55CDE,mem);

				std::cout << "  MousePointerX Physical Base=" << cpputil::Uitox(state.appSpecific_MousePtrX) << std::endl;
				std::cout << "  MousePointerY Physical Base=" << cpputil::Uitox(state.appSpecific_MousePtrY) << std::endl;
			}
			break;
		}
	}
}

void FMTowns::OnCDDAStart(void)
{
	switch(state.appSpecificSetting)
	{
	case TOWNS_APPSPECIFIC_WINGCOMMANDER2:
		// Wing Commander 2 requires mouse deltas need to be zero while the program detects a mouse.
		// CDDA will start after the mouse-presence check is done.
		// It is a good timing to enable mouse integration.
		state.appSpecific_HoldMouseIntegration=false;
		break;
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
	case TBIOS_V31L31_93:
		return "TBIOS_V31L31_93";
	case TBIOS_V31L35:
		return "TBIOS_V31L35";
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

static inline int ScaleStep(int d,int scale256)
{
	return d*scale256/256;

	/*! Strictly spcaking, the following code should be more correct.
	    But, the above code makes mouse cursor less shaky.
	if(0==d)
	{
		return d;
	}
	else
	{
		auto dd=d*scale256/256;
		if(0==dd)
		{
			if(0<d)
			{
				return 1;
			}
			else
			{
				return -1;
			}
		}
		return dd;
	} */
}

bool FMTowns::ControlMouse(int hostMouseX,int hostMouseY,unsigned int tbiosid)
{
	int diffX,diffY;
	return ControlMouse(diffX,diffY,hostMouseX,hostMouseY,tbiosid);
}
bool FMTowns::ControlMouse(int &diffX,int &diffY,int hostMouseX,int hostMouseY,unsigned int tbiosid)
{
	// Wing Commander 2 requires mouse deltas to be zero until the mouse-presence check is done.
	if(true!=state.mouseBIOSActive &&
	   TOWNS_APPSPECIFIC_WINGCOMMANDER2==state.appSpecificSetting &&
	   true==state.appSpecific_HoldMouseIntegration)
	{
		for(auto &p : gameport.state.ports)
		{
			if(p.device==TownsGamePort::MOUSE)
			{
				p.mouseMotion.Set(0,0);
			}
		}
		return true;
	}

	int mx,my;
	int slowDownRange=0;
	var.lastKnownMouseX=hostMouseX;
	var.lastKnownMouseY=hostMouseY;
	if(true==GetMouseCoordinate(mx,my,tbiosid) && true==var.mouseIntegration)
	{
		Vec2i origin;


		Vec2i zoom2x;
		unsigned int VRAMSize;
		if(true!=state.mouseBIOSActive &&
		   TOWNS_APPSPECIFIC_WINGCOMMANDER2==state.appSpecificSetting)
		{
			origin=crtc.GetPageOriginOnMonitor(0);
			zoom2x.Set(4,4);
			VRAMSize=0x80000;
		}
		else if(true==crtc.InSinglePageMode())
		{
			origin=crtc.GetPageOriginOnMonitor(0);
			zoom2x=crtc.GetPageZoom2X(0);
			VRAMSize=crtc.GetEffectiveVRAMSize();
		}
		else
		{
			origin=crtc.GetPageOriginOnMonitor(state.mouseDisplayPage);
			zoom2x=crtc.GetPageZoom2X(state.mouseDisplayPage);
			VRAMSize=crtc.GetEffectiveVRAMSize()/2;
		}
		hostMouseX-=origin.x();
		hostMouseY-=origin.y();
		if(0<zoom2x.x())
		{
			hostMouseX=hostMouseX*2/zoom2x.x();
		}
		if(0<zoom2x.y())
		{
			hostMouseY=hostMouseY*2/zoom2x.y();
		}

		// 2020/07/08
		// Lemmings uses double-buffering.  In TBIOS, mouse pointer is influenced by the VRAM offset.
		// However, in Lemmings the mouse pointer is distance from top-left corner of the monitor
		// regardless of the VRAM offset.  Therefore, the transformation needs to be skipped.
		// Also internally-stored X coordinate looks to be half of the actual coordinate.
		bool considerVRAMOffset=true;
		switch(state.appSpecificSetting)
		{
		case TOWNS_APPSPECIFIC_AMARANTH3:
			considerVRAMOffset=false;
			break;
		case TOWNS_APPSPECIFIC_LEMMINGS:
			considerVRAMOffset=false;
			hostMouseX*=zoom2x.x();
			hostMouseX/=4;
			slowDownRange=4;
			break;
		case TOWNS_APPSPECIFIC_LEMMINGS2:
			hostMouseY-=8;
			if(hostMouseY<-8)
			{
				hostMouseY=-8;
			}
			hostMouseX+=8;
			if(327==hostMouseX)
			{
				hostMouseX=328; // Otherwise cannot scroll to the right
			}
			break;
		}

		// 2020/06/13
		// SuperDAISENRYAKU uses mouse with VRAM offset=3BC00H.
		// This offset makes towns mouse cursor appear 32 pixels down from the Windows mouse cursor.
		// VRAM offset needs to be taken into account.
		if(true==considerVRAMOffset)
		{
			int bytesPerLine=crtc.GetPageBytesPerLine(state.mouseDisplayPage);
			if(0!=bytesPerLine)
			{
				int VRAMoffset=crtc.GetPageVRAMAddressOffset(state.mouseDisplayPage);
				unsigned int VRAMHeight=VRAMSize/bytesPerLine;

				// 2020/12/18
				// SuperDAISENRYAKU needs to make the VRAM offset signed, but I suspect making it signed
				// contradicted with something else.  I fix it this time, but it may break something else,
				// in which case I'll need to think about something else to support both.
				if(0x20000<=VRAMoffset)
				{
					VRAMoffset-=0x40000;
				}

				hostMouseY+=VRAMoffset/bytesPerLine;
				// The following must be signed int.
				// Wing Commander 2's mouse coordinate is signed.
				hostMouseY=std::min<int>(hostMouseY,VRAMHeight-1);
			}
			// At this time it only takes vertical displacement into account.
		}

		diffX=hostMouseX-mx;
		diffY=hostMouseY-my;
		return ControlMouseByDiff(diffX,diffY,tbiosid,slowDownRange);
	}
	return false;
}

bool FMTowns::ControlMouseInVMCoord(int goalMouseX,int goalMouseY,unsigned int tbiosid)
{
	int mx,my;
	GetMouseCoordinate(mx,my,tbiosid);
	return ControlMouseByDiff(goalMouseX-mx,goalMouseY-my,tbiosid);
}

bool FMTowns::ControlMouseByDiff(int diffX,int diffY,unsigned int tbiosid,int slowDownRange)
{
	if(state.MOS_pulsePerPixelH<8)
	{
		diffX*=state.MOS_pulsePerPixelH;
		diffX/=8;
	}
	if(state.MOS_pulsePerPixelV<8)
	{
		diffY*=state.MOS_pulsePerPixelV;
		diffY/=8;
	}

	int speed=state.mouseIntegrationSpeed;
	if(TOWNS_APPSPECIFIC_OPERATIONWOLF==state.appSpecificSetting)
	{
		speed*=2;
	}

	auto dx=ScaleStep(ClampStep(diffX),speed);
	auto dy=ScaleStep(ClampStep(diffY),speed);
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
	p.SetGamePadState(Abutton,Bbutton,left,right,up,down,run,pause);
}

void FMTowns::SetMouseMotion(int port,int dx,int dy)
{
	auto &p=gameport.state.ports[port&1];
	p.mouseMotion.Set(dx,dy);
}

void FMTowns::SetCyberStickState(int port,int x,int y,int z,int w,unsigned int trig)
{
	auto &p=gameport.state.ports[port&1];
	p.SetCyberStickState(x,y,z,w,trig);
}

void FMTowns::SetCAPCOMCPSFState(int port,bool left,bool right,bool up,bool down,bool A,bool B,bool X,bool Y,bool L,bool R, bool start,bool select)
{
	auto &p=gameport.state.ports[port&1];
	p.SetCAPCOMCPSFState(left,right,up,down,A,B,X,Y,L,R,start,select);
}

bool FMTowns::GetMouseCoordinate(int &mx,int &my,unsigned int tbiosid) const
{
	if(true==state.mouseBIOSActive &&
	   TOWNS_APPSPECIFIC_ULTIMAUNDERWORLD!=state.appSpecificSetting)
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
		case TBIOS_V31L31_93:
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
			// V2.1 L31
			// 0110:00014B2B BF804A0100                MOV     EDI,00014A80H

			// 0110:00014C94 8A6F1C                    MOV     CH,[EDI+1CH]
			// 0110:00014C97 8B570C                    MOV     EDX,[EDI+0CH]
			// 0110:00014C9A 0FA4D310                  SHLD    EBX,EDX,10H
			// 0110:00014C9E 886D1D                    MOV     [EBP+1DH],CH
			// 0110:00014CA1 66895518                  MOV     [EBP+18H],DX
			// 0110:00014CA5 66895D14                  MOV     [EBP+14H],BX
			// 0110:00014CA9 C3                        RET

			// V2.1 L50
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
		case TOWNS_APPSPECIFIC_ULTIMAUNDERWORLD:
			{
				auto debugStop=debugger.stop; // FetchWord may break due to MEMR.
				mx=(int)mem.FetchWord(state.appSpecific_MousePtrX);
				my=399-(int)mem.FetchWord(state.appSpecific_MousePtrY)*2;
				debugger.stop=debugStop;
			}
			return true;
		case TOWNS_APPSPECIFIC_WINGCOMMANDER1:
			{
				auto debugStop=debugger.stop; // FetchWord may break due to MEMR.
				mx=(int)mem.FetchWord(state.appSpecific_MousePtrX);
				my=(int)mem.FetchWord(state.appSpecific_MousePtrY);
				debugger.stop=debugStop;
			}
			return true;
		case TOWNS_APPSPECIFIC_WINGCOMMANDER2:
			{
				auto debugStop=debugger.stop; // FetchWord may break due to MEMR.
				mx=(int)mem.FetchWord(state.appSpecific_MousePtrX);
				my=(int)mem.FetchWord(state.appSpecific_MousePtrY);

				// In Wing Commander 2, mouse coordinate is signed.
				mx=(mx&0x7FFF)-(mx&0x8000);
				my=(my&0x7FFF)-(my&0x8000);

				unsigned int nQueueFilled=std::min(WC2_EVENTQUEUE_LENGTH,mem.FetchWord(state.appSpecific_WC2_EventQueueBaseAddr+WC2_EVENTQUEUE_FILLED));
				unsigned int queueReadPtr=mem.FetchDword(state.appSpecific_WC2_EventQueueBaseAddr+WC2_EVENTQUEUE_READ_PTR);
				queueReadPtr-=WC2_EVENTQUEUE_BASE_ADDR;
				for(unsigned int i=0; i<nQueueFilled; ++i)
				{
					unsigned int event=mem.FetchDword(state.appSpecific_WC2_EventQueueBaseAddr+queueReadPtr);
					if(0x01000000==(event&0xFF000000)) // Mouse event
					{
						// unsigned int buttons=(event>>16)&0xFF;
						int deltaX=(event>>8)&0xFF;
						int deltaY=(event&0xFF);
						deltaX=(deltaX&0x7F)-(deltaX&0x80);
						deltaY=(deltaY&0x7F)-(deltaY&0x80);
						mx-=deltaX;
						my-=deltaY;
					}
					if(WC2_EVENTQUEUE_LAST_OFFSET<=queueReadPtr)
					{
						queueReadPtr=0;
					}
					else
					{
						queueReadPtr+=8;
					}
				}

				debugger.stop=debugStop;
			}
			return true;
		case TOWNS_APPSPECIFIC_STRIKECOMMANDER:
			{
				auto debugStop=debugger.stop; // FetchWord may break due to MEMR.
				mx=(int)mem.FetchWord(state.appSpecific_MousePtrX)/2;
				my=(int)mem.FetchWord(state.appSpecific_MousePtrY);
				debugger.stop=debugStop;
			}
			return true;
		case TOWNS_APPSPECIFIC_LEMMINGS:
		case TOWNS_APPSPECIFIC_LEMMINGS2:
			{
				auto debugStop=debugger.stop; // FetchWord may break due to MEMR.
				mx=(int)mem.FetchWord(state.appSpecific_MousePtrX);
				my=(int)mem.FetchWord(state.appSpecific_MousePtrY);
				debugger.stop=debugStop;
			}
			return true;
		case TOWNS_APPSPECIFIC_BRANDISH:
			{
				mx=mem.FetchWord(0x30000+0x40E6);
				my=mem.FetchWord(0x30000+0x40E8);
			}
			return true;
		case TOWNS_APPSPECIFIC_OPERATIONWOLF:
			{
				auto debugStop=debugger.stop; // FetchWord may break due to MEMR.
				mx=(int)mem.FetchWord(state.appSpecific_MousePtrX);
				my=(int)mem.FetchWord(state.appSpecific_MousePtrY);
				debugger.stop=debugStop;
			}
			return true;
		case TOWNS_APPSPECIFIC_AMARANTH3:
			{
				// Observed:
				// MouseX 248F:0040
				// MouseY 248F:0042
				//
				// Read Mouse()
				// 2494:0000044E 8B0E0B00                  MOV     CX,[000BH]
				// 2494:00000452 FA                        CLI
				// 2494:00000453 E89500                    CALL    000004EB { Read Mouse 1 Byte()}

				const uint32_t signature[]=
				{
					0x000B0E8B,
					0x0095E8FA
				};
				const int32_t mouseCoordObserved=0x248F0+0x0040;
				const int32_t readMouseProcObserved=0x24940+0x044E;
				const int32_t physAddrOffset=mouseCoordObserved-readMouseProcObserved;

				for(uint32_t addr=0x24000; addr<=0x24FFF; addr+=0x10)
				{
					if(signature[0]==mem.FetchDword(addr+0x44E) &&
					   signature[1]==mem.FetchDword(addr+0x452))
					{
						auto debugStop=debugger.stop; // FetchWord may break due to MEMR.
						mx=(int)mem.FetchWord(int(addr+0x44E)+physAddrOffset);
						my=(int)mem.FetchWord(int(addr+0x44E)+physAddrOffset+2);
						debugger.stop=debugStop;
						return true;
					}
				}
			}
			break;
		}
	}
	return false;
}

void FMTowns::GetWingCommanderSetSpeedMaxSpeed(unsigned int &setSpeed,unsigned int &maxSpeed)
{
	setSpeed=mem.FetchByte(state.appSpecific_WC_setSpeedPtr);
	maxSpeed=mem.FetchByte(state.appSpecific_WC_maxSpeedPtr);
}
