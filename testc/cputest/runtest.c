#include <stdio.h>

#include "testcase.h"

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

extern C0_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_I8(unsigned int res[16],unsigned int v0);
extern C1_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_I8(unsigned int res[16],unsigned int v0);
extern D0_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_1(unsigned int res[16],unsigned int v0);
extern D1_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_1(unsigned int res[16],unsigned int v0);



int main(void)
{
	RunImulR32xR32Test();
	RunMulR32xR32Test();
	return 0;
}
