#include <stdio.h>

extern void TEST_IMUL_R32_R32(unsigned int eax_edx_eflags[3],unsigned int eax,unsigned int edx);
extern void TEST_MUL_R32_R32(unsigned int eax_edx_eflags[3],unsigned int eax,unsigned int edx);

static unsigned int testNumberSrc16[]=
{
	0x0000,
	0x0001,
	0x0005,
	0x0010,
	0x0105,
	0x1000,
	0x7007,
	0x0100,
	0x0301,
	0x0705,
	0x0810,
	0x0A05,
	0x1C00,
	0x7E07,
	0x7FFF,
	0x8000,
	0x8707,
	0x8000,
	0x8FFF,
	0x800B,
	0xC00B,
	0xB000,
	0xA0BD,
	0xFFFF,
};
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
	0xA0000000,
	0xA00B000B,
	0xB00B000B,
	0xD00B000B,
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
static unsigned int testNumberSrc8BCD[]=
{
	0x00,
	0x01,
	0x03,
	0x07,
	0x10,
	0x11,
	0x17,
	0x21,
	0x47,
	0x49,
	0x50,
	0x51,
	0x79,
	0x80,
	0x85,
	0x90,
	0x95,
	0x97,
	0x99,
};
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

extern void TEST_IMUL_R32_R32_I8(unsigned int res[32],unsigned int EAX,unsigned int EDX);

void GetnImulR32xR32xI8Test(FILE *ofp)
{
	unsigned int res[32];
	fprintf(ofp,"unsigned int IMUL_32_32_I8_TABLE[]={\n");
	for(int i=0; i<LEN(testNumberSrc32); ++i)
	{
		TEST_IMUL_R32_R32_I8(res,i,testNumberSrc32[i]);
		fprintf(ofp,"\t0x%08x,0x%08x,\n",i,testNumberSrc32[i]);
		for(int j=0; j<32; ++j)
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
extern void TEST_F7(unsigned int res[],unsigned int eax,unsigned int edx);

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
			unsigned int eax=testNumberSrc32[i];
			unsigned int edx=testNumberSrc32[j];
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
extern void TEST_AAS(unsigned int res[],unsigned int eax);

void GenAADAAMAAS(FILE *ofp)
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

	fprintf(ofp,"unsigned int AAS_TABLE[]={\n");
	for(i=0; i<16; ++i)
	{
		for(j=0; j<16; ++j)
		{
			unsigned int res[2];
			unsigned int eax=i*16+j;
			TEST_AAS(res,eax);
			fprintf(ofp,"\t0x%08x,0x%08x,0x%08x,\n",eax,res[0],res[1]);
		}
	}
	fprintf(ofp,"};\n");
}

extern void BTX_R32_I8(unsigned int res[],unsigned int EBX);
extern void BTX_R32_R32(unsigned int res[],unsigned int EBX,unsigned int ECX);

void GenBTx(FILE *ofp)
{
	int i;
	unsigned int res[64];
	fprintf(ofp,"unsigned int BTX_I8_TABLE[]={\n");
	for(i=0; i<LEN(testNumberSrc32); ++i)
	{
		BTX_R32_I8(res,testNumberSrc32[i]);
		fprintf(ofp,"\t0x%08x,\n",testNumberSrc32[i]);
		for(int j=0; j<64; ++j)
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

	fprintf(ofp,"unsigned int BTX_R32_R32_TABLE[]={\n");
	for(i=0; i<LEN(testNumberSrc32); ++i)
	{
		for(int j=0; j<LEN(testNumberSrc32); ++j)
		{
			BTX_R32_R32(res,testNumberSrc32[i],testNumberSrc32[j]);
			fprintf(ofp,"\t0x%08x,0x%08x,\n",testNumberSrc32[i],testNumberSrc32[j]);
			for(int j=0; j<8; ++j)
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
	}
	fprintf(ofp,"};\n");
}

extern void TEST_CBW(unsigned int *res,unsigned int eax);
extern void TEST_CWDE(unsigned int *res,unsigned int eax);
extern void TEST_CWD(unsigned int res[2],unsigned int eax,unsigned int edx);
extern void TEST_CDQ(unsigned int res[2],unsigned int eax,unsigned int edx);

void GenCBW_CWDE_CWD_CDQ(FILE *ofp)
{
	int i;
	unsigned int res[2];
	fprintf(ofp,"unsigned int CBW_TABLE[]={\n");
	for(i=0; i<256; ++i)
	{
		TEST_CBW(res,i);
		fprintf(ofp,"\t0x%08x,0x%08x,\n",i,res[0]);
	}
	fprintf(ofp,"};\n");

	fprintf(ofp,"unsigned int CWDE_TABLE[]={\n");
	for(i=0; i<LEN(testNumberSrc16); ++i)
	{
		TEST_CWDE(res,testNumberSrc16[i]);
		fprintf(ofp,"\t0x%08x,0x%08x,\n",testNumberSrc16[i],res[0]);
	}
	fprintf(ofp,"};\n");

	fprintf(ofp,"unsigned int CWD_TABLE[]={\n");
	for(i=0; i<LEN(testNumberSrc16); ++i)
	{
		TEST_CWD(res,testNumberSrc16[i],testNumberSrc16[i]);
		fprintf(ofp,"\t0x%08x,0x%08x,0x%08x,\n",testNumberSrc16[i],res[0],res[1]);
	}
	fprintf(ofp,"};\n");
	fprintf(ofp,"unsigned int CDQ_TABLE[]={\n");
	for(i=0; i<LEN(testNumberSrc32); ++i)
	{
		TEST_CDQ(res,testNumberSrc32[i],testNumberSrc32[i]);
		fprintf(ofp,"\t0x%08x,0x%08x,0x%08x,\n",testNumberSrc32[i],res[0],res[1]);
	}
	fprintf(ofp,"};\n");
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

void GenADC_SBB_SpecialCaseTable(FILE *ofp)
{
	int i;
	unsigned int res[64];

	fprintf(ofp,"unsigned int ADC_SP_TABLE[]={\n");
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
		fprintf(ofp,"0x%08x,",res[i]);
	}
	fprintf(ofp,"\n");
	fprintf(ofp,"};\n");

	fprintf(ofp,"unsigned int SBB_SP_TABLE[]={\n");
	TEST_SBB_SP0(res);
	TEST_SBB_SP1(res+2);
	TEST_SBB_SP2(res+4);
	TEST_SBB_SP3(res+6);
	TEST_SBB_SP4(res+8);
	for(i=0; i<10; ++i)
	{
		fprintf(ofp,"0x%08x,",res[i]);
	}
	fprintf(ofp,"\n");
	fprintf(ofp,"};\n");
}


void TEST_ADC_SBB_32(unsigned int res[8],unsigned int eax,unsigned int ecx);
void TEST_ADC_SBB_16(unsigned int res[8],unsigned int eax,unsigned int ecx);
void TEST_ADC_SBB_8(unsigned int res[8],unsigned int eax,unsigned int ecx);

void GenADC_SBB(FILE *ofp)
{
	int i;
	fprintf(ofp,"unsigned int ADC_SBB_8_TABLE[]={\n");
	for(i=0; i<LEN(testNumberSrc8); ++i)
	{
		for(int j=0; j<LEN(testNumberSrc8); ++j)
		{
			int k;
			unsigned int eax=testNumberSrc8[i];
			unsigned int ecx=testNumberSrc8[j];
			unsigned int res[8];
			TEST_ADC_SBB_8(res,eax,ecx);
			fprintf(ofp,"\t0x%08x,0x%08x,\n",eax,ecx);
			fprintf(ofp,"\t");
			for(k=0; k<8; ++k)
			{
				fprintf(ofp,"0x%08x,",res[k]);
			}
			fprintf(ofp,"\n");
		}
	}
	fprintf(ofp,"};\n");

	fprintf(ofp,"unsigned int ADC_SBB_16_TABLE[]={\n");
	for(i=0; i<LEN(testNumberSrc16); ++i)
	{
		for(int j=0; j<LEN(testNumberSrc16); ++j)
		{
			int k;
			unsigned int eax=testNumberSrc16[i];
			unsigned int ecx=testNumberSrc16[j];
			unsigned int res[8];
			TEST_ADC_SBB_16(res,eax,ecx);
			fprintf(ofp,"\t0x%08x,0x%08x,\n",eax,ecx);
			fprintf(ofp,"\t");
			for(k=0; k<8; ++k)
			{
				fprintf(ofp,"0x%08x,",res[k]);
			}
			fprintf(ofp,"\n");
		}
	}
	fprintf(ofp,"};\n");

	fprintf(ofp,"unsigned int ADC_SBB_32_TABLE[]={\n");
	for(i=0; i<LEN(testNumberSrc32); ++i)
	{
		for(int j=0; j<LEN(testNumberSrc32); ++j)
		{
			int k;
			unsigned int eax=testNumberSrc32[i];
			unsigned int ecx=testNumberSrc32[j];
			unsigned int res[8];
			TEST_ADC_SBB_32(res,eax,ecx);
			fprintf(ofp,"\t0x%08x,0x%08x,\n",eax,ecx);
			fprintf(ofp,"\t");
			for(k=0; k<8; ++k)
			{
				fprintf(ofp,"0x%08x,",res[k]);
			}
			fprintf(ofp,"\n");
		}
	}
	fprintf(ofp,"};\n");
}

extern void BT_MEM_R32(unsigned char res[512],const unsigned char data[64]);
extern void BT_MEM_R16(unsigned char res[512],const unsigned char data[64]);

void GenBT_MEM_R(FILE *ofp)
{
	int i;
	unsigned char res[512];
	BT_MEM_R32(res,dataSrc);
	fprintf(ofp,"unsigned char BT_M_R32_TABLE[]={\n");
	for(i=0; i<512; ++i)
	{
		fprintf(ofp,"0x%02x,",res[i]);
		if(0==(i+1)%16)
		{
			fprintf(ofp,"\n");
		}
	}
	fprintf(ofp,"};\n");

	BT_MEM_R16(res,dataSrc+64);
	fprintf(ofp,"unsigned char BT_M_R16_TABLE[]={\n");
	for(i=0; i<512; ++i)
	{
		fprintf(ofp,"0x%02x,",res[i]);
		if(0==(i+1)%16)
		{
			fprintf(ofp,"\n");
		}
	}
	fprintf(ofp,"};\n");
}

extern int TEST_AAA(unsigned int eax,unsigned int edx);
extern int TEST_DAS(unsigned int eax,unsigned int edx);
extern int TEST_DAA(unsigned int eax,unsigned int edx);

void GenDAADAS(FILE *ofp)
{
	int i,j;
	fprintf(ofp,"unsigned int DAA_DAS_TABLE[]={\n");
	for(i=0; i<LEN(testNumberSrc8); ++i)
	{
		for(j=0; j<LEN(testNumberSrc8); ++j)
		{
			fprintf(ofp,"0x%02x,0x%02x,0x%04x,0x%04x,0x%04x,\n",
			    testNumberSrc8BCD[i],
			    testNumberSrc8BCD[j],
			    TEST_DAA(testNumberSrc8BCD[i],testNumberSrc8BCD[j]),
			    TEST_DAS(testNumberSrc8BCD[i],testNumberSrc8BCD[j]),
			    TEST_AAA(testNumberSrc8BCD[i],testNumberSrc8BCD[j]));
		}
	}
	fprintf(ofp,"};\n");
}


int main(void)
{
	FILE *ofp=fopen("cputest/testcase.h","w");
	GenImulR32xR32Test(ofp);
	GenMulR32xR32Test(ofp);
	GetnImulR32xR32xI8Test(ofp);
	GenBitShift(ofp);
	GenF6F7_NOT_NEG_MUL_IMUL_DIV_IDIV(ofp);
	GenF6F7_TEST_R_I(ofp);
	GenAADAAMAAS(ofp);
	GenBTx(ofp);
	GenCBW_CWDE_CWD_CDQ(ofp);
	GenADC_SBB_SpecialCaseTable(ofp);
	GenADC_SBB(ofp);
	GenBT_MEM_R(ofp);
	GenDAADAS(ofp);
	fclose(ofp);
	return 0;
}
