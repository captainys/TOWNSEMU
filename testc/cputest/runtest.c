#include <stdio.h>

#include "testcase.h"

#define LEN(x) (sizeof(x)/sizeof(x[0]))

extern void TEST_IMUL_R32_R32(unsigned int eax_edx_eflags[3],unsigned int eax,unsigned int edx);
extern void TEST_IMUL_R32_MEM(unsigned int eax_edx_eflags[3],unsigned int eax,unsigned int edx);
extern void TEST_MUL_R32_R32(unsigned int eax_edx_eflags[3],unsigned int eax,unsigned int edx);
extern void TEST_MUL_R32_MEM(unsigned int eax_edx_eflags[3],unsigned int eax,unsigned int edx);

int RunImulR32xR32Test(void)
{
	printf("RunImulR32xR32/MEMTest\n");

	for(int i=0; i<sizeof(IMUL_32_32_TABLE)/sizeof(IMUL_32_32_TABLE[0]); i+=5)
	{
		unsigned int eax_edx_eflags[3];
		TEST_IMUL_R32_R32(eax_edx_eflags,IMUL_32_32_TABLE[i],IMUL_32_32_TABLE[i+1]);
		if(eax_edx_eflags[0]!=IMUL_32_32_TABLE[i+2] ||
		   eax_edx_eflags[1]!=IMUL_32_32_TABLE[i+3] ||
		   eax_edx_eflags[2]!=IMUL_32_32_TABLE[i+4])
		{
			fprintf(stderr,"Error! (R32xR32)\n");
			fprintf(stderr,"  %08x*%08x\n",IMUL_32_32_TABLE[i],IMUL_32_32_TABLE[i+1]);
			fprintf(stderr,"  Expected: %08x %08x %08x\n",IMUL_32_32_TABLE[i+2],IMUL_32_32_TABLE[i+3],IMUL_32_32_TABLE[i+4]);
			fprintf(stderr,"  Returned: %08x %08x %08x\n",eax_edx_eflags[0],eax_edx_eflags[1],eax_edx_eflags[2]);
			return 1;
		}

		TEST_IMUL_R32_MEM(eax_edx_eflags,IMUL_32_32_TABLE[i],IMUL_32_32_TABLE[i+1]);
		if(eax_edx_eflags[0]!=IMUL_32_32_TABLE[i+2] ||
		   eax_edx_eflags[1]!=IMUL_32_32_TABLE[i+3] ||
		   eax_edx_eflags[2]!=IMUL_32_32_TABLE[i+4])
		{
			fprintf(stderr,"Error! (R32xMEM)\n");
			fprintf(stderr,"  %08x*%08x\n",IMUL_32_32_TABLE[i],IMUL_32_32_TABLE[i+1]);
			fprintf(stderr,"  Expected: %08x %08x %08x\n",IMUL_32_32_TABLE[i+2],IMUL_32_32_TABLE[i+3],IMUL_32_32_TABLE[i+4]);
			fprintf(stderr,"  Returned: %08x %08x %08x\n",eax_edx_eflags[0],eax_edx_eflags[1],eax_edx_eflags[2]);
			return 1;
		}
	}
	return 0;
}

int RunMulR32xR32Test(void)
{
	printf("RunMulR32xR32/MEMTest\n");

	for(int i=0; i<sizeof(MUL_32_32_TABLE)/sizeof(MUL_32_32_TABLE[0]); i+=5)
	{
		unsigned int eax_edx_eflags[3];
		TEST_MUL_R32_R32(eax_edx_eflags,MUL_32_32_TABLE[i],MUL_32_32_TABLE[i+1]);
		if(eax_edx_eflags[0]!=MUL_32_32_TABLE[i+2] ||
		   eax_edx_eflags[1]!=MUL_32_32_TABLE[i+3] ||
		   eax_edx_eflags[2]!=MUL_32_32_TABLE[i+4])
		{
			fprintf(stderr,"Error! (R32xR32)\n");
			fprintf(stderr,"  %08x*%08x\n",MUL_32_32_TABLE[i],MUL_32_32_TABLE[i+1]);
			fprintf(stderr,"  Expected: %08x %08x %08x\n",MUL_32_32_TABLE[i+2],MUL_32_32_TABLE[i+3],MUL_32_32_TABLE[i+4]);
			fprintf(stderr,"  Returned: %08x %08x %08x\n",eax_edx_eflags[0],eax_edx_eflags[1],eax_edx_eflags[2]);
			return 1;
		}

		TEST_MUL_R32_MEM(eax_edx_eflags,MUL_32_32_TABLE[i],MUL_32_32_TABLE[i+1]);
		if(eax_edx_eflags[0]!=MUL_32_32_TABLE[i+2] ||
		   eax_edx_eflags[1]!=MUL_32_32_TABLE[i+3] ||
		   eax_edx_eflags[2]!=MUL_32_32_TABLE[i+4])
		{
			fprintf(stderr,"Error! (R32xMEM)\n");
			fprintf(stderr,"  %08x*%08x\n",MUL_32_32_TABLE[i],MUL_32_32_TABLE[i+1]);
			fprintf(stderr,"  Expected: %08x %08x %08x\n",MUL_32_32_TABLE[i+2],MUL_32_32_TABLE[i+3],MUL_32_32_TABLE[i+4]);
			fprintf(stderr,"  Returned: %08x %08x %08x\n",eax_edx_eflags[0],eax_edx_eflags[1],eax_edx_eflags[2]);
			return 1;
		}
	}
	return 0;
}

extern C0_BITSHIFT_R8_I8(unsigned int res[16],unsigned int v0);
extern C1_BITSHIFT_R_I8(unsigned int res[16],unsigned int v0);
extern D3_BITSHIFT_R_CL(unsigned int res[16],unsigned int v0);

//unsigned int BitShiftR8_I8_TABLE[]={
//unsigned int BitShiftR_I8_TABLE[]={
//unsigned int BitShiftR_CL_TABLE[]={

static int BitShiftErrorCheck(unsigned int value,unsigned int expected[],unsigned int res[])
{
	static const char *const inst[8]={
		"ROL",
        "ROR",
        "RCL",
        "RCR",
        "SAL",
        "SAR",
        "SHL",
        "SHR",
	};
	for(int j=0; j<8; ++j)
	{
		for(int k=0; k<16; ++k)
		{
			if(expected[j*16+k]!=res[j*16+k])
			{
				int x;
				fprintf(stderr,"Error in %s (value=%d,j=%d,k=%d,step=%d)\n",inst[j],value,j,k,k/2);
				fprintf(stderr,"(Note step may be different from count.)\n");
				fprintf(stderr,"Expected:\n");
				for(x=0; x<16; ++x)
				{
					fprintf(stderr," %08x,",expected[j*16+x]);
					if(7==x)
					{
						fprintf(stderr,"\n");
					}
				}
				fprintf(stderr,"\n");
				fprintf(stderr,"Returned:\n");
				for(x=0; x<16; ++x)
				{
					fprintf(stderr," %08x,",res[j*16+x]);
					if(7==x)
					{
						fprintf(stderr,"\n");
					}
				}
				fprintf(stderr,"\n");
				return 1;
			}
		}
	}
	return 0;
}

int RunBitShiftTest(void)
{
	int i;
	unsigned int res[128];
	printf("C0_BITSHIFT_R8_I8\n");
	for(i=0; i<LEN(BitShiftR8_I8_TABLE); i+=129)
	{
		unsigned int *expected=BitShiftR8_I8_TABLE+i+1;
		C0_BITSHIFT_R8_I8(res,BitShiftR8_I8_TABLE[i]);
		if(0!=BitShiftErrorCheck(BitShiftR8_I8_TABLE[i],expected,res))
		{
			return 1;
		}
	}
	printf("C1_BITSHIFT_R_I8\n");
	for(i=0; i<LEN(BitShiftR_I8_TABLE); i+=129)
	{
		unsigned int *expected=BitShiftR_I8_TABLE+i+1;
		C1_BITSHIFT_R_I8(res,BitShiftR_I8_TABLE[i]);
		if(0!=BitShiftErrorCheck(BitShiftR_I8_TABLE[i],expected,res))
		{
			return 1;
		}
	}
	printf("D3_BITSHIFT_R_CL\n");
	for(i=0; i<LEN(BitShiftR_CL_TABLE); i+=129)
	{
		unsigned int *expected=BitShiftR_CL_TABLE+i+1;
		D3_BITSHIFT_R_CL(res,BitShiftR_CL_TABLE[i]);
		if(0!=BitShiftErrorCheck(BitShiftR_CL_TABLE[i],expected,res))
		{
			return 1;
		}
	}
	return 0;
}



int main(void)
{
	RunImulR32xR32Test();
	RunMulR32xR32Test();
	RunBitShiftTest();
	return 0;
}
