#include "egb.h"
#include "io.h"

static char EGB_work[EgbWorkSize];

void SetScreenMode(int m1,int m2);

// Test 16-color mode

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
	300, 20,
	300,220,
	 20,220,
	 20, 20
};
short hishigata[]=
{
	5,
	 160,  0,
	 320,120,
	 160,240,
	   0,120,
	 160,  0,
};
short taikaku[]=
{
	4,
	   0,  0,
	 320,240,
	 320,  0,
	   0,240,
};
int main(void)
{
	EGB_init(EGB_work,EgbWorkSize);

	EGB_resolution(EGB_work,0,10);
	EGB_resolution(EGB_work,1,10);

	EGB_writePage(EGB_work,0);
	EGB_color(EGB_work,EGB_BACKGROUND_COLOR,0x8000);
	EGB_clearScreen(EGB_work);

	EGB_writePage(EGB_work,1);
	EGB_color(EGB_work,EGB_BACKGROUND_COLOR,0x8000);
	EGB_clearScreen(EGB_work);


	EGB_writePage(EGB_work,0);

	EGB_color(EGB_work,EGB_FOREGROUND_COLOR,0x7FFF);
	EGB_writeMode(EGB_work,EGB_PSET);

	EGB_connect(EGB_work,seihou);
	EGB_connect(EGB_work,hishigata);

	EGB_writePage(EGB_work,1);

	EGB_color(EGB_work,EGB_FOREGROUND_COLOR,0x001F);
	EGB_writeMode(EGB_work,EGB_PSET);

	EGB_unConnect(EGB_work,taikaku);

	return 0;
}
