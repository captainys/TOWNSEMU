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

static int F6ErrorCheck(unsigned int eax,unsigned int edx,unsigned int expected[],unsigned int res[])
{
	static const char *const inst[8]={
		"NOT",
        "NEG",
        "MUL",
        "IMUL",
        "DIV",
        "IDIV",
	};
	for(int j=0; j<6; ++j)
	{
		if(expected[j*2]!=res[j*2] || expected[j*2+1]!=res[j*2+1])
		{
			int x;
			fprintf(stderr,"Error in %s eax=%d,edx=%d\n",inst[j],eax,edx);
			fprintf(stderr,"Expected:\n");
			for(x=0; x<12; ++x)
			{
				fprintf(stderr," %08x,",expected[x]);
				if(5==x)
				{
					fprintf(stderr,"\n");
				}
			}
			fprintf(stderr,"\n");
			fprintf(stderr,"Returned:\n");
			for(x=0; x<12; ++x)
			{
				fprintf(stderr," %08x,",res[x]);
				if(5==x)
				{
					fprintf(stderr,"\n");
				}
			}
			fprintf(stderr,"\n");
			return 1;
		}
	}
	return 0;
}

extern void TEST_F6(unsigned int res[],unsigned int eax,unsigned int edx);
extern void TEST_F7(unsigned int res[],unsigned int eax,unsigned int edx);

int RunF6F7_NOT_NEG_MUL_IMUL_DIV_IDIV(void)
{
	int i;
	unsigned int res[12];
	printf("F6_8_8_TABLE\n");
	for(i=0; i<LEN(F6_8_8_TABLE); i+=14)
	{
		unsigned *expected=F6_8_8_TABLE+i+2;
		TEST_F6(res,F6_8_8_TABLE[i],F6_8_8_TABLE[i+1]);
		if(F6ErrorCheck(F6_8_8_TABLE[i],F6_8_8_TABLE[i+1],expected,res))
		{
			return 1;
		}
	}

	printf("F7_32_32_TABLE\n");
	for(i=0; i<LEN(F7_32_32_TABLE); i+=14)
	{
		unsigned *expected=F7_32_32_TABLE+i+2;
		TEST_F7(res,F7_32_32_TABLE[i],F7_32_32_TABLE[i+1]);
		if(F6ErrorCheck(F7_32_32_TABLE[i],F7_32_32_TABLE[i+1],expected,res))
		{
			return 1;
		}
	}
}

extern void TEST_R8_I8(unsigned int res[16],unsigned int ebx);
extern void TEST_R32_I32(unsigned int res[16],unsigned int ebx);

int F6F7TestIErrorCheck(unsigned int ebx,unsigned int expected[],unsigned int res[])
{
	for(int i=0; i<16; ++i)
	{
		if(expected[i]!=res[i])
		{
			int x;
			fprintf(stderr,"Error in TEST R,I or R8,I8 ebx=%d\n",ebx);
			fprintf(stderr,"Expected:\n");
			for(x=0; x<16; ++x)
			{
				fprintf(stderr," %08x",expected[x]);
				if(7==x)
				{
					fprintf(stderr,"\n");
				}
			}
			fprintf(stderr,"\n");
			fprintf(stderr,"Returned:\n");
			for(x=0; x<16; ++x)
			{
				fprintf(stderr," %08x",res[x]);
				if(7==x)
				{
					fprintf(stderr,"\n");
				}
			}
			fprintf(stderr,"\n");
			return 1;
		}
	}
	return 0;
}

int RunF6F7_TEST_I(void)
{
	int i;
	unsigned int res[16];
	printf("F6_TEST_I8_TABLE\n");
	for(i=0; i<LEN(F6_TEST_I8_TABLE); i+=17)
	{
		unsigned *expected=F6_TEST_I8_TABLE+i+1;
		TEST_R8_I8(res,F6_TEST_I8_TABLE[i]);
		if(F6F7TestIErrorCheck(F6_TEST_I8_TABLE[i],expected,res))
		{
			return 1;
		}
	}

	printf("F7_TEST_I32_TABLE\n");
	for(i=0; i<LEN(F7_TEST_I32_TABLE); i+=17)
	{
		unsigned *expected=F7_TEST_I32_TABLE+i+1;
		TEST_R32_I32(res,F7_TEST_I32_TABLE[i]);
		if(F6F7TestIErrorCheck(F7_TEST_I32_TABLE[i],expected,res))
		{
			return 1;
		}
	}
}

void TEST_AAD(unsigned int res[],unsigned int EAX);
void TEST_AAM(unsigned int res[],unsigned int EAX);

int RunAADAAM(void)
{
	int i;
	printf("AAD_TABLE\n");
	for(i=0; i<LEN(AAD_TABLE); i+=3)
	{
		unsigned res[2];
		TEST_AAD(res,AAD_TABLE[i]);
		if(res[0]!=AAD_TABLE[i+1] || res[1]!=AAD_TABLE[i+2])
		{
			printf("Error in AAD  EAX=%08xH\n",AAD_TABLE[i+0]);
			printf("Expected: %08x %08x\n",AAD_TABLE[i+1],AAD_TABLE[i+2]);
			printf("Returned: %08x %08x\n",res[0],res[1]);
			return 1;
		}
	}
	printf("AAM_TABLE\n");
	for(i=0; i<LEN(AAM_TABLE); i+=3)
	{
		unsigned res[2];
		TEST_AAM(res,AAM_TABLE[i]);
		if(res[0]!=AAM_TABLE[i+1] || res[1]!=AAM_TABLE[i+2])
		{
			printf("Error in AAM  EAX=%08xH\n",AAM_TABLE[i]);
			printf("Expected: %08x %08x\n",AAM_TABLE[i+1],AAM_TABLE[i+2]);
			printf("Returned: %08x %08x\n",res[0],res[1]);
			return 1;
		}
	}
}

int main(void)
{
	RunImulR32xR32Test();
	RunMulR32xR32Test();
	RunBitShiftTest();
	RunF6F7_NOT_NEG_MUL_IMUL_DIV_IDIV();
	RunF6F7_TEST_I();
	RunAADAAM();
	return 0;
}
