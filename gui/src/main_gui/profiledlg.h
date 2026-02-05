#ifndef PROFILEDLG_IS_INCLUDED
#define PROFILEDLG_IS_INCLUDED
/* { */

#include <vector>

#include "fsgui.h"
#include "fsguifiledialog.h"

#include "townsprofile.h"
#include "uitxt.h"

class ProfileDialog : public FsGuiDialog
{
public:
	typedef class ProfileDialog THISCLASS;

	enum
	{
		PATH_LENGTH=1024,
		PATH_SHOW=64,
		MAX_NUM_HOST_SHORTCUT=8,
		MAX_NUM_SHARED_DIR=4,
	};


	class FsGuiMainCanvas *canvasPtr;

	UiText ui;

	FsGuiTabControl *tab=nullptr;

	FsGuiStatic *profileFNameTxt=nullptr;

	FsGuiButton *ROMDirBtn=nullptr;
	FsGuiButton *CMOSFileBtn=nullptr;
	FsGuiButton *CDImgBtn=nullptr;
	FsGuiButton *FDImgBtn[TownsProfile::NUM_FDDRIVES]={nullptr,nullptr,nullptr,nullptr,};
	FsGuiButton *HDImgBtn[TownsProfile::MAX_NUM_SCSI_DEVICES]={nullptr};
	FsGuiButton *fastSCSIBtn=nullptr;
	FsGuiButton *FDWriteProtBtn[TownsProfile::NUM_FDDRIVES]={nullptr};
	FsGuiButton *scanLineEffectIn15KHzBtn=nullptr;
	FsGuiButton *damperWireLineBtn=nullptr;
	FsGuiTextBox *ROMDirTxt=nullptr;
	FsGuiTextBox *CMOSFileTxt=nullptr;
	FsGuiTextBox *CDImgTxt=nullptr;
	FsGuiTextBox *FDImgTxt[TownsProfile::NUM_FDDRIVES]={nullptr};
	FsGuiTextBox *HDImgTxt[TownsProfile::MAX_NUM_SCSI_DEVICES]={nullptr};
	FsGuiDropList *CDSpeedDrp=nullptr;
	FsGuiDropList *gamePortDrp[2]; // None, Pad0, Pad1, Keybord Emulation, Mouse,
	FsGuiSlider *mouseIntegSpdSlider=nullptr;
	FsGuiButton *mouseIntegConsiderVRAMOffsetBtn=nullptr;
	FsGuiButton *diffMouseBtn=nullptr;
	FsGuiTextBox *mouseMinXTxt=nullptr;
	FsGuiTextBox *mouseMinYTxt=nullptr;
	FsGuiTextBox *mouseMaxXTxt=nullptr;
	FsGuiTextBox *mouseMaxYTxt=nullptr;
	FsGuiTextBox *maxButtonHoldTimeTxt[2][2]={{nullptr,nullptr},{nullptr,nullptr}};

	FsGuiButton *bootKeyBtn[17]={nullptr};
	FsGuiTextBox *scrnScaleTxt=nullptr;
	FsGuiButton *scrnAutoScaleBtn=nullptr;
	FsGuiButton *scrnMaintainAspectBtn=nullptr;
	FsGuiDropList *scrnModeDrp=nullptr;
	FsGuiButton *autoStartBtn=nullptr;
	FsGuiTextBox *CPUFreqTxt=nullptr;
	FsGuiTextBox *RAMSizeTxt=nullptr;
	FsGuiDropList *CPUFidelityDrp=nullptr;
	FsGuiButton *CPUFidelityHelpBtn=nullptr;
	FsGuiButton *FPUBtn=nullptr;
	FsGuiButton *catchUpRealTimeBtn=nullptr;
	FsGuiButton *runBtn=nullptr;
	FsGuiButton *separateProcBtn=nullptr;
	FsGuiDropList *townsTypeDrp=nullptr;
	FsGuiButton *pretend386DXBtn=nullptr;
	FsGuiButton *startUpStateFNameBtn=nullptr;
	FsGuiTextBox *startUpStateFNameTxt=nullptr;

	FsGuiButton *flightMouseEnableBtn=nullptr;
	FsGuiDropList *flightMousePhysIdDrp=nullptr;
	FsGuiTextBox *flightMouseCenterXTxt=nullptr;
	FsGuiTextBox *flightMouseCenterYTxt=nullptr;
	FsGuiTextBox *flightMouseScaleXTxt=nullptr;
	FsGuiTextBox *flightMouseScaleYTxt=nullptr;
	FsGuiTextBox *flightMouseDeadZoneTxt=nullptr;
	FsGuiButton *flightMouseWC1=nullptr;
	FsGuiButton *flightMouseWC2=nullptr;
	FsGuiButton *flightMouseSTCM=nullptr;

	FsGuiButton *strikeCommanderThrottleEnableBtn=nullptr;
	FsGuiDropList *strikeCommanderThrottlePhysIdDrp=nullptr;
	FsGuiDropList *strikeCommanderThrottleAxisDrp=nullptr;


	FsGuiDropList *keyboardModeDrp=nullptr;
	FsGuiDropList *virtualKeyTownsKeyDrp[TownsProfile::MAX_NUM_VIRTUALKEYS]={nullptr};
	FsGuiDropList *virtualKeyPhysIdDrp[TownsProfile::MAX_NUM_VIRTUALKEYS]={nullptr};
	FsGuiDropList *virtualKeyButtonDrp[TownsProfile::MAX_NUM_VIRTUALKEYS]={nullptr};

	FsGuiTextBox *keyMapFileTxt=nullptr;
	FsGuiButton *selectKeyMapFileBtn=nullptr;


	FsGuiStatic *fmVolumeText,*pcmVolumeText=nullptr;
	FsGuiButton *fmVolumeDefaultBtn=nullptr;
	FsGuiButton *pcmVolumeDefaultBtn=nullptr;
	FsGuiSlider *fmVolumeSlider=nullptr;
	FsGuiSlider *pcmVolumeSlider=nullptr;
	FsGuiButton *maxSoundDoubldBufBtn=nullptr;
	FsGuiDropList *numMidiCardsDrp=nullptr;


	FsGuiDropList *hostShortCutKeyLabelDrp[MAX_NUM_HOST_SHORTCUT]={nullptr};
	FsGuiButton *hostShortCutKeyCtrlBtn[MAX_NUM_HOST_SHORTCUT]={nullptr};
	FsGuiButton *hostShortCutKeyShiftBtn[MAX_NUM_HOST_SHORTCUT]={nullptr};
	FsGuiDropList *hostShortCutFunctionDrp[MAX_NUM_HOST_SHORTCUT]={nullptr};
	FsGuiButton *quickSsDirBtn=nullptr;
	FsGuiTextBox *quickSsDirTxt=nullptr;

	FsGuiButton *quickStateSaveFNameBtn=nullptr;
	FsGuiTextBox *quickStateSaveFNameTxt=nullptr;

	FsGuiDropList *pauseResumeKeyDrp=nullptr;

	FsGuiDropList *appSpecificAugDrp=nullptr;
	FsGuiStatic *appSpecificExplanation=nullptr;

	FsGuiButton *browseShareDirBtn[MAX_NUM_SHARED_DIR]={nullptr};
	FsGuiTextBox *shareDirTxt[MAX_NUM_SHARED_DIR]={nullptr};

	FsGuiTextBox *scrnShotCropTxt[4]={nullptr};
	FsGuiTextBox *mapXYExpressionTxt[2]={nullptr};

	FsGuiTextBox *RS232CtoTCPTxt=nullptr;

	ProfileDialog(FsGuiMainCanvas *canvasPtr);

	void Make(const UiText &ui);

	void OnSliderPositionChange(FsGuiSlider *slider,const double &prevPos,const double &prevValue);

	virtual void OnDropListSelChange(FsGuiDropList *drp,int prevSel);

	virtual void OnButtonClick(FsGuiButton *btn);

	YsString GetDefaultCDImageFileName(void) const;
	YsString GetDefaultStateFileName(void) const;
	YsString GetDefaultFDImageFileName(int i) const;
	YsString GetDefaultHDImageFileName(int i) const;
	YsString MakeUpAnyImageFileName(YsString filePart) const;

	FsGuiTextBox *nowBrowsingTxt=nullptr;
	void Browse(const wchar_t label[],FsGuiTextBox *txt,YsString dflt,std::vector <const wchar_t *> extList);

	void BrowseDir(const wchar_t label[],FsGuiTextBox *txt,std::vector <const wchar_t *> extList);
	void OnSelectDir(FsGuiDialog *dlg,int returnCode);

	void BrowseSaveAs(const wchar_t label[],FsGuiTextBox *txt,std::vector <const wchar_t *> extList);
	void OnSelectFileSaveAs(FsGuiDialog *dlg,int returnCode);

	TownsProfile GetProfile(void) const;
	void SetProfile(const TownsProfile &profile);

	static void ToSystemEncoding(TownsProfile &profile);

	/*! Input:  UTF8
	    Output: System Encoding
	*/
	static void ToSystemEncoding(std::string &str);

	void SetDefaultFMVolume(void);
	void SetDefaultPCMVolume(void);
	void UpdateFMVolumeText(void);
	void UpdatePCMVolumeText(void);

	void ExplainCPUFidelity(void);
};


/* } */
#endif
