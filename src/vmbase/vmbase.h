#ifndef VMBASE_IS_INCLUDED
#define VMBASE_IS_INCLUDED
/* { */



#include <string>

class VMBase
{
public:
	mutable bool vmAbort=false;
	mutable std::string vmAbortDeviceName,vmAbortReason;

	void Abort(std::string devName,std::string abortReason);

	inline bool CheckAbort(void) const
	{
		return vmAbort;
	}
};



/* } */
#endif
