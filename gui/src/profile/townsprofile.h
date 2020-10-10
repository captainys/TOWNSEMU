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
	bool screenAutoScaling=false;
	bool screenMaximizeOnStartUp=false;
	bool autoStart;
	bool pretend386DX=false;
	unsigned int freq=25;
	unsigned int memSizeInMB=4;
	unsigned int mouseIntegrationSpeed=256;

	bool mouseByFlightstickAvailable=false;
	int mouseByFlightstickPhysicalId=-1;  // Physical joystick ID.
	int mouseByFlightstickCenterX=320,mouseByFlightstickCenterY=200;
	int mouseByFlightstickZeroZonePercent=0;
	int mouseByFlightstickScaleX=500,mouseByFlightstickScaleY=400;

	bool useStrikeCommanderThrottleAxis=false;
	int strikeCommanderThrottlePhysicalId=-1;
	unsigned int strikeCommanderThrottleAxis=2;

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
	unsigned int keyboardMode=TOWNS_KEYBOARD_MODE_DEFAULT;


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
