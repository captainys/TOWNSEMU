#include <stdio.h>
#include <conio.h>


#define IO_KEYBOARD_CMD_STATUS 0x602
#define IO_KEYBOARD_DATA 0x600

extern unsigned int A0A1(unsigned char res[]);
extern unsigned int A1A1(unsigned char res[]);
extern unsigned int A1(unsigned char res[]);
extern unsigned int A0(unsigned char res[]);

extern void RESETKEYBIOS(void);
extern void CLEAR_IF(void);
extern void SET_IF(void);

int main(void)
{
	int i,n;
	unsigned char res[128];

	n=A1(res);
	printf("  A1->\n",res);
	for(i=0; i<n; ++i)
	{
		printf("%02x ",res[i]);
	}
	printf("\n");

	n=A1A1(res);
	printf("A1A1->\n",res);
	for(i=0; i<n; ++i)
	{
		printf("%02x ",res[i]);
	}
	printf("\n");

	n=A0A1(res);
	printf("A0A1->\n",res);
	for(i=0; i<n; ++i)
	{
		printf("%02x ",res[i]);
	}
	printf("\n");

	n=A0(res);
	printf("  A0->\n",res);
	for(i=0; i<32; ++i)
	{
		printf("%02x ",res[i]);
	}
	printf("\n");



/*	CLEAR_IF();

	int c=10;
	for(;;)
	{
		auto flags=inp(IO_KEYBOARD_CMD_STATUS);
		if(flags&0x01)
		{
			auto code=inp(IO_KEYBOARD_DATA);
			printf("%02x\n",code);
			if(0==--c)
			{
				break;
			}
		}
	}

	SET_IF(); */

	RESETKEYBIOS();
	printf("May need reset.\n");

	return 0;
}
