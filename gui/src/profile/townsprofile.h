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
		MAX_NUM_VIRTUALKEYS=20,
	};


	// autoStart flag is unique to TownsProfile.  Not in TownsStartParameters.
	bool autoStart=false;

	// separateProcess flag is unique to TownsProfile.  Not in TownsStartParameters.
	bool separateProcess=false;

	std::string errorMsg;

	TownsProfile();
	void CleanUp(void);
	std::vector <std::string> Serialize(void) const;
	bool Deserialize(const std::vector <std::string> &text);

	/*! The first arg will be "Tsugaru_CUI".  Should be replaced with the full-path name.
	*/
	std::vector <std::string> MakeArgv(void) const;

	/*! Change file and directory names relative to the base directory.
	    Example:
	      MakeRelativePath("C:/Users/captainys/FMTOWNS","${profiledir}");
	*/
	void MakeRelativePath(std::string baseDir,std::string alias);

	void MakeRelative(std::string &fName,std::string baseDir,std::string alias);
};

/* } */
#endif
