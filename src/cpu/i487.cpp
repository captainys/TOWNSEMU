#include "i486.h"



void i486DX::FPUState::FNINIT(void)
{
}
bool i486DX::FPUState::ExceptionPending(void) const
{
	return false;// Tentative 
}
unsigned int i486DX::FPUState::GetStatusWord(void) const
{
	return 0;
}
