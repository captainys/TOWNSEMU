/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include "ym2612.h"


static unsigned int attackTime0to96dB[64]= // 1/100ms
{
0,
0,
0,
0,
897024,
897024,
598016,
598016, // 7: 897024*2/3
448512, // 8: 897024/2           x=8->897024/(2^1)
358810, // 9: 897024*6/15
299008, //10: 897024/3
258293,
224256, //12: 897024/4           x=12->897024/(2^2)
173405,                 // <- 179405?
149504, //14: 897024/6
128146,

112128, //16: 897024/8           x=16->897024/(2^3)  -> 897024/(2^((x-4)/4))
83702,
74752,  // 897024/12
64073,
56064,  // 897024/16
44851,
37376,  // 897024/24
32037,
28032,  // 897024/32
22425,
18688,  // 897024/48
16018,
14016,
11213,
9344,
8009,

7008,
5806,
4872,
4005,
3504,
2903,
2335,
2002,
1752,
1402,
1168,
1001,
876,
701,
584,
501,

438,
350,
232,
250,
240,
192,
160,
137,
126,
101,
84,
72,
60,
60,
0,
0,
};
static unsigned int sustainDecayReleaseTime0to96dB[64]= // 1/100ms
{
0,
0,
0,
0,
12398592,
12398592,
8265726,
8265728,
5199296,
4953437,
4132864,
3542455,
3099648,
2479719,
2066432,
1771227,

1549824,
1239859,
1033215,
885614,
774912,
619930,
516608,
442807,
387455,
309965,
258304,
221403,
193728,
154982,
129152,
110702,

96864,
77491,
64576,
55351,
48432,
38745,
32298,
27675,
24216,
19373,
16144,
13938,
12108,
9686,
8072,
6919,

6054,
4843,
4056,
3459,
3027,
2422,
2018,
1730,
1514,
1211,
1009,
865,
757,
757,
757,
757,
};
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



/*static*/ inline unsigned int YM2612::KSToRate(unsigned int KS,unsigned int BLOCK,unsigned int NOTE)
{
	// [2] pp.207 Table I-5-30
	static const unsigned int RateTable[4*32]=
	{
		 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
		 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7,
		 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9,10,10,11,11,12,12,13,13,14,14,15,15,
		 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
	};
	unsigned int keyCode=((BLOCK<<2)|NOTE)&31;
	return RateTable[KS*4+keyCode];
}

std::vector <unsigned char> YM2612::MakeWave(unsigned int chNum) const
{
	auto &ch=state.channels[chNum];
	std::vector <unsigned char> wave;

	const unsigned int toneDuration=1000; // Tentative    GetToneDuration(ch);
	const unsigned int hertz=440;         // Tentative

	const unsigned int numSamples=toneDuration*WAVE_SAMPLING_RATE/1000;
	unsigned int phase12[4]={0,0,0,0};        // phase>>12 gives the phase.  For each slot.
	unsigned long long int microSec12=0; // 4096*microseconds

	const unsigned int microSec12Step=4096000000/WAVE_SAMPLING_RATE;
	// Time runs 1/WAVE_SAMPLING_RATE seconds per step
	//           1000/WAVE_SAMPLING_RATE milliseconds per step
	//           1000000/WAVE_SAMPLING_RATE microseconds per step
	//           1000000000/WAVE_SAMPLING_RATE nanoseconds per step

	// If microSec12=4096*microseconds, tm runs
	//           4096000000/WAVE_SAMPLING_RATE per step

	const unsigned int phase12Step[4]=
	{
		((hertz*PHASE_STEPS)<<12)/WAVE_SAMPLING_RATE, // Should consider DETUNE.
		((hertz*PHASE_STEPS)<<12)/WAVE_SAMPLING_RATE,
		((hertz*PHASE_STEPS)<<12)/WAVE_SAMPLING_RATE,
		((hertz*PHASE_STEPS)<<12)/WAVE_SAMPLING_RATE
	};
	// hertz range 1 to 8000.  PHASE_STEPS=32.  hertz*PHASE_STEPS=near 256K.
	// 256K<<12=256K*4K=1024M=1G.  Fits in 32 bit.

	// Phase runs hertz*PHASE_STEPS times per second.
	//            hertz*PHASE_STEPS/WAVE_SAMPLING_RATE times per step.
	// Phase 24 runs
	//            0x1000000*hertz*PHASE_STEPS/WAVE_SAMPLING_RATE per step.


	for(int s=0; s<NUM_SLOTS; ++s)
	{
		// state.slots[s].CalculateEnvelope(env[i]);
		// 
	}


	wave.resize(4*numSamples);
	for(unsigned int i=0; i<numSamples; ++i)
	{
		const unsigned int microSec=microSec12/4096;

		// Formula [2] pp.204
		unsigned int F11=((ch.F_NUM>>11)&1);
		unsigned int F10=((ch.F_NUM>>10)&1);
		unsigned int F9= ((ch.F_NUM>> 9)&1);
		unsigned int F8= ((ch.F_NUM>> 8)&1);
		unsigned int N3=(F11|(F10&F9&F8))|((~F11)&(F10|F9|F8));
		unsigned int NOTE=(F11<<1)|N3;


		phase12[0]+=phase12Step[0];
		phase12[1]+=phase12Step[1];
		phase12[2]+=phase12Step[2];
		phase12[3]+=phase12Step[3];
		microSec12+=microSec12Step;
	}

	return wave;
}
