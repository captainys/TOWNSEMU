#ifndef PROFILEDLG_IS_INCLUDED
#define PROFILEDLG_IS_INCLUDED
/* { */

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
		NUM_GAMEPORT_CHOICE=13,
	};

	const unsigned int gamePortChoice[NUM_GAMEPORT_CHOICE]=
	{
		TOWNS_GAMEPORTEMU_NONE,
		TOWNS_GAMEPORTEMU_PHYSICAL0,
		TOWNS_GAMEPORTEMU_PHYSICAL1,
		TOWNS_GAMEPORTEMU_ANALOG0,
		TOWNS_GAMEPORTEMU_ANALOG1,
		TOWNS_GAMEPORTEMU_KEYBOARD,
		TOWNS_GAMEPORTEMU_MOUSE,

		TOWNS_GAMEPORTEMU_MOUSE_BY_KEY,
		TOWNS_GAMEPORTEMU_MOUSE_BY_NUMPAD,

		TOWNS_GAMEPORTEMU_MOUSE_BY_PHYSICAL0,
		TOWNS_GAMEPORTEMU_MOUSE_BY_PHYSICAL1,

		TOWNS_GAMEPORTEMU_MOUSE_BY_ANALOG0,
		TOWNS_GAMEPORTEMU_MOUSE_BY_ANALOG1,

		// Future options.
		//TOWNS_GAMEPORTEMU_PHYSICAL2,
		//TOWNS_GAMEPORTEMU_PHYSICAL3,
		//TOWNS_GAMEPORTEMU_PHYSICAL4,
		//TOWNS_GAMEPORTEMU_PHYSICAL5,
		//TOWNS_GAMEPORTEMU_PHYSICAL6,
		//TOWNS_GAMEPORTEMU_PHYSICAL7,
		//TOWNS_GAMEPORTEMU_ANALOG0,
		//TOWNS_GAMEPORTEMU_ANALOG1,
		//TOWNS_GAMEPORTEMU_ANALOG2,
		//TOWNS_GAMEPORTEMU_ANALOG3,
		//TOWNS_GAMEPORTEMU_ANALOG4,
		//TOWNS_GAMEPORTEMU_ANALOG5,
		//TOWNS_GAMEPORTEMU_ANALOG6,
		//TOWNS_GAMEPORTEMU_ANALOG7,
		//TOWNS_GAMEPORTEMU_PHYSICAL0_AS_CYBERSTICK,
		//TOWNS_GAMEPORTEMU_PHYSICAL1_AS_CYBERSTICK,
		//TOWNS_GAMEPORTEMU_PHYSICAL2_AS_CYBERSTICK,
		//TOWNS_GAMEPORTEMU_PHYSICAL3_AS_CYBERSTICK,
		//TOWNS_GAMEPORTEMU_PHYSICAL4_AS_CYBERSTICK,
		//TOWNS_GAMEPORTEMU_PHYSICAL5_AS_CYBERSTICK,
		//TOWNS_GAMEPORTEMU_PHYSICAL6_AS_CYBERSTICK,
		//TOWNS_GAMEPORTEMU_PHYSICAL7_AS_CYBERSTICK,
	};



	class FsGuiMainCanvas *canvasPtr;

	FsGuiTabControl *tab;

	FsGuiStatic *profileFNameTxt;

	FsGuiButton *ROMDirBtn,*CDImgBtn,*FDImgBtn[2][TownsProfile::NUM_STANDBY_FDIMG],*HDImgBtn[TownsProfile::MAX_NUM_SCSI_DEVICE];
	FsGuiButton *FDWriteProtBtn[2][TownsProfile::NUM_STANDBY_FDIMG];
	FsGuiTextBox *ROMDirTxt,*CDImgTxt,*FDImgTxt[2][TownsProfile::NUM_STANDBY_FDIMG],*HDImgTxt[TownsProfile::MAX_NUM_SCSI_DEVICE];
	FsGuiDropList *gamePortDrp[2]; // None, Pad0, Pad1, Keybord Emulation, Mouse,
	FsGuiSlider *mouseIntegSpdSlider;
	FsGuiButton *bootKeyBtn[15];
	FsGuiTextBox *scrnScaleTxt;
	FsGuiButton *autoStartBtn;
	FsGuiTextBox *CPUFreqTxt,*RAMSizeTxt;
	FsGuiButton *catchUpRealTimeBtn;
	FsGuiButton *runBtn;
	FsGuiButton *pretend386DXBtn;

	FsGuiButton *flightMouseEnableBtn;
	FsGuiTextBox *flightMousePhysIdTxt;
	FsGuiTextBox *flightMouseCenterXTxt;
	FsGuiTextBox *flightMouseCenterYTxt;
	FsGuiTextBox *flightMouseScaleXTxt;
	FsGuiTextBox *flightMouseScaleYTxt;
	FsGuiTextBox *flightMouseDeadZoneTxt;
	FsGuiButton *flightMouseWC1,*flightMouseWC2,*flightMouseSTCM;



	FsGuiDropList *appSpecificAugDrp;
	FsGuiStatic *appSpecificExplanation;

	ProfileDialog(FsGuiMainCanvas *canvasPtr);

	void Make(void);

	virtual void OnDropListSelChange(FsGuiDropList *drp,int prevSel);

	virtual void OnButtonClick(FsGuiButton *btn);
	void OnSelectROMFile(FsGuiDialog *dlg,int returnCode);

	FsGuiTextBox *nowBrowsingTxt=nullptr;
	void Browse(const wchar_t label[],FsGuiTextBox *txt,const wchar_t ext0[],const wchar_t ext1[]);
	void OnSelectFile(FsGuiDialog *dlg,int returnCode);

	TownsProfile GetProfile(void) const;
	void SetProfile(const TownsProfile &profile);
};


/* } */
#endif
