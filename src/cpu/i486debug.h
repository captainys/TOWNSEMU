#ifndef I486DEBUG_IS_INCLUDED
#define I486DEBUG_IS_INCLUDED
/* { */


#include <set>
#include "i486.h"

class i486Debugger
{
public:
	class CS_EIP
	{
	public:
		unsigned int CS;
		unsigned int EIP;

		inline unsigned long long int Combine(void) const
		{
			unsigned long long LCS=CS;
			return (LCS<<32)|EIP;
		}
		void Nullify(void);

		bool operator==(const CS_EIP rv) const
		{
			return this->Combine()==rv.Combine();
		}
		bool operator!=(const CS_EIP rv) const
		{
			return this->Combine()!=rv.Combine();
		}
		bool operator<(const CS_EIP rv) const
		{
			return this->Combine()<rv.Combine();
		}
		bool operator>(const CS_EIP rv) const
		{
			return this->Combine()>rv.Combine();
		}
		bool operator<=(const CS_EIP rv) const
		{
			return this->Combine()<=rv.Combine();
		}
		bool operator>=(const CS_EIP rv) const
		{
			return this->Combine()>=rv.Combine();
		}
	};
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

	unsigned int RunOneInstruction(i486DX &cpu,Memory &mem,InOut &io);
};


/* } */
#endif
