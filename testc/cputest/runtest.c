#include <stdio.h>
#include <string.h>

#include "../vmif.h"
#include "testcase.h"

#define LEN(x) (sizeof(x)/sizeof(x[0]))

extern void TEST_IMUL_R32_R32(unsigned int eax_edx_eflags[3],unsigned int eax,unsigned int edx);
extern void TEST_IMUL_R32_MEM(unsigned int eax_edx_eflags[3],unsigned int eax,unsigned int edx);
extern void TEST_MUL_R32_R32(unsigned int eax_edx_eflags[3],unsigned int eax,unsigned int edx);
extern void TEST_MUL_R32_MEM(unsigned int eax_edx_eflags[3],unsigned int eax,unsigned int edx);
extern void TEST_IMUL_R32_R32_I8(unsigned int res[32],unsigned int EAX,unsigned int EDX);

static unsigned char dataSrc[256]=
{
0x29,0x23,0xbe,0x84,0xe1,0x6c,0xd6,0xae,0x52,0x90,0x49,0xf1,0xf1,0xbb,0xe9,0xeb,
0xb3,0xa6,0xdb,0x3c,0x87,0x0c,0x3e,0x99,0x24,0x5e,0x0d,0x1c,0x06,0xb7,0x47,0xde,
0xb3,0x12,0x4d,0xc8,0x43,0xbb,0x8b,0xa6,0x1f,0x03,0x5a,0x7d,0x09,0x38,0x25,0x1f,
0x5d,0xd4,0xcb,0xfc,0x96,0xf5,0x45,0x3b,0x13,0x0d,0x89,0x0a,0x1c,0xdb,0xae,0x32,
0x20,0x9a,0x50,0xee,0x40,0x78,0x36,0xfd,0x12,0x49,0x32,0xf6,0x9e,0x7d,0x49,0xdc,
0xad,0x4f,0x14,0xf2,0x44,0x40,0x66,0xd0,0x6b,0xc4,0x30,0xb7,0x32,0x3b,0xa1,0x22,
0xf6,0x22,0x91,0x9d,0xe1,0x8b,0x1f,0xda,0xb0,0xca,0x99,0x02,0xb9,0x72,0x9d,0x49,
0x2c,0x80,0x7e,0xc5,0x99,0xd5,0xe9,0x80,0xb2,0xea,0xc9,0xcc,0x53,0xbf,0x67,0xd6,
0xbf,0x14,0xd6,0x7e,0x2d,0xdc,0x8e,0x66,0x83,0xef,0x57,0x49,0x61,0xff,0x69,0x8f,
0x61,0xcd,0xd1,0x1e,0x9d,0x9c,0x16,0x72,0x72,0xe6,0x1d,0xf0,0x84,0x4f,0x4a,0x77,
0x02,0xd7,0xe8,0x39,0x2c,0x53,0xcb,0xc9,0x12,0x1e,0x33,0x74,0x9e,0x0c,0xf4,0xd5,
0xd4,0x9f,0xd4,0xa4,0x59,0x7e,0x35,0xcf,0x32,0x22,0xf4,0xcc,0xcf,0xd3,0x90,0x2d,
0x48,0xd3,0x8f,0x75,0xe6,0xd9,0x1d,0x2a,0xe5,0xc0,0xf7,0x2b,0x78,0x81,0x87,0x44,
0x0e,0x5f,0x50,0x00,0xd4,0x61,0x8d,0xbe,0x7b,0x05,0x15,0x07,0x3b,0x33,0x82,0x1f,
0x18,0x70,0x92,0xda,0x64,0x54,0xce,0xb1,0x85,0x3e,0x69,0x15,0xf8,0x46,0x6a,0x04,
0x96,0x73,0x0e,0xd9,0x16,0x2f,0x67,0x68,0xd4,0xf7,0x4a,0x4a,0xd0,0x57,0x68,0x76,
};

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

int RunImulR32_R32xI8Test(void)
{
	printf("RunImulR32_R32xI8Test\n");

	unsigned int res[32];
	for(int i=0; i<sizeof(IMUL_32_32_I8_TABLE)/sizeof(IMUL_32_32_I8_TABLE[0]); i+=34)
	{
		TEST_IMUL_R32_R32_I8(res,IMUL_32_32_I8_TABLE[i],IMUL_32_32_I8_TABLE[i+1]);

		unsigned int *ref=IMUL_32_32_I8_TABLE+i+2;
		for(int j=0; j<32; ++j)
		{
			if(res[j]!=ref[j])
			{
				int k;
				fprintf(stderr,"Error! IMUL(R32=R32*I8)\n");
				fprintf(stderr,"%08x %08x\n",IMUL_32_32_I8_TABLE[i],IMUL_32_32_I8_TABLE[i+1]);
				fprintf(stderr,"Expected:\n");
				for(k=0; k<32; ++k)
				{
					fprintf(stderr,"%08x ",ref[k]);
					if(k%16==15)
					{
						fprintf(stderr,"\n");
					}
				}
				fprintf(stderr,"Returned:\n");
				for(k=0; k<32; ++k)
				{
					fprintf(stderr,"%08x ",res[k]);
					if(k%16==15)
					{
						fprintf(stderr,"\n");
					}
				}
				return 1;
			}
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
	return 0;
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
	return 0;
}

void TEST_AAD(unsigned int res[],unsigned int EAX);
void TEST_AAM(unsigned int res[],unsigned int EAX);
void TEST_AAS(unsigned int res[],unsigned int EAX);

int RunAADAAMAAS(void)
{
	int i;
	printf("AAD\n");
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
	printf("AAM\n");
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
	printf("AAS\n");
	for(i=0; i<LEN(AAS_TABLE); i+=3)
	{
		unsigned res[2];
		TEST_AAS(res,AAS_TABLE[i]);
		if(res[0]!=AAS_TABLE[i+1] || res[1]!=AAS_TABLE[i+2])
		{
			printf("Error in AAS  EAX=%08xH\n",AAS_TABLE[i]);
			printf("Expected: %08x %08x\n",AAS_TABLE[i+1],AAS_TABLE[i+2]);
			printf("Returned: %08x %08x\n",res[0],res[1]);
			return 1;
		}
	}
	return 0;
}

void BTX_R32_I8(unsigned int res[],unsigned int EBX);
void BTX_R32_R32(unsigned int res[],unsigned int OP1,unsigned int OP2);

int BTx_R32_I8_ErrorCheck(unsigned int EBX,const unsigned int expected[],const unsigned int returned[])
{
	const char *const inst[]={"BT","BTS","BTR","BTC"};
	for(int i=0; i<4; ++i)
	{
		for(int j=0; j<16; ++j)
		{
			int I=i*16+j;
			if(expected[I]!=returned[I])
			{
				int k;
				printf("Error in %s\n",inst[i]);
				printf("EBX: %08x\n",EBX);
				printf("Expected:\n");
				for(k=0; k<16; ++k)
				{
					printf("%08x ",expected[i*16+k]);
					if(k==7)
					{
						printf("\n");
					}
				}
				printf("\n");
				printf("Returned:\n");
				for(k=0; k<16; ++k)
				{
					printf("%08x ",returned[i*16+k]);
					if(k==7)
					{
						printf("\n");
					}
				}
				printf("\n");
				return 1;
			}
		}
	}
	return 0;
}

int BTx_R32_R32_ErrorCheck(unsigned int OP1,unsigned int OP2,const unsigned int expected[],const unsigned int returned[])
{
	const char *const inst[]={"BT","BTS","BTR","BTC"};
	for(int i=0; i<8; ++i)
	{
		if(expected[i]!=returned[i])
		{
			printf("Error in %s\n",inst[i/2]);
			printf("OP1,OP2:  %08x %08x\n",OP1,OP2);
			printf("Expected: %08x %08x\n",expected[i&~1],expected[(i&~1)+1]);
			printf("Returned: %08x %08x\n",returned[i&~1],returned[(i&~1)+1]);
			return 1;
		}
	}
	return 0;
}

int RunBTx(void)
{
	int i;
	printf("BT_BTS_BTR_BTC_I8\n");
	for(i=0; i<LEN(BTX_I8_TABLE); i+=65)
	{
		unsigned int res[64];
		unsigned int *expected=BTX_I8_TABLE+i+1;
		unsigned int *table=BTX_I8_TABLE+i;
		BTX_R32_I8(res,table[0]);
		if(0!=BTx_R32_I8_ErrorCheck(table[0],expected,res))
		{
			return 1;
		}
	}

	printf("BT_BTS_BTR_BTC_R32_R32\n");
	for(i=0; i<LEN(BTX_R32_R32_TABLE); i+=10)
	{
		unsigned int res[8];
		unsigned int *expected=BTX_R32_R32_TABLE+i+2;
		unsigned int *table=BTX_R32_R32_TABLE+i;
		BTX_R32_R32(res,table[0],table[1]);
		if(0!=BTx_R32_R32_ErrorCheck(table[0],table[1],expected,res))
		{
			return 1;
		}
	}

	return 0;
}

extern void TEST_CBW(unsigned int *res,unsigned int eax);
extern void TEST_CWDE(unsigned int *res,unsigned int eax);
extern void TEST_CWD(unsigned int res[2],unsigned int eax,unsigned int edx);
extern void TEST_CDQ(unsigned int res[2],unsigned int eax,unsigned int edx);

int CBW_CWDE_ErrorCheck(unsigned int input,unsigned int expected,unsigned int returned)
{
	if(expected!=returned)
	{
		printf("Error in CBW or CWDE\n");
		printf("EAX     : %08x\n",input);
		printf("Expected: %08x\n",expected);
		printf("Returned: %08x\n",returned);
		return 1;
	}
	return 0;
}

int CWD_CDQ_ErrorCheck(unsigned int input,unsigned int expected[2],unsigned int returned[2])
{
	if(expected[0]!=returned[0] || expected[1]!=returned[1])
	{
		printf("Error in CBW or CWDE\n");
		printf("EAX     : %08x\n",input);
		printf("Expected: %08x %08x\n",expected[0],expected[1]);
		printf("Returned: %08x %08x\n",returned[0],returned[1]);
		return 1;
	}
	return 0;
}

int RunCBW_CWDE_CWD_CDQ(void)
{
	int i;
	unsigned int res[2];
	printf("CBW_TABLE\n");
	for(i=0; i<LEN(CBW_TABLE); i+=2)
	{
		unsigned int *table=CBW_TABLE+i;
		unsigned int *expected=table+1;
		TEST_CBW(res,table[0]);
		if(0!=CBW_CWDE_ErrorCheck(table[0],*expected,res[0]))
		{
			return 1;
		}
	}
	printf("CWDE_TABLE\n");
	for(i=0; i<LEN(CWDE_TABLE); i+=2)
	{
		unsigned int *table=CWDE_TABLE+i;
		unsigned int *expected=table+1;
		TEST_CWDE(res,table[0]);
		if(0!=CBW_CWDE_ErrorCheck(table[0],*expected,res[0]))
		{
			return 1;
		}
	}

	printf("CWD_TABLE\n");
	for(i=0; i<LEN(CWD_TABLE); i+=3)
	{
		unsigned int *table=CWD_TABLE+i;
		unsigned int *expected=table+1;
		TEST_CWD(res,table[0],table[0]);
		if(0!=CWD_CDQ_ErrorCheck(table[0],expected,res))
		{
			return 1;
		}
	}

	printf("CDQ_TABLE\n");
	for(i=0; i<LEN(CDQ_TABLE); i+=3)
	{
		unsigned int *table=CDQ_TABLE+i;
		unsigned int *expected=table+1;
		TEST_CDQ(res,table[0],table[0]);
		if(0!=CWD_CDQ_ErrorCheck(table[0],expected,res))
		{
			return 1;
		}
	}
	return 0;
}

extern void TEST_ADC_SP0(unsigned int ret[2]);
extern void TEST_ADC_SP1(unsigned int ret[2]);
extern void TEST_ADC_SP2(unsigned int ret[2]);
extern void TEST_ADC_SP3(unsigned int ret[2]);
extern void TEST_ADC_SP4(unsigned int ret[2]);
extern void TEST_ADC_SP5(unsigned int ret[2]);
extern void TEST_ADC_SP6(unsigned int ret[2]);
extern void TEST_ADC_SP7(unsigned int ret[2]);
extern void TEST_SBB_SP0(unsigned int ret[2]);
extern void TEST_SBB_SP1(unsigned int ret[2]);
extern void TEST_SBB_SP2(unsigned int ret[2]);
extern void TEST_SBB_SP3(unsigned int ret[2]);
extern void TEST_SBB_SP4(unsigned int ret[2]);

int RunADC_SBB_SpecialCaseTable(void)
{
	int i;
	unsigned int res[64];

	printf("ADC Special Cases\n");

	TEST_ADC_SP0(res);
	TEST_ADC_SP1(res+2);
	TEST_ADC_SP2(res+4);
	TEST_ADC_SP3(res+6);
	TEST_ADC_SP4(res+8);
	TEST_ADC_SP5(res+10);
	TEST_ADC_SP6(res+12);
	TEST_ADC_SP7(res+14);
	for(i=0; i<16; ++i)
	{
		if(ADC_SP_TABLE[i]!=res[i])
		{
			printf("Error in ADC Special Case %d\n",(i>>1));
			printf("Returned: %08x %08x\n",res[(i>>1)*2],res[(i>>1)*2+1]);
			printf("Expected: %08x %08x\n",ADC_SP_TABLE[(i>>1)*2],ADC_SP_TABLE[(i>>1)*2+1]);
			return 1;
		}
	}

	printf("SBB Special Cases\n");

	TEST_SBB_SP0(res);
	TEST_SBB_SP1(res+2);
	TEST_SBB_SP2(res+4);
	TEST_SBB_SP3(res+6);
	TEST_SBB_SP4(res+8);
	for(i=0; i<10; ++i)
	{
		if(SBB_SP_TABLE[i]!=res[i])
		{
			printf("Error in SBB Special Case %d\n",(i>>1));
			printf("Returned: %08x %08x\n",res[(i>>1)*2],res[(i>>1)*2+1]);
			printf("Expected: %08x %08x\n",SBB_SP_TABLE[(i>>1)*2],SBB_SP_TABLE[(i>>1)*2+1]);
			return 1;
		}
	}

	return 0;
}

void TEST_ADC_SBB_32(unsigned int res[8],unsigned int eax,unsigned int ecx);
void TEST_ADC_SBB_16(unsigned int res[8],unsigned int eax,unsigned int ecx);
void TEST_ADC_SBB_8(unsigned int res[8],unsigned int eax,unsigned int ecx);

int RunADC_SBB(void)
{
	int i,j;
	printf("ADC_SBB_8_TABLE\n");
	for(i=0; i<LEN(ADC_SBB_8_TABLE); i+=10)
	{
		unsigned int *testPtr=ADC_SBB_8_TABLE+i;
		unsigned int *expected=testPtr+2;
		unsigned int res[8];
		TEST_ADC_SBB_8(res,testPtr[0],testPtr[1]);
		for(j=0; j<8; ++j)
		{
			if(res[j]!=expected[j])
			{
				int k;
				printf("Error in ADC SBB Test!\n");
				printf("Expected:");
				for(k=0; k<8; ++k)
				{
					printf(" %08x",expected[k]);
				}
				printf("Returned:");
				for(k=0; k<8; ++k)
				{
					printf(" %08x",res[k]);
				}
				return 1;
			}
		}
	}

	printf("ADC_SBB_16_TABLE\n");
	for(i=0; i<LEN(ADC_SBB_16_TABLE); i+=10)
	{
		unsigned int *testPtr=ADC_SBB_16_TABLE+i;
		unsigned int *expected=testPtr+2;
		unsigned int res[8];
		TEST_ADC_SBB_16(res,testPtr[0],testPtr[1]);
		for(j=0; j<8; ++j)
		{
			if(res[j]!=expected[j])
			{
				int k;
				printf("Error in ADC SBB Test!\n");
				printf("Expected:");
				for(k=0; k<8; ++k)
				{
					printf(" %08x",expected[k]);
				}
				printf("Returned:");
				for(k=0; k<8; ++k)
				{
					printf(" %08x",res[k]);
				}
				return 1;
			}
		}
	}

	printf("ADC_SBB_32_TABLE\n");
	for(i=0; i<LEN(ADC_SBB_32_TABLE); i+=10)
	{
		unsigned int *testPtr=ADC_SBB_32_TABLE+i;
		unsigned int *expected=testPtr+2;
		unsigned int res[8];
		TEST_ADC_SBB_32(res,testPtr[0],testPtr[1]);
		for(j=0; j<8; ++j)
		{
			if(res[j]!=expected[j])
			{
				int k;
				printf("Error in ADC SBB Test!\n");
				printf("Expected:");
				for(k=0; k<8; ++k)
				{
					printf(" %08x",expected[k]);
				}
				printf("Returned:");
				for(k=0; k<8; ++k)
				{
					printf(" %08x",res[k]);
				}
				return 1;
			}
		}
	}

	return 0;
}

extern void BT_MEM_R32(unsigned char res[512],const unsigned char data[64]);
extern void BT_MEM_R16(unsigned char res[512],const unsigned char data[64]);

int RunBT_MEM_R(void)
{
	int i,errCount=0;;
	unsigned char res[512];
	BT_MEM_R32(res,dataSrc);
	printf("BT_MEM_R32\n");
	for(i=0; i<512; ++i)
	{
		if(res[i]!=BT_M_R32_TABLE[i])
		{
			printf("Error at count=%d\n",i);
			++errCount;
			if(8<=errCount)
			{
				break;
			}
		}
	}

	BT_MEM_R16(res,dataSrc+64);
	printf("BT_MEM_R16\n");
	for(i=0; i<512; ++i)
	{
		if(res[i]!=BT_M_R16_TABLE[i])
		{
			printf("Error at count=%d\n",i);
			++errCount;
			if(8<=errCount)
			{
				break;
			}
		}
	}
	return (0<errCount ? 1 : 0);
}

extern int TEST_AAA(unsigned int eax,unsigned int edx);
extern int TEST_DAS(unsigned int eax,unsigned int edx);
extern int TEST_DAA(unsigned int eax,unsigned int edx);

int RunDAA_DAS(void)
{
	int i;
	printf("AAA_DAA_DAS_TABLE\n");
	for(i=0; i+3<LEN(DAA_DAS_TABLE); i+=5)
	{
		unsigned int resAAA=TEST_AAA(DAA_DAS_TABLE[i],DAA_DAS_TABLE[i+1]);
		unsigned int resDAA=TEST_DAA(DAA_DAS_TABLE[i],DAA_DAS_TABLE[i+1]);
		unsigned int resDAS=TEST_DAS(DAA_DAS_TABLE[i],DAA_DAS_TABLE[i+1]);
		if(resDAA!=DAA_DAS_TABLE[i+2])
		{
			printf("DAA error!\n");
			printf("Returned: %08x\n",resDAA);
			printf("[%d] %08x,%08x,%08x\n",i/4,DAA_DAS_TABLE[i],DAA_DAS_TABLE[i+1],DAA_DAS_TABLE[i+2]);
			return 1;
		}
		if(resDAS!=DAA_DAS_TABLE[i+3])
		{
			printf("DAS error!\n");
			printf("Returned: %08x\n",resDAS);
			printf("[%d] %08x,%08x,%08x\n",i/4,DAA_DAS_TABLE[i],DAA_DAS_TABLE[i+1],DAA_DAS_TABLE[i+3]);
			return 1;
		}
		if(resAAA!=DAA_DAS_TABLE[i+4])
		{
			printf("AAA error!\n");
			printf("Returned: %08x\n",resAAA);
			printf("[%d] %08x,%08x,%08x\n",i/4,DAA_DAS_TABLE[i],DAA_DAS_TABLE[i+1],DAA_DAS_TABLE[i+4]);
			return 1;
		}
	}
	return 0;
}

extern unsigned int TEST_MOV_A_TO_M(void);
extern unsigned int TEST_MOV_M_TO_A(void);

int RunMOV_M_TO_A_A_TO_M(void)
{
	printf("MOV A TO M\n");
	if(0!=TEST_MOV_A_TO_M())
	{
		printf("Error in MOV A TO M\n");
		return 1;
	}
	printf("MOV M TO A\n");
	if(0!=TEST_MOV_M_TO_A())
	{
		printf("Error in MOV A TO M\n");
		return 1;
	}
	return 0;
}

extern void TEST_SHLD_SHRD32(unsigned int res[6],unsigned int EAX,unsigned int EDX,unsigned int ECX);
extern void TEST_SHLD_SHRD16(unsigned int res[6],unsigned int EAX,unsigned int EDX,unsigned int ECX);

int RunSHLD_SHRD(void)
{
	int i;
	unsigned int res[6];

	printf("SHLD_SHRD32\n");
	for(i=0; i<LEN(SHLD_SHRD32); i+=9)
	{
		int j;
		unsigned int *truth=SHLD_SHRD32+i+3;
		TEST_SHLD_SHRD32(res,SHLD_SHRD32[i],SHLD_SHRD32[i+1],SHLD_SHRD32[i+2]);
		for(j=0; j<6; ++j)
		{
			if(truth[j]!=res[j])
			{
				printf("Error!\n");
				printf("Source %08x %08x %08x\n",SHLD_SHRD32[i],SHLD_SHRD32[i+1],SHLD_SHRD32[i+2]);
				printf("Returned %08x %08x %08x %08x %08x %08x\n",res[0],res[1],res[2],res[3],res[4],res[5]);
				printf("Correct  %08x %08x %08x %08x %08x %08x\n",truth[0],truth[1],truth[2],truth[3],truth[4],truth[5]);
				return 1;
			}
		}
	}

	printf("SHLD_SHRD16\n");
	for(i=0; i<LEN(SHLD_SHRD16); i+=9)
	{
		int j;
		unsigned int *truth=SHLD_SHRD16+i+3;
		TEST_SHLD_SHRD16(res,SHLD_SHRD16[i],SHLD_SHRD16[i+1],SHLD_SHRD16[i+2]);
		for(j=0; j<6; ++j)
		{
			if(truth[j]!=res[j])
			{
				printf("Error!\n");
				printf("Source %08x %08x %08x\n",SHLD_SHRD16[i],SHLD_SHRD16[i+1],SHLD_SHRD16[i+2]);
				printf("Returned %08x %08x %08x %08x %08x %08x\n",res[0],res[1],res[2],res[3],res[4],res[5]);
				printf("Correct  %08x %08x %08x %08x %08x %08x\n",truth[0],truth[1],truth[2],truth[3],truth[4],truth[5]);
				return 1;
			}
		}
	}

	return 0;
}



void BINARYOP_REG_REG(unsigned int res[72],unsigned int eax,unsigned int edx);
void BINARYOP_REG_MEM(unsigned int res[72],unsigned int eax,unsigned int edx);
void BINARYOP_MEM_REG(unsigned int res[72],unsigned int eax,unsigned int edx);

int RunBinaryOp(void)
{
	const char *const inst[]=
	{
		"ADC",
		"ADD",
		"AND",
		"CMP",
		"OR",
		"SBB",
		"SUB",
		"XOR",
	};
	const int bit[]=
	{
		32,16,8
	};
	int i;

	printf("BINARYOP_REG_REG\n");
	for(i=0; i<LEN(BINARYOP_TABLE); i+=74)
	{
		unsigned int received[72];
		const unsigned int *truth=BINARYOP_TABLE+i+2;
		const unsigned int eax=BINARYOP_TABLE[i];
		const unsigned int edx=BINARYOP_TABLE[i+1];
		BINARYOP_REG_REG(received,eax,edx);
		for(int j=0; j<72; ++j)
		{
			if(received[j]!=truth[j])
			{
				printf("Error!\n");
				printf("i=%d, j=%d, Instruction %s, Bit=%d\n",i,j,inst[(j%24)/3],bit[j/24]);
				printf("OP1=%08x  OP2=%08x\n",eax,edx);
				printf("Received: %08x %08x %08x\n",received[(j/3)*3],received[(j/3)*3+1],received[(j/3)*3+2]);
				printf("Correct:  %08x %08x %08x\n",truth[(j/3)*3],truth[(j/3)*3+1],truth[(j/3)*3+2]);
				return 1;
			}
		}
	}

	printf("BINARYOP_REG_MEM\n");
	for(i=0; i<LEN(BINARYOP_TABLE); i+=74)
	{
		unsigned int received[72];
		const unsigned int *truth=BINARYOP_TABLE+i+2;
		const unsigned int eax=BINARYOP_TABLE[i];
		const unsigned int edx=BINARYOP_TABLE[i+1];
		BINARYOP_REG_MEM(received,eax,edx);
		for(int j=0; j<72; ++j)
		{
			if(received[j]!=truth[j])
			{
				printf("Error!\n");
				printf("i=%d, j=%d, Instruction %s, Bit=%d\n",i,j,inst[(j%24)/3],bit[j/24]);
				printf("OP1=%08x  OP2=%08x\n",eax,edx);
				printf("Received: %08x %08x %08x\n",received[(j/3)*3],received[(j/3)*3+1],received[(j/3)*3+2]);
				printf("Correct:  %08x %08x %08x\n",truth[(j/3)*3],truth[(j/3)*3+1],truth[(j/3)*3+2]);
				return 1;
			}
		}
	}

	printf("BINARYOP_MEM_REG\n");
	for(i=0; i<LEN(BINARYOP_TABLE); i+=74)
	{
		unsigned int received[72];
		const unsigned int *truth=BINARYOP_TABLE+i+2;
		const unsigned int eax=BINARYOP_TABLE[i];
		const unsigned int edx=BINARYOP_TABLE[i+1];
		BINARYOP_MEM_REG(received,eax,edx);
		for(int j=0; j<72; ++j)
		{
			if(received[j]!=truth[j])
			{
				printf("Error!\n");
				printf("i=%d, j=%d, Instruction %s, Bit=%d\n",i,j,inst[(j%24)/3],bit[j/24]);
				printf("OP1=%08x  OP2=%08x\n",eax,edx);
				printf("Received: %08x %08x %08x\n",received[(j/3)*3],received[(j/3)*3+1],received[(j/3)*3+2]);
				printf("Correct:  %08x %08x %08x\n",truth[(j/3)*3],truth[(j/3)*3+1],truth[(j/3)*3+2]);
				return 1;
			}
		}
	}

	return 0;
}

extern int MOVEAXDS(int EAX);
extern int MOVEAXDS7FFF(void);

int RunMOV_EAX_DS(void)
{
	printf("MOV EAX,DS (operandSize=32)\n");
	// Upper 16-bit must clear.
	if(0!=(0xFFFF0000&MOVEAXDS(0xFFFF0000)) ||
	   0!=(0xFFFF0000&MOVEAXDS(0x00FF0000)) ||
	   0!=(0xFFFF0000&MOVEAXDS(0x00010000)) ||
	   0!=(0xFFFF0000&MOVEAXDS7FFF()))
	{
		printf("Error!\n");
		printf("MOV EAX,DS (operandSize=32) must clear upper 16-bits of EAX.\n");
		return 1;
	}
	return 0;
}

int main(int ac,char *av[])
{
	int nFail=0;
	nFail+=RunImulR32xR32Test();
	nFail+=RunMulR32xR32Test();
	nFail+=RunImulR32_R32xI8Test();
	nFail+=RunBitShiftTest();
	nFail+=RunF6F7_NOT_NEG_MUL_IMUL_DIV_IDIV();
	nFail+=RunF6F7_TEST_I();
	nFail+=RunAADAAMAAS();
	nFail+=RunBTx();
	nFail+=RunBT_MEM_R();
	nFail+=RunCBW_CWDE_CWD_CDQ();
	nFail+=RunADC_SBB_SpecialCaseTable();
	nFail+=RunADC_SBB();
	nFail+=RunDAA_DAS();
	nFail+=RunMOV_M_TO_A_A_TO_M();
	nFail+=RunSHLD_SHRD();
	nFail+=RunBinaryOp();
	nFail+=RunMOV_EAX_DS();
	printf("ARPL not covered.\n");
	printf("CALL and JMP not covered.\n");

	if(2<=ac && (0==strcmp("-unit",av[1]) || 0==strcmp("-UNIT",av[1])))
	{
		if(0<nFail)
		{
			TEST_FAILED();
		}
		else
		{
			TEST_SUCCEEDED();
		}
	}

	return 0;
}
