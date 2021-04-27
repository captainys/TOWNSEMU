#ifndef TOWNSPROFILE_IS_INCLUDED
#define TOWNSPROFILE_IS_INCLUDED
/* { */

#include <vector>
#include <string>

#include "townsdef.h"
#include "townsparam.h"

class TownsProfile : public TownsStartParameters
{
public:
	// MAX_NUM_VIRTUALKEYS is limitation for GUI environment only.
	enum
	{
		MAX_NUM_VIRTUALKEYS=10,
	};
	bool useStrikeCommanderThrottleAxis=false;


	// autoStart flag is unique to TownsProfile.  Not in TownsStartParameters.
	bool autoStart=false;

	std::string errorMsg;

	TownsProfile();
	void CleanUp(void);
	std::vector <std::string> Serialize(void) const;
	bool Deserialize(const std::vector <std::string> &text);

	/*! The first arg will be "Tsugaru_CUI".  Should be replaced with the full-path name.
	*/
	std::vector <std::string> MakeArgv(void) const;
};

/* } */
#endif
