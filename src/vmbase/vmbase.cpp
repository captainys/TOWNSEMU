#include "vmbase.h"



void VMBase::Abort(std::string devName,std::string abortReason)
{
	vmAbort=true;
	vmAbortDeviceName=devName;
	vmAbortReason=abortReason;
}
