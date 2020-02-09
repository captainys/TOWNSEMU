#include "i486.h"



i486DX::FPUState::FPUState()
{
	FPUEnabled=false; // Tentative.
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
}
bool i486DX::FPUState::ExceptionPending(void) const
{
	return false;// Tentative 
}
unsigned int i486DX::FPUState::GetStatusWord(void) const
{
	return 0xffff;
}
unsigned int i486DX::FPUState::GetControlWord(void) const
{
	return 0xffff;
}
