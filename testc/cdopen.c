#include <stdio.h>

// Based on Oh! FM TOWNS July 1994.
extern int cdr_dopen(int drvno); /// door open
extern int cdr_dclose(int drvno); // door close
extern int cdr_dlock(int drvno); /// door lock
extern int cdr_dunlock(int drvno); // door unlock

// Writing 02h to I/O 2386h will break Tsugaru.
#define Tsugaru_Break _outb(0x2386,2)

// HC386 cdopen.c -ld:\hc386\townslib\lib\cdrfrb.lib

int main(void)
{
	printf("Press Enter to Open the tray:");
	getchar();

	Tsugaru_Break;
	cdr_dopen(0);

	printf("Press Enter to Close the tray:");
	getchar();

	Tsugaru_Break;
	cdr_dclose(0);

	printf("Press Enter to Lock the drive:");
	getchar();

	Tsugaru_Break;
	cdr_dlock(0);

	printf("Press Enter to Unlock the drive:");
	getchar();

	Tsugaru_Break;
	cdr_dunlock(0);

	return 0;
}
