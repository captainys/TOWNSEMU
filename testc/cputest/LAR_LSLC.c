#include <stdio.h>

extern void TEST_LAR_LSL(
     unsigned int res[4],int opSize,unsigned int descType,unsigned int GFlag,unsigned int Pflag,unsigned int DPL,unsigned int RPL);

// void TEST_LAR_LSL(
//     unsigned int res[4],int opSize,unsigned int descType,unsigned int GFlag,unsigned int Pflag,unsigned int DPL,unsigned int RPL);
// opSize 16 or 32
// descType 0 to 1FH
// GFlag 1 or 0
// PFlag 1 or 0
// RPL 0 to 3
// DPL 0 to 3
// res[0] ZF from LSL
// res[1] Returned 32-bit reg from LSL
// res[0] ZF from LAR
// res[1] Returned 32-bit reg from LAR

// Combination:
//   opSize     descType      G      P    RPL    DPL
//     2     *    0x20     *  2  *   2  *  4   *  4=  4096

extern unsigned int sample[4096][4];

// #define SAMPLE_MODE

int main(void)
{
	unsigned int opSize=32,descType=0,G=0,P=0,RPL=0,DPL=0;
	unsigned int ctr=0,res[4];
	for(opSize=16; opSize<=32; opSize+=16)
	{
		for(descType=0; descType<0x20; ++descType)
		{
			for(G=0; G<2; ++G)
			{
				for(P=0; P<2; ++P)
				{
					for(DPL=0; DPL<4; ++DPL)
					{
						for(RPL=0; RPL<4; ++RPL)
						{
							TEST_LAR_LSL(res,opSize,descType,G,P,RPL,DPL);
						#ifdef SAMPLE_MODE
							printf("{0x%08x,0x%02x,0x%08x,0x%02x},"
							       "// [%4d] op=%d desc=0x%02x G=%d P=%d DPL=%d RPL=%d"
							       "\n",
							       res[0],res[1],res[2],res[3],
							       ctr,opSize,descType,G,P,DPL,RPL);
						#else
							if(res[0]!=sample[ctr][0] ||
							   res[1]!=sample[ctr][1] ||
							   (res[2]&0xFFF0FFFF)!=(sample[ctr][2]&0xFFF0FFFF) || // Upper four bits of the limit is undefined.
							   res[3]!=sample[ctr][3])
							{
								printf("Result does not match!\n");
								printf("[%4d] op=%d desc=0x%02x G=%d P=%d DPL=%d RPL=%d"
								       "\n",
								       ctr,opSize,descType,G,P,DPL,RPL);
								printf("VM returned: %08x %02x %08x %02x\n",res[0],res[1],res[2],res[3]);
								printf("Should be  : %08x %02x %08x %02x\n",sample[ctr][0],sample[ctr][1],sample[ctr][2],sample[ctr][3]);
								return 1;
							}
						#endif
							++ctr;

							//printf(">");
							//getchar();
						}
					}
				}
			}
		}
	}
	#ifndef SAMPLE_MODE
	printf("End.\n");
	#endif
	return 0;
}