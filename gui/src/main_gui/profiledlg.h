#ifndef PROFILEDLG_IS_INCLUDED
#define PROFILEDLG_IS_INCLUDED
/* { */

#include <vector>

#include "fsgui.h"
#include "fsguifiledialog.h"

#include "townsprofile.h"

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

	FsGuiTabControl *tab;

	FsGuiStatic *profileFNameTxt;

	FsGuiButton *ROMDirBtn,*CDImgBtn,*FDImgBtn[TownsProfile::NUM_FDDRIVES],*HDImgBtn[TownsProfile::MAX_NUM_SCSI_DEVICES];
	FsGuiButton *FDWriteProtBtn[TownsProfile::NUM_FDDRIVES];
	FsGuiButton *damperWireLineBtn;
	FsGuiTextBox *ROMDirTxt,*CDImgTxt,*FDImgTxt[TownsProfile::NUM_FDDRIVES],*HDImgTxt[TownsProfile::MAX_NUM_SCSI_DEVICES];
	FsGuiDropList *CDSpeedDrp;
	FsGuiDropList *gamePortDrp[2]; // None, Pad0, Pad1, Keybord Emulation, Mouse,
	FsGuiSlider *mouseIntegSpdSlider;
	FsGuiButton *mouseIntegConsiderVRAMOffsetBtn;
	FsGuiTextBox *mouseMinXTxt,*mouseMinYTxt,*mouseMaxXTxt,*mouseMaxYTxt;
	FsGuiTextBox *maxButtonHoldTimeTxt[2][2];

	FsGuiButton *bootKeyBtn[15];
	FsGuiTextBox *scrnScaleTxt;
	FsGuiButton *scrnAutoScaleBtn;
	FsGuiDropList *scrnModeDrp;
	FsGuiButton *autoStartBtn;
	FsGuiTextBox *CPUFreqTxt,*RAMSizeTxt;
	FsGuiDropList *CPUFidelityDrp;
	FsGuiButton *CPUFidelityHelpBtn;
	FsGuiButton *FPUBtn;
	FsGuiButton *catchUpRealTimeBtn;
	FsGuiButton *runBtn;
	FsGuiButton *separateProcBtn;
	FsGuiDropList *townsTypeDrp;
	FsGuiButton *pretend386DXBtn;
	FsGuiButton *startUpStateFNameBtn;
	FsGuiTextBox *startUpStateFNameTxt;

	FsGuiButton *flightMouseEnableBtn;
	FsGuiDropList *flightMousePhysIdDrp;
	FsGuiTextBox *flightMouseCenterXTxt;
	FsGuiTextBox *flightMouseCenterYTxt;
	FsGuiTextBox *flightMouseScaleXTxt;
	FsGuiTextBox *flightMouseScaleYTxt;
	FsGuiTextBox *flightMouseDeadZoneTxt;
	FsGuiButton *flightMouseWC1,*flightMouseWC2,*flightMouseSTCM;

	FsGuiButton *strikeCommanderThrottleEnableBtn;
	FsGuiDropList *strikeCommanderThrottlePhysIdDrp;
	FsGuiDropList *strikeCommanderThrottleAxisDrp;


	FsGuiDropList *keyboardModeDrp;
	FsGuiDropList *virtualKeyTownsKeyDrp[TownsProfile::MAX_NUM_VIRTUALKEYS];
	FsGuiDropList *virtualKeyPhysIdDrp[TownsProfile::MAX_NUM_VIRTUALKEYS];
	FsGuiDropList *virtualKeyButtonDrp[TownsProfile::MAX_NUM_VIRTUALKEYS];

	FsGuiTextBox *keyMapFileTxt;
	FsGuiButton *selectKeyMapFileBtn;


	FsGuiStatic *fmVolumeText,*pcmVolumeText;
	FsGuiButton *fmVolumeDefaultBtn,*pcmVolumeDefaultBtn;
	FsGuiSlider *fmVolumeSlider,*pcmVolumeSlider;


	FsGuiDropList *hostShortCutKeyLabelDrp[MAX_NUM_HOST_SHORTCUT];
	FsGuiButton *hostShortCutKeyCtrlBtn[MAX_NUM_HOST_SHORTCUT];
	FsGuiButton *hostShortCutKeyShiftBtn[MAX_NUM_HOST_SHORTCUT];
	FsGuiDropList *hostShortCutFunctionDrp[MAX_NUM_HOST_SHORTCUT];
	FsGuiButton *quickSsDirBtn;
	FsGuiTextBox *quickSsDirTxt;

	FsGuiButton *quickStateSaveFNameBtn;
	FsGuiTextBox *quickStateSaveFNameTxt;

	FsGuiDropList *pauseResumeKeyDrp;

	FsGuiDropList *appSpecificAugDrp;
	FsGuiStatic *appSpecificExplanation;

	FsGuiButton *browseShareDirBtn[MAX_NUM_SHARED_DIR];
	FsGuiTextBox *shareDirTxt[MAX_NUM_SHARED_DIR];

	FsGuiTextBox *scrnShotCropTxt[4];
	FsGuiTextBox *mapXYExpressionTxt[2];

	ProfileDialog(FsGuiMainCanvas *canvasPtr);

	void Make(void);

	void OnSliderPositionChange(FsGuiSlider *slider,const double &prevPos,const double &prevValue);

	virtual void OnDropListSelChange(FsGuiDropList *drp,int prevSel);

	virtual void OnButtonClick(FsGuiButton *btn);
	void OnSelectROMFile(FsGuiDialog *dlg,int returnCode);

	FsGuiTextBox *nowBrowsingTxt=nullptr;
	void Browse(const wchar_t label[],FsGuiTextBox *txt,std::vector <const wchar_t *> extList);
	void OnSelectFile(FsGuiDialog *dlg,int returnCode);

	void BrowseDir(const wchar_t label[],FsGuiTextBox *txt,std::vector <const wchar_t *> extList);
	void OnSelectDir(FsGuiDialog *dlg,int returnCode);

	void BrowseSaveAs(const wchar_t label[],FsGuiTextBox *txt,std::vector <const wchar_t *> extList);
	void OnSelectFileSaveAs(FsGuiDialog *dlg,int returnCode);

	TownsProfile GetProfile(void) const;
	void SetProfile(const TownsProfile &profile);

	void SetDefaultFMVolume(void);
	void SetDefaultPCMVolume(void);
	void UpdateFMVolumeText(void);
	void UpdatePCMVolumeText(void);

	void ExplainCPUFidelity(void);
};


/* } */
#endif
