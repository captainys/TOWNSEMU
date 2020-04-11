#include <stdio.h>

extern void TEST_IMUL_R32_R32(unsigned int eax_edx_eflags[3],unsigned int eax,unsigned int edx);
extern void TEST_MUL_R32_R32(unsigned int eax_edx_eflags[3],unsigned int eax,unsigned int edx);

void GenImulR32xR32Test(FILE *ofp)
{
	fprintf(ofp,"unsigned int IMUL_32_32_TABLE[]={\n");
	for(unsigned int eax=0; eax<0xf8000000; eax+=0x8000000)
	{
		for(unsigned int edx=0; edx<0xf8000000; edx+=0x8000000)
		{
			unsigned int eax_edx_eflags[3];
			TEST_IMUL_R32_R32(eax_edx_eflags,eax,edx);
			fprintf(ofp,"\t0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,\n",eax,edx,eax_edx_eflags[0],eax_edx_eflags[1],eax_edx_eflags[2]);
		}
	}
	fprintf(ofp,"};\n");
}

void GenMulR32xR32Test(FILE *ofp)
{
	fprintf(ofp,"unsigned int MUL_32_32_TABLE[]={\n");
	for(unsigned int eax=0; eax<0xf8000000; eax+=0x8000000)
	{
		for(unsigned int edx=0; edx<0xf8000000; edx+=0x8000000)
		{
			unsigned int eax_edx_eflags[3];
			TEST_MUL_R32_R32(eax_edx_eflags,eax,edx);
			fprintf(ofp,"\t0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,\n",eax,edx,eax_edx_eflags[0],eax_edx_eflags[1],eax_edx_eflags[2]);
		}
	}
	fprintf(ofp,"};\n");
}

extern C0_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_I8(unsigned int res[16],unsigned int v0);
extern C1_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_I8(unsigned int res[16],unsigned int v0);
extern D0_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_1(unsigned int res[16],unsigned int v0);
extern D1_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_1(unsigned int res[16],unsigned int v0);



int main(void)
{
	FILE *ofp=fopen("cputest/testcase.h","w");
	GenImulR32xR32Test(ofp);
	GenMulR32xR32Test(ofp);
	fclose(ofp);
	return 0;
}
