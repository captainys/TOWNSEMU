/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include "i486.h"
#include <algorithm>
#include <math.h>
#include <stdint.h>


static const double VALUE_OF_E= 2.71828182845904523536;
static const double VALUE_OF_PI=3.14159265358979323846;



// #define CHECK_FOR_NAN



void i486DXCommon::FPUState::BreakOnNan(i486DXCommon &cpu,double value)
{
	if(nullptr!=cpu.debuggerPtr && true==isnan(value))
	{
		cpu.debuggerPtr->ExternalBreak("FPU: NaN detected.");
	}
}



/* static */ int16_t i486DXCommon::FPUState::IntFrom16Bit(const unsigned char byteData[])
{
#ifdef YS_LITTLE_ENDIAN
	return *((int16_t *)byteData);
#else
	uint16_t ui;
	ui= (uint32_t)byteData[0]|
	   ((uint32_t)byteData[1]<<8);
	return *((int16_t *)&ui);
#endif
}

/* static */ int32_t i486DXCommon::FPUState::IntFrom32Bit(const unsigned char byteData[])
{
#ifdef YS_LITTLE_ENDIAN
	return *((int32_t *)byteData);
#else
	uint32_t ui;
	ui= (uint32_t)byteData[0]|
	   ((uint32_t)byteData[1]<<8)|
	   ((uint32_t)byteData[2]<<16)|
	   ((uint32_t)byteData[3]<<24)|;
	return *((int32_t *)&ui);
#endif
}

/* static */ int64_t i486DXCommon::FPUState::IntFrom64Bit(const unsigned char byteData[])
{
#ifdef YS_LITTLE_ENDIAN
	return *((int64_t *)byteData);
#else
	uint64_t ui;
	ui= (uint64_t)byteData[0]|
	   ((uint64_t)byteData[1]<<8)|
	   ((uint64_t)byteData[2]<<16)|
	   ((uint64_t)byteData[3]<<24)|
	   ((uint64_t)byteData[4]<<32)|
	   ((uint64_t)byteData[5]<<40)|
	   ((uint64_t)byteData[6]<<48)|
	   ((uint64_t)byteData[7]<<56);
	return *((int64_t *)&ui);
#endif
}

/* static */ void i486DXCommon::FPUState::DoubleTo80Bit(OperandValueBase &value80,double src)
{
	value80.numBytes=10;
	DoubleTo80Bit(value80.byteData,src);
}
/* static */ void i486DXCommon::FPUState::DoubleTo80Bit(uint8_t value80[],double src)
{
	// Assume sizeof(double) is 8-byte long.
	uint64_t binary=*((uint64_t *)&src);
	uint16_t exponent=((binary>>52)&2047);   // 1023=2^0
	uint64_t fraction=(binary&((1LL<<52)-1));
	unsigned char signBit=((binary>>63)<<7);

	// Reference  https://en.wikipedia.org/wiki/Double-precision_floating-point_format
	//            https://en.wikipedia.org/wiki/Extended_precision
	if(INFINITY==src)
	{
		value80[9]=0x7F;
		value80[8]=0xFF;
		value80[7]=0x80;
		value80[6]=0;
		value80[5]=0;
		value80[4]=0;
		value80[3]=0;
		value80[2]=0;
		value80[1]=0;
		value80[0]=0;
		return;
	}
	else if(-INFINITY==src)
	{
		value80[9]=0xFF;
		value80[8]=0xFF;
		value80[7]=0x80;
		value80[6]=0;
		value80[5]=0;
		value80[4]=0;
		value80[3]=0;
		value80[2]=0;
		value80[1]=0;
		value80[0]=0;
		return;
	}
	else if(isnan(src))
	{
		value80[9]=0x7F;
		value80[8]=0xFF;
		value80[7]=0xFF;
		value80[6]=0xFF;
		value80[5]=0xFF;
		value80[4]=0xFF;
		value80[3]=0xFF;
		value80[2]=0xFF;
		value80[1]=0xFF;
		value80[0]=0xFF;
		return;
	}


	// In 80-bit format, fraction needs to be expanded to 64-bit
	// Exponent 16383 is 2^0.
	exponent=exponent+16383-1023;
	fraction<<=11;
	fraction|=(1LL<<63);  // Integer bit.

	// It doesn't handle positive/negative infinity and NaN yet.

#ifdef YS_LITTLE_ENDIAN
	*((uint16_t *)(value80+8))=exponent;
	*((uint64_t *)value80)=fraction;
	value80[9]|=signBit;
#else
	value80[0]=( fraction     &255);
	value80[1]=((fraction>> 8)&255);
	value80[2]=((fraction>>16)&255);
	value80[3]=((fraction>>24)&255);
	value80[4]=((fraction>>32)&255);
	value80[5]=((fraction>>40)&255);
	value80[6]=((fraction>>48)&255);
	value80[7]=((fraction>>56)&255);
	value80[8]=(exponent&255);
	value80[9]=(((exponent>>8)&255)|signBit);
#endif
}
/* static */ double i486DXCommon::FPUState::DoubleFrom80Bit(const OperandValueBase &value80)
{
	return DoubleFrom80Bit(value80.byteData);
}
/* static */ double i486DXCommon::FPUState::DoubleFrom80Bit(const unsigned char byteData[])
{
	uint16_t exponent;
	uint64_t fraction;
	uint16_t signBit;

	signBit=(byteData[9]&0x80);

	if(0x7F==(byteData[9]&0x7F) && 0xFF==byteData[8])  // Exponent bits are all ones.
	{
		if(0xC0==(byteData[7]&0xC0)) // Nan
		{
			return NAN;
		}
	}

#ifdef YS_LITTLE_ENDIAN
	exponent=*((uint16_t *)(byteData+8));
	fraction=*((uint64_t *)byteData);
#else
	fraction=
		 (uint64_t)byteData[0]     |
		((uint64_t)byteData[1]<< 8)|
		((uint64_t)byteData[2]<<16)|
		((uint64_t)byteData[3]<<24)|
		((uint64_t)byteData[4]<<32)|
		((uint64_t)byteData[5]<<40)|
		((uint64_t)byteData[6]<<48)|
		((uint64_t)byteData[7]<<56);
	exponent=byteData[8]|(byteData[9]<<8);
#endif

	exponent&=0x7FFF;

	if(exponent+1023<16384) // exponent+1023-16383 -> Make it Zero
	{
		if(0!=signBit)
		{
			return -0.0;
		}
		else
		{
			return 0.0;
		}
	}
	else if(2048+16383<=exponent+1023) // 2048<=exponent+1023-16383 -> Make it Infinity
	{
		if(0!=signBit)
		{
			return -INFINITY;
		}
		else
		{
			return INFINITY;
		}
	}

	fraction>>=11;
	fraction&=((1LL<<52)-1);
	exponent=exponent+1023-16383;

	// Assume the endiannness for integers is same as floating points.
	double d;
	uint64_t *i=(uint64_t *)&d;

	*i=exponent;
	*i<<=52;
	*i|=fraction;
	if(0!=signBit)
	{
		*i|=(1LL<<63);
	}
	return d;
}

/* static */ double i486DXCommon::FPUState::DoubleFrom64Bit(const unsigned char byteData[])
{
	double d;

#ifdef YS_LITTLE_ENDIAN
	const double *dPtr=(const double *)byteData;
	d=*dPtr;
#else
	// Assume double and int are in the same byte order.
	uint64_t *i=(uint64_t *)&d;
	*i=	 (uint64_t)byteData[0]     |
		((uint64_t)byteData[1]<< 8)|
		((uint64_t)byteData[2]<<16)|
		((uint64_t)byteData[3]<<24)|
		((uint64_t)byteData[4]<<32)|
		((uint64_t)byteData[5]<<40)|
		((uint64_t)byteData[6]<<48)|
		((uint64_t)byteData[7]<<56);
#endif

	return d;
}

/* static */ double i486DXCommon::FPUState::DoubleFrom32Bit(const unsigned char byteData[])
{
	float f;

#ifdef YS_LITTLE_ENDIAN
	const float *fPtr=(const float *)byteData;
	f=*fPtr;
#else
	// Assume float and int are in the same byte order.
	uint32_t *i=(uint32_t *)&f;
	*i=	 (uint64_t)byteData[0]     |
		((uint64_t)byteData[1]<< 8)|
		((uint64_t)byteData[2]<<16)|
		((uint64_t)byteData[3]<<24);
#endif

	return (double)f;
}

/* static */ double i486DXCommon::FPUState::DoubleFrom80BitBCD(const unsigned char bcd80[])
{
	uint64_t i64=0,digit=1;
	for(int i=0; i<9; ++i)
	{
		i64+=(digit*(bcd80[i]&0x0F));
		digit*=10;
		i64+=(digit*((bcd80[i]>>4)&0x0F));
		digit*=10;
	}

	double value=(double)i64;
	if(0!=(bcd80[9]&0x80))
	{
		value=-value;
	}

	return value;
}


i486DXCommon::FPUState::FPUState()
{
	enabled=false; // Tentative.
	Reset();
}
void i486DXCommon::FPUState::Reset(void)
{
	controlWord=0x037F;
	statusWord=0;
	tagWord=0xFFFF;
	stackPtr=8;
	for(auto &s : stack)
	{
		s.tag=0;
		s.value=0.0;
	}
}
void i486DXCommon::FPUState::FNINIT(void)
{
// [1] pp.26-97 FNINIT
// CW<=037FH
// SW<=0
// TW<=FFFFH
// FEA<=0
// FDS<=0
// FIP<=0
// FOP<=0
// FCS<=0
	if(true==enabled)
	{
		Reset();
	}
}
bool i486DXCommon::FPUState::ExceptionPending(void) const
{
	return false;// Tentative 
}
unsigned int i486DXCommon::FPUState::GetStatusWord(void) const
{
	if(true==enabled)
	{
		return (statusWord&0xC7FF)|((stackPtr&7)<<11);
	}
	else
	{
		return 0xffff;
	}
}
unsigned int i486DXCommon::FPUState::GetControlWord(void) const
{
	if(true==enabled)
	{
		return controlWord;
	}
	else
	{
		return 0xffff;
	}
}
unsigned int i486DXCommon::FPUState::GetRC(void) const
{
	return (GetControlWord()>>10)&3;
}
double i486DXCommon::FPUState::RoundToInteger(double src) const
{
	switch(GetRC())
	{
	case 0: // Round to Nearest or Even
		if(src<0.0)
		{
			src-=0.5;
		}
		else if(0.0<src)
		{
			src+=0.5;
		}
		return src-fmod(src,1.0);
	case 1: // Round Down (Toward -INF)
		return floor(src);
	case 2: // Round Up (Toward +INF)
		return ceil(src);
	default:
	case 3: // Chop (Truncate)
		return src-fmod(src,1.0);
	}
	
}
void i486DXCommon::FPUState::GetSTAsDouble(class i486DXCommon &cpu,class OperandValueBase &value)
{
#ifdef YS_LITTLE_ENDIAN
	double *doublePtr=(double *)value.byteData;
	*doublePtr=ST(cpu).value;
#else
	uint64_t *intPtr=(uint64_t *)(&ST(cpu).value);
	value.byteData[0]=  (*intPtr)     &0xFF;
	value.byteData[1]= ((*intPtr)>> 8)&0xFF;
	value.byteData[2]= ((*intPtr)>>16)&0xFF;
	value.byteData[3]= ((*intPtr)>>24)&0xFF;
	value.byteData[4]= ((*intPtr)>>32)&0xFF;
	value.byteData[5]= ((*intPtr)>>40)&0xFF;
	value.byteData[6]= ((*intPtr)>>48)&0xFF;
	value.byteData[7]= ((*intPtr)>>56)&0xFF;
#endif
	value.numBytes=8;
}
void i486DXCommon::FPUState::GetSTAsFloat(class i486DXCommon &cpu,OperandValueBase &value)
{
#ifdef YS_LITTLE_ENDIAN
	float *floatPtr=(float *)value.byteData;
	*floatPtr=(float)ST(cpu).value;
#else
	float f=ST(cpu.value);
	uint32_t *intPtr=(uint64_t *)(&f);
	value.byteData[0]=  (*intPtr)     &0xFF;
	value.byteData[1]= ((*intPtr)>> 8)&0xFF;
	value.byteData[2]= ((*intPtr)>>16)&0xFF;
	value.byteData[3]= ((*intPtr)>>24)&0xFF;
#endif
	value.numBytes=4;
}
void i486DXCommon::FPUState::GetSTAsSignedInt(class i486DXCommon &cpu,class OperandValueBase &value)
{
	int64_t i=0;
	double d=RoundToInteger(ST(cpu).value);
	i=(int64_t)d;
	value.numBytes=8;
	value.byteData[0]=( i     &255);
	value.byteData[1]=((i>> 8)&255);
	value.byteData[2]=((i>>16)&255);
	value.byteData[3]=((i>>24)&255);
	value.byteData[4]=((i>>32)&255);
	value.byteData[5]=((i>>40)&255);
	value.byteData[6]=((i>>48)&255);
	value.byteData[7]=((i>>56)&255);
}
void i486DXCommon::FPUState::GetSTAs80BitBCD(class i486DXCommon &cpu,OperandValueBase &value)
{
	double src=ST(cpu).value;
	value.numBytes=10;

	if(src<0.0)
	{
		src=-src;
		value.byteData[9]=0x80;
	}
	else
	{
		value.byteData[9]=0x00;
	}
	uint64_t srcI=src;
	for(int i=0; i<9; ++i)
	{
		value.byteData[i]=srcI%10;
		srcI/=10;
		value.byteData[i]|=((srcI%10)<<4);
		srcI/=10;
	}
}
bool i486DXCommon::FPUState::Push(class i486DXCommon &cpu,double value)
{
	if(0<stackPtr)
	{
		--stackPtr;
		stack[stackPtr].value=value;
		stack[stackPtr].tag=0; // Should I do this?
		return true;
	}
	return false; // Should shoot an exception for this.
}
void i486DXCommon::FPUState::Pop(i486DXCommon &cpu)
{
	if(stackPtr<STACK_LEN)
	{
		++stackPtr;
	}
	else
	{
		// Raise NM exception
	}
}
void i486DXCommon::FPUState::Pop(class i486DXCommon &cpu,int level)
{
	if(stackPtr+level<=STACK_LEN)
	{
		stackPtr+=level;
	}
	else
	{
		// Raise NM exception
		// Should I make stack pointer to zero?
	}
}
unsigned int i486DXCommon::FPUState::NumFilled(void) const
{
	return STACK_LEN-stackPtr;
}

std::vector <std::string> i486DXCommon::FPUState::GetStateText(void) const
{
	std::vector <std::string> text;

	if(true==enabled)
	{
		text.push_back("FPU is enabled.");
	}
	else
	{
		text.push_back("FPU is disabled.");
	}

	text.push_back("");
	text.back()+="Stack Pointer:";
	text.back()+=cpputil::Uitox(stackPtr);

	for(int i=0; stackPtr+i<STACK_LEN; ++i)
	{
		char fmt[256];
		sprintf(fmt,"ST%d %32.10lf %02x",i,stack[stackPtr+i].value,stack[stackPtr+i].tag);
		text.push_back(fmt);
	}

	text.push_back("");
	text.back()+="Control Word:";
	text.back()+=cpputil::Ustox(controlWord);

	text.push_back("");
	text.back()+="Status Word :";
	text.back()+=cpputil::Ustox(statusWord);

	text.push_back("");
	text.back()+="Tag Word    :";
	text.back()+=cpputil::Ustox(tagWord);
	text.back()+=" (What's the hell is it?)";

	return text;
}

unsigned int i486DXCommon::FPUState::F2XM1(i486DXCommon &cpu)
{
	if(true==enabled)
	{
		statusWord&=~STATUS_C1;
		auto &st=ST(cpu);
		st.value=pow(2.0,st.value)-1.0;
		return 242;
	}
	return 0;
}
unsigned int i486DXCommon::FPUState::FABS(i486DXCommon &cpu)
{
	if(true==enabled)
	{
		statusWord&=~STATUS_C1;
		ST(cpu).value=fabs(ST(cpu).value);
		return 3;
	}
	return 0;
}
unsigned int i486DXCommon::FPUState::FADD_m32real(i486DXCommon &cpu,const unsigned char byteData[])
{
	if(true==enabled)
	{
		statusWord&=~STATUS_C1;

		auto src=DoubleFrom32Bit(byteData);
		ST(cpu).value+=src;
		return 10;
	}
	return 0;
}
unsigned int i486DXCommon::FPUState::FADD64(i486DXCommon &cpu,const unsigned char byteData[])
{
	if(true==enabled)
	{
		statusWord&=~STATUS_C1;

		auto src=DoubleFrom64Bit(byteData);
		ST(cpu).value+=src;
		return 10;
	}
	return 0;
}
unsigned int i486DXCommon::FPUState::FADD_ST_STi(i486DXCommon &cpu,int i)
{
	if(true==enabled)
	{
		statusWord&=~STATUS_C1;

		auto &ST=this->ST(cpu);
		auto &STi=this->ST(cpu,i);
		ST.value+=STi.value;
		return 10;
	}
	return 0; // Let it abort.
}
unsigned int i486DXCommon::FPUState::FADDP_STi_ST(i486DXCommon &cpu,int i)
{
	if(true==enabled)
	{
		statusWord&=~STATUS_C1;

		auto &ST=this->ST(cpu);
		auto &STi=this->ST(cpu,i);
		STi.value+=ST.value;
		Pop(cpu);

		return 10;
	}
	return 0; // Let it abort.
}
unsigned int i486DXCommon::FPUState::FIADD_m16int(i486DXCommon &cpu,const unsigned char byteData[])
{
	if(true==enabled)
	{
		statusWord&=~STATUS_C1;

		double src=(double)IntFrom16Bit(byteData);
		ST(cpu).value+=src;

		return 24;
	}
	return 0;
}
unsigned int i486DXCommon::FPUState::FBLD(i486DXCommon &cpu,const unsigned char byteData[])
{
	if(true==enabled)
	{
		statusWord&=~STATUS_C1;
		Push(cpu,DoubleFrom80BitBCD(byteData));
		return 75;
	}
	return 0;
}
unsigned int i486DXCommon::FPUState::FCHS(i486DXCommon &cpu)
{
	if(true==enabled)
	{
		auto &st=ST(cpu);
		st.value=-st.value;
		return 4;
	}
	return 0; // Let it abort.
}
unsigned int i486DXCommon::FPUState::FCLEX(i486DXCommon &cpu)
{
	return 7;
}
unsigned int i486DXCommon::FPUState::FCOM_m32real(i486DXCommon &cpu,const unsigned char byteData[])
{
	if(true==enabled)
	{
		Compare(ST(cpu).value,DoubleFrom32Bit(byteData));
		return 4;
	}
	return 0; // Let it abort.
}
unsigned int i486DXCommon::FPUState::FCOMP_m32real(i486DXCommon &cpu,const unsigned char byteData[])
{
	if(true==enabled)
	{
		Compare(ST(cpu).value,DoubleFrom32Bit(byteData));
		Pop(cpu);
		return 4;
	}
	return 0; // Let it abort.
}
unsigned int i486DXCommon::FPUState::FCOM_m64real(i486DXCommon &cpu,const unsigned char byteData[])
{
	if(true==enabled)
	{
		Compare(ST(cpu).value,DoubleFrom64Bit(byteData));
		return 4;
	}
	return 0; // Let it abort.
}
unsigned int i486DXCommon::FPUState::FCOMP_m64real(i486DXCommon &cpu,const unsigned char byteData[])
{
	if(true==enabled)
	{
		Compare(ST(cpu).value,DoubleFrom64Bit(byteData));
		Pop(cpu);
		return 4;
	}
	return 0; // Let it abort.
}
unsigned int i486DXCommon::FPUState::FCOM(i486DXCommon &cpu,int i)
{
	if(true==enabled)
	{
		// It should raise Invalid-Operation exception if one of the operands is NaN.
		// This CPU core is not emulating this behavior.  FCOM works same as FUCOM.
		Compare(ST(cpu).value,ST(cpu,i).value);
		return 4;
	}
	return 0; // Let it abort.
}
unsigned int i486DXCommon::FPUState::FCOMP(i486DXCommon &cpu,int i)
{
	if(true==enabled)
	{
		Compare(ST(cpu).value,ST(cpu,i).value);
		Pop(cpu);
		return 4;
	}
	return 0; // Let it abort.
}
unsigned int i486DXCommon::FPUState::FCOMPP(i486DXCommon &cpu)
{
	if(true==enabled)
	{
		Compare(ST(cpu).value,ST(cpu,1).value);
		Pop(cpu,2);
		return 5;
	}
	return 0; // Let it abort.
}
unsigned int i486DXCommon::FPUState::FUCOM_STi(i486DXCommon& cpu, int i)
{
	if (true == enabled)
	{
		Compare(ST(cpu).value, ST(cpu, i).value);
		return 4;
	}
	return 0; // Let it abort.
}
unsigned int i486DXCommon::FPUState::FUCOMP_STi(i486DXCommon& cpu, int i)
{
	if (true == enabled)
	{
		Compare(ST(cpu).value, ST(cpu, i).value);
		Pop(cpu);
		return 4;
	}
	return 0; // Let it abort.
}
unsigned int i486DXCommon::FPUState::FUCOMPP(i486DXCommon& cpu)
{
	if (true == enabled)
	{
		Compare(ST(cpu).value, ST(cpu, 1).value);
		Pop(cpu, 2);
		return 5;
	}
	return 0; // Let it abort.
}
unsigned int i486DXCommon::FPUState::FDIV_m32real(i486DXCommon &cpu,const unsigned char byteData[])
{
	if(true==enabled)
	{
		statusWord&=~STATUS_C1;

		auto src=DoubleFrom32Bit(byteData);
		auto &st=ST(cpu);
		if(0==src)
		{
			// Zero division
		}
		st.value=st.value/src;

		return 73;
	}
	return 0;
}
unsigned int i486DXCommon::FPUState::FDIVR_m32real(i486DXCommon &cpu,const unsigned char byteData[])
{
	if(true==enabled)
	{
		statusWord&=~STATUS_C1;

		auto src=DoubleFrom32Bit(byteData);
		auto &st=ST(cpu);
		if(0==st.value)
		{
			// Zero division
		}
		st.value=src/st.value;

	#ifdef CHECK_FOR_NAN
		BreakOnNan(cpu,st.value);
	#endif

		return 73;
	}
	return 0;
}
unsigned int i486DXCommon::FPUState::FIDIV_m32int(i486DXCommon &cpu,const unsigned char byteData[])
{
	if(true==enabled)
	{
		statusWord&=~STATUS_C1;

		double src=IntFrom32Bit(byteData);
		auto &st=ST(cpu);
		if(0==src)
		{
			// Zero division
		}
		st.value=st.value/src;

	#ifdef CHECK_FOR_NAN
		BreakOnNan(cpu,st.value);
	#endif

		return 73;
	}
	return 0;
}
unsigned int i486DXCommon::FPUState::FIDIVR_m32int(i486DXCommon &cpu,const unsigned char byteData[])
{
	if(true==enabled)
	{
		statusWord&=~STATUS_C1;

		double src=IntFrom32Bit(byteData);
		auto &st=ST(cpu);
		if(0==st.value)
		{
			// Zero division
		}
		st.value=src/st.value;

	#ifdef CHECK_FOR_NAN
		BreakOnNan(cpu,st.value);
	#endif

		return 73;
	}
	return 0;
}
unsigned int i486DXCommon::FPUState::FDIVP_STi_ST(i486DXCommon &cpu,int i)
{
	if(true==enabled)
	{
		statusWord&=~STATUS_C1;

		auto &ST=this->ST(cpu);
		auto &STi=this->ST(cpu,i);
		if(0.0==STi.value)
		{
			// Zero division.
		}
		STi.value=STi.value/ST.value; // Let it be a NaN if ST1.value is zero.
		Pop(cpu);

	#ifdef CHECK_FOR_NAN
		BreakOnNan(cpu,STi.value);
	#endif

		return 70;
	}
	return 0; // Let it abort.
}
unsigned int i486DXCommon::FPUState::FDIV_m64real(i486DXCommon &cpu,const unsigned char byteData[])
{
	if(true==enabled)
	{
		statusWord&=~STATUS_C1;

		auto src=DoubleFrom64Bit(byteData);
		auto &st=ST(cpu);
		if(0==st.value)
		{
			// Zero division
		}
		st.value/=src;

	#ifdef CHECK_FOR_NAN
		BreakOnNan(cpu,st.value);
	#endif

		return 73;
	}
	return 0;
}
unsigned int i486DXCommon::FPUState::FDIVR_m64real(i486DXCommon &cpu,const unsigned char byteData[])
{
	if(true==enabled)
	{
		statusWord&=~STATUS_C1;

		auto src=DoubleFrom64Bit(byteData);
		auto &st=ST(cpu);
		if(0==st.value)
		{
			// Zero division
		}
		st.value=src/st.value;

	#ifdef CHECK_FOR_NAN
		BreakOnNan(cpu,st.value);
	#endif

		return 73;
	}
	return 0;
}
unsigned int i486DXCommon::FPUState::FDIVRP_STi_ST(i486DXCommon &cpu,int i)
{
	if(true==enabled)
	{
		statusWord&=~STATUS_C1;

		auto &ST=this->ST(cpu);
		auto &STi=this->ST(cpu,i);
		if(0.0==ST.value)
		{
			// Zero division.
		}
		STi.value=ST.value/STi.value; // Let it be a NaN if ST1.value is zero.
		Pop(cpu);

	#ifdef CHECK_FOR_NAN
		BreakOnNan(cpu,STi.value);
	#endif

		return 73;
	}
	return 0; // Let it abort.
}
unsigned int i486DXCommon::FPUState::FIDIV_m16int(i486DXCommon &cpu,const unsigned char byteData[])
{
	if(true==enabled)
	{
		statusWord&=~STATUS_C1;

		double src=(double)IntFrom16Bit(byteData);
		auto &st=ST(cpu);
		if(0==st.value)
		{
			// Zero division
		}
		st.value/=src;

	#ifdef CHECK_FOR_NAN
		BreakOnNan(cpu,st.value);
	#endif

		return 73;
	}
	return 0;
}
unsigned int i486DXCommon::FPUState::FILD_m16int(i486DXCommon &cpu,const unsigned char byteData[])
{
	if(true==enabled)
	{
		statusWord&=~STATUS_C1;
		Push(cpu,(double)IntFrom16Bit(byteData));
		return 16;
	}
	return 0;
}
unsigned int i486DXCommon::FPUState::FILD_m32int(i486DXCommon &cpu,const unsigned char byteData[])
{
	if(true==enabled)
	{
		statusWord&=~STATUS_C1;
		Push(cpu,(double)IntFrom32Bit(byteData));
		return 16;
	}
	return 0;
}
unsigned int i486DXCommon::FPUState::FILD_m64int(i486DXCommon &cpu,const unsigned char byteData[])
{
	if(true==enabled)
	{
		statusWord&=~STATUS_C1;
		Push(cpu,(double)IntFrom64Bit(byteData));
		return 16;
	}
	return 0;
}
unsigned int i486DXCommon::FPUState::FLD32(i486DXCommon &cpu,const unsigned char byteData[])
{
	if(true==enabled)
	{
		float f;
		uint32_t *i=(uint32_t *)&f;
		// Assuming same endiannness for int and float.
		*i=byteData[0]|(byteData[1]<<8)|(byteData[2]<<16)|(byteData[3]<<24);
		Push(cpu,(double)f);
		return 3;
	}
	return 0;
}
unsigned int i486DXCommon::FPUState::FLD64(i486DXCommon &cpu,const unsigned char byteData[])
{
	if(true==enabled)
	{
		// Hope this CPU uses IEEE format.
		const double *dataPtr=(const double *)byteData;
		Push(cpu,*dataPtr);
	#ifdef CHECK_FOR_NAN
		BreakOnNan(cpu,ST(cpu).value);
	#endif
		return 3;
	}
	return 0;
}
unsigned int i486DXCommon::FPUState::FLD80(i486DXCommon &cpu,const unsigned char byteData[])
{
	if(true==enabled)
	{
		// Hope this CPU uses IEEE format.
		Push(cpu,DoubleFrom80Bit(byteData));
	#ifdef CHECK_FOR_NAN
		BreakOnNan(cpu,ST(cpu).value);
	#endif
		return 6;
	}
	return 0;
}
unsigned int i486DXCommon::FPUState::FLDCW(i486DXCommon &cpu,uint16_t cw)
{
	if(true==enabled)
	{
		controlWord=cw;
	}
	return 4;
}
unsigned int i486DXCommon::FPUState::FLD_ST(i486DXCommon &cpu,int i)
{
	if(true==enabled)
	{
		auto &STi=ST(cpu,i);
		Push(cpu,STi.value);
	#ifdef CHECK_FOR_NAN
		BreakOnNan(cpu,STi.value);
	#endif
		return 4;
	}
	return 0; // Let it abort.
}
unsigned int i486DXCommon::FPUState::FLD1(i486DXCommon &cpu)
{
	if(true==enabled)
	{
		Push(cpu,1.0);
	#ifdef CHECK_FOR_NAN
		BreakOnNan(cpu,ST(cpu).value);
	#endif
		return 4;
	}
	return 0; // Let it abort.
}
unsigned int i486DXCommon::FPUState::FLDL2T(i486DXCommon &cpu)
{
	if(true==enabled)
	{
		Push(cpu,log2(10.0));
		return 8;
	}
	return 0; // Let it abort.
}
unsigned int i486DXCommon::FPUState::FLDL2E(i486DXCommon &cpu)
{
	if(true==enabled)
	{
		Push(cpu,log2(VALUE_OF_E));
		return 8;
	}
	return 0; // Let it abort.
}
unsigned int i486DXCommon::FPUState::FLDLN2(i486DXCommon &cpu)
{
	if(true==enabled)
	{
		Push(cpu,log(2.0));
		return 8;
	}
	return 0; // Let it abort.
}
unsigned int i486DXCommon::FPUState::FLDPI(i486DXCommon &cpu)
{
	if(true==enabled)
	{
		Push(cpu,VALUE_OF_PI);
		return 8;
	}
	return 0; // Let it abort.
}
unsigned int i486DXCommon::FPUState::FLDZ(i486DXCommon &cpu)
{
	if(true==enabled)
	{
		Push(cpu,0.0);
		return 4;
	}
	return 0;
}
unsigned int i486DXCommon::FPUState::FMULP(i486DXCommon &cpu,int i)
{
	if(true==enabled)
	{
		statusWord&=~STATUS_C1;

		auto &ST=this->ST(cpu);
		auto &ST1=this->ST(cpu,i);
		ST1.value=ST.value*ST1.value;
		Pop(cpu);
	#ifdef CHECK_FOR_NAN
		BreakOnNan(cpu,ST1.value);
	#endif

		return 16;
	}
	return 0; // Let it abort.
}
unsigned int i486DXCommon::FPUState::FMUL_ST_STi(i486DXCommon &cpu,int i)
{
	if(true==enabled)
	{
		statusWord&=~STATUS_C1;

		auto &ST=this->ST(cpu);
		auto &STi=this->ST(cpu,i);
		ST.value=ST.value*STi.value;
	#ifdef CHECK_FOR_NAN
		BreakOnNan(cpu,ST.value);
	#endif

		return 16;
	}
	return 0; // Let it abort.
}
unsigned int i486DXCommon::FPUState::FMUL_m32real(i486DXCommon &cpu,const unsigned char byteData[])
{
	if(true==enabled)
	{
		statusWord&=~STATUS_C1;

		auto src=DoubleFrom32Bit(byteData);
		ST(cpu).value*=src;

	#ifdef CHECK_FOR_NAN
		BreakOnNan(cpu,ST(cpu).value);
	#endif

		return 14;
	}
	return 0;
}
unsigned int i486DXCommon::FPUState::FMUL_m64real(i486DXCommon &cpu,const unsigned char byteData[])
{
	if(true==enabled)
	{
		statusWord&=~STATUS_C1;

		auto src=DoubleFrom64Bit(byteData);
		ST(cpu).value*=src;

	#ifdef CHECK_FOR_NAN
		BreakOnNan(cpu,ST(cpu).value);
	#endif

		return 14;
	}
	return 0;
}
unsigned int i486DXCommon::FPUState::FIMUL_m16int(i486DXCommon &cpu,const unsigned char byteData[])
{
	if(true==enabled)
	{
		statusWord&=~STATUS_C1;

		double src=(double)IntFrom16Bit(byteData);
		ST(cpu).value*=src;

		return 25;
	}
	return 0;
}
unsigned int i486DXCommon::FPUState::FIMUL_m32int(i486DXCommon &cpu,const unsigned char byteData[])
{
	if(true==enabled)
	{
		statusWord&=~STATUS_C1;

		double src=(double)IntFrom32Bit(byteData);
		ST(cpu).value*=src;

		return 25;
	}
	return 0;
}
unsigned int i486DXCommon::FPUState::FPATAN(i486DXCommon &cpu)
{
	if(true==enabled)
	{
		statusWord&=~STATUS_C1;

		auto &ST=this->ST(cpu);
		auto &ST1=this->ST(cpu,1);
		ST1.value=atan2(ST1.value,ST.value);
		Pop(cpu);

	#ifdef CHECK_FOR_NAN
		BreakOnNan(cpu,ST1.value);
	#endif

		return 84;
	}
	return 0; // Let it abort.
}
unsigned int i486DXCommon::FPUState::FPREM(i486DXCommon &cpu)
{
	if(true==enabled)
	{
		statusWord&=~(STATUS_C0|STATUS_C1|STATUS_C2|STATUS_C3);

		auto &ST=this->ST(cpu);
		auto &ST1=this->ST(cpu,1);
		if(0==ST1.value)
		{
			statusWord|=STATUS_C2;
			// Zero division
		}
		else
		{
			// x86 manual tells that it should set LEAST significant bits of the quotient in C0,C3,C1.
			// LEAST?  Is there any use?  I need to do math to make sense of it.
			// For the meantime, I ignore those 3 bits.

			// -> I see, I see.  It makes sense.  C0, C3, C1 should be taken from the quotient converted to integer.
			//    I was thinking to take least significant bits from fraction part of floating point,
			//    which for sure doesn't do any good.
			int64_t quo=(int64_t)fabs(ST.value/ST1.value);
			if(0!=(quo&1))
			{
				statusWord|=STATUS_C1;
			}
			if(0!=(quo&2))
			{
				statusWord|=STATUS_C3;
			}
			if(0!=(quo&4))
			{
				statusWord|=STATUS_C0;
			}
		}
		ST.value=fmod(ST.value,ST1.value);

	#ifdef CHECK_FOR_NAN
		BreakOnNan(cpu,ST.value);
	#endif

		return 84;
	}
	return 0; // Let it abort.
}
unsigned int i486DXCommon::FPUState::FRNDINT(i486DXCommon &cpu)
{
	if(true==enabled)
	{
		statusWord&=~STATUS_C1;

		auto &ST=this->ST(cpu);
		ST.value=RoundToInteger(ST.value);

	#ifdef CHECK_FOR_NAN
		BreakOnNan(cpu,ST.value);
	#endif

		return 241;
	}
	return 0; // Let it abort.
}
unsigned int i486DXCommon::FPUState::FRSTOR(const i486DXCommon &cpu,unsigned int operandSize,const uint8_t data[])
{
	unsigned int STPtr=RestoreEnv(operandSize,cpu.IsInRealMode(),data);
	for(int i=0; i<STACK_LEN; ++i)
	{
		stack[i].value=DoubleFrom80Bit(data+STPtr);
		STPtr+=10;
	}
	return 131;
}
unsigned int i486DXCommon::FPUState::RestoreEnv(unsigned int operandSize,bool isInRealMode,const uint8_t data[])
{
	// 14 bytes or 28 bytes depends on operand size?  Figure 15-5 through 15-8 of i486 Programmer's Reference Manual 1990 for layout.
	// WTF!? Four types of layouts?  Intel designers were really creative in the evil way.
	// Protected Mode && 32-bit (Figure 15-5)
	//   RESERVED | CONTROL WORD     +0H
	//   RESERVED | STATUS WORD      +4H
	//   RESERVED | TAG WORD         +8H
	//        IP OFFSET              +CH
	//   RESERVED | CS selector      +10H
	//       Data Operand Offset     +14H
	//   RESERVED | Operand Selector +18H
	if(true!=isInRealMode && 32==operandSize)
	{
		controlWord=cpputil::GetDword(data);
		statusWord =cpputil::GetDword(data+ 4);
		tagWord    =cpputil::GetDword(data+ 8);
		// cpputil::GetDword(data+12);
		// cpputil::GetDword(data+16);
		// cpputil::GetDword(data+20);
		// cpputil::GetDword(data+24);
		return 28;
	}
	// Real Mode && 32-bit (Figure 15-6)
	//   RESERVED | CONTROL WORD                       +0H
	//   RESERVED | STATUS WORD                        +4H
	//   RESERVED | TAG WORD                           +8H
	//   RESERVED | IP OFFSET                          +CH
	//   0000 (IP 16bits) 0 (Opcode 11bits)            +10H
	//   RESERVED | Data Operand Offset                +14H
	//   0000 (Operand Selector 16bits) 000000000000   +18H
	else if(true==isInRealMode && 32==operandSize)
	{
		controlWord=cpputil::GetDword(data   );
		statusWord =cpputil::GetDword(data+ 4);
		tagWord    =cpputil::GetDword(data+ 8);
		// cpputil::GetDword(data+12);
		// cpputil::GetDword(data+16);
		// cpputil::GetDword(data+20);
		// cpputil::GetDword(data+24);
		return 28;
	}
	// Protected Mode && 16-bit (Figure 15-7)
	//   CONTROL WORD            +0H
	//   STATUS WORD             +2H
	//   TAG WORD                +4H
	//   IP OFFSET               +6H
	//   CS selector             +8H
	//   Data Operand Offset     +AH
	//   Operand Selector        +CH
	else if(true!=isInRealMode && 16==operandSize)
	{
		controlWord=cpputil::GetWord(data   );
		statusWord =cpputil::GetWord(data+ 2);
		tagWord    =cpputil::GetWord(data+ 4);
		//cpputil::GetWord(data+ 6);
		//cpputil::GetWord(data+ 8);
		//cpputil::GetWord(data+10);
		//cpputil::GetWord(data+12);
		return 14;
	}
	// Real Mode && 16-bit (Figure 15-8)
	//   CONTROL WORD                     +0H
	//   STATUS WORD                      +2H
	//   TAG WORD                         +4H
	//   IP OFFSET                        +6H
	//   IP(high-4bit) 0 Opcode(11 bits)  +8H
	//   Data Operand Offset              +AH
	//   DP(high 4-bit) <- 0 ->           +CH
	else if(true==isInRealMode && 16==operandSize)
	{
		controlWord=cpputil::GetWord(data   );
		statusWord =cpputil::GetWord(data+ 2);
		tagWord    =cpputil::GetWord(data+ 4);
		//cpputil::GetWord(data+ 6);
		//cpputil::GetWord(data+ 8);
		//cpputil::GetWord(data+10);
		//cpputil::GetWord(data+12);
		return 14;
	}
}
std::vector <uint8_t> i486DXCommon::FPUState::FSAVE(const i486DXCommon &cpu,unsigned int operandSize) const
{
	std::vector <uint8_t> data;
	if(16==operandSize)
	{
		data.resize(94);
	}
	else
	{
		data.resize(108);
	}
	unsigned int STPtr=PopulateFPUEnv(data.data(),operandSize,cpu.IsInRealMode());
	for(int i=0; i<STACK_LEN; ++i)
	{
		DoubleTo80Bit(data.data()+STPtr,stack[i].value);
		STPtr+=10;
	}
	if(data.size()!=STPtr)
	{
		std::cout << "FSAVE Error!" << std::endl;
	}
	return data;
}
std::vector <uint8_t> i486DXCommon::FPUState::FNSTENV(const i486DXCommon &cpu,unsigned int operandSize) const
{
	std::vector <uint8_t> data;
	if(16==operandSize)
	{
		data.resize(14);
	}
	else
	{
		data.resize(28);
	}
	unsigned int size=PopulateFPUEnv(data.data(),operandSize,cpu.IsInRealMode());
	if(data.size()!=size)
	{
		std::cout << "FNSTENV Error!" << std::endl;
	}
	return data;
}
unsigned int  i486DXCommon::FPUState::PopulateFPUEnv(uint8_t *data,unsigned int operandSize,bool isInRealMode) const
{
	// 14 bytes or 28 bytes depends on operand size?  Figure 15-5 through 15-8 of i486 Programmer's Reference Manual 1990 for layout.
	// WTF!? Four types of layouts?  Intel designers were really creative in the evil way.
	// Protected Mode && 32-bit (Figure 15-5)
	//   RESERVED | CONTROL WORD     +0H
	//   RESERVED | STATUS WORD      +4H
	//   RESERVED | TAG WORD         +8H
	//        IP OFFSET              +CH
	//   RESERVED | CS selector      +10H
	//       Data Operand Offset     +14H
	//   RESERVED | Operand Selector +18H
	if(true!=isInRealMode && 32==operandSize)
	{
		cpputil::PutDword(data   ,controlWord);
		cpputil::PutDword(data+ 4,statusWord);
		cpputil::PutDword(data+ 8,tagWord);
		cpputil::PutDword(data+12,0);
		cpputil::PutDword(data+16,0);
		cpputil::PutDword(data+20,0);
		cpputil::PutDword(data+24,0);
		return 28;
	}
	// Real Mode && 32-bit (Figure 15-6)
	//   RESERVED | CONTROL WORD                       +0H
	//   RESERVED | STATUS WORD                        +4H
	//   RESERVED | TAG WORD                           +8H
	//   RESERVED | IP OFFSET                          +CH
	//   0000 (IP 16bits) 0 (Opcode 11bits)            +10H
	//   RESERVED | Data Operand Offset                +14H
	//   0000 (Operand Selector 16bits) 000000000000   +18H
	else if(true==isInRealMode && 32==operandSize)
	{
		cpputil::PutDword(data   ,controlWord);
		cpputil::PutDword(data+ 4,statusWord);
		cpputil::PutDword(data+ 8,tagWord);
		cpputil::PutDword(data+12,0);
		cpputil::PutDword(data+16,0);
		cpputil::PutDword(data+20,0);
		cpputil::PutDword(data+24,0);
		return 28;
	}
	// Protected Mode && 16-bit (Figure 15-7)
	//   CONTROL WORD            +0H
	//   STATUS WORD             +2H
	//   TAG WORD                +4H
	//   IP OFFSET               +6H
	//   CS selector             +8H
	//   Data Operand Offset     +AH
	//   Operand Selector        +CH
	else if(true!=isInRealMode && 16==operandSize)
	{
		cpputil::PutWord(data   ,controlWord);
		cpputil::PutWord(data+ 2,statusWord);
		cpputil::PutWord(data+ 4,tagWord);
		cpputil::PutWord(data+ 6,0);
		cpputil::PutWord(data+ 8,0);
		cpputil::PutWord(data+10,0);
		cpputil::PutWord(data+12,0);
		return 14;
	}
	// Real Mode && 16-bit (Figure 15-8)
	//   CONTROL WORD                     +0H
	//   STATUS WORD                      +2H
	//   TAG WORD                         +4H
	//   IP OFFSET                        +6H
	//   IP(high-4bit) 0 Opcode(11 bits)  +8H
	//   Data Operand Offset              +AH
	//   DP(high 4-bit) <- 0 ->           +CH
	else if(true==isInRealMode && 16==operandSize)
	{
		cpputil::PutWord(data   ,controlWord);
		cpputil::PutWord(data+ 2,statusWord);
		cpputil::PutWord(data+ 4,tagWord);
		cpputil::PutWord(data+ 6,0);
		cpputil::PutWord(data+ 8,0);
		cpputil::PutWord(data+10,0);
		cpputil::PutWord(data+12,0);
		return 14;
	}
}
unsigned int i486DXCommon::FPUState::FSCALE(i486DXCommon &cpu)
{
	if(true==enabled)
	{
		statusWord&=~STATUS_C1;
		// Intel 80486 Programmer's Reference Manual 26-125
		// If the value is not integral, FSCALE uses the nearest integer smaller in magnitude.
		auto &ST=this->ST(cpu);
		auto &ST1=this->ST(cpu,1);
		int p=(int)ST1.value;
		ST.value*=pow(2.0,p);

	#ifdef CHECK_FOR_NAN
		BreakOnNan(cpu,ST.value);
	#endif

		return 31;
	}
	return 0; // Let it abort.
}
unsigned int i486DXCommon::FPUState::FSIN(i486DXCommon &cpu)
{
	if(true==enabled)
	{
		statusWord&=~(STATUS_C1|STATUS_C2);

		auto &ST=this->ST(cpu);
		ST.value=sin(ST.value);

	#ifdef CHECK_FOR_NAN
		BreakOnNan(cpu,ST.value);
	#endif

		return 241;
	}
	return 0; // Let it abort.
}
unsigned int i486DXCommon::FPUState::FSINCOS(i486DXCommon &cpu)
{
	if(true==enabled)
	{
		statusWord&=~(STATUS_C1|STATUS_C2);

		auto &ST=this->ST(cpu);
		auto c=cos(ST.value);
		ST.value=sin(ST.value);
		Push(cpu,c);

	#ifdef CHECK_FOR_NAN
		BreakOnNan(cpu,ST.value);
		BreakOnNan(cpu,this->ST(cpu).value);
	#endif

		return 291;
	}
	return 0;
}
unsigned int i486DXCommon::FPUState::FCOS(i486DXCommon &cpu)
{
	if(true==enabled)
	{
		statusWord&=~(STATUS_C1|STATUS_C2);

		auto &ST=this->ST(cpu);
		ST.value=cos(ST.value);

	#ifdef CHECK_FOR_NAN
		BreakOnNan(cpu,ST.value);
	#endif

		return 241;
	}
	return 0; // Let it abort.
}
unsigned int i486DXCommon::FPUState::FPTAN(i486DXCommon &cpu)
{
	if(true==enabled)
	{
		statusWord&=~STATUS_C1;

		auto &ST=this->ST(cpu);
		auto t=tan(ST.value);
		if(true==isnan(t))
		{
			statusWord|=STATUS_C2;
		}
		else
		{
			statusWord&=~STATUS_C2;
			ST.value=tan(ST.value);
			Push(cpu,1.0); // I have no idea why it does it.
		}

	#ifdef CHECK_FOR_NAN
		BreakOnNan(cpu,ST.value);
	#endif

		return 244;
	}
	return 0; // Let it abort.
}
unsigned int i486DXCommon::FPUState::FSQRT(i486DXCommon &cpu)
{
	if(true==enabled)
	{
		statusWord&=~STATUS_C1;

		ST(cpu).value=sqrt(ST(cpu).value);

	#ifdef CHECK_FOR_NAN
		BreakOnNan(cpu,ST(cpu).value);
	#endif

		return 70;
	}
	return 0;
}
unsigned int i486DXCommon::FPUState::FSTP_STi(i486DXCommon &cpu,int i)
{
	if(true==enabled)
	{
		auto &st=ST(cpu);
		auto &sti=ST(cpu,i);
		sti=st;
		Pop(cpu);

		return 3;
	}
	return 0;
}
unsigned int i486DXCommon::FPUState::FSUB_m32real(i486DXCommon &cpu,const unsigned char byteData[])
{
	if(true==enabled)
	{
		statusWord&=~STATUS_C1;

		auto src=DoubleFrom32Bit(byteData);
		auto &st=ST(cpu);
		st.value=st.value-src;

	#ifdef CHECK_FOR_NAN
		BreakOnNan(cpu,st.value);
	#endif

		return 10;
	}
	return 0;
}
unsigned int i486DXCommon::FPUState::FSUB_m64real(i486DXCommon &cpu,const unsigned char byteData[])
{
	if(true==enabled)
	{
		statusWord&=~STATUS_C1;

		auto src=DoubleFrom64Bit(byteData);
		auto &st=ST(cpu);
		st.value=st.value-src;

	#ifdef CHECK_FOR_NAN
		BreakOnNan(cpu,st.value);
	#endif

		return 10;
	}
	return 0;
}
unsigned int i486DXCommon::FPUState::FSUB_ST_STi(i486DXCommon &cpu,int i)
{
	if(true==enabled)
	{
		statusWord&=~STATUS_C1;
		auto &ST=this->ST(cpu);
		auto &STi=this->ST(cpu,i);
		ST.value=ST.value-STi.value;
		return 10;
	}
	return 0;
}
unsigned int i486DXCommon::FPUState::FSUB_STi_ST(i486DXCommon &cpu,int i)
{
	if(true==enabled)
	{
		statusWord&=~STATUS_C1;
		auto &ST=this->ST(cpu);
		auto &STi=this->ST(cpu,i);
		STi.value=STi.value-ST.value;
		return 10;
	}
	return 0;
}
unsigned int i486DXCommon::FPUState::FSUBP_STi_ST(i486DXCommon &cpu,int i)
{
	if(true==enabled)
	{
		statusWord&=~STATUS_C1;
		auto &ST=this->ST(cpu);
		auto &STi=this->ST(cpu,i);
		STi.value=STi.value-ST.value;
		Pop(cpu);
		return 10;
	}
	return 0;
}
unsigned int i486DXCommon::FPUState::FISUB_m16int(i486DXCommon& cpu, const unsigned char byteData[])
{
	if (true == enabled)
	{
		statusWord &= ~STATUS_C1;

		double src = (double)IntFrom16Bit(byteData);
		ST(cpu).value -= src;

		return 24;
	}
	return 0;
}
unsigned int i486DXCommon::FPUState::FSUBR_m32real(i486DXCommon &cpu,const unsigned char byteData[])
{
	if(true==enabled)
	{
		statusWord&=~STATUS_C1;

		auto src=DoubleFrom32Bit(byteData);
		auto &st=ST(cpu);
		st.value=src-st.value;

		return 10;
	}
	return 0;
}
unsigned int i486DXCommon::FPUState::FSUBR_m64real(i486DXCommon &cpu,const unsigned char byteData[])
{
	if(true==enabled)
	{
		statusWord&=~STATUS_C1;

		auto src=DoubleFrom64Bit(byteData);
		auto &st=ST(cpu);
		st.value=src-st.value;

		return 10;
	}
	return 0;
}
unsigned int i486DXCommon::FPUState::FSUBRP_STi_ST(i486DXCommon &cpu,int i)
{
	if(true==enabled)
	{
		statusWord&=~STATUS_C1;
		auto &ST=this->ST(cpu);
		auto &STi=this->ST(cpu,i);
		STi.value=ST.value-STi.value;
		Pop(cpu);
		return 10;
	}
	return 0;
}
unsigned int i486DXCommon::FPUState::FTST(i486DXCommon &cpu)
{
	if(true==enabled)
	{
		statusWord&=~STATUS_C1;
		Compare(ST(cpu).value,0.0);
		return 4;
	}
	return 0;
}
unsigned int i486DXCommon::FPUState::FXAM(i486DXCommon &cpu)
{
	if(true==enabled)
	{
		statusWord&=~(STATUS_C0|STATUS_C1|STATUS_C2|STATUS_C3);
		if(0==NumFilled())
		{
			// Empty      C3,C2,C0=100
			statusWord|=STATUS_C3;
		}
		else
		{
			auto &st=ST(cpu);
			if(0.0>st.value)
			{
				statusWord|=STATUS_C1;
			}
			if(isnan(st.value))
			{
				// Nan C3,C2,C0=001
				statusWord|=STATUS_C0;
			}
			else if(INFINITY==st.value || -INFINITY==st.value)
			{
				statusWord|=STATUS_C2|STATUS_C0;
			}
			// Unsupported C3,C2,C0=000
			// Infinity    C3,C2,C0=011
			// Denormal    C3,C2,C0=110
			else if(0==st.value)
			{
				// Zero        C3,C2,C0=100
				statusWord|=STATUS_C3;
			}
			else
			{
				// Normal      C3,C2,C0=010
				statusWord|=STATUS_C2;
			}
		}
		return 8;
	}
	return 0;
}
unsigned int i486DXCommon::FPUState::FXCH(i486DXCommon &cpu,int i)
{
	if(true==enabled)
	{
		statusWord&=~STATUS_C1;
		std::swap(ST(cpu),ST(cpu,i));
		return 4;
	}
	return 0;
}
unsigned int i486DXCommon::FPUState::FYL2X(i486DXCommon &cpu)
{
	if(true==enabled)
	{
		statusWord&=~STATUS_C1;
		auto &st=ST(cpu);
		auto &st1=ST(cpu,1);
		st1.value=log2(st.value)*st1.value;
		Pop(cpu);

	#ifdef CHECK_FOR_NAN
		BreakOnNan(cpu,st1.value);
	#endif

		return 311;
	}
	return 0;
}
