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

	FsGuiButton *ROMDirBtn,*CDImgBtn,*FDImgBtn[TownsProfile::NUM_STANDBY_FDIMG],*HDImgBtn[TownsProfile::MAX_NUM_SCSI_DEVICE];
	FsGuiTextBox *ROMDirTxt,*CDImgTxt,*FDImgTxt[TownsProfile::NUM_STANDBY_FDIMG],*HDImgTxt[TownsProfile::MAX_NUM_SCSI_DEVICE];
	FsGuiButton *gamePortBtn[2][5]; // None, Pad0, Pad1, Keybord Emulation, Mouse,
	FsGuiButton *bootKeyBtn[15];
	FsGuiButton *autoStartBtn;
	FsGuiButton *runBtn;

	void Make(void);
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
