#include <stdio.h>



extern unsigned int GET_DS_BASE_LINEAR(void);
extern unsigned int SET_UP_EXPDOWN_SEG(unsigned int DorB,unsigned int Gbit,unsigned int baseLinear,unsigned int limit);
extern unsigned int READ_FROM_SEG(unsigned int selector,unsigned int offset);

#define TEST_BUF_SIZE 256*1024
unsigned int *TestBuffer;  // 1MB

unsigned int PointerToLinearAddr(void *ptr)
{
	return GET_DS_BASE_LINEAR()+(unsigned int)ptr;
}

void TestExpandDownSegment(unsigned int DorB,unsigned int Gbit,unsigned int baseLinear,unsigned int limit)
{
	unsigned int SEG=SET_UP_EXPDOWN_SEG(DorB,Gbit,baseLinear,limit);
	printf("Expand-Down Segment: %04x   Base=%08x  Limit=%06x\n",SEG,baseLinear,limit);

	unsigned int offset=PointerToLinearAddr(TestBuffer+0x20000);
	printf("Offset %08x is mapped to linear address %08x in Expand-Down Segment.\n",
		offset,
		READ_FROM_SEG(SEG,offset)
		);
}

int main(void)
{
	TestBuffer=(unsigned int *)malloc(sizeof(int)*TEST_BUF_SIZE);

	printf("Buffer Linear Addr: %08x\n",PointerToLinearAddr(TestBuffer));
	for(int i=0; i<TEST_BUF_SIZE; ++i)
	{
		TestBuffer[i]=PointerToLinearAddr(TestBuffer+i);
	}

	printf("Linear Address of TestBuffer[0x00000]: %08x\n",PointerToLinearAddr(TestBuffer+0x00000));
	printf("Content: %08x\n",TestBuffer[0x00000]);

	printf("Linear Address of TestBuffer[0x20000]: %08x\n",PointerToLinearAddr(TestBuffer+0x20000));
	printf("Content: %08x\n",TestBuffer[0x20000]);

	TestExpandDownSegment(1,0,0x00000000,0x0FFFF);
	TestExpandDownSegment(1,0,0x00000000,0x07FFF);
	TestExpandDownSegment(1,0,0x00000000,0x03FFF);

	TestExpandDownSegment(1,0,0x00000800,0x0FFFF);
	TestExpandDownSegment(1,0,0x00000800,0x07FFF);
	TestExpandDownSegment(1,0,0x00000800,0x03FFF);

	TestExpandDownSegment(1,0,0x00001000,0x0FFFF);
	TestExpandDownSegment(1,0,0x00001000,0x07FFF);
	TestExpandDownSegment(1,0,0x00001000,0x03FFF);

	printf("The following should cause GP fault.\n");
	unsigned int SEG=SET_UP_EXPDOWN_SEG(1,0,0x00000000,0xFFFF);
	READ_FROM_SEG(SEG,0);
	printf("Therefore I am not supposed to see this line.\n");

	return 0;
}
