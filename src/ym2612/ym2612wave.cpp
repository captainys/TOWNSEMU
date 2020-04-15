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

////////////////////////////////////////////////////////////

inline int YM2612::Slot::UnscaledOutput(int phase) const
{
	return sineTable[phase&PHASE_MASK];
}
inline int YM2612::Slot::UnscaledOutput(int phase,unsigned int FB) const
{
	if(0==FB)
	{
		return sineTable[phase&PHASE_MASK];
	}
	else
	{
		static const unsigned int rShift[8]={0,4,3,2,1,0,0,0};
		static const unsigned int lShift[8]={0,0,0,0,0,0,1,2};

		int o=sineTable[phase&PHASE_MASK];
		int sign=(o&0xFF000000);

		// FB must be 0 to 7
		o=(((o<<lShift[FB])>>rShift[FB])|sign);
		return sineTable[(phase+o)&PHASE_MASK];
	}
}
inline int YM2612::Slot::InterpolateEnvelope(unsigned int timeInMS) const
{
	if(true!=InReleasePhase)
	{
		if(timeInMS<env[0])
		{
			return env[1]*timeInMS/env[0];
		}
		else
		{
			timeInMS-=env[0];
			if(timeInMS<env[2])
			{
				return env[3]+(env[5]-env[3])*timeInMS/env[2];
			}
			else
			{
				timeInMS-=env[2];
				if(timeInMS<env[4])
				{
					return env[5]-env[5]*timeInMS/env[4];
				}
			}
		}
		return 0;
	}
	else
	{
		return 0; // Not supported yet.
	}
}
inline int YM2612::Slot::EnvelopedOutput(int phase,unsigned int timeInMS,unsigned int FB) const
{
	int env=InterpolateEnvelope(timeInMS);
	lastAmplitudeCache=env;
	int unscaledOut=UnscaledOutput(phase,FB);
	return (unscaledOut*env)/4096;
}
inline int YM2612::Slot::EnvelopedOutput(int phase,unsigned int timeInMS) const
{
	int env=InterpolateEnvelope(timeInMS);
	lastAmplitudeCache=env;
	int unscaledOut=UnscaledOutput(phase);
	return (unscaledOut*env)/4096;
}

////////////////////////////////////////////////////////////

/*static*/ inline unsigned int YM2612::KSToRate(unsigned int KS,unsigned int BLOCK,unsigned int NOTE)
{
	// [2] pp.207 Table I-5-30
	static const unsigned int RateTable[4*32]=
	{
		 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,  // KC<<3
		 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7,  // KC<<2
		 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9,10,10,11,11,12,12,13,13,14,14,15,15,  // KC<<1
		 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31   // KC
	};
	unsigned int keyCode=((BLOCK<<2)|NOTE)&31;
	return RateTable[KS*4+keyCode];
}

void YM2612::KeyOn(unsigned int chNum)
{
	auto &ch=state.channels[chNum];

	const unsigned int hertzX16=BLOCK_FNUM_to_FreqX16(ch.BLOCK,ch.F_NUM);

	state.playingCh|=(1<<chNum);
	ch.playState=CH_PLAYING;
	ch.microsec12=0;


	// Formula [2] pp.204
	// There is an error.  F_NUM is 11bits.  There is no F11.
	// Probably, F11, F10, F9, F8 should be read F10, F9, F8, F7.
	//unsigned int F10=((ch.F_NUM>>10)&1);
	//unsigned int F9= ((ch.F_NUM>> 9)&1);
	//unsigned int F8= ((ch.F_NUM>> 8)&1);
	//unsigned int F7=((ch.F_NUM>>11)&1);
	//unsigned int N3=(F10&(F9|F8|F7))|((~F10)&F9&F8&F7);
	//unsigned int NOTE=(F10<<1)|N3;
	//unsigned int KC=(ch.BLOCK<<2)|NOTE;
	// Doesn't make sense.

	// SEGA Genesis Software Manaual tells KC is just top 5 bits of BLOCK|F_NUM2.
	// Which makes more sense.
	unsigned int KC=(ch.BLOCK<<2)|((ch.F_NUM>>9)&3);


	for(auto &slot : ch.slots)
	{
		slot.InReleasePhase=false;
		slot.lastAmplitudeCache=0;
		slot.phase12=0;

		// Hz ranges 1 to roughly 8000.  PHASE_STEPS=32.  hertz*PHASE_STEPS=near 256K.
		// 256K<<12=256K*4K=1024M=1G.  Fits in 32 bit.

		// Phase runs hertz*PHASE_STEPS times per second.
		//            hertz*PHASE_STEPS/WAVE_SAMPLING_RATE times per step.
		// Phase 24 runs
		//            0x1000000*hertz*PHASE_STEPS/WAVE_SAMPLING_RATE per step.
		if(0==slot.MULTI)
		{
			slot.phase12Step=((hertzX16*PHASE_STEPS)<<7)/WAVE_SAMPLING_RATE; // Should consider DETUNE.
		}
		else
		{
			slot.phase12Step=((slot.MULTI*hertzX16*PHASE_STEPS)<<8)/WAVE_SAMPLING_RATE; // Should consider DETUNE.
		}
		// (hertzX16*PHASE_STEPS)<<8==hertz*PHASE_STEPS*4096
		CalculateEnvelope(slot.env,slot.RRCache,KC,slot);
	};

	ch.toneDuration12=CalculateToneDurationMilliseconds(chNum);
	ch.toneDuration12<<=12;

printf("%d BLOCK %03xH F_NUM %03xH Hertz %d Max Duration %d\n",KC,ch.BLOCK,ch.F_NUM,hertzX16/16,ch.toneDuration12>>12);
}

std::vector <unsigned char> YM2612::MakeWave(unsigned int chNum) const
{
	auto &ch=state.channels[chNum];
	std::vector <unsigned char> wave;

	unsigned long long int numSamples=ch.toneDuration12;
	numSamples*=WAVE_SAMPLING_RATE;
	numSamples/=1000;
	numSamples>>=12;

	const unsigned int microsec12Step=4096000000/WAVE_SAMPLING_RATE;
	// Time runs 1/WAVE_SAMPLING_RATE seconds per step
	//           1000/WAVE_SAMPLING_RATE milliseconds per step
	//           1000000/WAVE_SAMPLING_RATE microseconds per step
	//           1000000000/WAVE_SAMPLING_RATE nanoseconds per step

	// If microSec12=4096*microseconds, tm runs
	//           4096000000/WAVE_SAMPLING_RATE per step

	auto microsec12=ch.microsec12;
	unsigned int phase12[4]=
	{
		ch.slots[0].phase12,
		ch.slots[1].phase12,
		ch.slots[2].phase12,
		ch.slots[3].phase12,
	};

	wave.resize(4*numSamples);
	for(unsigned int i=0; i<numSamples; ++i)
	{
		const unsigned int microsec=(unsigned int)(microsec12>>12);

		auto ampl=CalculateAmplitude(chNum,microsec/1000,phase12);  // Envelope takes milliseconds.
		wave.push_back(ampl&255);
		wave.push_back((ampl>>8)&255);
		wave.push_back(ampl&255);
		wave.push_back((ampl>>8)&255);

		phase12[0]+=ch.slots[0].phase12Step;
		phase12[1]+=ch.slots[1].phase12Step;
		phase12[2]+=ch.slots[2].phase12Step;
		phase12[3]+=ch.slots[3].phase12Step;
		microsec12+=microsec12Step;
	}

	ch.nextMicrosec12=microsec12;
	ch.slots[0].nextPhase12=phase12[0];
	ch.slots[1].nextPhase12=phase12[1];
	ch.slots[2].nextPhase12=phase12[2];
	ch.slots[3].nextPhase12=phase12[3];

std::cout << (microsec12>>12) << "us " << std::endl;
std::cout << phase12[0] << "," << (phase12[0]>>12)/PHASE_STEPS << "cycles" << std::endl;
std::cout << phase12[1] << "," << (phase12[1]>>12)/PHASE_STEPS << "cycles" << std::endl;
std::cout << phase12[2] << "," << (phase12[2]>>12)/PHASE_STEPS << "cycles" << std::endl;
std::cout << phase12[3] << "," << (phase12[3]>>12)/PHASE_STEPS << "cycles" << std::endl;

	return wave;
}

unsigned int YM2612::CalculateToneDurationMilliseconds(unsigned int chNum) const
{
	unsigned int durationInMS=0;
	auto &ch=state.channels[chNum];
	for(int slotNum=0; slotNum<NUM_SLOTS; ++slotNum)
	{
		if(0!=connToOutChannel[ch.CONNECT][slotNum])
		{
			auto &slot=ch.slots[slotNum];
			durationInMS=std::max(durationInMS,slot.env[0]+slot.env[2]+slot.env[4]);
		}
	}
	return durationInMS;
}

bool YM2612::CalculateEnvelope(unsigned int env[6],unsigned int &RR,unsigned int KC,const Slot &slot) const
{
	KC&=31;

std::cout << KC << "," << slot.KS << "," << (KC>>(3-slot.KS)) << ", ";

	unsigned int AR=slot.AR*2+(KC>>(3-slot.KS));
	unsigned int DR=slot.DR*2+(KC>>(3-slot.KS));
	unsigned int SR=slot.SR*2+(KC>>(3-slot.KS));
	             RR=slot.RR*2+(KC>>(3-slot.KS));
	AR=std::min(AR,63U);
	DR=std::min(DR,63U);
	SR=std::min(SR,63U);
	RR=std::min(RR,63U);

	if(AR<4)
	{
		goto NOTONE;
	}

	auto TLdB100=TLtoDB100[slot.TL];
	auto SLdB100=SLtoDB100[slot.SL];

	if(9600<=TLdB100)
	{
		goto NOTONE;
	}

	const unsigned int TLinv=9600-TLdB100;
	const unsigned int SLinv=9600-SLdB100;
	const unsigned int TLampl=DB100to4095Scale[TLinv];
	const unsigned int SLampl=DB100to4095Scale[SLinv];

std::cout << "AR=" << AR << " DR=" << DR << " SR=" << SR << " TL=" << slot.TL  << " SL=" << slot.SL ;
std::cout << " ";

	env[1]=TLampl;
	env[3]=SLampl*TLampl/4095;
	env[5]=0;


	// After reading fmgen.c (Written by cisc, Author of M88 PC8801 emulator), it looks to be that
	// the time for attack doesn't depend on the total level, but it takes time to raise 0dB to 96dB all the time.
	// Then, the time for decay is based on SL only.  Just like dropping from 96dB to 96-dB(SL) dB.
	// The secondary decay duration should also depend only on SL, the time for 96-dB(SL)dB to 0dB.
	// The amplitude change is not linear, but I approximate by a linear function.  I may come back to the envelope
	// generation once I get a good enough approximation.
	unsigned long long int mul;
	env[0]=attackTime0to96dB[AR]/100;
	mul=SLdB100;
	mul*=sustainDecayReleaseTime0to96dB[DR];
	mul/=960000;
	env[2]=(unsigned int)mul;
	mul=9600-SLdB100;
	mul*=sustainDecayReleaseTime0to96dB[SR];
	mul/=960000;
	env[4]=(unsigned int)mul;

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

for(int i=0; i<6; ++i){std::cout << env[i] << ",";}
std::cout << "  RR=" << RR << "(" << sustainDecayReleaseTime0to96dB[RR]/100 << ")";
std::cout << std::endl;

	return true;
NOTONE:
	env[0]=0;
	env[1]=0;
	env[2]=0;
	env[3]=0;
	env[4]=0;
	env[5]=0;
	RR=0;
	return false;
}

int YM2612::CalculateAmplitude(int chNum,unsigned int timeInMS,const unsigned int slotPhase12[4]) const
{
	#define SLOTOUT_0(phaseShift,timeInMS) (0==(ch.usingSlot&1) ? 0 : ch.slots[0].EnvelopedOutput((slotPhase12[0]>>12)+phaseShift,timeInMS,ch.FB))
	#define SLOTOUT_1(phaseShift,timeInMS) (0==(ch.usingSlot&2) ? 0 : ch.slots[1].EnvelopedOutput((slotPhase12[1]>>12)+phaseShift,timeInMS))
	#define SLOTOUT_2(phaseShift,timeInMS) (0==(ch.usingSlot&4) ? 0 : ch.slots[2].EnvelopedOutput((slotPhase12[2]>>12)+phaseShift,timeInMS))
	#define SLOTOUT_3(phaseShift,timeInMS) (0==(ch.usingSlot&8) ? 0 : ch.slots[3].EnvelopedOutput((slotPhase12[3]>>12)+phaseShift,timeInMS))

	auto &ch=state.channels[chNum];
	int s0out,s1out,s2out,s3out;
	switch(ch.CONNECT)
	{
	default:
	case 0:
		s0out=SLOTOUT_0(0,    timeInMS);
		s1out=SLOTOUT_1(s0out,timeInMS);
		s2out=SLOTOUT_2(s1out,timeInMS);
		return SLOTOUT_3(s2out,timeInMS);
	case 1:
		s0out=SLOTOUT_0(0,timeInMS);
		s1out=SLOTOUT_1(0,timeInMS);
		s2out=SLOTOUT_2(s0out+s1out,timeInMS);
		return SLOTOUT_3(s2out,timeInMS);
	case 2:
		s0out=SLOTOUT_0(0,timeInMS);
		s1out=SLOTOUT_1(0,timeInMS);
		s2out=SLOTOUT_2(s1out,timeInMS);
		return SLOTOUT_3(s0out+s2out,timeInMS);
	case 3:
		s0out=SLOTOUT_0(0,    timeInMS);
		s1out=SLOTOUT_1(s0out,timeInMS);
		s2out=SLOTOUT_2(0    ,timeInMS);
		return SLOTOUT_3(s1out+s2out,timeInMS);
	case 4:
		s0out=SLOTOUT_0(0,    timeInMS);
		s1out=SLOTOUT_1(s0out,timeInMS);
		s2out=SLOTOUT_2(0    ,timeInMS);
		s3out=SLOTOUT_3(s2out,timeInMS);
		return (s1out+s3out)/2;
	case 5:
		s0out=SLOTOUT_0(0,    timeInMS);
		s1out=SLOTOUT_1(s0out,timeInMS);
		s2out=SLOTOUT_2(s0out,timeInMS);
		s3out=SLOTOUT_3(s0out,timeInMS);
		return (s1out+s2out+s3out)/3;
	case 6:
		s0out=SLOTOUT_0(0,    timeInMS);
		s1out=SLOTOUT_1(s0out,timeInMS);
		s2out=SLOTOUT_2(0    ,timeInMS);
		s3out=SLOTOUT_3(0    ,timeInMS);
		return (s1out+s2out+s3out)/3;
	case 7:
		s0out=SLOTOUT_0(0,timeInMS);
		s1out=SLOTOUT_1(0,timeInMS);
		s2out=SLOTOUT_2(0,timeInMS);
		s3out=SLOTOUT_3(0,timeInMS);
		return (s0out+s1out+s2out+s3out)/4;
	}
}
