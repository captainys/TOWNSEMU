// Daikoukaijidai2 for FM TOWNS
// Keyboard Shortcuts
#include "towns.h"
#include "townsdef.h"

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

// Left Arrow  (515,380)
// Up Arrow    (547,346)
// Right Arrow (580,380)
// Down Arrow  (547,411)

void FMTownsCommon::Daikoukai2_CaptureFlags(void)
{
	auto &cpu=CPU();

	i486DXCommon::SegmentRegister DS;
	unsigned int exceptionType,exceptionCode;
	cpu.DebugLoadSegmentRegister(DS,0x0014,mem,false);

	state.appSpecific_Daikoukai2_p_flag=cpu.DebugFetchByte(32,DS,0x2899,mem);
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
		int x,y;
		switch(code2)
		{
		case TOWNS_JISKEY_0:
		case TOWNS_JISKEY_NUM_0:
			x=523;
			y=410;
			break;
		case TOWNS_JISKEY_1:
		case TOWNS_JISKEY_NUM_1:
			x=523;
			y=386;
			break;
		case TOWNS_JISKEY_2:
		case TOWNS_JISKEY_NUM_2:
			x=547;
			y=386;
			break;
		case TOWNS_JISKEY_3:
		case TOWNS_JISKEY_NUM_3:
			x=571;
			y=386;
			break;
		case TOWNS_JISKEY_4:
		case TOWNS_JISKEY_NUM_4:
			x=523;
			y=362;
			break;
		case TOWNS_JISKEY_5:
		case TOWNS_JISKEY_NUM_5:
			x=547;
			y=362;
			break;
		case TOWNS_JISKEY_6:
		case TOWNS_JISKEY_NUM_6:
			x=571;
			y=362;
			break;
		case TOWNS_JISKEY_7:
		case TOWNS_JISKEY_NUM_7:
			x=523;
			y=338;
			break;
		case TOWNS_JISKEY_8:
		case TOWNS_JISKEY_NUM_8:
			x=547;
			y=338;
			break;
		case TOWNS_JISKEY_9:
		case TOWNS_JISKEY_NUM_9:
			x=571;
			y=338;
			break;
		case TOWNS_JISKEY_NUM_PLUS:
			x=571;
			y=410;
			break;
		case TOWNS_JISKEY_NUM_MINUS:
			x=596;
			y=386;
			break;
		case TOWNS_JISKEY_NUM_STAR:
			x=596;
			y=338;
			break;
		case TOWNS_JISKEY_NUM_SLASH:
			x=596;
			y=362;
			break;
		case TOWNS_JISKEY_RETURN:
		case TOWNS_JISKEY_NUM_RETURN:
			x=608;
			y=410;
			break;
		case TOWNS_JISKEY_NUM_EQUAL:
		case TOWNS_JISKEY_NUM_DOT:
			x=547;
			y=410;
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
		case TOWNS_JISKEY_BACKSPACE:
			x=619;
			y=362;
			break;
		default:
			return;
		}

		eventLog.CleanUp();

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

		eventLog.BeginPlayback();
	}
}
// 0  (523,410)   =  (547,410)  +  (571,410)  Return (608,410)
// 1  (523,386)   2  (547,386)  3  (571,386)  -  (596,386)  +- (619,386)
// 4  (523,362)   5  (547,362)  6  (571,362)  /  (596,362)  AC (619,362)
// 7  (523,338)   8  (547,338)  9  (571,338)  *  (596,338)  Max(619,338)

bool FMTownsCommon::Daikoukaijidai2_MapXY(int &x,int &y) const
{
	x=16*state.appSpecific_Daikoukai2_MapX;
	y=16*state.appSpecific_Daikoukai2_MapY;
	return true;
}
