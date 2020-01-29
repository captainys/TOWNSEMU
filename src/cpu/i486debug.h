#ifndef I486DEBUG_IS_INCLUDED
#define I486DEBUG_IS_INCLUDED
/* { */


#include <set>
#include "i486.h"

class i486Debugger
{
public:
	typedef i486DX::FarPointer CS_EIP;

	std::set <CS_EIP> breakPoint;
	CS_EIP oneTimeBreakPoint;

	bool stop;

	bool disassembleEveryStep;

	CS_EIP lastDisassembleAddr;


	i486Debugger();
	void CleanUp(void);

	void AddBreakPoint(unsigned int CS,unsigned int EIP);
	void RemoveBreakPoint(unsigned int CS,unsigned int EIP);

	void SetOneTimeBreakPoint(unsigned int CS,unsigned int EIP);

	/*! Callback from i486DX::RunOneInstruction.
	*/
	void BeforeRunOneInstruction(i486DX &cpu,Memory &mem,InOut &io,const i486DX::Instruction &inst);

	/*! Callback from i486DX::RunOneInstruction.
	*/
	void AfterRunOneInstruction(unsigned int clocksPassed,i486DX &cpu,Memory &mem,InOut &io,const i486DX::Instruction &inst);
};


/* } */
#endif
