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
	enum
	{
		MAX_NUM_SCSI_DEVICE=7,
	};
	std::string FDImgFile[2];
	bool FDWriteProtect[2];
	std::string SCSIImgFile[MAX_NUM_SCSI_DEVICE];
	bool autoStart=false;

	bool useStrikeCommanderThrottleAxis=false;

	class VirtualKey
	{
	public:
		unsigned int townsKey;
		int physId=-1;
		unsigned int button;
	};
	enum
	{
		MAX_NUM_VIRTUALKEYS=10,
	};
	VirtualKey virtualKeys[MAX_NUM_VIRTUALKEYS];


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
