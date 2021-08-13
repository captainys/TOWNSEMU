#ifndef TOWNSPARAM_IS_INCLUDED
#define TOWNSPARAM_IS_INCLUDED
/* { */



#include <string>
#include <vector>
#include "townsdef.h"

class TownsStartParameters
{
public:
	enum
	{
		NUM_GAMEPORTS=2,
		NUM_FDDRIVES=2,
		MAX_NUM_SCSI_DEVICES=7,
	};

	class VirtualKey
	{
	public:
		std::string townsKey;
		int physicalId;
		unsigned int button;
	};

	class HostShortCut
	{
	public:
		std::string hostKey;
		bool ctrl,shift;
		std::string cmdStr;
	};

	unsigned int townsType=TOWNSTYPE_UNKNOWN;

	unsigned int gamePort[NUM_GAMEPORTS]={TOWNS_GAMEPORTEMU_PHYSICAL0,TOWNS_GAMEPORTEMU_MOUSE};

	unsigned int bootKeyComb=BOOT_KEYCOMB_NONE;

	bool windowShift=false;

	bool noWait=true;
	bool noWaitStandby=false;

	bool highResAvailable=true;

	bool catchUpRealTime=true;

	bool damperWireLine=false;
	bool scanLineEffectIn15KHz=false;

	// If not "", VM starts from this saved state.
	std::string startUpStateFName;

	std::string ROMPath;
	std::string CMOSFName;
	bool autoSaveCMOS=true; // If this flag is false, CMOSFName will not be copied to FMTowns::Variable.
	std::string fdImgFName[NUM_FDDRIVES];
	bool fdImgWriteProtect[NUM_FDDRIVES]={false,false};
	std::string cdImgFName;
	unsigned int cdSpeed=0;  // 0->Use default.

	unsigned int memCardType=TOWNS_MEMCARD_TYPE_NONE;
	std::string memCardImgFName;
	std::string startUpScriptFName;
	std::string symbolFName;
	std::string playbackEventLogFName;
	std::string keyMapFName;

	std::vector <std::string> sharedDir;

	std::vector <std::string> fdSearchPaths;
	std::vector <std::string> cdSearchPaths;

	int keyboardMode=TOWNS_KEYBOARD_MODE_DEFAULT;
	std::vector <VirtualKey> virtualKeys;
	std::vector <HostShortCut> hostShortCutKeys;

	int fmVol=-1,pcmVol=-1;

	bool mouseByFlightstickAvailable=false;
	bool cyberStickAssignment=false;
	int mouseByFlightstickPhysicalId=-1;  // Physical joystick ID.
	int mouseByFlightstickCenterX=320,mouseByFlightstickCenterY=200;
	float mouseByFlightstickZeroZoneX=0.0F,mouseByFlightstickZeroZoneY=0.0F;
	float mouseByFlightstickScaleX=500.0F,mouseByFlightstickScaleY=400.0F;


	// When adding a throttle control for an app, also update Outside_World::CacheGamePadIndicesThatNeedUpdates.
	int throttlePhysicalId=-1;
	int throttleAxis=2;

	enum
	{
		SCSIIMAGE_NONE,
		SCSIIMAGE_HARDDISK,
		SCSIIMAGE_CDROM
	};
	class SCSIImage
	{
	public:
		unsigned imageType=SCSIIMAGE_NONE;
		std::string imgFName;
	};
	SCSIImage scsiImg[MAX_NUM_SCSI_DEVICES];

	class FileToSend
	{
	public:
		std::string hostFName,vmFName;
	};
	std::vector <FileToSend> toSend;

	bool autoStart=true;
	bool debugger=false;
	bool interactive=true;

	bool powerOffAtBreakPoint=false;
	bool forceQuitOnPowerOff=false;
	std::string powerOffAt;

	bool pretend386DX=false;

	unsigned int memSizeInMB=4;

	unsigned int freq=33;

	unsigned int mouseIntegrationSpeed=256;
	bool considerVRAMOffsetInMouseIntegration=true;
	enum
	{
		DEFAULT_MOUSE_MINX=0,
		DEFAULT_MOUSE_MAXX=1023,
		DEFAULT_MOUSE_MINY=0,
		DEFAULT_MOUSE_MAXY=767,
	};
	int mouseMinX=DEFAULT_MOUSE_MINX;
	int mouseMaxX=DEFAULT_MOUSE_MAXX;
	int mouseMinY=DEFAULT_MOUSE_MINY;
	int mouseMaxY=DEFAULT_MOUSE_MAXY;

	unsigned int appSpecificSetting=TOWNS_APPSPECIFIC_NONE;

	enum
	{
		SCALING_DEFAULT=100,
		SCALING_MIN=50,
		SCALING_MAX=1000,
	};
	unsigned int scaling=100;
	bool autoScaling=false;
	bool maximizeOnStartUp=false;

	std::string quickScrnShotDir;
	std::string quickStateSaveFName;

	std::string pauseResumeKeyLabel="SCROLLLOCK";

	TownsStartParameters();
};


/* } */
#endif
