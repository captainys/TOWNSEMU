// Daikoukaijidai2 for FM TOWNS
// Keyboard Shortcuts
#include "towns.h"
#include "townsdef.h"
#include "render.h"
#include "crtc.h"

// Field view (96,40)-(479,423)=384x384
//   Port Mode/Oceam Mode 16x16 tile  24x24 tile

// City mode
//   MapLeftX
//   PHYS:000508A0
//   000C:00017EA2 A3A0280000                MOV     [000028A0H(m_x)],EAX
//   MapUpY
//   PHYS:000508A4
//   000C:00017EEF A3A4280000                MOV     [000028A4H(m_y)],EAX

// Ocean mode
//   MapLeftX
//   phys:50DFE
//   000C:00049F2B 66FF0DFE2D0000            DEC     WORD PTR [00002DFEH(DispX)]
//   000C:00049F4D 66FF05FE2D0000            INC     WORD PTR [00002DFEH(DispX)]
//   MapLeftY
//   phys:50E00
//   000C:00049F89 66FF0D002E0000            DEC     WORD PTR [00002E00H(DispY)]
//   000C:00049FAB 66FF05002E0000            INC     WORD PTR [00002E00H(DispY)]

// Over the ocean, day palette:
// Page0:000000 0044DD DD4400 DD66AA 00AA66 00AAFF FFAA66 FFEEDD 777799 0044CC AA6600 DDBB55 0088EE 007766 00BB66 FFEEDD
// Page1:000000 0044DD DD4400 DD66AA 00AA66 00AAFF FFAA66 FFEEDD 777799 0044CC AA6600 DDBB55 0088EE 007766 00BB66 FFEEDD
// Arctic, day palette:
// Page0:000000 0044DD DD4400 DD66AA 00AA66 00AAFF FFAA66 FFEEDD 777799 0044CC AA6600 EEBB55 0088FF 007766 00BB66 FFEEDD
// Page1:000000 0044DD DD4400 DD66AA 00AA66 00AAFF FFAA66 FFEEDD 777799 0044CC AA6600 EEBB55 0088FF 007766 00BB66 FFEEDD

// London, day palette:
// Page0:000000 0044DD DD4400 DD66AA 00AA66 00AAFF FFAA66 FFEEDD 777799 0044CC AA6600 EEBB55 0088FF 007766 00BB66 FFEEDD
// Page1:000000 0044DD DD4400 DD66AA 00AA66 00AAFF FFAA66 FFEEDD 777799 0044CC AA6600 EEBB55 0088FF 007766 00BB66 FFEEDD

// Left Arrow  (515,380)
// Up Arrow    (547,346)
// Right Arrow (580,380)
// Down Arrow  (547,411)

void FMTownsCommon::Daikoukai2_CaptureFlags(void)
{
	auto &cpu=CPU();

	i486DXCommon::SegmentRegister DS;
	unsigned int excType,excCode;
	cpu.DebugLoadSegmentRegister(DS,0x0014,mem,false);

	state.appSpecific_Daikoukai2_p_flag=cpu.DebugFetchByte(32,DS,0x2899,mem);
	state.appSpecific_Daikoukai2_GetaYAddr=cpu.DebugLinearAddressToPhysicalAddress(excType,excCode,DS.baseLinearAddr+0xAF80,mem);
	state.appSpecific_Daikoukai2_DentakuXAddr=cpu.DebugLinearAddressToPhysicalAddress(excType,excCode,DS.baseLinearAddr+0xC4E8,mem);
	state.appSpecific_Daikoukai2_DentakuYAddr=cpu.DebugLinearAddressToPhysicalAddress(excType,excCode,DS.baseLinearAddr+0xC4EC,mem);
	if(0xFF==state.appSpecific_Daikoukai2_p_flag) // Ocean mode
	{
		state.appSpecific_Daikoukai2_MapX=cpu.DebugFetchWord(32,DS,0x2DFE,mem);
		state.appSpecific_Daikoukai2_MapY=cpu.DebugFetchWord(32,DS,0x2E00,mem);
	}
	else
	{
		state.appSpecific_Daikoukai2_MapX=cpu.DebugFetchDword(32,DS,0x28A0,mem);
		state.appSpecific_Daikoukai2_MapY=cpu.DebugFetchDword(32,DS,0x28A4,mem);
	}

	// YesNoDsp ()
	// 000C:00027210 53                        PUSH    EBX
	// 000C:00027211 56                        PUSH    ESI
	// 000C:00027212 57                        PUSH    EDI
	// 000C:00027213 A1C0F50000                MOV     EAX,[0000F5C0H]
	// 000C:00027218 8D0480                    LEA     EAX,[EAX+EAX*4]
	// 000C:0002721B 8D048500000000            LEA     EAX,[EAX*4]
	// 000C:00027222 8BB8D8F50000              MOV     EDI,[EAX+0000F5D8H]
	// 000C:00027228 8BB0DCF50000              MOV     ESI,[EAX+0000F5DCH]
	uint32_t EAX=cpu.DebugFetchDword(32,DS,0xF5C0,mem);
	EAX*=20;
	state.appSpecific_Daikoukai2_YesNoXAddr=cpu.DebugLinearAddressToPhysicalAddress(excType,excCode,DS.baseLinearAddr+0xF5D8+EAX,mem);
	state.appSpecific_Daikoukai2_YesNoYAddr=cpu.DebugLinearAddressToPhysicalAddress(excType,excCode,DS.baseLinearAddr+0xF5DC+EAX,mem);
}
bool FMTownsCommon::Daikoukai2_ControlMouseByArrowKeys(
		int &lb,int &mb,int &rb,int &mx,int &my,
		unsigned int leftKey,
		unsigned int upKey,
		unsigned int rightKey,
		unsigned int downKey)
{
	int destX,destY;
	if(0xFF==state.appSpecific_Daikoukai2_p_flag)  // 0xFF->Ocean  0x1D->Port
	{
		if(0!=leftKey && 0!=upKey)
		{
			destX=514;
			destY=323;
		}
		else if(0!=rightKey && 0!=upKey)
		{
			destX=588;
			destY=323;
		}
		else if(0!=rightKey && 0!=downKey)
		{
			destX=588;
			destY=396;
		}
		else if(0!=leftKey && 0!=downKey)
		{
			destX=514;
			destY=396;
		}
		else if(0!=leftKey)
		{
			destX=501;
			destY=360;
		}
		else if(0!=upKey)
		{
			destX=552;
			destY=308;
		}
		else if(0!=rightKey)
		{
			destX=599;
			destY=360;
		}
		else if(0!=downKey)
		{
			destX=551;
			destY=410;
		}
		else
		{
			return false;
		}
	}
	else
	{
		if(0!=leftKey)
		{
			destX=515;
			destY=381;
		}
		else if(0!=upKey)
		{
			destX=547;
			destY=346;
		}
		else if(0!=rightKey)
		{
			destX=580;
			destY=380;
		}
		else if(0!=downKey)
		{
			destX=547;
			destY=411;
		}
		else
		{
			return false;
		}
	}

	mx=destX;
	my=destY;

	int cx,cy;
	if(true==GetMouseCoordinate(cx,cy,state.tbiosVersion))
	{
		cx-=destX;
		cy-=destY;
		if(-5<=cx && cx<=5 && -5<=cy && cy<=5)
		{
			lb=1;
		}
	}

	return true;
}
void FMTownsCommon::Daikoukai2_TakeOverKeystroke(unsigned int code1,unsigned int code2)
{
	if(0==(TOWNS_KEYFLAG_RELEASE&code1))
	{
		auto &cpu=CPU();
		if((0x000C==cpu.state.CS().value || 0x0110==cpu.state.CS().value) && 0x0014==cpu.state.DS().value)
		{
			Daikoukai2_CaptureFlags();
			// Double-buffering is not used in sea-battle mode.
			// If state-loaded after the victory, it may not know the addresses.
		}

		int dentakuX=mem.FetchDword(state.appSpecific_Daikoukai2_DentakuXAddr);
		int dentakuY=mem.FetchDword(state.appSpecific_Daikoukai2_DentakuYAddr);
		int getaY=mem.FetchDword(state.appSpecific_Daikoukai2_GetaYAddr);

		int yesNoX=mem.FetchDword(state.appSpecific_Daikoukai2_YesNoXAddr);
		int yesNoY=mem.FetchDword(state.appSpecific_Daikoukai2_YesNoYAddr);

		int dentakuOffsetX=dentakuX-0x1F8;
		int dentakuOffsetY=dentakuY-0xF8+(getaY-0x20);

		int x,y;
		TownsEventLog::Event e;
		switch(code2)
		{
		case TOWNS_JISKEY_0:
		case TOWNS_JISKEY_NUM_0:
			x=523+dentakuOffsetX;
			y=410+dentakuOffsetY;
			break;
		case TOWNS_JISKEY_1:
		case TOWNS_JISKEY_NUM_1:
			x=523+dentakuOffsetX;
			y=386+dentakuOffsetY;
			break;
		case TOWNS_JISKEY_2:
		case TOWNS_JISKEY_NUM_2:
			x=547+dentakuOffsetX;
			y=386+dentakuOffsetY;
			break;
		case TOWNS_JISKEY_3:
		case TOWNS_JISKEY_NUM_3:
			x=571+dentakuOffsetX;
			y=386+dentakuOffsetY;
			break;
		case TOWNS_JISKEY_4:
		case TOWNS_JISKEY_NUM_4:
			x=523+dentakuOffsetX;
			y=362+dentakuOffsetY;
			break;
		case TOWNS_JISKEY_5:
		case TOWNS_JISKEY_NUM_5:
			x=547+dentakuOffsetX;
			y=362+dentakuOffsetY;
			break;
		case TOWNS_JISKEY_6:
		case TOWNS_JISKEY_NUM_6:
			x=571+dentakuOffsetX;
			y=362+dentakuOffsetY;
			break;
		case TOWNS_JISKEY_7:
		case TOWNS_JISKEY_NUM_7:
			x=523+dentakuOffsetX;
			y=338+dentakuOffsetY;
			break;
		case TOWNS_JISKEY_8:
		case TOWNS_JISKEY_NUM_8:
			x=547+dentakuOffsetX;
			y=338+dentakuOffsetY;
			break;
		case TOWNS_JISKEY_9:
		case TOWNS_JISKEY_NUM_9:
			x=571+dentakuOffsetX;
			y=338+dentakuOffsetY;
			break;
		case TOWNS_JISKEY_NUM_PLUS:
			x=571+dentakuOffsetX;
			y=410+dentakuOffsetY;
			break;
		case TOWNS_JISKEY_NUM_MINUS:
			x=596+dentakuOffsetX;
			y=386+dentakuOffsetY;
			break;
		case TOWNS_JISKEY_NUM_STAR:
			x=596+dentakuOffsetX;
			y=338+dentakuOffsetY;
			break;
		case TOWNS_JISKEY_X:
			x=619+dentakuOffsetX;
			y=338+dentakuOffsetY;
			break;
		case TOWNS_JISKEY_NUM_SLASH:
			x=596+dentakuOffsetX;
			y=362+dentakuOffsetY;
			break;
		case TOWNS_JISKEY_RETURN:
		case TOWNS_JISKEY_NUM_RETURN:
			x=608+dentakuOffsetX;
			y=410+dentakuOffsetY;
			break;
		case TOWNS_JISKEY_NUM_EQUAL:
		case TOWNS_JISKEY_NUM_DOT:
			x=547+dentakuOffsetX;
			y=410+dentakuOffsetY;
			break;
		case TOWNS_JISKEY_C:
		case TOWNS_JISKEY_BACKSPACE:
			x=619+dentakuOffsetX;
			y=362+dentakuOffsetY;
			break;
		case TOWNS_JISKEY_E:	// Empty
		case TOWNS_JISKEY_F:	// Full
			{
				int fillOrEmptyX=0,fillOrEmptyY=0;
				if(TOWNS_JISKEY_E==code2)
				{
					fillOrEmptyX=619+dentakuOffsetX;
					fillOrEmptyY=362+dentakuOffsetY;
				}
				else
				{
					fillOrEmptyX=619+dentakuOffsetX;
					fillOrEmptyY=338+dentakuOffsetY;
				}
				int cx,cy;
				if(true!=GetMouseCoordinate(cx,cy,state.tbiosVersion))
				{
					cx=320;
					cy=240;
				}
				eventLog.CleanUp();
				eventLog.AddClick(cx,cy);
				eventLog.AddClick(fillOrEmptyX,fillOrEmptyY);
				eventLog.AddClick(608+dentakuOffsetX,410+dentakuOffsetY);
				eventLog.BeginPlayback();
				return;
			}
			break;
		case TOWNS_JISKEY_H:
			{
				int cx,cy;
				if(true!=GetMouseCoordinate(cx,cy,state.tbiosVersion))
				{
					cx=320;
					cy=240;
				}
				eventLog.CleanUp();
				eventLog.AddClick(cx,cy);
				eventLog.AddClick(619+dentakuOffsetX,338+dentakuOffsetY);  // Max
				eventLog.AddClick(596+dentakuOffsetX,362+dentakuOffsetY);  // /
				eventLog.AddClick(547+dentakuOffsetX,386+dentakuOffsetY);  // 2
				eventLog.AddClick(547+dentakuOffsetX,410+dentakuOffsetY);  // =
				eventLog.AddClick(608+dentakuOffsetX,410+dentakuOffsetY);  // Enter
				eventLog.BeginPlayback();
				return;
			}
			break;
		case TOWNS_JISKEY_PF01:
			x=629;
			y=384;
			break;
		case TOWNS_JISKEY_PF02:
			x=629;
			y=302;
			break;
		case TOWNS_JISKEY_PF03:
			x=629;
			y=226;
			break;
		case TOWNS_JISKEY_PF04:
			x=629;
			y=143;
			break;
		case TOWNS_JISKEY_BREAK:
		case TOWNS_JISKEY_ESC:
			eventLog.CleanUp();
			{
				int cx,cy;
				if(true!=GetMouseCoordinate(cx,cy,state.tbiosVersion))
				{
					cx=320;
					cy=240;
				}

				e.eventType=TownsEventLog::EVT_RBUTTONDOWN;
				e.t=std::chrono::milliseconds(75);
				e.mos.Set(cx,cy);
				e.mosTolerance=2;
				eventLog.AddEvent(e);

				e.eventType=TownsEventLog::EVT_RBUTTONUP;
				e.t=std::chrono::milliseconds(75);
				e.mos.Set(cx,cy);
				e.mosTolerance=2;
				eventLog.AddEvent(e);
			}
			eventLog.BeginPlayback();
			return;
		case TOWNS_JISKEY_Y:
			x=yesNoX+40;
			y=yesNoY+30+getaY;
			break;
		case TOWNS_JISKEY_N:
			x=yesNoX+100;
			y=yesNoY+30+getaY;
			break;
		default:
			return;
		}

		if(0<=x && x<640 && 0<=y && y<480)
		{
			eventLog.CleanUp();
			eventLog.AddClick(x,y);
			eventLog.BeginPlayback();
		}
	}
}
// 0  (523,410)   =  (547,410)  +  (571,410)  Return (608,410)
// 1  (523,386)   2  (547,386)  3  (571,386)  -  (596,386)  +- (619,386)
// 4  (523,362)   5  (547,362)  6  (571,362)  /  (596,362)  AC (619,362)
// 7  (523,338)   8  (547,338)  9  (571,338)  *  (596,338)  Max(619,338)

bool FMTownsCommon::Daikoukaijidai2_MapXY(int &x,int &y) const
{
	x=state.appSpecific_Daikoukai2_MapX;
	y=state.appSpecific_Daikoukai2_MapY;
	return true;
}
void FMTownsCommon::Daikoukaijidai2_ScreenshotOverride(TownsRender &render,TownsCRTC::AnalogPalette &palette) const
{
	const unsigned char dayPalette[16][3]=
	{
		{0x00,0x00,0x00},
		{0x00,0x44,0xDD},
		{0xDD,0x44,0x00},
		{0xDD,0x66,0xAA},
		{0x00,0xAA,0x66},
		{0x00,0xAA,0xFF},
		{0xFF,0xAA,0x66},
		{0xFF,0xEE,0xDD},
		{0x77,0x77,0x99},
		{0x00,0x44,0xCC},
		{0xAA,0x66,0x00},
		{0xEE,0xBB,0x55},
		{0x00,0x88,0xFF},
		{0x00,0x77,0x66},
		{0x00,0xBB,0x66},
		{0xFF,0xEE,0xDD},
	};
	for(int page=0; page<2; ++page)
	{
		for(int i=0; i<16; ++i)
		{
			palette.plt16[page][i][0]=dayPalette[i][0];
			palette.plt16[page][i][1]=dayPalette[i][1];
			palette.plt16[page][i][2]=dayPalette[i][2];
		}
	}
}
