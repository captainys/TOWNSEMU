#include <stdio.h>

extern void TEST_IMUL_R32_R32(unsigned int eax_edx_eflags[3],int eax,int edx);
extern void TEST_MUL_R32_R32(unsigned int eax_edx_eflags[3],int eax,int edx);

void GenImulR32xR32Test(FILE *ofp)
{
	fprintf(ofp,"unsigned int IMUL_32_32_TABLE[]={\n");
	for(int eax=-0x80000000; eax<0x7fefffff; eax+=0x100000)
	{
		for(int edx=-0x80000000; eax<0x7effffff; eax+=0x1000000)
		{
			unsigned int eax_edx_eflags[3];
			TEST_IMUL_R32_R32(eax_edx_eflags,eax,edx);
			fprintf(ofp,"\t0x%08x,0x%08x,0x%08x,\n",eax_edx_eflags[0],eax_edx_eflags[1],eax_edx_eflags[2]);
		}
	}
	fprintf(ofp,"}\n");
}

void GenMulR32xR32Test(FILE *ofp)
{
	fprintf(ofp,"unsigned int IMUL_32_32_TABLE[]={\n");
	for(int eax=-0x80000000; eax<0x7fefffff; eax+=0x100000)
	{
		for(int edx=-0x80000000; eax<0x7effffff; eax+=0x1000000)
		{
			unsigned int eax_edx_eflags[3];
			TEST_MUL_R32_R32(eax_edx_eflags,eax,edx);
			fprintf(ofp,"\t0x%08x,0x%08x,0x%08x,\n",eax_edx_eflags[0],eax_edx_eflags[1],eax_edx_eflags[2]);
		}
	}
	fprintf(ofp,"}\n");
}

extern C0_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_I8(unsigned int res[16],unsigned int v0);
extern C1_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_I8(unsigned int res[16],unsigned int v0);
extern D0_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_1(unsigned int res[16],unsigned int v0);
extern D1_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_1(unsigned int res[16],unsigned int v0);



int main(void)
{
	GenImulR32xR32Test(stdout);
	return 0;
}
