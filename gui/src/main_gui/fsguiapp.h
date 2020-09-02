/* ////////////////////////////////////////////////////////////

File Name: fsguiapp.h
Copyright (c) 2017 Soji Yamakawa.  All rights reserved.
http://www.ysflight.com

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS 
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE 
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

//////////////////////////////////////////////////////////// */

#ifndef FSGUI_APP_IS_INCLUDED
#define FSGUI_APP_IS_INCLUDED
/* { */

#include <fsgui.h>
#include "profiledlg.h"
#include "subproc.h"

class FsGuiMainCanvas : public FsGuiCanvas
{
public:
	/*! Main menu.  MainMenu is created in MakeMainMenu function, which is called 
	    from the constructor.
	*/
	FsGuiPopUpMenu *mainMenu;

	/*! Low-level interface, FsLazyWindow framework, checks for this value to see
	    if the application run-loop should be terminated.
	*/
	YSBOOL appMustTerminate;

private:
	/*! For convenience, you can use THISCLASS instead of FsGuiMainCanvas 
	    in the member functions.
	*/
	typedef FsGuiMainCanvas THISCLASS;

public:
	// [Core data structure]
	Subprocess subproc;
	std::vector <std::string> VMLog;


	// [Modeless dialogs]
	//   (1) Add a pointer in the following chunk.
	//   (2) Add an initialization in the constructor of the aplication.
	//   (3) Add deletion in the destructor of the application.
	//   (4) Add RemoveDialog in Edit_ClearUIIMode
	ProfileDialog *profileDlg;


	// [Modal dialogs]


	/*! Constructor is called after OpenGL context is created.
	    It is safe to make OpenGL function calls inside.
	*/
	FsGuiMainCanvas();

	/*! */
	~FsGuiMainCanvas();

	/*! This function is called from the low-level interface to get an
	    application pointer.
	*/
	static FsGuiMainCanvas *GetMainCanvas();

	/*! This funciion is called from the low-level interface for
	    deleting the application.
	*/
	static void DeleteMainCanvas(void);

	/*! Customize this function for adding menus.
	*/
	void MakeMainMenu(void);
	void DeleteMainMenu(void);

public:
	/*! In this function, shared GLSL renderers are created,
	    View-target is set to (-5,-5,-5) to (5,5,5),
	    and view distance is set to 7.5 by default.
	*/
	void Initialize(int argc,char *argv[]);

	/*! This function is called regularly from the low-level interface.
	*/
	void OnInterval(void);

	/*! This function is called from the low-level interface when the
	    window needs to be re-drawn.
	*/
	void Draw(void);


	/*! Start Tsugaru_CUI with the profile set in the profileDlg.
	*/
	void Run(void);
private:
	bool ReallyRun(bool usePipe=true);
	std::string FindTsugaruCUI(void) const;
	std::vector <YsWString> CheckMissingROMFiles(void) const;
	std::string GetCMOSFileName(void) const;


private:
	// [Menu pointers for check marks]

	// [Menu call-backs]
	/*! Sample call-back functions.
	*/
	void VM_Not_Running_Error(void);
	void VM_Already_Running_Error(void);

	void File_SaveDefaultProfile(FsGuiPopUpMenuItem *);
	void File_SaveDefaultConfirm(FsGuiDialog *dlg,int returnCode);
	void File_ReloadDefaultProfile(FsGuiPopUpMenuItem *);
	YsWString GetDefaultProfileFileName(void) const;
	YsWString GetTsugaruProfileDir(void) const;

	void SaveProfile(YsWString fName) const;
	void LoadProfile(YsWString fName);

	void File_SaveProfile(FsGuiPopUpMenuItem *);
	void File_OpenProfile(FsGuiPopUpMenuItem *);
	void File_OpenProfile_FileSelected(FsGuiDialog *dlg,int returnCode);
	void File_SaveProfileAs(FsGuiPopUpMenuItem *);
	void File_SaveProfileAs_FileSelected(FsGuiDialog *dlg,int returnCode);
	void File_SaveProfileAs_OverwriteConfirm(FsGuiDialog *dlg,int returnCode);

	void File_Exit(FsGuiPopUpMenuItem *);
	void File_Exit_ConfirmExitCallBack(FsGuiDialog *,int);
	void File_Exit_ReallyExit(void);


	bool genFloppyDisk=true;
	unsigned int genDiskSize=0;
	void File_New_1232KB(FsGuiPopUpMenuItem *);
	void File_New_1440KB(FsGuiPopUpMenuItem *);
	void File_New_720KB(FsGuiPopUpMenuItem *);
	void File_New_640KB(FsGuiPopUpMenuItem *);

	void File_New_HDD(FsGuiPopUpMenuItem *);
	void File_New_HDD_SizeSelected(FsGuiDialog *dlg,int returnCode);

	void File_New_FileSelected(FsGuiDialog *dlg,int returnCode);


	void VM_Start(FsGuiPopUpMenuItem *);
	void VM_StartAndCloseGUI(FsGuiPopUpMenuItem *);
	void VM_PowerOff(FsGuiPopUpMenuItem *);
	void VM_PowerOffConfirm(FsGuiDialog *dlg,int returnCode);
	void VM_Pause(FsGuiPopUpMenuItem *);
	void VM_Resume(FsGuiPopUpMenuItem *);

	void VM_1MHz(FsGuiPopUpMenuItem *);
	void VM_4MHz(FsGuiPopUpMenuItem *);
	void VM_8MHz(FsGuiPopUpMenuItem *);
	void VM_12MHz(FsGuiPopUpMenuItem *);
	void VM_16MHz(FsGuiPopUpMenuItem *);
	void VM_25MHz(FsGuiPopUpMenuItem *);

	void VM_Keyboard_Direct(FsGuiPopUpMenuItem *);
	void VM_Keyboard_Translation(FsGuiPopUpMenuItem *);


	void CD_SelectImageFile(FsGuiPopUpMenuItem *);
	void CD_ImageFileSelected(FsGuiDialog *dlg,int returnCode);
	void CD_Eject(FsGuiPopUpMenuItem *);
	void CD_OpenClose(FsGuiPopUpMenuItem *);
	void CD_CDDAStop(FsGuiPopUpMenuItem *);

	FsGuiPopUpMenuItem *FD0_writeProtectMenu,*FD0_writeUnprotectMenu;
	void FD0_SelectImageFile(FsGuiPopUpMenuItem *);
	void FD0_ImageFileSelected(FsGuiDialog *dlg,int returnCode);
	void FD0_WriteProtect(FsGuiPopUpMenuItem *);
	void FD0_WriteUnprotect(FsGuiPopUpMenuItem *);
	void FD0_Eject(FsGuiPopUpMenuItem *);

	FsGuiPopUpMenuItem *FD1_writeProtectMenu,*FD1_writeUnprotectMenu;
	void FD1_SelectImageFile(FsGuiPopUpMenuItem *);
	void FD1_ImageFileSelected(FsGuiDialog *dlg,int returnCode);
	void FD1_WriteProtect(FsGuiPopUpMenuItem *);
	void FD1_WriteUnprotect(FsGuiPopUpMenuItem *);
	void FD1_Eject(FsGuiPopUpMenuItem *);



	void Help_About(FsGuiPopUpMenuItem *);
	void Help_Help(FsGuiPopUpMenuItem *);
};

/* } */
#endif
