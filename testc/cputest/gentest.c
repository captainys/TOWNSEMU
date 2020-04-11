#include <stdio.h>

extern void TEST_IMUL_R32_R32(unsigned int eax_edx_eflags[3],unsigned int eax,unsigned int edx);
extern void TEST_MUL_R32_R32(unsigned int eax_edx_eflags[3],unsigned int eax,unsigned int edx);

static unsigned int testNumberSrc32[]=
{
	0x00000000,
	0x00000001,
	0x00000005,
	0x00000010,
	0x00000105,
	0x00001000,
	0x00007007,
	0x00010000,
	0x00100055,
	0x01000000,
	0x10000707,
	0x40000000,
	0x7FFFFFFF,
	0x80000000,
	0x800B000B,
	0xB00B000B,
	0xF0000000,
	0xFFFFFFFF,
};
static unsigned int testNumberSrc8[]=
{
	0x00,
	0x01,
	0x03,
	0x07,
	0x10,
	0x21,
	0x47,
	0x7F,
	0x80,
	0xB0,
	0xC0,
	0xD0,
	0xF0,
	0xFF,
};

#define LEN(x) (sizeof(x)/sizeof(x[0]))

void GenImulR32xR32Test(FILE *ofp)
{
	fprintf(ofp,"unsigned int IMUL_32_32_TABLE[]={\n");
	for(int i=0; i<LEN(testNumberSrc32); ++i)
	{
		for(int j=0; j<LEN(testNumberSrc32); ++j)
		{
			unsigned int eax=testNumberSrc32[i];
			unsigned int edx=testNumberSrc32[j];
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
	for(int i=0; i<LEN(testNumberSrc32); ++i)
	{
		for(int j=0; j<LEN(testNumberSrc32); ++j)
		{
			unsigned int eax=testNumberSrc32[i];
			unsigned int edx=testNumberSrc32[j];
			unsigned int eax_edx_eflags[3];
			TEST_MUL_R32_R32(eax_edx_eflags,eax,edx);
			fprintf(ofp,"\t0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,\n",eax,edx,eax_edx_eflags[0],eax_edx_eflags[1],eax_edx_eflags[2]);
		}
	}
	fprintf(ofp,"};\n");
}

extern C0_BITSHIFT_R8_I8(unsigned int res[16],unsigned int v0);
extern C1_BITSHIFT_R_I8(unsigned int res[16],unsigned int v0);
extern D3_BITSHIFT_R_CL(unsigned int res[16],unsigned int v0);

void GenBitShift(FILE *ofp)
{
	unsigned int res[128];
	{
		fprintf(ofp,"unsigned int BitShiftR8_I8_TABLE[]={\n");
		for(int i=0; i<LEN(testNumberSrc8); ++i)
		{
			unsigned int eax=testNumberSrc8[i];
			C0_BITSHIFT_R8_I8(res,eax);
			fprintf(ofp,"\t0x%08x,\n",eax);
			for(int j=0; j<128; ++j)
			{
				if(0==(j%16))
				{
					fprintf(ofp,"\t");
				}
				fprintf(ofp,"0x%08x,",res[j]);
				if(15==(j%16))
				{
					fprintf(ofp,"\n");
				}
			}
		}
		fprintf(ofp,"};\n");
	}
	{
		fprintf(ofp,"unsigned int BitShiftR_I8_TABLE[]={\n");
		for(int i=0; i<LEN(testNumberSrc32); ++i)
		{
			unsigned int eax=testNumberSrc32[i];
			C1_BITSHIFT_R_I8(res,eax);
			fprintf(ofp,"\t0x%08x,\n",eax);
			for(int j=0; j<128; ++j)
			{
				if(0==(j%16))
				{
					fprintf(ofp,"\t");
				}
				fprintf(ofp,"0x%08x,",res[j]);
				if(15==(j%16))
				{
					fprintf(ofp,"\n");
				}
			}
		}
		fprintf(ofp,"};\n");
	}
	{
		fprintf(ofp,"unsigned int BitShiftR_CL_TABLE[]={\n");
		for(int i=0; i<LEN(testNumberSrc32); ++i)
		{
			unsigned int eax=testNumberSrc32[i];
			D3_BITSHIFT_R_CL(res,eax);
			fprintf(ofp,"\t0x%08x,\n",eax);
			for(int j=0; j<128; ++j)
			{
				if(0==(j%16))
				{
					fprintf(ofp,"\t");
				}
				fprintf(ofp,"0x%08x,",res[j]);
				if(15==(j%16))
				{
					fprintf(ofp,"\n");
				}
			}
		}
		fprintf(ofp,"};\n");
	}
}


int main(void)
{
	FILE *ofp=fopen("cputest/testcase.h","w");
	GenImulR32xR32Test(ofp);
	GenMulR32xR32Test(ofp);
	GenBitShift(ofp);
	fclose(ofp);
	return 0;
}
