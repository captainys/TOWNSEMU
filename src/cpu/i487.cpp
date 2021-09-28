/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include "i486.h"



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
