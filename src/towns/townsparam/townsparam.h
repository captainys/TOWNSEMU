#ifndef TOWNSPARAM_IS_INCLUDED
#define TOWNSPARAM_IS_INCLUDED
/* { */



#include <string>
#include <vector>
#include <unordered_map>
#include <utility>
#include "townsdef.h"
#include "i486.h" // Need FidelityLevel.

class TownsStartParameters
{
public:
	enum
	{
		WINDOW_NORMAL,
		WINDOW_MAXIMIZE,
		WINDOW_FULLSCREEN
	};

	enum
	{
		NUM_GAMEPORTS=2,
		NUM_FDDRIVES=4,
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

	bool verbose=false;

	unsigned int townsType=TOWNSTYPE_UNKNOWN;

	unsigned int CPUFidelityLevel=i486DXCommon::MID_FIDELITY;

	unsigned int gamePort[NUM_GAMEPORTS]={TOWNS_GAMEPORTEMU_PHYSICAL0,TOWNS_GAMEPORTEMU_MOUSE};

	unsigned int bootKeyComb=BOOT_KEYCOMB_NONE;

	bool alwaysBootToFASTMode=true;

	bool windowShift=false;

	bool noWait=true;
	bool noWaitStandby=false;
	bool fastSCSI=false,fastFD=false;

	bool highResAvailable=true;

	bool highResPCM=true;
	int nMidiCards=0;

	bool catchUpRealTime=true;

	bool damperWireLine=true;
	bool scanLineEffectIn15KHz=false;

	// If not "", VM starts from this saved state.
	std::string startUpStateFName;

	std::string ROMPath;
	std::string CMOSFName;
	bool autoSaveCMOS=true; // If this flag is false, CMOSFName will not be copied to FMTownsCommon::Variable.
	bool zeroCMOS=false; // Clear CMOS on Start Up
	std::string fdImgFName[NUM_FDDRIVES];
	bool fdImgWriteProtect[NUM_FDDRIVES]={false,false,false,false};
	std::string cdImgFName;
	unsigned int cdSpeed=0;  // 0->Use default.

	unsigned int memCardType=TOWNS_MEMCARD_TYPE_NONE;
	std::string memCardImgFName;
	std::string startUpScriptFName;
	std::string symbolFName;
	std::string playbackEventLogFName;
	std::string keyMapFName;
	bool memCardWriteProtected=false;

	std::unordered_map <std::string,std::string> fileNameAlias;
	std::vector <std::pair <std::string,std::string> > specialPath;

	std::vector <std::string> sharedDir;

	std::vector <std::string> fdSearchPaths;
	std::vector <std::string> cdSearchPaths;

	int keyboardMode=TOWNS_KEYBOARD_MODE_DEFAULT;
	std::vector <VirtualKey> virtualKeys;
	std::vector <HostShortCut> hostShortCutKeys;

	int fmVol=-1,pcmVol=-1;
	bool maximumSoundDoubleBuffering=false;

	bool mouseByFlightstickAvailable=false;
	bool cyberStickAssignment=false;
	int mouseByFlightstickPhysicalId=-1;  // Physical joystick ID.
	int mouseByFlightstickCenterX=320,mouseByFlightstickCenterY=200;
	float mouseByFlightstickZeroZoneX=0.0F,mouseByFlightstickZeroZoneY=0.0F;
	float mouseByFlightstickScaleX=500.0F,mouseByFlightstickScaleY=400.0F;
	long long int maxButtonHoldTime[2][2]={{0,0},{0,0}};

	bool customMouseIntegration=false;
	std::string customMouseX,customMouseY;
	unsigned int customMouseCaptureTiming=TOWNS_CUSTOM_MOUSE_CAPTURE_NEVER;

	bool differentialMouseIntegration=false;

	std::vector <std::string> initCmd;


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
	uint32_t powerOffAtPassCount=0;
	bool forceQuitOnPowerOff=false;
	std::string powerOffAt;

	bool pretend386DX=false;
	bool useFPU=false;

	unsigned int memSizeInMB=4;

	unsigned int freq=40;
	unsigned int slowModeFreq=0; // 0->No Change

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
	bool autoScaling=false,maintainAspect=true;
	unsigned int windowModeOnStartUp=WINDOW_NORMAL;

	std::string quickScrnShotDir;
	std::string quickStateSaveFName;
	unsigned int scrnShotX0=0,scrnShotY0=0,scrnShotWid=0,scrnShotHei=0;
	std::string mapXYExpression[2];

	std::string pauseResumeKeyLabel="SCROLLLOCK";

	std::string RS232CtoTCPAddr;

	TownsStartParameters();
};


/* } */
#endif
