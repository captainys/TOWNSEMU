#ifndef I486DEBUG_IS_INCLUDED
#define I486DEBUG_IS_INCLUDED
/* { */


#include <set>
#include <map>
#include "i486.h"

class i486Debugger
{
public:
	typedef i486DX::FarPointer CS_EIP;

	std::set <CS_EIP> breakPoint;
	CS_EIP oneTimeBreakPoint;
	std::string externalBreakReason;
	std::map <unsigned int,std::string> ioLabel;

	bool stop;
	bool monitorIO;
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

	/*! Return formatted call-stack text.
	*/
	std::vector <std::string> GetCallStackText(const i486DX &cpu) const;

	/*! Break from an external reason.
	    It sets stop==1 and externalBreakReason=reason.
	*/
	void ExternalBreak(const std::string &reason);

	/*! Clear stop flag and externalBreakReason.
	*/
	void ClearStopFlag(void);

	/*! Called from i486DX class when a byte is written to I/O.
	*/
	void IOWriteByte(unsigned int ioport,unsigned int data);

	/*! Called from i486DX class when a word is written to I/O.
	*/
	void IOWriteWord(unsigned int ioport,unsigned int data);

	/*! Called from i486DX class when a word is written to I/O.
	*/
	void IOWriteDword(unsigned int ioport,unsigned int data);

	/*! Called from i486DX class when a byte is read from I/O.
	*/
	void IOReadByte(unsigned int ioport,unsigned int data);

	/*! Called from i486DX class when a byte is read from I/O.
	*/
	void IOReadWord(unsigned int ioport,unsigned int data);

	/*! Called from i486DX class when a byte is read from I/O.
	*/
	void IOReadDword(unsigned int ioport,unsigned int data);
};


/* } */
#endif
