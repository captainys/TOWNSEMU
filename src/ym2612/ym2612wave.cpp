/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include <algorithm>
#include <iostream>
#include <cstring>
#include <cstdlib> // for std::abs

#include "ym2612.h"



inline void WordOp_Set(unsigned char *ptr,short value)
{
#ifdef YS_LITTLE_ENDIAN
	if(value<-32767)
	{
		*((short *)ptr)=-32767;
	}
	else if(32767<value)
	{
		*((short *)ptr)=32767;
	}
	else
	{
		*((short *)ptr)=value;
	}
#else
	if(value<-32767)
	{
		value=-32767;
	}
	else if(32767<value)
	{
		value=32767;
	}
	ptr[0]=value&255;
	ptr[1]=(value>>8)&255;
#endif
}

static inline int Gain(int a,int b)
{
	int c=a+b;
	if(c<-32768 || 32767<c)
	{
		return c/2;
	}
	else
	{
		return c;
	}
}


// #define YM2612_DEBUGOUTPUT


/*static*/ int YM2612::MULTITable[16]= // Multiple x2.
{
	1,
	2,
	4,
	6,
	8,
	10,
	12,
	14,
	16,
	18,
	20,
	22,
	24,
	26,
	28,
	30,
};

// Attack/Decay/Sustain/Release rate becomes double every four steps in 64-level scale.
// However between i*4 and (i+1)*4, the slope is linearly interpolated.
// Therefore, the slope should be:
//    i=N*4    a
//    i=N*4+1  a*1.25
//    i=N*4+2  a*1.5
//    i=N*4+3  a*1.75
//    i=N*4+4  a*2
//     :
//    i=N*4+8  a*4
//
// The duration of the segment is inverse of the slope, therefore:
//    i=N*4    d
//    i=N*4+1  d/1.25
//    i=N*4+2  d/1.5
//    i=N*4+3  d/1.75
//    i=N*4+4  d/2
//     :
//    i=N*4+8  d/4
//
// The duration of decay/sustain/release depends on how much dB the segment needs to drop.
// It is linear to the dB-drop.
//
// The duration of attack (based on the observation) is constant regardless of the total level.
//
// The observation indicated that the values listed in FM TOWNS Technical Databook was
// about 10% slower than actual.

static unsigned int attackTime0to96dB[64]= // 1/100ms
{
// Note: The time must be proportional to the base clock.
// Measured/Estimated    FM TOWNS Technical Databook Table I-5-31
1606000,                 // Infinity,
1284800,                 // Infinity,
1070666,                 // Infinity,
917714,                  // Infinity,
803000,                  // 897024,
642400,                  // 897024,
535333,                  // 598016,
458857,                  // 598016,
401500,                  // 448512,
321200,                  // 358810,
267666,                  // 299008,   // Actual Measurement 2677ms
229428,                  // 258293,
200750,                  // 224256,
160600,                  // 173405,
133833,                  // 149504,
114714,                  // 128146,

100375,                  // 112128,
80300,                   // 83702,
66916,                   // 74752,
57357,                   // 64073,
50187,                   // 56064,
40149,                   // 44851,
33458,                   // 37376,
28678,                   // 32037,
25093,                   // 28032,
20074,                   // 22425,
16728,                   // 18688,
14338,                   // 16018,
12546,                   // 14016,
10036,                   // 11213,
8364,                    // 9344,
7169,                    // 8009,

6273,                    // 7008,
5018,                    // 5806,
4182,                    // 4872,
3584,                    // 4005,
3136,                    // 3504,
2508,                    // 2903,
2090,                    // 2335,
1792,                    // 2002,
1568,                    // 1752,
1254,                    // 1402,
1045,                    // 1168,
896,                     // 1001,
784,                     // 876,
627,                     // 701,
522,                     // 584,
448,                     // 501,

392,                     // 438,
313,                     // 350,
261,                     // 232,
224,                     // 250,
196,                     // 240,
156,                     // 192,
130,                     // 160,
112,                     // 137,
98,                      // 126,
78,                      // 101,
65,                      // 84,
56,                      // 72,
49,                      // 60,
39,                      // 60,
32,                      // 0,
28,                      // 0,
};
static unsigned int sustainDecayReleaseTime0to96dB[64]= // 1/100ms
{
// Measured/Estimated    FM TOWNS Technical Databook Table I-5-31
22662400,               // Infinity
18129920,               // Infinity
15108266,               // Infinity
12949942,               // Infinity
11331200,               // 12398592,
9064960,                // 12398592,
7554133,                // 8265726,
6474971,                // 8265728,
5665600,                // 5199296,   // Actual measurement: 56656ms (5665600)
4532480,                // 4953437,
3777066,                // 4132864,
3237485,                // 3542455,
2832800,                // 3099648,
2266240,                // 2479719,
1888533,                // 2066432,
1618742,                // 1771227,

1416400,                // 1549824,
1133120,                // 1239859,
944266,                 // 1033215,
809371,                 // 885614,
708200,                 // 774912,
566560,                 // 619930,
472133,                 // 516608,
404685,                 // 442807,
354100,                 // 387455,
283280,                 // 309965,
236066,                 // 258304,
202342,                 // 221403,
177050,                 // 193728,
141640,                 // 154982,
118033,                 // 129152,
101171,                 // 110702,

88525,                  // 96864,
70820,                  // 77491,
59016,                  // 64576,
50585,                  // 55351,
44262,                  // 48432,
35409,                  // 38745,
29508,                  // 32298,
25292,                  // 27675,
22131,                  // 24216,
17704,                  // 19373,
14754,                  // 16144,
12646,                  // 13938,
11065,                  // 12108,
8852,                   // 9686,
7376,                   // 8072,
6322,                   // 6919,

5532,                   // 6054,
4425,                   // 4843,
3688,                   // 4056,
3161,                   // 3459,
2766,                   // 3027,
2212,                   // 2422,
1844,                   // 2018,
1580,                   // 1730,
1383,                   // 1514,
1106,                   // 1211,
922,                    // 1009,
790,                    // 865,
691,                    // 757,
552,                    // 757,
460,                    // 757,
394,                    // 757,
};

// I just took the following table from FM TOWNS Technical Databook,
// but I have no idea what's the hell this table is for.
static unsigned int attackTime10to90Percent[64]=
{
0,
0,
0,
0,
0,
468891,
333005,
333005,
249446,
200294,
166502,
141926,
124723,
100147,
83251,
70963,

62362,
50074,
41625,
35482,
31181,
25037,
20813,
17741,
15590,
12518,
10406,
8670,
7795,
6259,
5203,
4435,

3899,
3130,
2602,
2218,
1949,
1565,
1301,
1109,
974,
782,
650,
554,
497,
391,
325,
277,

241,
194,
165,
140,
125,
99,
92,
70,
61,
46,
37,
32,
26,
26,
0,
0,
};
static unsigned int sustainDecayReleaseTime10to90Percent[64]=
{
0,
0,
0,
0,
0,
2491484, // 5:
1658880, // 6:
1658880, // 7:
1247232, // 8: 2491484/2
998400,  // 9: 1996800/2
823440,  //10: 
712704,  //11:
623616,  //12: 2491484/4
498200,  //13: 1996800/4  499200?
414720,  //14: 
356352,  //15: 

311808,  //16: 2491484/8
249600,
207360,
178175,
155904,
124800,
103680,
89088,
77952,
62400,
51840,
44544,
38876,
31200,
25920,
22272,

19488,
15800,
12960,
11136,
9744,
7800,
6480,
5568,
4872,
3900,
3240,
2784,
2436,
1950,
1620,
1392,

1218,
975,
810,
696,
509,
480,
407,
350,
305,
243,
203,
173,
152,
152,
152,
152,
};

// FM TOWNS Technical Data Book Table I-5-34 pp.210
static const unsigned int SSG_EG_DecayTime0dBTo95dB[]= // 1/100ms
{
1572864, // 0
1572864,
1572864,
1572864,

1572864, // 4
1572864,
1048576,
1048576,

 786432, // 8
 629146,
 524298,
 449390,

 393216, // 12
 314573,
 262144,
 224695,

 196608, // 16
 157286,
 131072,
 112347,

  98304, // 20
  78643,
  65536,
  56174,

  49152, // 24
  39322,
  32768,
  28097,

  24576, // 28
  19661,
  16384,
  14043,

  12288, // 32
   9830,
   8192,
   7022,

   6144, // 36
   4915,
   4096,
   3511,

   3072, // 40
   2458,
   2048,
   1755,

   1536, // 44
   1229,
   1024,
    878,

    768, // 48
    614,
    512,
    439,

    384, // 52
    307,
    256,
    219,

    192, // 56
    154,
    128,
    110,

     96, // 60
     96,
     96,
     96,
};

const struct YM2612::ConnectionToOutputSlot YM2612::connectionToOutputSlots[8]=
{
	{1,{3,-1,-1,-1}},
	{1,{3,-1,-1,-1}},
	{1,{3,-1,-1,-1}},
	{1,{3,-1,-1,-1}},
	{2,{1,3,-1,-1}},
	{3,{1,2,3,-1}},
	{3,{1,2,3,-1}},
	{4,{0,1,2,3}},
};

static unsigned int detune1000Table[]=
{
	   0,   0,  48,  95,
	   0,   0,  48,  95,
	   0,   0,  48,  95,
	   0,   0,  48,  95,
	   0,  48,  95,  95,
	   0,  48,  95, 143,
	   0,  48,  95, 143,
	   0,  48,  95, 143,
	   0,  48,  95, 191,
	   0,  48, 143, 191,

	   0,  48, 143, 191,
	   0,  48, 143, 238,
	   0,  95, 191, 238,
	   0,  95, 191, 286,
	   0,  95, 191, 288,
	   0,  95, 238, 334,
	   0,  85, 238, 381, // 85?
	   0, 143, 286, 381,
	   0, 143, 298, 429,
	   0, 143, 334, 477,

	   0, 191, 381, 525,
	   0, 191, 381, 572,
	   0, 191, 429, 620,
	   0, 238, 477, 668,
	   0, 238, 525, 763,
	   0, 286, 672, 811,
	   0, 286, 520, 906,
	   0, 334, 668, 354,
	   0, 391, 763,1049,
	   0, 391, 763,1049,

	   0, 391, 763,1049,
	   0, 391, 763,1049,
};

const unsigned char YM2612::SSG_EG_PTN[8][2]=
{
	{SSGEG_DOWN,SSGEG_DOWN}, // 8
	{SSGEG_DOWN,SSGEG_ZERO}, // 9
	{SSGEG_DOWN,SSGEG_UP, }, // 10
	{SSGEG_DOWN,SSGEG_ONE,}, // 11
	{SSGEG_UP,  SSGEG_UP, }, // 12
	{SSGEG_UP,  SSGEG_ONE,}, // 13
	{SSGEG_UP  ,SSGEG_DOWN}, // 14
	{SSGEG_UP,  SSGEG_ZERO}, // 15
};


////////////////////////////////////////////////////////////

/*static*/ unsigned int YM2612::attackExp[4096];         // 0 to 4095 scale in -> 0 to 4095 scale out.
/*static*/ unsigned int YM2612::attackExpInverse[4096];  // 0 to 4095 scale in -> 0 to 4095 scale out.

////////////////////////////////////////////////////////////



static unsigned int LFOCycleMicroSec[8]=
{
	100000000/ 398, // 1000000/ 3.98,
	100000000/ 556, // 1000000/ 5.56,
	100000000/ 602, // 1000000/ 6.02,
	100000000/ 637, // 1000000/ 6.37,
	100000000/ 688, // 1000000/ 6.88,
	100000000/ 963, // 1000000/ 9.63,
	100000000/4810, // 1000000/48.10,
	100000000/7220  // 1000000/72.20
};

// Definition of cent.
// https://ja.wikipedia.org/wiki/%E3%82%BB%E3%83%B3%E3%83%88_(%E9%9F%B3%E6%A5%BD)
static int PMS16384Table[8]=
{
  0,
 32,// scale(3.400000)=1.001966, (1.001966-1.0)*16384.0=32
 63,// scale(6.700000)=1.003878, (1.003878-1.0)*16384.0=63
 94,// scale(10.000000)=1.005793, (1.005793-1.0)*16384.0=94
133,// scale(14.000000)=1.008120, (1.008120-1.0)*16384.0=133
190,// scale(20.000000)=1.011619, (1.011619-1.0)*16384.0=190
382,// scale(40.000000)=1.023374, (1.023374-1.0)*16384.0=382
774,// scale(80.000000)=1.047294, (1.047294-1.0)*16384.0=774
};

// dB=20log10(P/Pref)
// 0=20log10(P/Pref) -> P/Pref=1.0
// 1.4=20log10(P/Pref) -> 10^(1.4/20)=P/Pref -> 
// 5.9=20log10(P/Pref) -> 10^(5.9/20)=P/Pref ->
// 11.8=20log10(P/Pref) -> 10^(11.8/20)=P/Pref ->
static int AMS4096Table[4]=
{
 4096-4096,  // 1.0
 4812-4096,  // 1.1749
 8079-4096,  // 1.97242
15935-4096, // 3.89045
};

////////////////////////////////////////////////////////////

int YM2612::Slot::DetuneContributionToPhaseStepS12(unsigned int BLOCK,unsigned int NOTE) const
{
	// Hz ranges 1 to roughly 8000.  PHASE_STEPS=4096.  hertz*PHASE_STEPS=2^13*2^12=2^25. Fits in 32 bit.
	long long int detuneStepContribution=0;
	if(0!=DT)
	{
		long long int hertz1000=detune1000Table[(BLOCK<<4)+(NOTE<<2)+(DT&3)];
		detuneStepContribution=(hertz1000*PHASE_STEPS);
		detuneStepContribution<<=12;
		detuneStepContribution/=1000;  // Now it is (hertz*PHASE_STEPS)<<12.
		if(0!=(DT&4))
		{
			detuneStepContribution=-detuneStepContribution;
		}
	}
	return (int)detuneStepContribution;
}

inline int YM2612::Slot::UnscaledOutput(int phase,int phaseShift) const
{
	// phaseShift is input from the upstream slot.
	// -4096 to 4096.  4096 should be counted as 8pi.
	// UNSCALED_MAX is 2048. therefore 8x.

	//phaseShift*=MULTITable[this->MULTI];
	//phaseShift>>=1;

	//                     8.0       * (2PI / 2)     /   1.0
	const int outputScale=SLOTOUT_TO_NPI*(PHASE_STEPS/2)/UNSCALED_MAX;
	return sineTable[(phase+(phaseShift*outputScale))&PHASE_MASK];
}
inline int YM2612::Slot::UnscaledOutput(int phase,int phaseShift,unsigned int FB,int lastSlot0Out) const
{
	if(0!=FB)
	{
		static const int FBScaleTable[]=
		{
			0,1,2,4,8,16,32,64
		};
		// lastSlotOut=1.0=>UNSCALED_MAX   4096=>2PI
		// To make it 4PI(8192) at FB=7(scale=64), must divide by UNSCALED_MAX*64/8192.
		// What was I thinking when I made div=32?  It should be 16.
		const int div=UNSCALED_MAX*64/8192;
		phase+=(lastSlot0Out*FBScaleTable[FB]/div);
	}
	//                     8.0       * (2PI / 2)     /   1.0
	const int outputScale=SLOTOUT_TO_NPI*(PHASE_STEPS/2)/UNSCALED_MAX;
	return sineTable[(phase+(phaseShift*outputScale))&PHASE_MASK];
}
inline int YM2612::Slot::EnvelopedOutputDbToAmpl(int phase,int phaseShift,unsigned int envTime,unsigned int FB,int lastSlot0Out) const
{
	int dB=InterpolateEnvelope(envTime);
	lastDbX100Cache=dB;
	int ampl=DB100to4095Scale[dB];
	int unscaledOut=UnscaledOutput(phase,phaseShift,FB,lastSlot0Out);
	return (unscaledOut*ampl)/4096;
}
inline int YM2612::Slot::EnvelopedOutputDbToAmpl(int phase,int phaseShift,unsigned int envTime) const
{
	int dB=InterpolateEnvelope(envTime);
	lastDbX100Cache=dB;
	int ampl=DB100to4095Scale[dB];
	int unscaledOut=UnscaledOutput(phase,phaseShift);
	return (unscaledOut*ampl)/4096;
}
inline int YM2612::Slot::InterpolateEnvelope(unsigned int envTime) const
{
	if(true!=InReleasePhase)
	{
		if(0!=(SSG_EG&8))
		{
			unsigned int timeTotal=env[0]+env[2]+env[4]+env[6]+env[8]+env[10];
			if(0==timeTotal)
			{
				return 0;
			}
			if(timeTotal<=envTime && 0!=(SSG_EG&1)) // 9,11,13,14 keep the last output.  No repeat.
			{
				return env[11];
			}
			envTime=envTime%timeTotal;
			unsigned int prev=0;
			for(int i=0; i<12; i+=2)
			{
				if(envTime<env[i])
				{
					int64_t y0=prev;
					int64_t dY=env[i+1];
					dY-=prev;
					int64_t W=env[i];
					int64_t w=envTime;
					return (int)y0+MulDiv(dY,w,W);
				}
				envTime-=env[i];
				prev=env[i+1];
			}
			return env[11];
		}

		if(envTime<env[0]) // Attack
		{
			unsigned int x=4096*envTime/env[0];
			return (env[1]*attackExp[x])>>12;
		}
		else
		{
			envTime-=env[0];
			if(envTime<env[2])
			{
				return env[1]-(env[1]-env[3])*envTime/env[2];
			}
			else
			{
				envTime-=env[2];
				if(envTime<env[4])
				{
					return env[3]-env[3]*envTime/env[4];
				}
			}
		}
		return 0;
	}
	else
	{
		if(envTime<ReleaseEndTime && ReleaseStartTime<ReleaseEndTime)
		{
			return MulDivU(ReleaseStartDbX100,ReleaseEndTime-envTime,ReleaseEndTime-ReleaseStartTime);
		}
		return 0; // Not supported yet.
	}
}
inline unsigned int YM2612::Slot::MulDivU(unsigned int c,unsigned int numer,unsigned int denom) const
{
	uint64_t C=c;
	uint64_t N=numer;
	uint64_t D=denom;
	return (unsigned int)(C*N/D);
}
inline int YM2612::Slot::MulDiv(int c,int numer,int denom) const
{
	int64_t C=c;
	int64_t N=numer;
	int64_t D=denom;
	return (int)(C*N/D);
}

unsigned int YM2612::Channel::Note(void) const
{
	// Formula [2] pp.204
	// There is an error.  F_NUM is 11bits.  There is no F11.
	// Probably, F11, F10, F9, F8 should be read bit10, bit9, bit8, bit7.
	unsigned int F10=((F_NUM>>10)&1);
	unsigned int F9= ((F_NUM>> 9)&1);
	unsigned int F8= ((F_NUM>> 8)&1);
	unsigned int F7=((F_NUM>>11)&1);
	unsigned int N3=(F10&(F9|F8|F7)); // |((~F10)&F9&F8&F7);  Measurement from actual FM TOWNS 2MX suggests no contribution from the second term.
	unsigned int NOTE=(F10<<1)|N3;
	return NOTE;
}

unsigned int YM2612::Channel::KC(unsigned int BLOCK,unsigned int F_NUM)
{
	// Formulat in [2] pp.204 suggests:
 	//   unsigned int KC=(ch.BLOCK<<2)|NOTE;
	// which doesn't make sense.
	// SEGA Genesis Software Manaual tells KC is just top 5 bits of BLOCK|F_NUM2.
	// Which makes more sense.

	// Measurement taken from FM TOWNS 2MX gave a conclusion:
	//   NOTE=(N4<<1)|N3
	//   N4=F11
	//   N3=F11&(F10|F9|F8)
	// F11 Highest bit of F_NUMBER
	// F10 Second highest bit of F_NUMBER
	// F9  Third highest bit of F_NUMBER
	// F8  Fourth highest bit of F_NUMBER

	// YAMAHA official manual for YM2608 Section 2-4-1-A, and FM Towns Technical Databook suggests:
	//    N3=(F11&(F10|F9|F8))|((~F11)&F10&F9&F8)
	// However, the actual YM2612 didn't show any contribution from ((~F11)&F10&F9&F8).

	unsigned int N4=((F_NUM>>9)&2);
	unsigned int N3=((F_NUM>>10)&((F_NUM>>9)|(F_NUM>>8)|(F_NUM>>7)))&1;
	unsigned int KC=(BLOCK<<2)|N4|N3;
	return KC&0x1F;
}

unsigned int YM2612::Channel::KC(void) const
{
	return KC(BLOCK,F_NUM);
}

////////////////////////////////////////////////////////////

void YM2612::KeyOn(unsigned int chNum,unsigned int slotFlags)
{
	if(0==slotFlags)
	{
		return;
	}


	auto &ch=state.channels[chNum];

	const unsigned int hertzX16=BLOCK_FNUM_to_FreqX16(ch.BLOCK,ch.F_NUM);
	const unsigned int chKC=ch.KC();

	state.playingCh|=(1<<chNum);
	ch.playState=CH_PLAYING;
	if(0!=(1&slotFlags))
	{
		ch.lastSlot0Out[0]=0;
		ch.lastSlot0Out[1]=0;
	}

	unsigned int slotHertzX16[NUM_SLOTS]=
	{
		hertzX16,hertzX16,hertzX16,hertzX16,
	};
	unsigned int slotKC[NUM_SLOTS]=
	{
		chKC,chKC,chKC,chKC
	};

	// Oh, damn it!  Why 3ch f-numbers and slots are shuffled around?
	// FM Towns Technical Data Book pp. 212, Table I-5-38
	//   Reg                    Ch  SLot
	//   A2H   F-Number 1        3   4     F_NUM       ->Slot[3]
	//   A6H   Block|F-Number 2  3   4
	//   A8H   F-Number 1        3   3     F_NUM_3CH[0]->Slot[2]
	//   A9H   F-Number 1        3   1     F_NUM_3CH[1]->Slot[0]
	//   AAH   F-Number 1        3   2     F_NUM_3CH[2]->Slot[1]
	//   ACH   Block|F-Number 2  3   3
	//   ACH   Block|F-Number 2  3   1
	//   ACH   Block|F-Number 2  3   2
	// Also confirmed by YM2608J OPNA Application Manual section 2-2-2, table 2-2.
	if(2==chNum && MODE_NONE!=GetChannel3Mode())
	{
		CalculateHertzX16Channel3SpecialMode(slotHertzX16,hertzX16);
		slotKC[0]=CalculateSlotKCChannel3SpecialMode(1);
		slotKC[1]=CalculateSlotKCChannel3SpecialMode(2);
		slotKC[2]=CalculateSlotKCChannel3SpecialMode(0);
		slotKC[3]=chKC;
	}

	for(int i=0; i<NUM_SLOTS; ++i)
	{
		if(0!=(slotFlags&(1<<i)))
		{
			auto &slot=ch.slots[i];

			slot.InReleasePhase=false;
			if(MODE_CSM!=GetChannel3Mode())
			{
				slot.phaseS12=0;
			}

			UpdatePhase12StepSlot(slot,slotHertzX16[i],slot.DetuneContributionToPhaseStepS12(ch.BLOCK,ch.Note()));

			// (hertzX16*PHASE_STEPS)<<8==hertz*PHASE_STEPS*4096
			UpdateSlotEnvelope(ch,slot,slotKC[i]);

			// Observation tells that if key is turned on while the previous tone is still playing, 
			// The initial output level must start from the last output level, in which case
			// microsec12 must fast-forwarded so that the output matches the lastDbX100Cache.
			// Linear interpolation will have error, but should be better than nothing.
			if(0!=(slot.SSG_EG&8) || slot.lastDbX100Cache<=0)
			{
				slot.microsecS12=0;
			}
			else if(slot.env[1]<=slot.lastDbX100Cache)
			{
				slot.microsecS12=(slot.env[0]<<(12+10-ENVELOPE_PRECISION_SHIFT));
			}
			else if(0!=slot.env[1])
			{
				unsigned int scale=attackExpInverse[4096*slot.lastDbX100Cache/slot.env[1]];
				slot.microsecS12=(slot.env[0]*scale/4096)<<(12+10-ENVELOPE_PRECISION_SHIFT);
			}
			else
			{
				slot.microsecS12=0;
			}
			slot.lastDbX100Cache=0;
		}
	}

#ifdef YM2612_DEBUGOUTPUT
	printf("%d BLOCK %03xH F_NUM %03xH Hertz %d\n",KC,ch.BLOCK,ch.F_NUM,hertzX16/16);
#endif
}

void YM2612::CalculateHertzX16Channel3SpecialMode(unsigned int slotHertzX16[],unsigned int hertzX16Default) const
{
	slotHertzX16[0]=BLOCK_FNUM_to_FreqX16(state.BLOCK_3CH[1],state.F_NUM_3CH[1]);
	slotHertzX16[1]=BLOCK_FNUM_to_FreqX16(state.BLOCK_3CH[2],state.F_NUM_3CH[2]);
	slotHertzX16[2]=BLOCK_FNUM_to_FreqX16(state.BLOCK_3CH[0],state.F_NUM_3CH[0]);
	slotHertzX16[3]=hertzX16Default;
}

unsigned int YM2612::CalculateSlotKCChannel3SpecialMode(unsigned int slotNum) const
{
	unsigned int BLOCK,F_NUM;
	switch(slotNum)
	{
	case 0:
		BLOCK=state.BLOCK_3CH[1];
		F_NUM=state.F_NUM_3CH[1];
		break;
	case 1:
		BLOCK=state.BLOCK_3CH[2];
		F_NUM=state.F_NUM_3CH[2];
		break;
	case 2:
		BLOCK=state.BLOCK_3CH[0];
		F_NUM=state.F_NUM_3CH[0];
		break;
	default:
		BLOCK=state.channels[2].BLOCK;
		F_NUM=state.channels[2].F_NUM;
		break;
	}
	return Channel::KC(BLOCK,F_NUM);
}

void YM2612::UpdatePhase12StepSlot(Slot &slot,const unsigned int hertzX16,int detuneContribution)
{
	// Phase runs hertz*PHASE_STEPS times per second.
	//            hertz*PHASE_STEPS/WAVE_SAMPLING_RATE times per step.
	// Phase 12 runs
	//            0x1000*hertz*PHASE_STEPS/WAVE_SAMPLING_RATE per step.
	unsigned long long phaseS12Step;
	phaseS12Step=MULTITable[slot.MULTI]*hertzX16*PHASE_STEPS; // 2X from MULTITable, 16X from hertzX16
	phaseS12Step<<=7;                                         // 128X  Overall 2x16x128=4096X
	phaseS12Step+=MULTITable[slot.MULTI]*detuneContribution/2;
	phaseS12Step/=WAVE_SAMPLING_RATE;
	slot.phaseS12Step=(unsigned int)phaseS12Step;
}

void YM2612::UpdatePhase12StepSlot(Channel &ch)
{
	const unsigned int hertzX16=BLOCK_FNUM_to_FreqX16(ch.BLOCK,ch.F_NUM);
	unsigned int slotHertzX16[NUM_SLOTS]=
	{
		hertzX16,hertzX16,hertzX16,hertzX16,
	};

	if(&ch==&state.channels[2] && MODE_NONE!=GetChannel3Mode())
	{
		CalculateHertzX16Channel3SpecialMode(slotHertzX16,hertzX16);
	}

	for(int i=0; i<NUM_SLOTS; ++i)
	{
		UpdatePhase12StepSlot(ch.slots[i],slotHertzX16[i],ch.slots[i].DetuneContributionToPhaseStepS12(ch.BLOCK,ch.Note()));
	};
}

void YM2612::KeyOff(unsigned int chNum,unsigned int slotFlags)
{
	if(0!=(state.playingCh&(1<<chNum)))
	{
		auto &ch=state.channels[chNum];
		for(auto &slot : ch.slots)
		{
			if(true!=slot.InReleasePhase)
			{
				slot.InReleasePhase=true;
				UpdateRelease(ch,slot);
			}
		}
	}
}

void YM2612::CheckToneDone(unsigned int chNum)
{
	auto &ch=state.channels[chNum];
	if(CH_PLAYING!=ch.playState)
	{
		state.playingCh&=~(1<<chNum);
	}
	else
	{
		bool slotStillPlaying=false;
		for(int i=0; i<connectionToOutputSlots[ch.CONNECT].nOutputSlots; ++i)
		{
			auto &slot=ch.slots[connectionToOutputSlots[ch.CONNECT].slots[i]];
			auto envTime=(slot.microsecS12>>(10+12-ENVELOPE_PRECISION_SHIFT));
			if(true==slot.InReleasePhase && envTime<slot.ReleaseEndTime)
			{
				slotStillPlaying=true;
				break;;
			}
			else if(true!=slot.InReleasePhase && envTime<slot.envDurationCache)
			{
				slotStillPlaying=true;
				break;;
			}
		}
		if(true!=slotStillPlaying)
		{
			state.playingCh&=~(1<<chNum);
			ch.playState=CH_IDLE;
			for(auto &s : ch.slots)
			{
				s.lastDbX100Cache=0;
				s.InReleasePhase=false;
			}
		}
	}
}

void YM2612::CheckToneDoneAllChannels(void)
{
	for(unsigned int chNum=0; chNum<NUM_CHANNELS; ++chNum)
	{
		CheckToneDone(chNum);
	}
}

std::vector <unsigned char> YM2612::MakeWaveAllChannels(unsigned long long int millisec) const
{
	std::vector <unsigned char> wave;

	unsigned long long int numSamples=(millisec<<12);
	numSamples*=WAVE_SAMPLING_RATE;
	numSamples/=1000;
	numSamples>>=12;

	wave.resize(4*numSamples);

	unsigned int nPlayingCh=0;
	unsigned int playingCh[NUM_CHANNELS];
	for(unsigned int chNum=0; chNum<NUM_CHANNELS; ++chNum)
	{
		if(0!=(state.playingCh&(1<<chNum)))
		{
			playingCh[nPlayingCh++]=chNum;
		}
	}
	MakeWaveForNSamples(wave.data(),nPlayingCh,playingCh,numSamples);
	return wave;
}

std::vector <unsigned char> YM2612::MakeWave(unsigned int ch,unsigned long long int millisec) const
{
	std::vector <unsigned char> wave;

	unsigned long long int numSamples=(millisec<<12);
	numSamples*=WAVE_SAMPLING_RATE;
	numSamples/=1000;
	numSamples>>=12;

	wave.resize(4*numSamples);

	unsigned int playingCh[1]={ch};
	MakeWaveForNSamples(wave.data(),1,playingCh,numSamples);
	return wave;
}

class YM2612::WithLFO
{
public:
	static inline void CalculateLFO(int AMSAdjustment[4],int PMSAdjustment[4],unsigned int FREQCTRL,const Channel &ch)
	{
		if(0!=ch.PMS)
		{
			for(unsigned int i=0; i<connectionToOutputSlots[ch.CONNECT].nOutputSlots; ++i)
			{
				auto sl=connectionToOutputSlots[ch.CONNECT].slots[i];
				int signedStep=ch.slots[sl].phaseS12Step;

				unsigned long long int LFOPhase=(ch.slots[sl].microsecS12>>12);
				LFOPhase=LFOPhase*PHASE_STEPS/LFOCycleMicroSec[FREQCTRL];

				int PMSAdj=PMS16384Table[ch.PMS]*sineTable[LFOPhase&PHASE_MASK]/UNSCALED_MAX;
				PMSAdjustment[sl]=signedStep*PMSAdj/16384/2;
			}
		}
		{
			for(unsigned int i=0; i<connectionToOutputSlots[ch.CONNECT].nOutputSlots; ++i)
			{
				auto sl=connectionToOutputSlots[ch.CONNECT].slots[i];
				if(0!=ch.slots[sl].AM)
				{
					unsigned long long int LFOPhase=(ch.slots[sl].microsecS12>>12);
					LFOPhase=LFOPhase*PHASE_STEPS/LFOCycleMicroSec[FREQCTRL];
					AMSAdjustment[sl]=4096+(AMS4096Table[ch.AMS]*sineTable[LFOPhase&PHASE_MASK])/UNSCALED_MAX;
				}
			}
		}
	}
	static inline const int AMSMul(const int AMS)
	{
		return AMS;
	}
	static inline constexpr int AMSDiv(void)
	{
		return 4096;
	}
};

class YM2612::WithoutLFO
{
public:
	static inline void CalculateLFO(int AMSAdjustment[4],int PMSAdjustment[4],unsigned int FREQCTRL,const Channel &ch)
	{
	}
	static inline constexpr int AMSMul(const int)
	{
		return 1;
	}
	static inline constexpr int AMSDiv(void)
	{
		return 1;
	}
};

template <class LFOClass>
long long int YM2612::MakeWaveForNSamplesTemplate(unsigned char wave[],unsigned int nPlayingCh,unsigned int playingCh[],unsigned long long int numSamples) const
{
	const unsigned int microsecS12Step=4096000000/WAVE_SAMPLING_RATE;
	// Time runs 1/WAVE_SAMPLING_RATE seconds per step
	//           1000/WAVE_SAMPLING_RATE milliseconds per step
	//           1000000/WAVE_SAMPLING_RATE microseconds per step
	//           1000000000/WAVE_SAMPLING_RATE nanoseconds per step

	// If microSec12=4096*microseconds, tm runs
	//           4096000000/WAVE_SAMPLING_RATE per step

	unsigned int LeftANDPtn[NUM_CHANNELS];
	unsigned int RightANDPtn[NUM_CHANNELS];

	for(unsigned int chNum=0; chNum<NUM_CHANNELS; ++chNum)
	{
		auto &ch=state.channels[chNum];
		LeftANDPtn[chNum]=(0!=ch.L ? ~0 : 0);
		RightANDPtn[chNum]=(0!=ch.R ? ~0 : 0);
		ch.lastAmplitudeMax=0;
	}

	unsigned int i;
	for(i=0; i<numSamples && 0<nPlayingCh; ++i)
	{
		int leftOut=0,rightOut=0;
		for(int j=nPlayingCh-1; 0<=j; --j)
		{
			auto chNum=playingCh[j];
			auto &ch=state.channels[chNum];
			if(ch.slots[0].toneDurationMicrosecS12<=ch.slots[0].microsecS12 &&
			   ch.slots[1].toneDurationMicrosecS12<=ch.slots[1].microsecS12 &&
			   ch.slots[2].toneDurationMicrosecS12<=ch.slots[2].microsecS12 &&
			   ch.slots[3].toneDurationMicrosecS12<=ch.slots[3].microsecS12)
			{
				playingCh[j]=playingCh[nPlayingCh-1];
				--nPlayingCh;
				break;
			}

			int PMSAdjustment[4]=
			{
				0,0,0,0
			};
			int AMSAdjustment[4]=
			{
				4096,4096,4096,4096
			};

			LFOClass::CalculateLFO(AMSAdjustment,PMSAdjustment,state.FREQCTRL,ch);


			// Why take an average of last two samples, not just the last sample, for feedback?
			// Cisc's FMGEN YM emulator does it.  I was wondering why.  But, it works as a damper to prevent
			// premature divergence.
			// 
			// When feedback is given, the output from slot 0 is calculated as:
			// 
			//     y(i+1)=A*sin(i*dt+C*y(i))
			// 
			// A is amplitude calculated from the envelope, and C is defined by feedback level, and
			// dt depends on the frequency of the tone.
			// 
			// Let's denote the angle given to the sine function as:
			// 
			//     X(i)=i*dt+C*y(i)
			// 
			// When the slope of sin(X(i)) is negative (0.5PI<X(i)<1.5PI), means the value is decreasing with
			// increasing X, the function can become unstable and diverge, until the function returns to the
			// positive slope.  Here's what can happen.
			// 
			// X(i) monotonicly increase if C=0, means no feedback.  However, if C is non-zero, i*dt increases, 
			// but C*y(i) can increase or decrease depending on the slope of y(i).
			// 
			// If the one-step decrease of C*y(i) exceeds the increase of i*dt (which is dt), 
			// X(i) will decrease overall, which means the input to the sine functions goes backward.
			// Amplitude is positive, and if the slope of the sine function at X(i) was negative, and if 
			// the input goes backward, y suddenly increases. i.e., y(i+1) is greater than y(i).
			// 
			// Then in the next step, both C*y and i*dt terms increase.  This makes a large jump of X.
			// The sine function is going down with X, as a result y dives down bigger than the last 
			// increase.  Then, the next comes even bigger increase of y, followed by even bigger dive, 
			// and the function of y(i) starts oscillating every sample.
			// 
			// From the above, the condition that starts this divergence is:
			// 
			//     dt<-dY
			// 
			// where Y=C*y(i).  Interestingly, because of this condition, this particular mode of oscillation
			// only appears when dY<0.
			// 
			// While it is a legitimate divergence that adds some noise component to the tone in some settings,
			// Slot 0 starts this oscillation too easily.
			// 
			// Cisc's FMGEN YM-chip emulator, used in XM7 and M88 emulators (and maybe other emulators as well)
			// solves this problem by feeding the average of the last two samples back to slot 0.
			// 
			// During the oscillation, the value of y jumps up and down.  But, if you look at the middle of the two
			// consecutive outputs, it goes through a smooth decreasing curve.
			// 
			// The cause of this divergence is from the oscillation of the feedback term, C*y.  By taking average
			// of the last two samples, it essentially damps the oscillation, and effectively prevents this divergence.
			//
			// So the modified formulation is:
			//
			//     y(i+1)=A*sin(i*dt+C*(y(i)+y(i-1))/2)
			//
			// Here I am explicitly writing it out, but Cisc's implementation embedded this division by two
			// in the anyway-required bit shift.
			//
			// Very genious solution it is.

			auto s0Out=(ch.lastSlot0Out[1]+ch.lastSlot0Out[0])/2;
			const uint64_t microsecS12[4]=
			{
				ch.slots[0].microsecS12,
				ch.slots[1].microsecS12,
				ch.slots[2].microsecS12,
				ch.slots[3].microsecS12,
			};
			const unsigned int phaseS12[4]=
			{
				ch.slots[0].phaseS12,
				ch.slots[1].phaseS12,
				ch.slots[2].phaseS12,
				ch.slots[3].phaseS12,
			};
			auto ampl=CalculateAmplitude<LFOClass>(chNum,microsecS12,phaseS12,AMSAdjustment,s0Out);
			ch.lastSlot0Out[1]=ch.lastSlot0Out[0];
			ch.lastSlot0Out[0]=s0Out;

			ch.lastAmplitudeMax=std::max(ch.lastAmplitudeMax,std::abs(ampl));

			leftOut=Gain(leftOut,(LeftANDPtn[chNum]&ampl));
			rightOut=Gain(rightOut,(RightANDPtn[chNum]&ampl));

			ch.slots[0].phaseS12+=ch.slots[0].phaseS12Step+PMSAdjustment[0];
			ch.slots[1].phaseS12+=ch.slots[1].phaseS12Step+PMSAdjustment[1];
			ch.slots[2].phaseS12+=ch.slots[2].phaseS12Step+PMSAdjustment[2];
			ch.slots[3].phaseS12+=ch.slots[3].phaseS12Step+PMSAdjustment[3];
			ch.slots[0].microsecS12+=microsecS12Step;
			ch.slots[1].microsecS12+=microsecS12Step;
			ch.slots[2].microsecS12+=microsecS12Step;
			ch.slots[3].microsecS12+=microsecS12Step;
		}
		WordOp_Set(wave+i*4  ,leftOut);
		WordOp_Set(wave+i*4+2,rightOut);
	}

	std::memset(wave+i*4,0,(numSamples-i)*4);

// std::cout << (microsec12>>12) << "us " << std::endl;
// std::cout << phase12[0] << "," << (phase12[0]>>12)/PHASE_STEPS << "cycles" << std::endl;
// std::cout << phase12[1] << "," << (phase12[1]>>12)/PHASE_STEPS << "cycles" << std::endl;
// std::cout << phase12[2] << "," << (phase12[2]>>12)/PHASE_STEPS << "cycles" << std::endl;
// std::cout << phase12[3] << "," << (phase12[3]>>12)/PHASE_STEPS << "cycles" << std::endl;

	return i;
}

long long int YM2612::MakeWaveForNSamples(unsigned char wavBuf[],unsigned long long int numSamplesRequested) const
{
	unsigned int nPlayingCh=0;
	unsigned int playingCh[NUM_CHANNELS];
	for(unsigned int chNum=0; chNum<NUM_CHANNELS; ++chNum)
	{
		if(0!=(state.playingCh&(1<<chNum)))
		{
			playingCh[nPlayingCh++]=chNum;
		}
	}
	return MakeWaveForNSamples(wavBuf,nPlayingCh,playingCh,numSamplesRequested);
}

long long int YM2612::MakeWaveForNSamples(unsigned char wave[],unsigned int nPlayingCh,unsigned int playingCh[],unsigned long long int numSamples) const
{
	if(true==state.LFO)
	{
		return MakeWaveForNSamplesTemplate <WithLFO> (wave,nPlayingCh,playingCh,numSamples);
	}
	else
	{
		return MakeWaveForNSamplesTemplate <WithoutLFO> (wave,nPlayingCh,playingCh,numSamples);
	}
}

void YM2612::UpdateSlotEnvelope(unsigned int chNum,unsigned int slotNum)
{
	auto &ch=state.channels[chNum];
	unsigned int KC;
	if(2==chNum && MODE_NONE!=GetChannel3Mode())
	{
		KC=CalculateSlotKCChannel3SpecialMode(slotNum);
	}
	else
	{
		KC=ch.KC();
	}
	UpdateSlotEnvelope(ch,ch.slots[slotNum],KC);
}

void YM2612::UpdateSlotEnvelope(const Channel &ch,Slot &slot,unsigned int KC)
{
	CalculateEnvelope(slot.env,KC,slot);
	slot.envDurationCache=slot.env[0]+slot.env[2]+slot.env[4]+slot.env[6]+slot.env[8]+slot.env[10];
	slot.toneDurationMicrosecS12=slot.envDurationCache;  // In Microsec/1024
	slot.toneDurationMicrosecS12<<=(12+10-ENVELOPE_PRECISION_SHIFT);
}

void YM2612::UpdateRelease(const Channel &ch,Slot &slot)
{
	auto KC=ch.KC();
	auto RR=(slot.RR*2+1)*2+(KC>>(3-slot.KS));  // [2] pp.206 Double RR and add 1.

	// 2020/12/15 nextMicrosecS12 retains up to what time point wave has been generated.
	//            Therefore, here must be nextMicrosecS12, instead of microsecS12.
	//            If microsecS12 is used, it virtually skips first 20ms of release,
	//            and the amplitude drops like a stairstep.
	//            It is inaudible in many situations, but clearly audible in Super Daisenryaku opening.
	// 2020/12/23 Got rid of nextMicrosecS12.
	slot.ReleaseStartTime=(slot.microsecS12>>(12+10-ENVELOPE_PRECISION_SHIFT));
	slot.ReleaseStartDbX100=slot.lastDbX100Cache;

	// Strike Commander set TL to 126, and then key off after landing and full stop.
	// Since no wave is calculated between TL=126 and key off, 
	// it creates a situation in which last output level is greater than the peak output level.
	// Therefore in such a situation, ReleaseStartDbX100 should be made at least less than TL.
	if(slot.env[1]<slot.ReleaseStartDbX100)
	{
		slot.ReleaseStartDbX100=slot.env[1];
	}


	uint64_t releaseTime=sustainDecayReleaseTime0to96dB[std::min<unsigned int>(RR,63)];
	releaseTime*=slot.lastDbX100Cache;
	releaseTime/=((9600*1024/10)/ENVELOPE_PRECISION);
	slot.ReleaseEndTime=slot.ReleaseStartTime+releaseTime;
#ifdef YM2612_DEBUGOUTPUT
	std::cout << "Release Time " << releaseTime << "ms  " << 
	             "Start " << slot.ReleaseStartTime << "ms  " << 
	             "End " << slot.ReleaseEndTime << "ms" << std::endl;
#endif
}

bool YM2612::CalculateEnvelope(unsigned int env[12],unsigned int KC,const Slot &slot) const
{
#ifdef YM2612_DEBUGOUTPUT
	std::cout << KC << "," << slot.KS << "," << (KC>>(3-slot.KS)) << ", ";
#endif

	unsigned int AR=slot.AR*2+(KC>>(3-slot.KS));
	unsigned int DR=slot.DR*2+(KC>>(3-slot.KS));
	unsigned int SR=slot.SR*2+(KC>>(3-slot.KS));
	AR=std::min(AR,63U);
	DR=std::min(DR,63U);
	SR=std::min(SR,63U);

	if(AR<4)
	{
		return NoTone(env);
	}

	if(0!=(slot.SSG_EG&8))
	{
		return CalculateEnvelopeSSG_EG(env,KC,slot);
	}

	auto TLdB100=TLtoDB100[slot.TL];
	auto SLdB100=SLtoDB100[slot.SL];

	if(9600<=TLdB100)
	{
		return NoTone(env);
	}

	const unsigned int TLinv=9600-TLdB100;

#ifdef YM2612_DEBUGOUTPUT
	std::cout << "AR=" << AR << " DR=" << DR << " SR=" << SR << " TL=" << slot.TL  << " SL=" << slot.SL ;
	std::cout << " ";
#endif

	// Ealier I was linearly interpolating the amplitude, but maybe it is linear in dB scale.
	env[1]=TLinv;
	env[3]=(SLdB100<TLinv ? TLinv-SLdB100 : 0);
	env[5]=0;


	// After reading fmgen.c (Written by cisc, Author of M88 PC8801 emulator), it looks to be that
	// the time for attack doesn't depend on the total level, but it takes time to raise 0dB to 96dB all the time.
	// Then, the time for decay is based on SL only.  Just like dropping from 96dB to 96-dB(SL) dB.
	// The secondary decay duration should also depend only on SL, the time for 96-dB(SL)dB to 0dB.
	// The amplitude change is not linear, but I approximate by a linear function.  I may come back to the envelope
	// generation once I get a good enough approximation.
	unsigned long long int mul;
	env[0]=(attackTime0to96dB[AR]*10)>>(10-ENVELOPE_PRECISION_SHIFT);  // *10 to make it microsec, and then divide by 1024.
	mul=SLdB100;
	mul*=sustainDecayReleaseTime0to96dB[DR];
	mul/=((9600*1024/10)/ENVELOPE_PRECISION);
	env[2]=(unsigned int)mul;
	mul=9600-SLdB100;
	mul*=sustainDecayReleaseTime0to96dB[SR];
	mul/=((9600*1024/10)/ENVELOPE_PRECISION);
	env[4]=(unsigned int)mul;

	env[6]=0;
	env[7]=0;
	env[8]=0;
	env[9]=0;
	env[10]=0;
	env[11]=0;

	// ?
	// If, AR, DR, SR, and RR are really rates, the duration for attack, decay, and sustain should depend
	// on the amplitude.  If the amplitude for total level is high, it should take longer to get to the level.
	// Or, if it is the rate, the slope of the decay should be the same regardless of the TL amplitude.
	// But, the value calculated from this assumption doesn't make sense at all.
	// It rather makes sense if I take the number from the table without scaling by the amplitude.

	// env[0]=attackTime0to96dB[AR]/100;
	// env[2]=sustainDecayReleaseTime0to96dB[DR]/100;
	// env[4]=sustainDecayReleaseTime0to96dB[SR]/100;

	// ?
	// If it is really rate, the following code should better emulate, but doesn't look to be.
	// unsigned long long int attackTime=attackTime0to96dB[AR]; // 1/100 milliseconds for jumping from 0 to 127 (96dB)
	// // If TLampl==127, it takes attackTime/100 milliseconds to reach TLampl.
	// attackTime*=TLampl;
	// env[0]=(unsigned int)(attackTime/12700);

	// unsigned long long int decayTime=sustainDecayReleaseTime0to96dB[DR];
	// decayTime*=(TLampl-SLampl);
	// env[2]=(unsigned int)(decayTime/12700);

	// unsigned long long int sustainTime=sustainDecayReleaseTime0to96dB[SR];
	// sustainTime*=SLampl;
	// sustainTime/=127;
	// if(sustainTime<TONE_CHOPOFF_MILLISEC*100)
	// {
	// 	env[4]=(unsigned int)(sustainTime/12700);
	// }
	// else
	// {
	// 	env[4]=TONE_CHOPOFF_MILLISEC;
	// }

#ifdef YM2612_DEBUGOUTPUT
	for(int i=0; i<6; ++i){std::cout << env[i] << ",";}
	std::cout << std::endl;
#endif

	return true;
}

bool YM2612::CalculateEnvelopeSSG_EG(unsigned int env[6],unsigned int KC,const Slot &slot) const
{
	auto TLdB100=TLtoDB100[slot.TL];
	auto SLdB100=SLtoDB100[slot.SL];

	if(9600<=TLdB100)
	{
		return NoTone(env);
	}

	const unsigned int TLinv=9600-TLdB100;

	unsigned int DR=slot.DR*2+(KC>>(3-slot.KS));
	unsigned int SR=slot.SR*2+(KC>>(3-slot.KS));
	DR=std::min(DR,63U);
	SR=std::min(SR,63U);

	unsigned int TLinvMinusSL=(SLdB100<TLinv ? TLinv-SLdB100 : 0);
	unsigned int attackTime;
	unsigned int TLtoSLTime;
	unsigned int SLtoZeroTime;

	uint64_t mul;

	for(int i=0; i<2; ++i)
	{
		switch(SSG_EG_PTN[slot.SSG_EG&7][i])
		{
		case SSGEG_UP:
			mul=TLinvMinusSL;
			mul*=SSG_EG_DecayTime0dBTo95dB[DR];
			mul/=((9500*1024/10)/ENVELOPE_PRECISION);
			TLtoSLTime=(unsigned int)mul;

			mul=SLdB100;
			mul*=SSG_EG_DecayTime0dBTo95dB[SR];
			mul/=((9500*1024/10)/ENVELOPE_PRECISION);
			SLtoZeroTime=(unsigned int)mul;

			env[6*i+0]=0;
			env[6*i+1]=0;
			env[6*i+2]=SLtoZeroTime;
			env[6*i+3]=TLinvMinusSL;
			env[6*i+4]=TLtoSLTime;
			env[6*i+5]=TLinv;
			break;
		case SSGEG_DOWN:
			mul=SLdB100;
			mul*=SSG_EG_DecayTime0dBTo95dB[DR];
			mul/=((9500*1024/10)/ENVELOPE_PRECISION);
			TLtoSLTime=(unsigned int)mul;

			mul=TLinvMinusSL;
			mul*=SSG_EG_DecayTime0dBTo95dB[SR];
			mul/=((9500*1024/10)/ENVELOPE_PRECISION);
			SLtoZeroTime=(unsigned int)mul;

			env[6*i+0]=0;
			env[6*i+1]=TLinv;
			env[6*i+2]=TLtoSLTime;
			env[6*i+3]=TLinvMinusSL;
			env[6*i+4]=SLtoZeroTime;
			env[6*i+5]=0;
			break;
		case SSGEG_ONE:
			env[6*i+0]=0; // Jump up right away
			env[6*i+1]=TLinv;
			env[6*i+2]=100000; // Indefinite actually
			env[6*i+3]=TLinv;
			env[6*i+4]=100000;
			env[6*i+5]=TLinv;
			break;
		case SSGEG_ZERO:
			env[6*i+0]=100000; // Jump up right away
			env[6*i+1]=0;
			env[6*i+2]=100000; // Indefinite actually
			env[6*i+3]=0;
			env[6*i+4]=100000;
			env[6*i+5]=0;
			break;
		}
	}
	return true;
}

template <class LFOClass>
int YM2612::CalculateAmplitude(int chNum,const uint64_t timeInMicrosecS12[NUM_SLOTS],const unsigned int slotPhaseS12[NUM_SLOTS],const int AMS4096[4],int &lastSlot0Out) const
{
	if(true==channelMute[chNum])
	{
		return 0;
	}

	auto &ch=state.channels[chNum];
	bool slotActive[4]=
	{
		0!=(ch.usingSlot&1) || ch.slots[0].InReleasePhase,
		0!=(ch.usingSlot&2) || ch.slots[1].InReleasePhase,
		0!=(ch.usingSlot&4) || ch.slots[2].InReleasePhase,
		0!=(ch.usingSlot&8) || ch.slots[3].InReleasePhase,
	};

	unsigned int envTime[NUM_SLOTS]=
	{
		(unsigned int)(timeInMicrosecS12[0]>>(12+10-ENVELOPE_PRECISION_SHIFT)),
		(unsigned int)(timeInMicrosecS12[1]>>(12+10-ENVELOPE_PRECISION_SHIFT)),
		(unsigned int)(timeInMicrosecS12[2]>>(12+10-ENVELOPE_PRECISION_SHIFT)),
		(unsigned int)(timeInMicrosecS12[3]>>(12+10-ENVELOPE_PRECISION_SHIFT)),
	};

	#define SLOTOUTEV_Db_0(phaseShift) ((true!=slotActive[0] ? 0 : ch.slots[0].EnvelopedOutputDbToAmpl((slotPhaseS12[0]>>12),phaseShift,envTime[0],ch.FB,lastSlot0Out))*LFOClass::AMSMul(AMS4096[0])/LFOClass::AMSDiv())
	#define SLOTOUTEV_Db_1(phaseShift) ((true!=slotActive[1] ? 0 : ch.slots[1].EnvelopedOutputDbToAmpl((slotPhaseS12[1]>>12),phaseShift,envTime[1]))*LFOClass::AMSMul(AMS4096[1])/LFOClass::AMSDiv())
	#define SLOTOUTEV_Db_2(phaseShift) ((true!=slotActive[2] ? 0 : ch.slots[2].EnvelopedOutputDbToAmpl((slotPhaseS12[2]>>12),phaseShift,envTime[2]))*LFOClass::AMSMul(AMS4096[2])/LFOClass::AMSDiv())
	#define SLOTOUTEV_Db_3(phaseShift) ((true!=slotActive[3] ? 0 : ch.slots[3].EnvelopedOutputDbToAmpl((slotPhaseS12[3]>>12),phaseShift,envTime[3]))*LFOClass::AMSMul(AMS4096[3])/LFOClass::AMSDiv())

	int s0out,s1out,s2out,s3out;
	switch(ch.CONNECT)
	{
	default:
	case 0:
		s0out=SLOTOUTEV_Db_0(0);
		lastSlot0Out=s0out;
		s1out=SLOTOUTEV_Db_1(s0out);
		s2out=SLOTOUTEV_Db_2(s1out);
		return SLOTOUTEV_Db_3(s2out)*state.volume/DIV_CONNECT0;
	case 1:
		s0out=SLOTOUTEV_Db_0(0);
		lastSlot0Out=s0out;
		s1out=SLOTOUTEV_Db_1(0);
		s2out=SLOTOUTEV_Db_2(s0out+s1out);
		return SLOTOUTEV_Db_3(s2out)*state.volume/DIV_CONNECT1;
	case 2:
		s0out=SLOTOUTEV_Db_0(0);
		lastSlot0Out=s0out;
		s1out=SLOTOUTEV_Db_1(0);
		s2out=SLOTOUTEV_Db_2(s1out);
		return SLOTOUTEV_Db_3(s0out+s2out)*state.volume/DIV_CONNECT2;
	case 3:
		s0out=SLOTOUTEV_Db_0(0);
		lastSlot0Out=s0out;
		s1out=SLOTOUTEV_Db_1(s0out);
		s2out=SLOTOUTEV_Db_2(0);
		return SLOTOUTEV_Db_3(s1out+s2out)*state.volume/DIV_CONNECT3;
	case 4:
		s0out=SLOTOUTEV_Db_0(0);
		lastSlot0Out=s0out;
		s1out=SLOTOUTEV_Db_1(s0out);
		s2out=SLOTOUTEV_Db_2(0);
		s3out=SLOTOUTEV_Db_3(s2out);
		return ((s1out+s3out)*state.volume/DIV_CONNECT4);
		// Test only Slot 3 -> return SLOTOUTEV_Db_3(0)*state.volume/UNSCALED_MAX;
	case 5:
		s0out=SLOTOUTEV_Db_0(0);
		lastSlot0Out=s0out;
		s1out=SLOTOUTEV_Db_1(s0out);
		s2out=SLOTOUTEV_Db_2(s0out);
		s3out=SLOTOUTEV_Db_3(s0out);
		return ((s1out+s2out+s3out)*state.volume/DIV_CONNECT5);
	case 6:
		s0out=SLOTOUTEV_Db_0(0);
		lastSlot0Out=s0out;
		s1out=SLOTOUTEV_Db_1(s0out);
		s2out=SLOTOUTEV_Db_2(0    );
		s3out=SLOTOUTEV_Db_3(0    );
		return ((s1out+s2out+s3out)*state.volume/DIV_CONNECT6);
	case 7:
		s0out=SLOTOUTEV_Db_0(0);
		lastSlot0Out=s0out;
		s1out=SLOTOUTEV_Db_1(0);
		s2out=SLOTOUTEV_Db_2(0);
		s3out=SLOTOUTEV_Db_3(0);
		return ((s0out+s1out+s2out+s3out)*state.volume/DIV_CONNECT7);
	}
}
