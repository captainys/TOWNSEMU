#include <stdio.h>



extern unsigned int GET_DS_BASE_LINEAR(void);
extern unsigned int SET_UP_EXPDOWN_SEG(unsigned int DorB,unsigned int Gbit,unsigned int baseLinear,unsigned int limit);
extern unsigned int READ_FROM_SEG(unsigned int selector,unsigned int offset);

unsigned int First64KB[65536]; // Force to use first 64KB.  Probably unnecessary.

#define TEST_BUF_SIZE 256*1024
unsigned int TestBuffer[TEST_BUF_SIZE];  // 1MB

unsigned int PointerToLinearAddr(void *ptr)
{
	return GET_DS_BASE_LINEAR()+(unsigned int)ptr;
}

int main(void)
{
	printf("Buffer Linear Addr: %08x\n",PointerToLinearAddr(TestBuffer));
	for(int i=0; i<TEST_BUF_SIZE; ++i)
	{
		TestBuffer[i]=PointerToLinearAddr(TestBuffer+i);
	}

	printf("Linear Address of TestBuffer[0x00000]: %08x\n",PointerToLinearAddr(TestBuffer+0x00000));
	printf("Content: %08x\n",TestBuffer[0x00000]);

	printf("Linear Address of TestBuffer[0x20000]: %08x\n",PointerToLinearAddr(TestBuffer+0x20000));
	printf("Content: %08x\n",TestBuffer[0x20000]);

	unsigned int SEG=SET_UP_EXPDOWN_SEG(1,0,0x00000000,0x0FFFF);
	printf("Expand-Down Segment: %04x\n",SEG);

	unsigned int offset=PointerToLinearAddr(TestBuffer+0x20000);
	printf("Offset %08x is mapped to linear address %08x in Expand-Down Segment.\n",
		offset,
		READ_FROM_SEG(SEG,offset)
		);
	return 0;
}
