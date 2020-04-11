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

extern void TEST_F6(unsigned int res[],unsigned int eax,unsigned int edx);

void GenF6F7_NOT_NEG_MUL_IMUL_DIV_IDIV(FILE *ofp)
{
	int i;
	fprintf(ofp,"unsigned int F6_8_8_TABLE[]={\n");
	for(i=0; i<LEN(testNumberSrc8); ++i)
	{
		for(int j=0; j<LEN(testNumberSrc8); ++j)
		{
			int k;
			unsigned int eax=testNumberSrc8[i];
			unsigned int edx=testNumberSrc8[j];
			unsigned int res[12];
			TEST_F6(res,eax,edx);
			fprintf(ofp,"\t0x%08x,0x%08x,\n",eax,edx);
			fprintf(ofp,"\t");
			for(k=0; k<6; ++k)
			{
				fprintf(ofp,"0x%08x,",res[k]);
			}
			fprintf(ofp,"\n");
			fprintf(ofp,"\t");
			for(k=6; k<12; ++k)
			{
				fprintf(ofp,"0x%08x,",res[k]);
			}
			fprintf(ofp,"\n");
		}
	}
	fprintf(ofp,"};\n");

	fprintf(ofp,"unsigned int F7_32_32_TABLE[]={\n");
	for(i=0; i<LEN(testNumberSrc32); ++i)
	{
		for(int j=0; j<LEN(testNumberSrc32); ++j)
		{
			int k;
			unsigned int eax=testNumberSrc8[i];
			unsigned int edx=testNumberSrc8[j];
			unsigned int res[12];
			TEST_F7(res,eax,edx);
			fprintf(ofp,"\t0x%08x,0x%08x,\n",eax,edx);
			fprintf(ofp,"\t");
			for(k=0; k<6; ++k)
			{
				fprintf(ofp,"0x%08x,",res[k]);
			}
			fprintf(ofp,"\n");
			fprintf(ofp,"\t");
			for(k=6; k<12; ++k)
			{
				fprintf(ofp,"0x%08x,",res[k]);
			}
			fprintf(ofp,"\n");
		}
	}
	fprintf(ofp,"};\n");

}

extern void TEST_R8_I8(unsigned int res[16],unsigned int ebx);
extern void TEST_R32_I32(unsigned int res[16],unsigned int ebx);

void GenF6F7_TEST_R_I(FILE *ofp)
{
	int i;
	fprintf(ofp,"unsigned int F6_TEST_I8_TABLE[]={\n");
	for(i=0; i<LEN(testNumberSrc8); ++i)
	{
		unsigned int res[16];
		TEST_R8_I8(res,testNumberSrc8[i]);
		fprintf(ofp,"\t0x%08x,\n",testNumberSrc8[i]);
		for(int j=0; j<16; ++j)
		{
			if(0==(j%8))
			{
				fprintf(ofp,"\t");
			}
			fprintf(ofp,"0x%08x,",res[j]);
			if(7==(j%8))
			{
				fprintf(ofp,"\n");
			}
		}
	}
	fprintf(ofp,"};\n");

	fprintf(ofp,"unsigned int F7_TEST_I32_TABLE[]={\n");
	for(i=0; i<LEN(testNumberSrc32); ++i)
	{
		unsigned int res[16];
		TEST_R32_I32(res,testNumberSrc32[i]);
		fprintf(ofp,"\t0x%08x,\n",testNumberSrc32[i]);
		for(int j=0; j<16; ++j)
		{
			if(0==(j%8))
			{
				fprintf(ofp,"\t");
			}
			fprintf(ofp,"0x%08x,",res[j]);
			if(7==(j%8))
			{
				fprintf(ofp,"\n");
			}
		}
	}
	fprintf(ofp,"};\n");
}

extern void TEST_AAD(unsigned int res[],unsigned int eax);
extern void TEST_AAM(unsigned int res[],unsigned int eax);

void GenAADAAM(FILE *ofp)
{
	int i,j;
	fprintf(ofp,"unsigned int AAD_TABLE[]={\n");
	for(i=0; i<16; ++i)
	{
		for(j=0; j<16; ++j)
		{
			unsigned int res[2];
			unsigned int eax=i*256+j;
			TEST_AAD(res,eax);
			fprintf(ofp,"\t0x%08x,0x%08x,0x%08x,\n",eax,res[0],res[1]);
		}
	}
	fprintf(ofp,"};\n");

	fprintf(ofp,"unsigned int AAM_TABLE[]={\n");
	for(i=0; i<16; ++i)
	{
		for(j=0; j<16; ++j)
		{
			unsigned int res[2];
			unsigned int eax=i*16+j;
			TEST_AAM(res,eax);
			fprintf(ofp,"\t0x%08x,0x%08x,0x%08x,\n",eax,res[0],res[1]);
		}
	}
	fprintf(ofp,"};\n");
}

int main(void)
{
	FILE *ofp=fopen("cputest/testcase.h","w");
	GenImulR32xR32Test(ofp);
	GenMulR32xR32Test(ofp);
	GenBitShift(ofp);
	GenF6F7_NOT_NEG_MUL_IMUL_DIV_IDIV(ofp);
	GenF6F7_TEST_R_I(ofp);
	GenAADAAM(ofp);
	fclose(ofp);
	return 0;
}
