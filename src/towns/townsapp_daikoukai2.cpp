// Daikoukaijidai2 for FM TOWNS
// Keyboard Shortcuts
#include "towns.h"


// City mode
//   MapLeftX
//   PHYS:000508A0
//   000C:00017EA2 A3A0280000                MOV     [000028A0H],EAX
//   MapUpY
//   PHYS:000508A4
//   000C:00017EEF A3A4280000                MOV     [000028A4H],EAX

// Ocean mode
//   MapLeftX
//   phys:50DFE
//   000C:00049F2B 66FF0DFE2D0000            DEC     WORD PTR [00002DFEH]
//   000C:00049F4D 66FF05FE2D0000            INC     WORD PTR [00002DFEH]
//   MapLeftY
//   phys:50E00
//   000C:00049F89 66FF0D002E0000            DEC     WORD PTR [00002E00H]
//   000C:00049FAB 66FF05002E0000            INC     WORD PTR [00002E00H]

// Left Arrow  (515,380)
// Up Arrow    (547,346)
// Right Arrow (580,380)
// Down Arrow  (547,411)

bool FMTownsCommon::Daikoukai2_ControlMouseByArrowKeys(
		int &lb,int &mb,int &rb,int &mx,int &my,
		unsigned int leftKey,
		unsigned int upKey,
		unsigned int rightKey,
		unsigned int downKey)
{
	int destX,destY;
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

// 0  (523,410)   =  (547,410)  +  (571,410)  Return (608,410)
// 1  (523,386)   2  (547,386)  3  (571,386)  -  (596,386)  +- (619,386)
// 4  (523,362)   5  (547,362)  6  (571,362)  /  (596,362)  AC (619,362)
// 7  (523,338)   8  (547,338)  9  (571,338)  *  (596,338)  Max(619,338)
