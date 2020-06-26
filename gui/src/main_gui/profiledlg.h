#ifndef PROFILEDLG_IS_INCLUDED
#define PROFILEDLG_IS_INCLUDED
/* { */

#include "fsgui.h"
#include "fsguifiledialog.h"

#include "townsprofile.h"

class ProfileDialog : public FsGuiDialog
{
public:
	FsGuiButton *ROMDirBtn,*CDImgBtn,*FDImgBtn[TownsProfile::NUM_STANDBY_FDIMG],*HDImgBtn[TownsProfile::MAX_NUM_SCSI_DEVICE];
	FsGuiTextBox *ROMDirTxt,*CDImgTxt,*FDImgTxt[TownsProfile::NUM_STANDBY_FDIMG],*HDImgTxt[TownsProfile::MAX_NUM_SCSI_DEVICE];
	FsGuiButton *gamePortBtn[2][5]; // None, Pad0, Pad1, Keybord Emulation, Mouse,
	FsGuiButton *bootKeyBtn[15];
	FsGuiButton *autoStartBtn;
	FsGuiButton *runBtn;

	void Make(void);
};


/* } */
#endif
