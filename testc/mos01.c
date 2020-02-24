#include "egb.h"
#include "mos.h"
#include "io.h"

static char EGB_work[EgbWorkSize];
static char MOS_work[MosWorkSize];

// Test Mouse

#define EGB_FOREGROUND_COLOR 0
#define EGB_BACKGROUND_COLOR 1
#define EGB_FILL_COLOR 2
#define EGB_TRANSPARENT_COLOR 3

#define EGB_PSET 0
#define EGB_PRESET 1
#define EGB_OR 2
#define EGB_AND 3
#define EGB_XOR 4
#define EGB_NOT 5
#define EGB_MATTE 6
#define EGB_PASTEL 7
#define EGB_OPAGUE 9
#define EGB_MASKSET 13
#define EGB_MASKRESET 14
#define EGB_MASKNOT 15

short seihou[]=
{
	5,
	 20, 20,
	 20,100,
	100,100,
	100, 20,
	 20, 20
};

short hishigata[]=
{
	5,
	  80,120,
	  20,180,
	  80,240,
	 140,180,
	  80,120,
};

int main(void)
{
	EGB_init(EGB_work,EgbWorkSize);

	EGB_resolution(EGB_work,0,3);
	EGB_resolution(EGB_work,1,3);

	EGB_writePage(EGB_work,1);
	EGB_color(EGB_work,EGB_BACKGROUND_COLOR,0);
	EGB_clearScreen(EGB_work);

	EGB_writePage(EGB_work,0);
	EGB_color(EGB_work,EGB_BACKGROUND_COLOR,0);
	EGB_clearScreen(EGB_work);

	EGB_color(EGB_work,EGB_FOREGROUND_COLOR,15);
	EGB_writeMode(EGB_work,EGB_PSET);

	EGB_connect(EGB_work,hishigata);
	EGB_unConnect(EGB_work,seihou);

	MOS_start(MOS_work,MosWorkSize);
	MOS_setpos(100,100);
	MOS_disp(1);
	IOWriteByte(TOWNSIO_VM_HOST_IF_CMD_STATUS,TOWNS_VMIF_CMD_PAUSE);

	for(;;)
	{
		int btn,x,y;
		MOS_rdpos(&btn,&x,&y);
		if(0!=(btn&3))
		{
			break;
		}
	}

	MOS_end();

	return 0;
}
