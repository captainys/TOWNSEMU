#ifndef TOWNSPROFILE_IS_INCLUDED
#define TOWNSPROFILE_IS_INCLUDED
/* { */

#include <vector>
#include <string>

#include "townsdef.h"

class TownsProfile
{
public:
	enum
	{
		NUM_STANDBY_FDIMG=4,
		MAX_NUM_SCSI_DEVICE=7,
	};
	std::string ROMDir;
	std::string CDImgFile;
	std::string FDImgFile[2][NUM_STANDBY_FDIMG];
	bool FDWriteProtect[2][NUM_STANDBY_FDIMG];
	std::string SCSIImgFile[MAX_NUM_SCSI_DEVICE];
	unsigned int bootKeyComb=BOOT_KEYCOMB_NONE;
	unsigned int gamePort[2]={TOWNS_GAMEPORTEMU_PHYSICAL0,TOWNS_GAMEPORTEMU_MOUSE};
	unsigned screenScaling=150;
	bool autoStart;
	unsigned int freq=25;
	unsigned int memSizeInMB=4;

	std::string errorMsg;

	unsigned int appSpecificAugmentation=TOWNS_APPSPECIFIC_NONE;

	bool catchUpRealTime=true;

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
