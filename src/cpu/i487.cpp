/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include "i486.h"
#include <math.h>
#include <stdint.h>



/* static */ void i486DX::FPUState::DoubleTo80Bit(OperandValueBase &value80,double src)
{
	// Reference  https://en.wikipedia.org/wiki/Double-precision_floating-point_format

	// Assume sizeof(double) is 8-byte long.
	uint64_t binary=*((uint64_t *)&src);
	uint16_t exponent=((binary>>52)&2047);   // 1023=2^0
	uint64_t fraction=(binary&((1LL<<52)-1));
	unsigned char signBit=((binary>>63)<<7);

	// In 80-bit format, fraction needs to be expanded to 64-bit
	// Exponent 16383 is 2^0.
	exponent=exponent+16383-1023;
	fraction<<=11;
	fraction|=(1LL<<63);  // Integer bit.

	// It doesn't handle positive/negative infinity and NaN yet.

	value80.numBytes=10;
#ifdef YS_LITTLE_ENDIAN
	*((uint16_t *)(value80.byteData+8))=exponent;
	*((uint64_t *)value80.byteData)=fraction;
	value80.byteData[9]|=signBit;
#else
	value80.byteData[0]=( fraction     &255);
	value80.byteData[1]=((fraction>> 8)&255);
	value80.byteData[2]=((fraction>>16)&255);
	value80.byteData[3]=((fraction>>24)&255);
	value80.byteData[4]=((fraction>>32)&255);
	value80.byteData[5]=((fraction>>40)&255);
	value80.byteData[6]=((fraction>>48)&255);
	value80.byteData[7]=((fraction>>56)&255);
	value80.byteData[8]=(exponent&255);
	value80.byteData[9]=(((exponent>>8)&255)|signBit);
#endif
}
/* static */ double i486DX::FPUState::DoubleFrom80Bit(const OperandValueBase &value80)
{
	return DoubleFrom80Bit(value80.byteData);
}
/* static */ double i486DX::FPUState::DoubleFrom80Bit(const unsigned char byteData[])
{
	uint16_t exponent;
	uint64_t fraction;
	uint16_t signBit;

	signBit=(byteData[9]&0x80);

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

i486DX::FPUState::FPUState()
{
	enabled=false; // Tentative.
}
void i486DX::FPUState::FNINIT(void)
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
		controlWord=0x037F;
		statusWord=0;
		tagWord=0xFFFF;
	}
}
bool i486DX::FPUState::ExceptionPending(void) const
{
	return false;// Tentative 
}
unsigned int i486DX::FPUState::GetStatusWord(void) const
{
	if(true==enabled)
	{
		return statusWord;
	}
	else
	{
		return 0xffff;
	}
}
unsigned int i486DX::FPUState::GetControlWord(void) const
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
unsigned int i486DX::FPUState::GetRC(void) const
{
	return (GetControlWord()>>10)&3;
}
void i486DX::FPUState::GetSTAsDouble(class i486DX &cpu,class OperandValueBase &value)
{
	if(0<stackPtr)
	{
		double *doublePtr=(double *)value.byteData;
		*doublePtr=ST(cpu).value;
		value.numBytes=8;
	}
	else
	{
		// Raise NM fault.
	}
}
void i486DX::FPUState::GetSTAsSignedInt(class i486DX &cpu,class OperandValueBase &value)
{
	if(0<stackPtr)
	{
		uint64_t i=0;
		double d=ST(cpu).value;
		switch(GetRC())
		{
		case 0: // Round to Nearest or Even
			if(d<0.0)
			{
				d-=0.5;
			}
			break;
		case 1: // Round Down (Toward -INF)
			if(d<0.0)
			{
				d-=1.0;
			}
			break;
		case 2: // Round Up (Toward +INF)
			if(0.0<d)
			{
				d+=1.0;
			}
			break;
		case 3: // Chop (Truncate)
			break;
		}
		i=(uint64_t)d;
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
	else
	{
		// Raise NM fault.
	}
}
void i486DX::FPUState::GetSTAs80BitBCD(class i486DX &cpu,OperandValueBase &value)
{
	if(0<stackPtr)
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
	else
	{
		// Raise NM fault.
	}
}
bool i486DX::FPUState::Push(double value)
{
	if(stackPtr<STACK_LEN)
	{
		stack[stackPtr].value=value;
		stack[stackPtr].tag=0; // Should I do this?
		++stackPtr;
		return true;
	}
	return false; // Should shoot an exception for this.
}
i486DX::FPUState::Stack i486DX::FPUState::Pop(void)
{
	if(0<stackPtr)
	{
		auto ret=stack[stackPtr-1];
		--stackPtr;
		return ret;
	}
	return stack[0]; // Should shoot an exception for this.
}

std::vector <std::string> i486DX::FPUState::GetStateText(void) const
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

	for(int i=0; i<stackPtr; ++i)
	{
		char fmt[256];
		sprintf(fmt,"[%d] %32.10lf %02x",i,stack[i].value,stack[i].tag);
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

unsigned int i486DX::FPUState::FADD64(i486DX &cpu,const unsigned char byteData[])
{
	if(true==enabled)
	{
		// Hope this CPU uses IEEE format.
		const double *dataPtr=(const double *)byteData;
		if(0<stackPtr)
		{
			ST(cpu).value+=*dataPtr;
		}
		else
		{
			// Raise NM exception.
		}
		return 3;
	}
	return 0;
}
unsigned int i486DX::FPUState::FADDP_STi_ST(i486DX &cpu,int i)
{
	if(true==enabled)
	{
		if(i<stackPtr)
		{
			auto &ST=this->ST(cpu);
			auto &STi=this->ST(cpu,i);
			STi.value+=STi.value;
			Pop();
		}
		return 10;
	}
	return 0; // Let it abort.
}
unsigned int i486DX::FPUState::FCHS(i486DX &cpu)
{
	if(true==enabled)
	{
		if(0<stackPtr)
		{
			stack[stackPtr-1].value=-stack[stackPtr-1].value;
			return 4;
		}
		else
		{
			// Raise NM exception.
		}
	}
	return 0; // Let it abort.
}
unsigned int i486DX::FPUState::FCOMPP(i486DX &cpu)
{
	if(true==enabled)
	{
		if(2<=stackPtr)
		{
			Compare(ST(cpu).value,ST(cpu,1).value);
			stackPtr-=2;
			return 5;
		}
		else
		{
			// Raise NM exception.
		}
	}
	return 0; // Let it abort.
}
unsigned int i486DX::FPUState::FDIV(i486DX &cpu)
{
	if(true==enabled)
	{
		if(2<=stackPtr)
		{
			auto &ST=this->ST(cpu);
			auto &ST1=this->ST(cpu,1);
			if(0.0==ST1.value)
			{
				// Zero division.
			}
			ST1.value=ST.value/ST1.value; // Let it be a NaN if ST1.value is zero.
			Pop();
		}
		return 70;
	}
	return 0; // Let it abort.
}
unsigned int i486DX::FPUState::FDIVRP_STi_ST(i486DX &cpu,int i)
{
	if(true==enabled)
	{
		if(i<stackPtr)
		{
			auto &ST=this->ST(cpu);
			auto &STi=this->ST(cpu,i);
			if(0.0==ST.value)
			{
				// Zero division.
			}
			STi.value=STi.value/ST.value; // Let it be a NaN if ST1.value is zero.
			Pop();
		}
		return 70;
	}
	return 0; // Let it abort.
}
unsigned int i486DX::FPUState::FLD32(i486DX &cpu,const unsigned char byteData[])
{
	if(true==enabled)
	{
		float f;
		uint32_t *i=(uint32_t *)&f;
		// Assuming same endiannness for int and float.
		*i=byteData[0]|(byteData[1]<<8)|(byteData[2]<<16)|(byteData[3]<<24);
		Push((double)f);
		return 3;
	}
	return 0;
}
unsigned int i486DX::FPUState::FLD64(i486DX &cpu,const unsigned char byteData[])
{
	if(true==enabled)
	{
		// Hope this CPU uses IEEE format.
		const double *dataPtr=(const double *)byteData;
		Push(*dataPtr);
		return 3;
	}
	return 0;
}
unsigned int i486DX::FPUState::FLD80(i486DX &cpu,const unsigned char byteData[])
{
	if(true==enabled)
	{
		// Hope this CPU uses IEEE format.
		Push(DoubleFrom80Bit(byteData));
		return 6;
	}
	return 0;
}
unsigned int i486DX::FPUState::FLDCW(i486DX &cpu,uint16_t cw)
{
	if(true==enabled)
	{
		controlWord=cw;
	}
	return 4;
}
unsigned int i486DX::FPUState::FLD_ST(i486DX &cpu,int i)
{
	if(true==enabled)
	{
		if(i<stackPtr)
		{
			auto &STi=ST(cpu,i);
			Push(STi.value);
		}
		return 4;
	}
	return 0; // Let it abort.
}
unsigned int i486DX::FPUState::FLD1(i486DX &cpuState)
{
	if(true==enabled)
	{
		Push(1.0);
		return 4;
	}
	return 0; // Let it abort.
}
unsigned int i486DX::FPUState::FLDL2T(i486DX &cpu)
{
	if(true==enabled)
	{
		Push(log2(10.0));
		return 8;
	}
	return 0; // Let it abort.
}
unsigned int i486DX::FPUState::FLDZ(i486DX &cpu)
{
	if(true==enabled)
	{
		Push(0.0);
		return 4;
	}
	return 0;
}
unsigned int i486DX::FPUState::FMUL(i486DX &cpu)
{
	if(true==enabled)
	{
		if(1<=stackPtr)
		{
			auto &ST=this->ST(cpu);
			auto &ST1=this->ST(cpu,1);
			ST1.value=ST.value*ST1.value;
			Pop();
		}
		return 70;
	}
	return 0; // Let it abort.
}
unsigned int i486DX::FPUState::FSTP_STi(i486DX &cpu,int i)
{
	if(true==enabled)
	{
		if(0<stackPtr)
		{
			auto &st=ST(cpu);
			auto &sti=ST(cpu,i);
			sti=st;
			Pop();
		}
		else
		{
			// Raise NM exception
		}
		return 3;
	}
	return 0;
}
unsigned int i486DX::FPUState::FXAM(i486DX &cpu)
{
	if(true==enabled)
	{
		statusWord&=~(STATUS_C0|STATUS_C1|STATUS_C2|STATUS_C3);
		if(0==stackPtr)
		{
			// Empty      C3,C2,C0=100
			statusWord|=(STATUS_C3|STATUS_C0);
		}
		else
		{
			auto &st=ST(cpu);
			if(isnan(st.value))
			{
				// Nan C3,C2,C0=001
				statusWord|=STATUS_C0;
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
