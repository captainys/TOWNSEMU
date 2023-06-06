/* ////////////////////////////////////////////////////////////

File Name: fsguiapp.cpp
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

#include <iostream>

#include <ysclass.h>
#include <ysport.h>
#include <yscompilerwarning.h>
#include <ysgl.h>

#include "fsguiapp.h"
#include "cpputil.h"
#include "d77.h"
#include "diskimg.h"



// If you want to use fixed-function pipeline of OpenGL 1.x,
//   (1) comment out the following line, and
//   (2) change linking libraries in CMakeLists.txt from:
//          fsguilib_gl2 fsgui3d_gl2
//       to
//          fsguilib_gl1 fsgui3d_gl1
//#define DONT_USE_FIXED_FUNCTION_PIPELINE



static FsGuiMainCanvas *appMainCanvas;

////////////////////////////////////////////////////////////

static const FsGuiMainCanvas::HumanReadable gameportEmulationTypes[]
{
	{TOWNS_GAMEPORTEMU_NONE,"None"},
	{TOWNS_GAMEPORTEMU_MOUSE,"Mouse"},
	{TOWNS_GAMEPORTEMU_KEYBOARD,"Game Pad by Host Keyboard"},
	{TOWNS_GAMEPORTEMU_PHYSICAL0,"Game Pad by Host Game Controller 0"},
	{TOWNS_GAMEPORTEMU_PHYSICAL1,"Game Pad by Host Game Controller 1"},
	{TOWNS_GAMEPORTEMU_PHYSICAL2,"Game Pad by Host Game Controller 2"},
	{TOWNS_GAMEPORTEMU_PHYSICAL3,"Game Pad by Host Game Controller 3"},
	{TOWNS_GAMEPORTEMU_PHYSICAL4,"Game Pad by Host Game Controller 4"},
	{TOWNS_GAMEPORTEMU_PHYSICAL5,"Game Pad by Host Game Controller 5"},
	{TOWNS_GAMEPORTEMU_PHYSICAL6,"Game Pad by Host Game Controller 6"},
	{TOWNS_GAMEPORTEMU_PHYSICAL7,"Game Pad by Host Game Controller 7"},
	{TOWNS_GAMEPORTEMU_ANALOG0,"Game Pad by Host Game Controller (Analog) 0"},
	{TOWNS_GAMEPORTEMU_ANALOG1,"Game Pad by Host Game Controller (Analog) 1"},
	{TOWNS_GAMEPORTEMU_ANALOG2,"Game Pad by Host Game Controller (Analog) 2"},
	{TOWNS_GAMEPORTEMU_ANALOG3,"Game Pad by Host Game Controller (Analog) 3"},
	{TOWNS_GAMEPORTEMU_ANALOG4,"Game Pad by Host Game Controller (Analog) 4"},
	{TOWNS_GAMEPORTEMU_ANALOG5,"Game Pad by Host Game Controller (Analog) 5"},
	{TOWNS_GAMEPORTEMU_ANALOG6,"Game Pad by Host Game Controller (Analog) 6"},
	{TOWNS_GAMEPORTEMU_ANALOG7,"Game Pad by Host Game Controller (Analog) 7"},
	{TOWNS_GAMEPORTEMU_PHYSICAL0_AS_CYBERSTICK,"Cyberstick by Host Game Controller 0"},
	{TOWNS_GAMEPORTEMU_PHYSICAL1_AS_CYBERSTICK,"Cyberstick by Host Game Controller 1"},
	{TOWNS_GAMEPORTEMU_PHYSICAL2_AS_CYBERSTICK,"Cyberstick by Host Game Controller 2"},
	{TOWNS_GAMEPORTEMU_PHYSICAL3_AS_CYBERSTICK,"Cyberstick by Host Game Controller 3"},
	{TOWNS_GAMEPORTEMU_PHYSICAL4_AS_CYBERSTICK,"Cyberstick by Host Game Controller 4"},
	{TOWNS_GAMEPORTEMU_PHYSICAL5_AS_CYBERSTICK,"Cyberstick by Host Game Controller 5"},
	{TOWNS_GAMEPORTEMU_PHYSICAL6_AS_CYBERSTICK,"Cyberstick by Host Game Controller 6"},
	{TOWNS_GAMEPORTEMU_PHYSICAL7_AS_CYBERSTICK,"Cyberstick by Host Game Controller 7"},
	{TOWNS_GAMEPORTEMU_MOUSE_BY_KEY,"Mouse by Host Arrow Keys"},
	{TOWNS_GAMEPORTEMU_MOUSE_BY_NUMPAD,"Mouse by Host NUM Keys"},
	{TOWNS_GAMEPORTEMU_MOUSE_BY_PHYSICAL0,"Mouse by Host Game Controller 0"},
	{TOWNS_GAMEPORTEMU_MOUSE_BY_PHYSICAL1,"Mouse by Host Game Controller 1"},
	{TOWNS_GAMEPORTEMU_MOUSE_BY_PHYSICAL2,"Mouse by Host Game Controller 2"},
	{TOWNS_GAMEPORTEMU_MOUSE_BY_PHYSICAL3,"Mouse by Host Game Controller 3"},
	{TOWNS_GAMEPORTEMU_MOUSE_BY_PHYSICAL4,"Mouse by Host Game Controller 4"},
	{TOWNS_GAMEPORTEMU_MOUSE_BY_PHYSICAL5,"Mouse by Host Game Controller 5"},
	{TOWNS_GAMEPORTEMU_MOUSE_BY_PHYSICAL6,"Mouse by Host Game Controller 6"},
	{TOWNS_GAMEPORTEMU_MOUSE_BY_PHYSICAL7,"Mouse by Host Game Controller 7"},
	{TOWNS_GAMEPORTEMU_MOUSE_BY_ANALOG0,"Mouse by Host Game Controller (Analog) 0"},
	{TOWNS_GAMEPORTEMU_MOUSE_BY_ANALOG1,"Mouse by Host Game Controller (Analog) 1"},
	{TOWNS_GAMEPORTEMU_MOUSE_BY_ANALOG2,"Mouse by Host Game Controller (Analog) 2"},
	{TOWNS_GAMEPORTEMU_MOUSE_BY_ANALOG3,"Mouse by Host Game Controller (Analog) 3"},
	{TOWNS_GAMEPORTEMU_MOUSE_BY_ANALOG4,"Mouse by Host Game Controller (Analog) 4"},
	{TOWNS_GAMEPORTEMU_MOUSE_BY_ANALOG5,"Mouse by Host Game Controller (Analog) 5"},
	{TOWNS_GAMEPORTEMU_MOUSE_BY_ANALOG6,"Mouse by Host Game Controller (Analog) 6"},
	{TOWNS_GAMEPORTEMU_MOUSE_BY_ANALOG7,"Mouse by Host Game Controller (Analog) 7"},
	{TOWNS_GAMEPORTEMU_CYBERSTICK,"Cyberstick by Custom Axis Button Assignments"},
	{TOWNS_GAMEPORTEMU_CAPCOM_BY_PHYSICAL0,"CAPCOM CPSF by Host Game Controller 0"},
	{TOWNS_GAMEPORTEMU_CAPCOM_BY_PHYSICAL1,"CAPCOM CPSF by Host Game Controller 1"},
	{TOWNS_GAMEPORTEMU_CAPCOM_BY_PHYSICAL2,"CAPCOM CPSF by Host Game Controller 2"},
	{TOWNS_GAMEPORTEMU_CAPCOM_BY_PHYSICAL3,"CAPCOM CPSF by Host Game Controller 3"},
	{TOWNS_GAMEPORTEMU_CAPCOM_BY_PHYSICAL4,"CAPCOM CPSF by Host Game Controller 4"},
	{TOWNS_GAMEPORTEMU_CAPCOM_BY_PHYSICAL5,"CAPCOM CPSF by Host Game Controller 5"},
	{TOWNS_GAMEPORTEMU_CAPCOM_BY_PHYSICAL6,"CAPCOM CPSF by Host Game Controller 6"},
	{TOWNS_GAMEPORTEMU_CAPCOM_BY_PHYSICAL7,"CAPCOM CPSF by Host Game Controller 7"},
};

static const unsigned int selectableGameportDevices[]=
{
	TOWNS_GAMEPORTEMU_NONE,
	TOWNS_GAMEPORTEMU_MOUSE,
	TOWNS_GAMEPORTEMU_KEYBOARD,
	TOWNS_GAMEPORTEMU_PHYSICAL0,
	TOWNS_GAMEPORTEMU_PHYSICAL1,
	TOWNS_GAMEPORTEMU_PHYSICAL2,
	TOWNS_GAMEPORTEMU_PHYSICAL3,
	TOWNS_GAMEPORTEMU_ANALOG0,
	TOWNS_GAMEPORTEMU_ANALOG1,
	TOWNS_GAMEPORTEMU_ANALOG2,
	TOWNS_GAMEPORTEMU_ANALOG3,
	TOWNS_GAMEPORTEMU_PHYSICAL0_AS_CYBERSTICK,
	TOWNS_GAMEPORTEMU_PHYSICAL1_AS_CYBERSTICK,
	TOWNS_GAMEPORTEMU_PHYSICAL2_AS_CYBERSTICK,
	TOWNS_GAMEPORTEMU_PHYSICAL3_AS_CYBERSTICK,
	TOWNS_GAMEPORTEMU_MOUSE_BY_KEY,
	TOWNS_GAMEPORTEMU_MOUSE_BY_NUMPAD,
	TOWNS_GAMEPORTEMU_MOUSE_BY_PHYSICAL0,
	TOWNS_GAMEPORTEMU_MOUSE_BY_PHYSICAL1,
	TOWNS_GAMEPORTEMU_MOUSE_BY_PHYSICAL2,
	TOWNS_GAMEPORTEMU_MOUSE_BY_PHYSICAL3,
	TOWNS_GAMEPORTEMU_MOUSE_BY_ANALOG0,
	TOWNS_GAMEPORTEMU_MOUSE_BY_ANALOG1,
	TOWNS_GAMEPORTEMU_MOUSE_BY_ANALOG2,
	TOWNS_GAMEPORTEMU_MOUSE_BY_ANALOG3,
	TOWNS_GAMEPORTEMU_CYBERSTICK,
	TOWNS_GAMEPORTEMU_CAPCOM_BY_PHYSICAL0,
	TOWNS_GAMEPORTEMU_CAPCOM_BY_PHYSICAL1,
	TOWNS_GAMEPORTEMU_CAPCOM_BY_PHYSICAL2,
	TOWNS_GAMEPORTEMU_CAPCOM_BY_PHYSICAL3,
};

/* static */ std::string FsGuiMainCanvas::GamePortEmulationTypeToHumanReadable(unsigned int emuType)
{
	for(auto x : gameportEmulationTypes)
	{
		if(x.code==emuType)
		{
			return x.label;
		}
	}
	return "None";
}
/* static */ unsigned int FsGuiMainCanvas::HumanReadableToGamePortEmulationType(std::string label)
{
	for(auto x : gameportEmulationTypes)
	{
		if(x.label==label)
		{
			return x.code;
		}
	}
	return TOWNS_GAMEPORTEMU_NONE;
}

/* static */ std::vector <std::string> FsGuiMainCanvas::GetSelectableGamePortEmulationTypeHumanReadable(void)
{
	// Who cares O(N^2) here?
	std::vector <std::string> lst;
	for(auto x : selectableGameportDevices)
	{
		lst.push_back(GamePortEmulationTypeToHumanReadable(x));
	}
	return lst;
}

////////////////////////////////////////////////////////////

FsGuiMainCanvas::FsGuiMainCanvas()
{
	appMustTerminate=YSFALSE;
	mainMenu=nullptr;
	profileDlg=new ProfileDialog(this);
}

FsGuiMainCanvas::~FsGuiMainCanvas()
{
	// The following two lines ensure that all self-destructive dialogs are cleaned. 2015/03/18
	RemoveDialogAll();
	PerformScheduledDeletion();
	delete profileDlg;

	subproc.TerminateSubprocess();

	DeleteMainMenu();
}

void FsGuiMainCanvas::Initialize(int argc,char *argv[])
{
	MakeMainMenu();
	profileDlg->Make();
	LoadProfile(GetDefaultProfileFileName());
	AddDialog(profileDlg);

	// Pause/Resume key is made a variable.  Need to be checked in OnInterval.
	// BindKeyCallBack(FSKEY_SCROLLLOCK,YSFALSE,YSFALSE,YSFALSE,&FsGuiMainCanvas::VM_Resume,this);

	YsDisregardVariable(argc);
	YsDisregardVariable(argv);
	YsGLSLCreateSharedRenderer();
}

void FsGuiMainCanvas::MakeMainMenu(void)
{
	mainMenu=new FsGuiPopUpMenu;
	mainMenu->Initialize();
	mainMenu->SetIsPullDownMenu(YSTRUE);

	{
		auto *subMenu=mainMenu->AddTextItem(0,FSKEY_F,L"File")->GetSubMenu();
		subMenu->AddTextItem(0,FSKEY_O,L"Open Profile")->BindCallBack(&THISCLASS::File_OpenProfile,this);
		subMenu->AddTextItem(0,FSKEY_S,L"Save Profile")->BindCallBack(&THISCLASS::File_SaveProfile,this);
		subMenu->AddTextItem(0,FSKEY_A,L"Save Profile As")->BindCallBack(&THISCLASS::File_SaveProfileAs,this);
		subMenu->AddTextItem(0,FSKEY_NULL,L"Save as Default")->BindCallBack(&THISCLASS::File_SaveDefaultProfile,this);
		subMenu->AddTextItem(0,FSKEY_NULL,L"Reload Default")->BindCallBack(&THISCLASS::File_ReloadDefaultProfile,this);

		{
			auto subSubMenu=subMenu->AddTextItem(0,FSKEY_NULL,L"CMOS RAM")->AddSubMenu();
			subSubMenu->AddTextItem(0,FSKEY_NULL,"Clear CMOS RAM")->BindCallBack(&THISCLASS::File_ClearCMOS,this);
		}

		{
			auto subSubMenu=subMenu->AddTextItem(0,FSKEY_N,L"New")->AddSubMenu();
			subSubMenu->AddTextItem(0,FSKEY_NULL,L"1232KB Floppy Disk Image")->BindCallBack(&THISCLASS::File_New_1232KB,this);
			subSubMenu->AddTextItem(0,FSKEY_NULL,L"1440KB Floppy Disk Image")->BindCallBack(&THISCLASS::File_New_1440KB,this);
			subSubMenu->AddTextItem(0,FSKEY_NULL,L"640KB Floppy Disk Image")->BindCallBack(&THISCLASS::File_New_640KB,this);
			subSubMenu->AddTextItem(0,FSKEY_NULL,L"720KB Floppy Disk Image")->BindCallBack(&THISCLASS::File_New_720KB,this);
			subSubMenu->AddTextItem(0,FSKEY_NULL,L"Hard-Disk Image")->BindCallBack(&THISCLASS::File_New_HDD,this);
			subSubMenu->AddTextItem(0,FSKEY_NULL,L"Make Default Key Mapping File")->BindCallBack(&THISCLASS::File_MakeDefaultKeyMappingFile,this);
		}

		fileRecentProfile=subMenu->AddTextItem(0,FSKEY_R,L"Recent")->AddSubMenu();
		RefreshRecentlyUsedFileList();
		recentProfile.PopulateMenu(*fileRecentProfile,20,File_Recent,this);

		subMenu->AddTextItem(0,FSKEY_X,L"Exit")->BindCallBack(&THISCLASS::File_Exit,this);
	}

	{
		auto *subMenu=mainMenu->AddTextItem(0,FSKEY_V,L"View")->GetSubMenu();
		subMenu->AddTextItem(0,FSKEY_P,L"Profile Dialog")->BindCallBack(&THISCLASS::View_OpenProfileDialog,this);
	}

	{
		auto *subMenu=mainMenu->AddTextItem(0,FSKEY_S,"State")->GetSubMenu();
		subMenu->AddTextItem(0,FSKEY_S,L"Save Machine State")->BindCallBack(&THISCLASS::State_SaveState,this);
		subMenu->AddTextItem(0,FSKEY_L,L"Load Machine State")->BindCallBack(&THISCLASS::State_LoadState,this);
		subMenu->AddTextItem(0,FSKEY_NULL,L"Load Machine State and Pause")->BindCallBack(&THISCLASS::State_LoadStateAndPause,this);
		subMenu->AddTextItem(0,FSKEY_NULL,L"Load Last Machine State")->BindCallBack(&THISCLASS::State_LoadLastState,this);
		subMenu->AddTextItem(0,FSKEY_NULL,L"Load Last Machine State and Pause")->BindCallBack(&THISCLASS::State_LoadLastStateAndPause,this);
	}

	{
		auto *subMenu=mainMenu->AddTextItem(0,FSKEY_T,L"FM TOWNS")->GetSubMenu();
		subMenu->AddTextItem(0,FSKEY_S,L"Start Virtual Machine")->BindCallBack(&THISCLASS::VM_Start,this);
		subMenu->AddTextItem(0,FSKEY_NULL,L"Start and Close GUI")->BindCallBack(&THISCLASS::VM_StartAndCloseGUI,this);
		subMenu->AddTextItem(0,FSKEY_NULL,L"Reset Virtual Machine")->BindCallBack(&THISCLASS::VM_Reset,this);
		subMenu->AddTextItem(0,FSKEY_Q,L"Power Off")->BindCallBack(&THISCLASS::VM_PowerOff,this);
		subMenu->AddTextItem(0,FSKEY_P,L"Pause")->BindCallBack(&THISCLASS::VM_Pause,this);
		subMenu->AddTextItem(0,FSKEY_R,L"Resume")->BindCallBack(&THISCLASS::VM_Resume,this);

		{
			auto *freqSubMenu=subMenu->AddTextItem(0,FSKEY_F,L"CPU Frequency")->AddSubMenu();
			freqSubMenu->AddTextItem(0,FSKEY_1,L"1MHz")->BindCallBack(&THISCLASS::VM_1MHz,this);
			freqSubMenu->AddTextItem(0,FSKEY_2,L"5MHz")->BindCallBack(&THISCLASS::VM_5MHz,this);
			freqSubMenu->AddTextItem(0,FSKEY_3,L"8MHz")->BindCallBack(&THISCLASS::VM_8MHz,this);
			freqSubMenu->AddTextItem(0,FSKEY_4,L"12MHz")->BindCallBack(&THISCLASS::VM_12MHz,this);
			freqSubMenu->AddTextItem(0,FSKEY_5,L"16MHz")->BindCallBack(&THISCLASS::VM_16MHz,this);
			freqSubMenu->AddTextItem(0,FSKEY_6,L"25MHz")->BindCallBack(&THISCLASS::VM_25MHz,this);
			freqSubMenu->AddTextItem(0,FSKEY_7,L"33MHz")->BindCallBack(&THISCLASS::VM_33MHz,this);
			freqSubMenu->AddTextItem(0,FSKEY_8,L"50MHz")->BindCallBack(&THISCLASS::VM_50MHz,this);
			freqSubMenu->AddTextItem(0,FSKEY_9,L"66MHz")->BindCallBack(&THISCLASS::VM_66MHz,this);
		}

		{
			auto *keyboardSubMenu=subMenu->AddTextItem(0,FSKEY_K,L"Keyboard Mode")->AddSubMenu();
			keyboardSubMenu->AddTextItem(0,FSKEY_D,L"Direct (for Gaming)")->BindCallBack(&THISCLASS::VM_Keyboard_Direct,this);
			keyboardSubMenu->AddTextItem(0,FSKEY_T,L"Translation1 (for Typing, ESC->ESC+BREAK)")->BindCallBack(&THISCLASS::VM_Keyboard_Translation1,this);
			keyboardSubMenu->AddTextItem(0,FSKEY_2,L"Translation2 (for Typing, ESC->ESC)")->BindCallBack(&THISCLASS::VM_Keyboard_Translation2,this);
			keyboardSubMenu->AddTextItem(0,FSKEY_3,L"Translation3 (for Typing, ESC->BREAK)")->BindCallBack(&THISCLASS::VM_Keyboard_Translation3,this);
		}

		subMenu->AddTextItem(0,FSKEY_NULL,L"Save Screenshot")->BindCallBack(&THISCLASS::VM_SaveScreenshot,this);

		{
			auto *testSubMenu=subMenu->AddTextItem(0,FSKEY_NULL,L"Test")->AddSubMenu();
			testSubMenu->AddTextItem(0,FSKEY_NULL,L"Print Time Balance")->BindCallBack(&THISCLASS::VM_Test_PrintTimeBalance,this);
		}
	}

	{
		auto *subMenu=mainMenu->AddTextItem(0,FSKEY_C,L"CD-ROM")->GetSubMenu();
		subMenu->AddTextItem(0,FSKEY_S,L"Select CD Image")->BindCallBack(&THISCLASS::CD_SelectImageFile,this);
		subMenu->AddTextItem(0,FSKEY_C,L"Open and Close CD drive")->BindCallBack(&THISCLASS::CD_OpenClose,this);
		subMenu->AddTextItem(0,FSKEY_E,L"Eject")->BindCallBack(&THISCLASS::CD_Eject,this);
		subMenu->AddTextItem(0,FSKEY_B,L"Stop CDDA")->BindCallBack(&THISCLASS::CD_CDDAStop,this);
	}

	{
		auto *subMenu=mainMenu->AddTextItem(0,FSKEY_0,L"FD0")->GetSubMenu();
		subMenu->AddTextItem(0,FSKEY_S,L"Select FD Image")->BindCallBack(&THISCLASS::FD0_SelectImageFile,this);
		FD0_writeProtectMenu=subMenu->AddTextItem(0,FSKEY_P,L"Write Protect");
		FD0_writeProtectMenu->BindCallBack(&THISCLASS::FD0_WriteProtect,this);
		FD0_writeUnprotectMenu=subMenu->AddTextItem(0,FSKEY_U,L"Write Unprotect");
		FD0_writeUnprotectMenu->BindCallBack(&THISCLASS::FD0_WriteUnprotect,this);
		subMenu->AddTextItem(0,FSKEY_J,L"Eject")->BindCallBack(&THISCLASS::FD0_Eject,this);
	}

	{
		auto *subMenu=mainMenu->AddTextItem(0,FSKEY_1,L"FD1")->GetSubMenu();
		subMenu->AddTextItem(0,FSKEY_S,L"Select FD Image")->BindCallBack(&THISCLASS::FD1_SelectImageFile,this);
		FD1_writeProtectMenu=subMenu->AddTextItem(0,FSKEY_P,L"Write Protect");
		FD1_writeProtectMenu->BindCallBack(&THISCLASS::FD1_WriteProtect,this);
		FD1_writeUnprotectMenu=subMenu->AddTextItem(0,FSKEY_U,L"Write Unprotect");
		FD1_writeUnprotectMenu->BindCallBack(&THISCLASS::FD1_WriteUnprotect,this);
		subMenu->AddTextItem(0,FSKEY_J,L"Eject")->BindCallBack(&THISCLASS::FD1_Eject,this);
	}

	{
		auto *subMenu=mainMenu->AddTextItem(0,FSKEY_D,"Devices")->GetSubMenu();
		subMenu->AddTextItem(0,FSKEY_0,"Game Port 0")->BindCallBack(&THISCLASS::Device_GamePort0,this);
		subMenu->AddTextItem(0,FSKEY_1,"Game Port 1")->BindCallBack(&THISCLASS::Device_GamePort1,this);

		auto *autoShotMenu=subMenu->AddTextItem(0,FSKEY_A,"Auto Shot")->AddSubMenu();
		for(int port=0; port<2; ++port)
		{
			char str[256];
			sprintf(str,"Game Port %d",port);
			auto *portMenu=autoShotMenu->AddTextItem(0,FSKEY_0+port,str)->AddSubMenu();
			for(int button=0; button<MAX_NUM_BUTTONS; ++button)
			{
				sprintf(str,"Button %d",button);
				auto *buttonMenu=portMenu->AddTextItem(0,FSKEY_0+button,str)->AddSubMenu();
				padAutoShot[port][button][0]=buttonMenu->AddTextItem(0,FSKEY_0,"None");
				padAutoShot[port][button][1]=buttonMenu->AddTextItem(0,FSKEY_1,"8 shots per second");
				padAutoShot[port][button][2]=buttonMenu->AddTextItem(0,FSKEY_2,"12 shots per second");
				padAutoShot[port][button][3]=buttonMenu->AddTextItem(0,FSKEY_3,"16 shots per second");

				padAutoShot[port][button][0]->BindCallBack(&THISCLASS::Device_AutoShot,this);
				padAutoShot[port][button][1]->BindCallBack(&THISCLASS::Device_AutoShot,this);
				padAutoShot[port][button][2]->BindCallBack(&THISCLASS::Device_AutoShot,this);
				padAutoShot[port][button][3]->BindCallBack(&THISCLASS::Device_AutoShot,this);

				padAutoShot[port][button][0]->SetCheck(YSTRUE);
			}
		}
	}

	{
		auto *subMenu=mainMenu->AddTextItem(0,FSKEY_U,L"Sound")->GetSubMenu();
		subMenu->AddTextItem(0,FSKEY_R,"Select WAV File for PCM Sampling in the VM")->BindCallBack(&THISCLASS::Audio_SelectWAVToPCMRecording,this);
	}

	{
		auto *subMenu=mainMenu->AddTextItem(0,FSKEY_A,L"Automation")->GetSubMenu();
		subMenu->AddTextItem(0,FSKEY_B,"Begin Recording")->BindCallBack(&THISCLASS::EventLog_StartRecording,this);
		subMenu->AddTextItem(0,FSKEY_E,"End Recording")->BindCallBack(&THISCLASS::EventLog_EndRecording,this);
		subMenu->AddTextItem(0,FSKEY_R,"Make Repeat")->BindCallBack(&THISCLASS::EventLog_MakeRepeat,this);
		subMenu->AddTextItem(0,FSKEY_P,"Play Back")->BindCallBack(&THISCLASS::EventLog_Replay,this);
		subMenu->AddTextItem(0,FSKEY_A,"Stop Play Back")->BindCallBack(&THISCLASS::EventLog_Stop,this);
		subMenu->AddTextItem(0,FSKEY_S,"Save Recording")->BindCallBack(&THISCLASS::EventLog_Save,this);
		subMenu->AddTextItem(0,FSKEY_O,"Open Recording")->BindCallBack(&THISCLASS::EventLog_Open,this);
	}

	{
		auto *subMenu=mainMenu->AddTextItem(0,FSKEY_H,L"Help")->GetSubMenu();
		subMenu->AddTextItem(0,FSKEY_H,L"Help")->BindCallBack(&THISCLASS::Help_Help,this);
		subMenu->AddTextItem(0,FSKEY_A,L"About")->BindCallBack(&THISCLASS::Help_About,this);
	}

	SetMainMenu(mainMenu);
}

void FsGuiMainCanvas::DeleteMainMenu(void)
{
	delete mainMenu;
	mainMenu=nullptr;
}

void FsGuiMainCanvas::OnInterval(void)
{
	FsGuiCanvas::Interval();

	if(YSTRUE==VMMustResume)
	{
		// VM must not be from close-modal call back.
		// macOS file dialog is not fully closed whehn close-modal call back is called, and resuming VM
		// (VM thread taking over main loop) will freeze the graphics until MENU button is clicked again.
		// Safe workaround is adding a flag, and always resume VM from OnInterval.
		VMMustResume=YSFALSE;
		ResumeVMIfSameProc();
		return;
	}

	if(true==subproc.SubprocRunning())
	{
		for(;;)
		{
			std::string str;
			if(true==subproc.Receive(str))
			{
				VMLog.push_back(str);
				std::cout << "VM:" << str;
			}
			else
			{
				break;
			}
		}
	}

	if(true!=IsVMRunning() && YSTRUE!=IsDialogUsed(profileDlg))
	{
		AddDialog(profileDlg);
	}

	{
		int key;
		while(FSKEY_NULL!=(key=FsInkey()))
		{
			if(pauseResumeKey==key)
			{
				VM_Resume(nullptr);
			}
			this->KeyIn(key,(YSBOOL)FsGetKeyState(FSKEY_SHIFT),(YSBOOL)FsGetKeyState(FSKEY_CTRL),(YSBOOL)FsGetKeyState(FSKEY_ALT));
		}
	}

	{
		int charCode;
		while(0!=(charCode=FsInkeyChar()))
		{
			this->CharIn(charCode);
		}
	}

	{
		int lb,mb,rb,mx,my;
		while(FSMOUSEEVENT_NONE!=FsGetMouseEvent(lb,mb,rb,mx,my))
		{
			if(YSOK!=this->SetMouseState((YSBOOL)lb,(YSBOOL)mb,(YSBOOL)rb,mx,my))
			{
			}
		}
	}

	{
		auto nTouch=FsGetNumCurrentTouch();
		auto touch=FsGetCurrentTouch();
		if(YSOK!=this->SetTouchState(nTouch,touch))
		{
		}
	}

	int winWid,winHei;
	FsGetWindowSize(winWid,winHei);
	this->SetWindowSize(winWid,winHei,/*autoArrangeDialog=*/YSTRUE);

	if(0!=FsCheckWindowExposure())
	{
		SetNeedRedraw(YSTRUE);
	}
}

template <class VMClass>
void FsGuiMainCanvas::DrawVMLastImage(VMClass &VM)
{
	if(true!=separateProcess &&
	   true==VM.IsRunning() &&
	   nullptr!=VM.outsideWorldPtr &&
	   nullptr!=VM.townsPtr)
	{
		auto image=VM.lastImage.GetImage();
		if(0<image.wid && 0<image.hei)
		{
			VM.outsideWorldPtr->UpdateStatusBitmap(*VM.townsPtr);
			VM.outsideWorldPtr->RenderBeforeSwapBuffers(image,*VM.townsPtr);
		}
	}
}

void FsGuiMainCanvas::Draw(void)
{
	// Do this at the beginning of Draw funtion.  This will allow one of the elements set SetNeedRedraw(YSTRUE) 
	// within drawing function so that Draw function will be called again in the next iteragion. >>
	SetNeedRedraw(YSFALSE);
	// <<

	glUseProgram(0);


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);


	int wid,hei;
	FsGetWindowSize(wid,hei);
	glViewport(0,0,wid,hei);

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);



	// 2D Drawing
#ifndef DONT_USE_FIXED_FUNCTION_PIPELINE
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0,(double)wid,(double)hei,0.0,-1.0,1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
#endif

	DrawVMLastImage(VMDefaultFidelity);
	DrawVMLastImage(VMHighFidelity);

	YsGLSLUsePlain2DRenderer(YsGLSLSharedPlain2DRenderer());
	YsGLSLUseWindowCoordinateInPlain2DDrawing(YsGLSLSharedPlain2DRenderer(),YSTRUE);
	YsGLSLEndUsePlain2DRenderer(YsGLSLSharedPlain2DRenderer());

	glDisable(GL_DEPTH_TEST);

	glUseProgram(0);
	glPixelZoom(1,1);
	FsGuiCanvas::Show();

	FsSwapBuffers();
}

void FsGuiMainCanvas::RefreshRecentlyUsedFileList(void)
{
	if(nullptr!=fileRecentProfile)
	{
		YsWString recentFn=GetRecentFileListFileName();
		FILE *fp=YsFileIO::Fopen(recentFn,"r");
		if(NULL!=fp)
		{
			YsTextFileInputStream inStream(fp);
			recentProfile.Open(inStream);
			fclose(fp);

			recentProfile.PopulateMenu(*fileRecentProfile,16,File_Recent,this);
		}
	}
}

void FsGuiMainCanvas::AddRecentlyUsedFile(const wchar_t wfn[])
{
	if(nullptr!=fileRecentProfile)
	{
		recentProfile.AddFile(wfn);
		recentProfile.PopulateMenu(*fileRecentProfile,16,File_Recent,this);

		YsWString recentFn=GetRecentFileListFileName();

		FILE *fp=YsFileIO::Fopen(recentFn,"w");
		if(NULL!=fp)
		{
			YsTextFileOutputStream outStream(fp);
			recentProfile.Save(outStream,16);
			fclose(fp);
		}
	}
}

/* static */ void FsGuiMainCanvas::File_Recent(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *itm)
{
	const wchar_t *wfn=itm->GetString();
	FsGuiMainCanvas *canvasPtr=(FsGuiMainCanvas *)appPtr;
	canvasPtr->LoadProfile(wfn);
	canvasPtr->AddRecentlyUsedFile(wfn); // This will move the selected to the top of the list.
}

YsWString FsGuiMainCanvas::GetRecentFileListFileName(void) const
{
	YsWString fName;
	fName.MakeFullPathName(GetTsugaruProfileDir(),L"RecentProfile.txt");
	return fName;
}



void FsGuiMainCanvas::Run(void)
{
	// Warn if the VM is already running.
	ReallyRun();
}

template <class VMClass>
void FsGuiMainCanvas::ReallyRunWithinSameProcess(VMClass &VM)
{
	VM.profile=profileDlg->GetProfile();
	if(""==VM.profile.CMOSFName)
	{
		VM.profile.CMOSFName=GetCMOSFileName();
	}
	RemoveDialog(profileDlg);

	VM.Run();
	if(true!=VM.IsRunning())
	{
		AddDialog(profileDlg);
	}
	SetNeedRedraw(YSTRUE);
}

bool FsGuiMainCanvas::ReallyRun(bool usePipe)
{
	if(true==IsVMRunning())
	{
		VM_Already_Running_Error();
		return false;
	}

	auto missing=CheckMissingROMFiles();
	if(0<missing.size())
	{
		YsWString msg;
		msg=L"Missing ROM files:";
		for(auto fName : missing)
		{
			msg+=L" ";
			msg+=fName;
		}
		auto msgDlg=FsGuiDialog::CreateSelfDestructiveDialog <FsGuiMessageBoxDialog>();
		msgDlg->Make(L"Error",msg,L"OK",nullptr);
		AttachModalDialog(msgDlg);
		return false;
	}


	auto profile=profileDlg->GetProfile();
	separateProcess=profile.separateProcess; // This is the only chance to change this flag.

	{
		pauseResumeKey=FsStringToKeyCode(profile.pauseResumeKeyLabel.c_str());
		if(FSKEY_NULL==pauseResumeKey)
		{
			pauseResumeKey=FSKEY_SCROLLLOCK;
		}
	}

	if(true==separateProcess)
	{
		auto argv=profile.MakeArgv();
		argv[0]=FindTsugaruCUI();
		argv.push_back("-CMOS");
		argv.push_back(GetCMOSFileName());


		for(auto &arg : argv)
		{
			YsWString utf16;
			utf16.SetUTF8String(arg.c_str());
			YsString sysEncode;
			YsUnicodeToSystemEncoding(sysEncode,utf16);
			arg=sysEncode.c_str();
		}


		for(auto arg : argv)
		{
			std::cout << arg << std::endl;
		}

		if(0==argv[0].size())
		{
			auto msgDlg=FsGuiDialog::CreateSelfDestructiveDialog <FsGuiMessageBoxDialog>();
			msgDlg->Make(
			    L"Error",
				L"Cannot find Tsugaru CUI module.\n"
				L"Tsugaru CUI module must be in the same directory\n"
				L"as the GUI module.",
				L"OK",nullptr);
			AttachModalDialog(msgDlg);
			return false;
		}
		else if(true!=subproc.StartProc(argv,usePipe))
		{
			YsWString msg;
			msg.SetUTF8String(subproc.errMsg.c_str());

			auto msgDlg=FsGuiDialog::CreateSelfDestructiveDialog <FsGuiMessageBoxDialog>();
			msgDlg->Make(L"Error",msg,L"OK",nullptr);
			AttachModalDialog(msgDlg);
			return false;
		}
		RemoveDialog(profileDlg);
	}
	else
	{
		if(i486DXCommon::HIGH_FIDELITY==profile.CPUFidelityLevel)
		{
			ReallyRunWithinSameProcess(VMHighFidelity);
		}
		else
		{
			ReallyRunWithinSameProcess(VMDefaultFidelity);
		}
	}

	return true;
}

bool FsGuiMainCanvas::IsVMRunning(void) const
{
	if(true==separateProcess)
	{
		return subproc.SubprocRunning();
	}
	else
	{
		return VMHighFidelity.IsRunning() || VMDefaultFidelity.IsRunning();
	}
}
void FsGuiMainCanvas::SendVMCommand(std::string cmd)
{
	if(true==separateProcess)
	{
		subproc.Send(cmd);
	}
	else
	{
		if(true==VMDefaultFidelity.IsRunning())
		{
			VMDefaultFidelity.SendCommand(cmd);
		}
		else if(true==VMHighFidelity.IsRunning())
		{
			VMHighFidelity.SendCommand(cmd);
		}
	}
}

template <class VMClass>
bool FsGuiMainCanvas::ResumeVMIfSameProc(VMClass &VM)
{
	if(true!=separateProcess && true==VM.IsRunning())
	{
		VM.Run();
		if(true!=VM.IsRunning())
		{
			AddDialog(profileDlg);
		}
		SetNeedRedraw(YSTRUE);
		return true;
	}
	return false;
}

void FsGuiMainCanvas::ResumeVMIfSameProc(void)
{
	if(true!=ResumeVMIfSameProc(VMDefaultFidelity))
	{
		if(true!=ResumeVMIfSameProc(VMHighFidelity))
		{
			// ResumeVMIfSameProc(VMLowFidelity);
		}
	}
}

std::string FsGuiMainCanvas::FindTsugaruCUI(void) const
{
	YsWString exeFile(L"Tsugaru_CUI");
#ifdef _WIN32
	exeFile.Append(L".exe");
#endif
	YsWString exePath=YsSpecialPath::GetProgramBaseDirW();

	YsWString ful;
	ful.MakeFullPathName(exePath,exeFile);
	if(YSTRUE==YsFileIO::CheckFileExist(ful))
	{
		YsString utf8;
		YsUnicodeToSystemEncoding(utf8,ful);
		return utf8.c_str();
	}

	YsWString pth,fil;
	YsSpecialPath::GetProgramFileNameW().SeparatePathFile(pth,fil);
	ful.MakeFullPathName(pth,exeFile);
	if(YSTRUE==YsFileIO::CheckFileExist(ful))
	{
		YsString utf8;
		YsUnicodeToSystemEncoding(utf8,ful);
		return utf8.c_str();
	}

	return "";
}

std::string FsGuiMainCanvas::GetCMOSFileName(void) const
{
	YsWString ful;
	ful.MakeFullPathName(GetTsugaruProfileDir(),L"CMOS.DAT");
	YsString utf8;
	YsUnicodeToSystemEncoding(utf8,ful);
	return utf8.c_str();
}

YsWString FsGuiMainCanvas::GetDefaultNewDiskImageFileName(void) const
{
	YsWString fName;
	if(0<lastSelectedFDFName.Strlen())
	{
		fName=lastSelectedFDFName;
	}
	else if(0<lastSelectedProfileFName.Strlen())
	{
		fName=lastSelectedProfileFName;
	}
	else
	{
		fName=profileDlg->profileFNameTxt->GetWText();
	}

	YsWString path,file;
	fName.SeparatePathFile(path,file);

	YsWString ful;
	ful.MakeFullPathName(path,L"disk.d77");
	return ful;
}

YsWString FsGuiMainCanvas::GetDefaultOpenDiskImageFileName(void) const
{
	if(0<lastSelectedFDFName.Strlen())
	{
		return lastSelectedFDFName;
	}

	YsWString fName,path,file;
	if(0<lastSelectedProfileFName.Strlen())
	{
		fName=lastSelectedProfileFName;
	}
	else
	{
		fName=profileDlg->profileFNameTxt->GetWText();
	}

	fName.SeparatePathFile(path,file);

	YsWString ful;
	ful.MakeFullPathName(path,L"*.d77");
	return ful;
}

YsWString FsGuiMainCanvas::GetDefaultNewHardDiskImageFileName(void) const
{
	YsWString fName;
	if(0<lastSelectedHDFName.Strlen())
	{
		fName=lastSelectedHDFName;
	}
	else if(0<lastSelectedProfileFName.Strlen())
	{
		fName=lastSelectedProfileFName;
	}
	else
	{
		fName=profileDlg->profileFNameTxt->GetWText();
	}

	YsWString path,file;
	fName.SeparatePathFile(path,file);

	YsWString ful;
	ful.MakeFullPathName(path,L"harddisk.h0");
	return ful;
}

YsWString FsGuiMainCanvas::GetDefaultNewEventLogFileName(void) const
{
	YsWString fName;
	if(0<lastEventFName.Strlen())
	{
		fName=lastEventFName;
	}
	else if(0<lastSelectedProfileFName.Strlen())
	{
		fName=lastSelectedProfileFName;
	}
	else
	{
		fName=profileDlg->profileFNameTxt->GetWText();
	}

	YsWString path,file;
	fName.SeparatePathFile(path,file);

	YsWString ful;
	ful.MakeFullPathName(path,L"newevent.evt");
	return ful;
}

std::vector <YsWString> FsGuiMainCanvas::CheckMissingROMFiles(void) const
{
	std::vector <YsWString> missing;
	const YsWString ROMFName[]=
	{
		L"FMT_DIC.ROM",
		L"FMT_DOS.ROM",
		// L"FMT_F20.ROM",
		L"FMT_FNT.ROM",
		L"FMT_SYS.ROM",
	};
	YsWString path=profileDlg->ROMDirTxt->GetWString();
	for(auto file : ROMFName)
	{
		YsWString ful;
		ful.MakeFullPathName(path,file);
		if(YSTRUE!=YsFileIO::CheckFileExist(ful))
		{
			missing.push_back(file);
		}
	}
	return missing;
}


////////////////////////////////////////////////////////////



void FsGuiMainCanvas::VM_Not_Running_Error(void)
{
	auto dlg=FsGuiDialog::CreateSelfDestructiveDialog <FsGuiMessageBoxDialog>();
	dlg->Make(L"Error!",L"Virtual Machine is not runnnig.",L"OK",nullptr);
	AttachModalDialog(dlg);
}

void FsGuiMainCanvas::VM_Already_Running_Error(void)
{
	auto dlg=FsGuiDialog::CreateSelfDestructiveDialog <FsGuiMessageBoxDialog>();
	dlg->Make(
	    L"Error!",
	    L"Virtual Machine is already runnnig.\n"
	    L"Close the Virtual Machine before starting a new session.",
	    L"OK",nullptr);
	AttachModalDialog(dlg);
}



////////////////////////////////////////////////////////////



void FsGuiMainCanvas::File_SaveDefaultProfile(FsGuiPopUpMenuItem *)
{
	auto dlg=FsGuiDialog::CreateSelfDestructiveDialog <FsGuiMessageBoxDialogWithPayload<YsWString> >();
	dlg->payload=L""; // Not used.
	dlg->Make(L"Overwrite Default",L"Are you sure?",L"Yes",L"No");
	dlg->BindCloseModalCallBack(&FsGuiMainCanvas::File_SaveDefaultConfirm,this);
	AttachModalDialog(dlg);
}
void FsGuiMainCanvas::File_SaveDefaultConfirm(FsGuiDialog *dlg,int returnCode)
{
	auto fdlg=dynamic_cast <FsGuiMessageBoxDialogWithPayload<YsWString> *>(dlg);
	if(nullptr!=fdlg && (int)YSOK==returnCode)
	{
		SaveProfile(GetDefaultProfileFileName());
	}
}
void FsGuiMainCanvas::File_ReloadDefaultProfile(FsGuiPopUpMenuItem *)
{
	LoadProfile(GetDefaultProfileFileName());
}

void FsGuiMainCanvas::File_SaveProfile(FsGuiPopUpMenuItem *)
{
	auto fName=profileDlg->profileFNameTxt->GetWText();
	if(fName==GetDefaultProfileFileName())
	{
		File_SaveDefaultProfile(nullptr);
	}
	else
	{
		SaveProfile(fName);
	}
}
void FsGuiMainCanvas::File_OpenProfile(FsGuiPopUpMenuItem *)
{
	if(true!=IsVMRunning())
	{
		auto fdlg=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiFileDialog>();
		fdlg->Initialize();
		fdlg->mode=FsGuiFileDialog::MODE_OPEN;
		fdlg->multiSelect=YSFALSE;
		fdlg->title.Set(L"Open Profile");
		fdlg->fileExtensionArray.Append(L".Tsugaru");
		fdlg->defaultFileName=profileDlg->profileFNameTxt->GetWText();
		fdlg->BindCloseModalCallBack(&THISCLASS::File_OpenProfile_FileSelected,this);
		AttachModalDialog(fdlg);
	}
	else
	{
		VM_Already_Running_Error();
	}
}
void FsGuiMainCanvas::File_OpenProfile_FileSelected(FsGuiDialog *dlg,int returnCode)
{
	auto fdlg=dynamic_cast <FsGuiFileDialog *>(dlg);
	if(nullptr!=fdlg && (int)YSOK==returnCode)
	{
		AddDialog(profileDlg);
		profileDlg->profileFNameTxt->SetText(fdlg->selectedFileArray[0]);
		LoadProfile(fdlg->selectedFileArray[0]);
		AddRecentlyUsedFile(fdlg->selectedFileArray[0]);
	}
}
void FsGuiMainCanvas::File_SaveProfileAs(FsGuiPopUpMenuItem *)
{
	auto fdlg=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiFileDialog>();
	fdlg->Initialize();
	fdlg->mode=FsGuiFileDialog::MODE_SAVE;
	fdlg->multiSelect=YSFALSE;
	fdlg->title.Set(L"Save Profile As");
	fdlg->fileExtensionArray.Append(L".Tsugaru");
	fdlg->defaultFileName=profileDlg->profileFNameTxt->GetWText();
	fdlg->BindCloseModalCallBack(&THISCLASS::File_SaveProfileAs_FileSelected,this);
	AttachModalDialog(fdlg);
}
void FsGuiMainCanvas::File_SaveProfileAs_FileSelected(FsGuiDialog *dlg,int returnCode)
{
	auto fdlg=dynamic_cast <FsGuiFileDialog *>(dlg);
	if(nullptr!=fdlg && (int)YSOK==returnCode)
	{
		auto fName=fdlg->selectedFileArray[0];
		if(YSTRUE==YsFileIO::CheckFileExist(fName))
		{
			auto dlg=FsGuiDialog::CreateSelfDestructiveDialog <FsGuiMessageBoxDialogWithPayload<YsWString> >();
			dlg->payload=fName;
			dlg->Make(L"Overwrite Default",L"Are you sure?",L"Yes",L"No");
			dlg->BindCloseModalCallBack(&FsGuiMainCanvas::File_SaveProfileAs_OverwriteConfirm,this);
			AttachModalDialog(dlg);
		}
		else
		{
			profileDlg->profileFNameTxt->SetText(fdlg->selectedFileArray[0]);
			SaveProfile(fName);
			AddRecentlyUsedFile(fName);
		}
	}
}
void FsGuiMainCanvas::File_SaveProfileAs_OverwriteConfirm(FsGuiDialog *dlgIn,int returnCode)
{
	auto dlg=dynamic_cast <FsGuiMessageBoxDialogWithPayload<YsWString> *>(dlgIn);
	if(nullptr!=dlg && (int)YSOK==returnCode)
	{
		profileDlg->profileFNameTxt->SetText(dlg->payload);
		SaveProfile(dlg->payload);
		AddRecentlyUsedFile(dlg->payload);
	}
}


void FsGuiMainCanvas::File_MakeDefaultKeyMappingFile(FsGuiPopUpMenuItem *)
{
	auto fdlg=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiFileDialog>();
	fdlg->Initialize();
	fdlg->mode=FsGuiFileDialog::MODE_SAVE;
	fdlg->multiSelect=YSFALSE;
	fdlg->title.Set(L"Make Default Key Mapping File");
	fdlg->fileExtensionArray.Append(L".txt");
	fdlg->defaultFileName=L"Tsugaru_keymap.txt";
	fdlg->BindCloseModalCallBack(&THISCLASS::File_MakeDefaultKeyMappingFile_Selected,this);
	AttachModalDialog(fdlg);
}
void FsGuiMainCanvas::File_MakeDefaultKeyMappingFile_Selected(FsGuiDialog *dlg,int returnCode)
{
	auto fdlg=dynamic_cast <FsGuiFileDialog *>(dlg);
	if(nullptr!=fdlg && (int)YSOK==returnCode)
	{
		auto fName=fdlg->selectedFileArray[0];
		if(YSTRUE==YsFileIO::CheckFileExist(fName))
		{
			auto dlg=FsGuiDialog::CreateSelfDestructiveDialog <FsGuiMessageBoxDialogWithPayload<YsWString> >();
			dlg->payload=fName;
			dlg->Make(L"Overwrite Default",L"Are you sure?",L"Yes",L"No");
			dlg->BindCloseModalCallBack(&FsGuiMainCanvas::File_MakeDefaultKeyMappingFile_OverwriteConfirm,this);
			AttachModalDialog(dlg);
		}
		else
		{
			YsFileIO::File ofp(fName,"w");
			if(nullptr!=ofp.Fp())
			{
				std::unique_ptr <FsSimpleWindowConnection> outside_world(new FsSimpleWindowConnection);
				for(auto txt : outside_world->MakeDefaultKeyMappingText())
				{
					fprintf(ofp.Fp(),"%s\n",txt.c_str());
				}
			}
		}
	}
}
void FsGuiMainCanvas::File_MakeDefaultKeyMappingFile_OverwriteConfirm(FsGuiDialog *dlgIn,int returnCode)
{
	auto dlg=dynamic_cast <FsGuiMessageBoxDialogWithPayload<YsWString> *>(dlgIn);
	if(nullptr!=dlg && (int)YSOK==returnCode)
	{
		auto fName=dlg->payload;
		YsFileIO::File ofp(fName,"w");
		if(nullptr!=ofp.Fp())
		{
			std::unique_ptr <FsSimpleWindowConnection> outside_world(new FsSimpleWindowConnection);
			for(auto txt : outside_world->MakeDefaultKeyMappingText())
			{
				fprintf(ofp.Fp(),"%s\n",txt.c_str());
			}
		}
	}
}


void FsGuiMainCanvas::SaveProfile(YsWString fName) const
{
	auto profile=profileDlg->GetProfile();
	YsFileIO::File fp(fName,"w");
	auto outStream=fp.OutStream();
	for(auto str : profile.Serialize())
	{
		outStream.Printf("%s\n",str.c_str());
	}
	fp.Fclose();
	lastSelectedProfileFName=fName;
}
void FsGuiMainCanvas::LoadProfile(YsWString fName)
{
	std::vector <std::string> text;

	profileDlg->profileFNameTxt->SetText(fName);

	YsFileIO::File fp(fName,"r");
	if(fp!=nullptr)
	{
		auto inStream=fp.InStream();
		YsString str;
		while(YSTRUE!=inStream.EndOfFile())
		{
			text.push_back(inStream.Fgets().c_str());
		}
		TownsProfile profile;
		if(true==profile.Deserialize(text))
		{
			profileDlg->SetProfile(profile);
			if(true==profile.autoStart)
			{
				VM_Start(nullptr);
			}
		}
		else
		{
			auto dlg=FsGuiDialog::CreateSelfDestructiveDialog <FsGuiMessageBoxDialog>();
			YsWString errMsg;
			errMsg.SetUTF8String(profile.errorMsg.c_str());
			dlg->Make(L"Profile Load Error",errMsg,L"OK",nullptr);
			AttachModalDialog(dlg);
		}

		lastSelectedProfileFName=fName;
		for(auto imgFName : profile.fdImgFName)
		{
			if(""!=imgFName)
			{
				lastSelectedFDFName.SetUTF8String(imgFName.c_str());
			}
		}
		if(""!=profile.cdImgFName)
		{
			lastSelectedCDFName.SetUTF8String(profile.cdImgFName.data());
		}
		for(auto scsi : profile.scsiImg)
		{
			if(TownsProfile::SCSIIMAGE_HARDDISK==scsi.imageType && ""!=scsi.imgFName)
			{
				lastSelectedHDFName.SetUTF8String(scsi.imgFName.data());
				break;
			}
		}
	}
}

YsWString FsGuiMainCanvas::GetDefaultProfileFileName(void) const
{
	YsWString ful;
	ful.MakeFullPathName(GetTsugaruProfileDir(),L"Tsugaru_Default.Tsugaru");
	return ful;
}

YsWString FsGuiMainCanvas::GetTsugaruProfileDir(void) const
{
	YsWString path;
	path.MakeFullPathName(YsSpecialPath::GetUserDocDirW(),L"Tsugaru_TOWNS");
	YsFileIO::MkDir(path);
	return path;
}



////////////////////////////////////////////////////////////

void FsGuiMainCanvas::File_Exit(FsGuiPopUpMenuItem *)
{
	auto msgDlg=FsGuiDialog::CreateSelfDestructiveDialog <FsGuiMessageBoxDialog>();
	msgDlg->Make(L"Confirm Exit?",L"Confirm Exit?",L"Yes",L"No");
	msgDlg->BindCloseModalCallBack(&THISCLASS::File_Exit_ConfirmExitCallBack,this);
	AttachModalDialog(msgDlg);
}

void FsGuiMainCanvas::File_Exit_ConfirmExitCallBack(FsGuiDialog *,int returnValue)
{
	if(YSOK==(YSRESULT)returnValue)
	{
		File_Exit_ReallyExit();
	}
}

void FsGuiMainCanvas::File_Exit_ReallyExit(void)
{
	this->appMustTerminate=YSTRUE;
}

void FsGuiMainCanvas::File_New_1232KB(FsGuiPopUpMenuItem *)
{
	genFloppyDisk=true;
	genDiskSize=1232;

	auto fdlg=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiFileDialog>();
	fdlg->Initialize();
	fdlg->mode=FsGuiFileDialog::MODE_SAVE;
	fdlg->multiSelect=YSFALSE;
	fdlg->title.Set(L"Create 1232KB Disk Image");
	fdlg->fileExtensionArray.Append(L".BIN");
	fdlg->fileExtensionArray.Append(L".D77");
	fdlg->fileExtensionArray.Append(L".RDD");
	fdlg->fileExtensionArray.Append(L".D88");
	fdlg->fileExtensionArray.Append(L".XDF");
	fdlg->defaultFileName=GetDefaultNewDiskImageFileName();
	fdlg->BindCloseModalCallBack(&THISCLASS::File_New_FileSelected,this);
	AttachModalDialog(fdlg);
}
void FsGuiMainCanvas::File_New_1440KB(FsGuiPopUpMenuItem *)
{
	genFloppyDisk=true;
	genDiskSize=1440;

	auto fdlg=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiFileDialog>();
	fdlg->Initialize();
	fdlg->mode=FsGuiFileDialog::MODE_SAVE;
	fdlg->multiSelect=YSFALSE;
	fdlg->title.Set(L"Create 1440KB Disk Image");
	fdlg->fileExtensionArray.Append(L".BIN");
	fdlg->fileExtensionArray.Append(L".D77");
	fdlg->fileExtensionArray.Append(L".RDD");
	fdlg->fileExtensionArray.Append(L".D88");
	fdlg->fileExtensionArray.Append(L".XDF");
	fdlg->defaultFileName=GetDefaultNewDiskImageFileName();
	fdlg->BindCloseModalCallBack(&THISCLASS::File_New_FileSelected,this);
	AttachModalDialog(fdlg);
}
void FsGuiMainCanvas::File_New_720KB(FsGuiPopUpMenuItem *)
{
	genFloppyDisk=true;
	genDiskSize=720;

	auto fdlg=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiFileDialog>();
	fdlg->Initialize();
	fdlg->mode=FsGuiFileDialog::MODE_SAVE;
	fdlg->multiSelect=YSFALSE;
	fdlg->title.Set(L"Create 720KB Disk Image");
	fdlg->fileExtensionArray.Append(L".BIN");
	fdlg->fileExtensionArray.Append(L".D77");
	fdlg->fileExtensionArray.Append(L".RDD");
	fdlg->fileExtensionArray.Append(L".D88");
	fdlg->fileExtensionArray.Append(L".XDF");
	fdlg->defaultFileName=GetDefaultNewDiskImageFileName();
	fdlg->BindCloseModalCallBack(&THISCLASS::File_New_FileSelected,this);
	AttachModalDialog(fdlg);
}
void FsGuiMainCanvas::File_New_640KB(FsGuiPopUpMenuItem *)
{
	genFloppyDisk=true;
	genDiskSize=640;

	auto fdlg=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiFileDialog>();
	fdlg->Initialize();
	fdlg->mode=FsGuiFileDialog::MODE_SAVE;
	fdlg->multiSelect=YSFALSE;
	fdlg->title.Set(L"Create 640KB Disk Image");
	fdlg->fileExtensionArray.Append(L".BIN");
	fdlg->fileExtensionArray.Append(L".D77");
	fdlg->fileExtensionArray.Append(L".RDD");
	fdlg->fileExtensionArray.Append(L".D88");
	fdlg->fileExtensionArray.Append(L".XDF");
	fdlg->defaultFileName=GetDefaultNewDiskImageFileName();
	fdlg->BindCloseModalCallBack(&THISCLASS::File_New_FileSelected,this);
	AttachModalDialog(fdlg);
}
void FsGuiMainCanvas::File_New_HDD(FsGuiPopUpMenuItem *)
{
	genFloppyDisk=false;
	auto dlg=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiInputNumberDialog>();
	dlg->Make(
		20.0,0,
	    L"Create HDD Image",L"Specify Size in MB",L"",
	    L"OK",L"Cancel");
	dlg->BindCloseModalCallBack(&THISCLASS::File_New_HDD_SizeSelected,this);
	AttachModalDialog(dlg);
}
void FsGuiMainCanvas::File_New_HDD_SizeSelected(FsGuiDialog *dlg,int returnCode)
{
	auto numDlg=dynamic_cast <FsGuiInputNumberDialog *>(dlg);
	if(nullptr!=numDlg && (int)YSOK==returnCode)
	{
		int MB=(int)numDlg->GetNumber();
		if(MB<1 || 1024<MB)
		{
			auto dlg=FsGuiDialog::CreateSelfDestructiveDialog <FsGuiMessageBoxDialog>();
			dlg->Make(L"Error",L"The number needs to be between 1 and 1024.",L"OK",nullptr);
			AttachModalDialog(dlg);
		}
		else
		{
			genDiskSize=MB;
			auto fdlg=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiFileDialog>();
			fdlg->Initialize();
			fdlg->mode=FsGuiFileDialog::MODE_SAVE;
			fdlg->multiSelect=YSFALSE;
			fdlg->title.Set(L"Create Hard Disk Image");
			fdlg->fileExtensionArray.Append(L".HD");
			fdlg->fileExtensionArray.Append(L".HDI");
			fdlg->fileExtensionArray.Append(L".HDM");
			fdlg->fileExtensionArray.Append(L".H0");
			fdlg->fileExtensionArray.Append(L".H1");
			fdlg->fileExtensionArray.Append(L".H2");
			fdlg->fileExtensionArray.Append(L".H3");
			fdlg->defaultFileName=GetDefaultNewHardDiskImageFileName();
			fdlg->BindCloseModalCallBack(&THISCLASS::File_New_FileSelected,this);
			AttachModalDialog(fdlg);
		}
	}
}
void FsGuiMainCanvas::File_New_FileSelected(FsGuiDialog *dlg,int returnCode)
{
	auto fdlg=dynamic_cast <FsGuiFileDialog *>(dlg);
	if(nullptr!=fdlg && (int)YSOK==returnCode)
	{
		auto fName=fdlg->selectedFileArray[0];
		if(YSTRUE==YsFileIO::CheckFileExist(fName))
		{
			auto dlg=FsGuiDialog::CreateSelfDestructiveDialog <FsGuiMessageBoxDialog>();
			dlg->Make(L"File Already Exists",L"File Already Exists.  It does not overwrite a file.",L"OK",nullptr);
			AttachModalDialog(dlg);
		}
		else
		{
			if(true==genFloppyDisk)
			{
				lastSelectedFDFName=fName;
				std::vector <unsigned char> img;

				switch(genDiskSize)
				{
				case 1232:
					img=Get1232KBFloppyDiskImage();
					break;
				case 1440:
					img=Get1440KBFloppyDiskImage();
					break;
				case 720:
					img=Get720KBFloppyDiskImage();
					break;
				case 640:
					img=Get640KBFloppyDiskImage();
					break;
				}

				auto ext=fName.GetExtension();
				if(0==ext.STRCMP(L".D77") || 0==ext.STRCMP(L".D88"))
				{
					D77File d77;
					d77.SetRawBinary(img);
					img=d77.MakeD77Image();
				}
				else if(0==ext.STRCMP(L".RDD"))
				{
					D77File d77;
					d77.SetRawBinary(img);
					img=d77.MakeRDDImage();
				}

				bool result=false;
				YsFileIO::File ofp(fName,"wb");
				if(nullptr!=ofp.Fp())
				{
					auto wroteSize=fwrite(img.data(),1,img.size(),ofp.Fp());
					if(wroteSize==img.size())
					{
						result=true;
					}
					ofp.Fclose();
				}

				if(true!=result)
				{
					auto dlg=FsGuiDialog::CreateSelfDestructiveDialog <FsGuiMessageBoxDialog>();
					dlg->Make(L"Error",L"Failed to write file.",L"OK",nullptr);
					AttachModalDialog(dlg);
				}
				else
				{
					auto dlg=FsGuiDialog::CreateSelfDestructiveDialog <FsGuiMessageBoxDialog>();
					dlg->Make(L"Success.",L"Created a floppy disk image.",L"OK",nullptr);
					AttachModalDialog(dlg);
				}
			}
			else
			{
				lastSelectedHDFName=fName;

				bool result=false;
				YsFileIO::File ofp(fName,"wb");
				if(nullptr!=ofp.Fp())
				{
					long long int wroteSize=0;
					std::vector <unsigned char> zero;
					zero.resize(1024*1024);
					for(auto &z : zero)
					{
						z=0;
					}
					for(unsigned int i=0; i<genDiskSize; ++i)
					{
						wroteSize+=fwrite(zero.data(),1,zero.size(),ofp.Fp());
					}
					if(wroteSize==1024*1024*genDiskSize)
					{
						result=true;
					}
					ofp.Fclose();
				}

				if(true!=result)
				{
					auto dlg=FsGuiDialog::CreateSelfDestructiveDialog <FsGuiMessageBoxDialog>();
					dlg->Make(L"Error",L"Failed to write file.",L"OK",nullptr);
					AttachModalDialog(dlg);
				}
				else
				{
					auto dlg=FsGuiDialog::CreateSelfDestructiveDialog <FsGuiMessageBoxDialog>();
					dlg->Make(L"Success.",L"Created a hard disk image.",L"OK",nullptr);
					AttachModalDialog(dlg);
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////

class FsGuiMainCanvas::File_ClearCMOSDialog : public FsGuiDialog
{
public:
	FsGuiButton *confirmClearCMOSBtn,*reallyConfirmBtn,*reallyReallyConfirmBtn;
	FsGuiButton *cancelBtn;
	std::string cmosFName;
	void Make(void);
	virtual void OnButtonClick(FsGuiButton *btn);
};

void FsGuiMainCanvas::File_ClearCMOSDialog::Make(void)
{
	AddStaticText(0,FSKEY_NULL,
		"CMOS stores boot drive, drive-letter assignments, and other\n"
		"system settings.  When the system does not start, clearing\n"
		"CMOS may fix the boot problem.  Otherwise, you may have to\n"
		"re-configure your drive-letters etc.\n",
		YSTRUE);

	confirmClearCMOSBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"Confirm Clear CMOS",YSTRUE);
	reallyConfirmBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"Really Confirm Clear CMOS",YSFALSE);
	reallyConfirmBtn->Disable();
	reallyReallyConfirmBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"Really Really Confirm Clear",YSFALSE);
	reallyReallyConfirmBtn->Disable();
	cancelBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"Cancel",YSTRUE);
	Fit();
}

/* virtual */ void FsGuiMainCanvas::File_ClearCMOSDialog::OnButtonClick(FsGuiButton *btn)
{
	if(btn==cancelBtn)
	{
		CloseModalDialog(0);
	}
	if(btn==confirmClearCMOSBtn)
	{
		reallyConfirmBtn->Enable();
	}
	if(btn==reallyConfirmBtn)
	{
		reallyReallyConfirmBtn->Enable();
	}
	if(btn==reallyReallyConfirmBtn)
	{
		remove(cmosFName.c_str());
		CloseModalDialog(0);
	}
}

void FsGuiMainCanvas::File_ClearCMOS(FsGuiPopUpMenuItem *)
{
	if(true!=IsVMRunning())
	{
		auto dlg=FsGuiDialog::CreateSelfDestructiveDialog<File_ClearCMOSDialog>();
		dlg->Make();
		dlg->cmosFName=GetCMOSFileName();
		AttachModalDialog(dlg);
	}
	else
	{
		VM_Already_Running_Error();
	}
}

////////////////////////////////////////////////////////////



void FsGuiMainCanvas::View_OpenProfileDialog(FsGuiPopUpMenuItem *)
{
	if(true!=IsVMRunning())
	{
		AddDialog(profileDlg);
	}
	else
	{
		VM_Already_Running_Error();
	}
}



////////////////////////////////////////////////////////////



void FsGuiMainCanvas::State_SaveState(FsGuiPopUpMenuItem *)
{
	if(true==IsVMRunning())
	{
		auto defFn=lastStateFName;
		if(0==defFn.Strlen())
		{
			defFn.MakeFullPathName(GetTsugaruProfileDir(),L"*.TState");
		}
		auto fdlg=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiFileDialog>();
		fdlg->Initialize();
		fdlg->mode=FsGuiFileDialog::MODE_SAVE;
		fdlg->multiSelect=YSFALSE;
		fdlg->title.Set(L"Save Machine State");
		fdlg->fileExtensionArray.Append(L".TState");
		fdlg->defaultFileName=defFn;
		fdlg->BindCloseModalCallBack(&THISCLASS::State_SaveState_FileSelected,this);
		AttachModalDialog(fdlg);
	}
	else
	{
		VM_Not_Running_Error();
	}
}
void FsGuiMainCanvas::State_SaveState_FileSelected(FsGuiDialog *dlg,int returnCode)
{
	auto fdlg=dynamic_cast<FsGuiFileDialog *>(dlg);
	if(nullptr!=fdlg && (int)YSOK==returnCode)
	{
		selectedStateFName=fdlg->selectedFileArray[0];
		if(YSTRUE==YsFileIO::CheckFileExist(selectedStateFName))
		{
			auto dlg=FsGuiDialog::CreateSelfDestructiveDialog <FsGuiMessageBoxDialog>();
			dlg->Make(L"File Already Exists",L"File Already Exists.  Overwrite?",L"Confirm",L"Cancel");
			dlg->BindCloseModalCallBack(&THISCLASS::State_SaveState_Confirm,this);
			AttachModalDialog(dlg);
		}
		else
		{
			State_SaveState_Save(selectedStateFName);
		}
	}
}
void FsGuiMainCanvas::State_SaveState_Confirm(FsGuiDialog *dlg,int returnCode)
{
	if((int)YSOK==returnCode)
	{
		State_SaveState_Save(selectedStateFName);
	}
}
void FsGuiMainCanvas::State_SaveState_Save(YsWString fName)
{
	lastStateFName=selectedStateFName;
	YsString utf8;
	YsUnicodeToSystemEncoding(utf8,selectedStateFName);

	YsString cmd;
	cmd="SAVESTATE \"";
	cmd.Append(utf8);
	cmd.Append("\"");
	SendVMCommand(cmd.data());

	VMMustResume=YSTRUE;
}

void FsGuiMainCanvas::State_LoadState(FsGuiPopUpMenuItem *)
{
	if(true==IsVMRunning())
	{
		auto defFn=lastStateFName;
		if(0==defFn.Strlen())
		{
			defFn.MakeFullPathName(GetTsugaruProfileDir(),L"*.TState");
		}
		auto fdlg=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiFileDialog>();
		fdlg->Initialize();
		fdlg->mode=FsGuiFileDialog::MODE_OPEN;
		fdlg->multiSelect=YSFALSE;
		fdlg->title.Set(L"Load Machine State");
		fdlg->fileExtensionArray.Append(L".TState");
		fdlg->defaultFileName=defFn;
		fdlg->BindCloseModalCallBack(&THISCLASS::State_LoadState_FileSelected,this);
		AttachModalDialog(fdlg);

		loadStateThenPause=false;
	}
	else
	{
		VM_Not_Running_Error();
	}
}
void FsGuiMainCanvas::State_LoadStateAndPause(FsGuiPopUpMenuItem *)
{
	if(true==IsVMRunning())
	{
		auto defFn=lastStateFName;
		if(0==defFn.Strlen())
		{
			defFn.MakeFullPathName(GetTsugaruProfileDir(),L"*.TState");
		}
		auto fdlg=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiFileDialog>();
		fdlg->Initialize();
		fdlg->mode=FsGuiFileDialog::MODE_OPEN;
		fdlg->multiSelect=YSFALSE;
		fdlg->title.Set(L"Load Machine State");
		fdlg->fileExtensionArray.Append(L".TState");
		fdlg->defaultFileName=defFn;
		fdlg->BindCloseModalCallBack(&THISCLASS::State_LoadState_FileSelected,this);
		AttachModalDialog(fdlg);

		loadStateThenPause=true;
	}
	else
	{
		VM_Not_Running_Error();
	}
}
void FsGuiMainCanvas::State_LoadState_FileSelected(FsGuiDialog *dlg,int returnCode)
{
	auto fdlg=dynamic_cast<FsGuiFileDialog *>(dlg);
	if(nullptr!=fdlg && (int)YSOK==returnCode)
	{
		selectedStateFName=fdlg->selectedFileArray[0];
		if(YSTRUE==YsFileIO::CheckFileExist(selectedStateFName))
		{
			YsString utf8;
			YsUnicodeToSystemEncoding(utf8,selectedStateFName);

			YsString cmd;
			cmd="LOADSTATE \"";
			cmd.Append(utf8);
			cmd.Append("\"");
			SendVMCommand(cmd.data());

			if(true==loadStateThenPause)
			{
				SendVMCommand("PAUSE");
			}

			VMMustResume=YSTRUE;

			lastStateFName=selectedStateFName;
		}
	}
}
void FsGuiMainCanvas::State_LoadLastState(FsGuiPopUpMenuItem *)
{
	if(true==IsVMRunning())
	{
		if(0<lastStateFName.Strlen())
		{
			lastStateFName=selectedStateFName;
			YsString utf8;
			YsUnicodeToSystemEncoding(utf8,selectedStateFName);

			YsString cmd;
			cmd="LOADSTATE \"";
			cmd.Append(utf8);
			cmd.Append("\"");
			SendVMCommand(cmd.data());

			VMMustResume=YSTRUE;
		}
	}
	else
	{
		VM_Not_Running_Error();
	}
}
void FsGuiMainCanvas::State_LoadLastStateAndPause(FsGuiPopUpMenuItem *)
{
	if(true==IsVMRunning())
	{
		if(0<lastStateFName.Strlen())
		{
			lastStateFName=selectedStateFName;
			YsString utf8;
			YsUnicodeToSystemEncoding(utf8,selectedStateFName);

			YsString cmd;
			cmd="LOADSTATE \"";
			cmd.Append(utf8);
			cmd.Append("\"");
			SendVMCommand(cmd.data());
			SendVMCommand("PAUSE");

			VMMustResume=YSTRUE;
		}
	}
	else
	{
		VM_Not_Running_Error();
	}
}



////////////////////////////////////////////////////////////

void FsGuiMainCanvas::VM_Start(FsGuiPopUpMenuItem *)
{
	Run();
}
void FsGuiMainCanvas::VM_StartAndCloseGUI(FsGuiPopUpMenuItem *)
{
	if(true==ReallyRun())
	{
		exit(1);
	}
}
void FsGuiMainCanvas::VM_PowerOff(FsGuiPopUpMenuItem *)
{
	if(true==IsVMRunning())
	{
		auto msgDlg=FsGuiDialog::CreateSelfDestructiveDialog <FsGuiMessageBoxDialog>();
		msgDlg->Make(L"Confirm Power Off?",L"Power Off?",L"Yes",L"No");
		msgDlg->BindCloseModalCallBack(&THISCLASS::VM_PowerOffConfirm,this);
		AttachModalDialog(msgDlg);
	}
	else
	{
		VM_Not_Running_Error();
	}
}
void FsGuiMainCanvas::VM_PowerOffConfirm(FsGuiDialog *dlg,int returnCode)
{
	if((int)YSOK==returnCode)
	{
		SendVMCommand("Q\n");
		VMMustResume=YSTRUE;
	}
}
void FsGuiMainCanvas::VM_Reset(FsGuiPopUpMenuItem *)
{
	if(true==IsVMRunning())
	{
		SendVMCommand("RESET\n");
		VMMustResume=YSTRUE;
	}
	else
	{
		VM_Not_Running_Error();
	}
}
void FsGuiMainCanvas::VM_Pause(FsGuiPopUpMenuItem *)
{
	if(true==IsVMRunning())
	{
		SendVMCommand("PAU\n");
	}
	else
	{
		VM_Not_Running_Error();
	}
}
void FsGuiMainCanvas::VM_Resume(FsGuiPopUpMenuItem *)
{
	if(true==IsVMRunning())
	{
		SendVMCommand("RUN\n");
		VMMustResume=YSTRUE;
	}
	else
	{
		VM_Not_Running_Error();
	}
}
void FsGuiMainCanvas::VM_1MHz(FsGuiPopUpMenuItem *)
{
	if(true==IsVMRunning())
	{
		SendVMCommand("FREQ 1\n");
		VMMustResume=YSTRUE;
	}
	else
	{
		VM_Not_Running_Error();
	}
}
void FsGuiMainCanvas::VM_5MHz(FsGuiPopUpMenuItem *)
{
	if(true==IsVMRunning())
	{
		SendVMCommand("FREQ 5\n");
		VMMustResume=YSTRUE;
	}
	else
	{
		VM_Not_Running_Error();
	}
}
void FsGuiMainCanvas::VM_8MHz(FsGuiPopUpMenuItem *)
{
	if(true==IsVMRunning())
	{
		SendVMCommand("FREQ 8\n");
		VMMustResume=YSTRUE;
	}
	else
	{
		VM_Not_Running_Error();
	}
}
void FsGuiMainCanvas::VM_12MHz(FsGuiPopUpMenuItem *)
{
	if(true==IsVMRunning())
	{
		SendVMCommand("FREQ 12\n");
		VMMustResume=YSTRUE;
	}
	else
	{
		VM_Not_Running_Error();
	}
}
void FsGuiMainCanvas::VM_16MHz(FsGuiPopUpMenuItem *)
{
	if(true==IsVMRunning())
	{
		SendVMCommand("FREQ 16\n");
		VMMustResume=YSTRUE;
	}
	else
	{
		VM_Not_Running_Error();
	}
}
void FsGuiMainCanvas::VM_25MHz(FsGuiPopUpMenuItem *)
{
	if(true==IsVMRunning())
	{
		SendVMCommand("FREQ 25\n");
		VMMustResume=YSTRUE;
	}
	else
	{
		VM_Not_Running_Error();
	}
}
void FsGuiMainCanvas::VM_33MHz(FsGuiPopUpMenuItem *)
{
	if(true==IsVMRunning())
	{
		SendVMCommand("FREQ 33\n");
		VMMustResume=YSTRUE;
	}
	else
	{
		VM_Not_Running_Error();
	}
}
void FsGuiMainCanvas::VM_50MHz(FsGuiPopUpMenuItem *)
{
	if(true==IsVMRunning())
	{
		SendVMCommand("FREQ 50\n");
		VMMustResume=YSTRUE;
	}
	else
	{
		VM_Not_Running_Error();
	}
}
void FsGuiMainCanvas::VM_66MHz(FsGuiPopUpMenuItem *)
{
	if(true==IsVMRunning())
	{
		SendVMCommand("FREQ 66\n");
		VMMustResume=YSTRUE;
	}
	else
	{
		VM_Not_Running_Error();
	}
}

void FsGuiMainCanvas::VM_Keyboard_Direct(FsGuiPopUpMenuItem *)
{
	if(true==IsVMRunning())
	{
		SendVMCommand("KEYBOARD DIRECT\n");
		VMMustResume=YSTRUE;
	}
	else
	{
		VM_Not_Running_Error();
	}
}
void FsGuiMainCanvas::VM_Keyboard_Translation1(FsGuiPopUpMenuItem *)
{
	if(true==IsVMRunning())
	{
		SendVMCommand("KEYBOARD TRANS1\n");
		VMMustResume=YSTRUE;
	}
	else
	{
		VM_Not_Running_Error();
	}
}
void FsGuiMainCanvas::VM_Keyboard_Translation2(FsGuiPopUpMenuItem *)
{
	if(true==IsVMRunning())
	{
		SendVMCommand("KEYBOARD TRANS2\n");
		VMMustResume=YSTRUE;
	}
	else
	{
		VM_Not_Running_Error();
	}
}
void FsGuiMainCanvas::VM_Keyboard_Translation3(FsGuiPopUpMenuItem *)
{
	if(true==IsVMRunning())
	{
		SendVMCommand("KEYBOARD TRANS3\n");
		VMMustResume=YSTRUE;
	}
	else
	{
		VM_Not_Running_Error();
	}
}

void FsGuiMainCanvas::VM_SaveScreenshot(FsGuiPopUpMenuItem *)
{
	if(true==IsVMRunning())
	{
		YsWString path,file;
		profileDlg->profileFNameTxt->GetWText().SeparatePathFile(path,file);

		auto fdlg=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiFileDialog>();
		fdlg->Initialize();
		fdlg->mode=FsGuiFileDialog::MODE_SAVE;
		fdlg->multiSelect=YSFALSE;
		fdlg->title.Set(L"Save Screenshot");
		fdlg->fileExtensionArray.Append(L".PNG");
		fdlg->defaultFileName=path;
		fdlg->BindCloseModalCallBack(&THISCLASS::VM_SaveScreenshot_FileSelected,this);
		AttachModalDialog(fdlg);
	}
	else
	{
		VM_Not_Running_Error();
	}
}

void FsGuiMainCanvas::VM_Test_PrintTimeBalance(FsGuiPopUpMenuItem *)
{
	if(true==IsVMRunning())
	{
		SendVMCommand("PRI TIMEBALANCE");
		VMMustResume=YSTRUE;
	}
	else
	{
		VM_Not_Running_Error();
	}
}

void FsGuiMainCanvas::VM_SaveScreenshot_FileSelected(FsGuiDialog *dlg,int returnCode)
{
	auto fdlg=dynamic_cast <FsGuiFileDialog *>(dlg);
	if(nullptr!=fdlg && (int)YSOK==returnCode)
	{
		YsWString fName=fdlg->selectedFileArray[0];
		YsString utf8;
		YsUnicodeToSystemEncoding(utf8,fName);
		std::string cmd="SS ";
		cmd.push_back('\"');
		cmd+=utf8.c_str();
		cmd.push_back('\"');
		cmd.push_back('\n');
		SendVMCommand(cmd);
		VMMustResume=YSTRUE;
	}
}

////////////////////////////////////////////////////////////

void FsGuiMainCanvas::Audio_SelectWAVToPCMRecording(FsGuiPopUpMenuItem *)
{
	if(true==IsVMRunning())
	{
		YsWString path,file;
		profileDlg->profileFNameTxt->GetWText().SeparatePathFile(path,file);

		auto fdlg=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiFileDialog>();
		fdlg->Initialize();
		fdlg->mode=FsGuiFileDialog::MODE_OPEN;
		fdlg->multiSelect=YSFALSE;
		fdlg->title.Set(L"Select WAV for PCM Recording");
		fdlg->fileExtensionArray.Append(L".wav");
		fdlg->defaultFileName=path;
		fdlg->BindCloseModalCallBack(&THISCLASS::Audio_Audio_SelectWAVToPCMRecording_FileSelected,this);
		AttachModalDialog(fdlg);
	}
	else
	{
		VM_Not_Running_Error();
	}
}
void FsGuiMainCanvas::Audio_Audio_SelectWAVToPCMRecording_FileSelected(FsGuiDialog *dlg,int returnCode)
{
	auto fdlg=dynamic_cast <FsGuiFileDialog *>(dlg);
	if(nullptr!=fdlg && (int)YSOK==returnCode)
	{
		YsWString fName=fdlg->selectedFileArray[0];
		YsString utf8;
		YsUnicodeToSystemEncoding(utf8,fName);
		std::string cmd="LOADWAV ";
		cmd.push_back('\"');
		cmd+=utf8.c_str();
		cmd.push_back('\"');
		cmd.push_back('\n');
		SendVMCommand(cmd);
		VMMustResume=YSTRUE;
	}
}

////////////////////////////////////////////////////////////

void FsGuiMainCanvas::CD_SelectImageFile(FsGuiPopUpMenuItem *)
{
	if(true==IsVMRunning())
	{
		auto fdlg=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiFileDialog>();
		fdlg->Initialize();
		fdlg->mode=FsGuiFileDialog::MODE_OPEN;
		fdlg->multiSelect=YSFALSE;
		fdlg->title.Set(L"Open CD Image");
		fdlg->fileExtensionArray.Append(L".CUE");
		fdlg->fileExtensionArray.Append(L".ISO");
		fdlg->fileExtensionArray.Append(L".MDS");
		fdlg->defaultFileName=profileDlg->CDImgTxt->GetWString();
		fdlg->BindCloseModalCallBack(&THISCLASS::CD_ImageFileSelected,this);
		AttachModalDialog(fdlg);
	}
	else
	{
		VM_Not_Running_Error();
	}
}
void FsGuiMainCanvas::CD_ImageFileSelected(FsGuiDialog *dlg,int returnCode)
{
	auto fdlg=dynamic_cast <FsGuiFileDialog *>(dlg);
	if(nullptr!=fdlg && (int)YSOK==returnCode)
	{
		YsWString fName=fdlg->selectedFileArray[0];
		YsString utf8;
		YsUnicodeToSystemEncoding(utf8,fName);
		std::string cmd="CDLOAD ";
		cmd.push_back('\"');
		cmd+=utf8.c_str();
		cmd.push_back('\"');
		cmd.push_back('\n');
		SendVMCommand(cmd);
		lastSelectedCDFName=fName;
	}
}

void FsGuiMainCanvas::CD_Eject(FsGuiPopUpMenuItem *)
{
	if(true==IsVMRunning())
	{
		SendVMCommand("CDEJECT\n");
		VMMustResume=YSTRUE;
	}
	else
	{
		VM_Not_Running_Error();
	}
}
void FsGuiMainCanvas::CD_OpenClose(FsGuiPopUpMenuItem *)
{
	if(true==IsVMRunning())
	{
		SendVMCommand("CDOPENCLOSE\n");
		VMMustResume=YSTRUE;
	}
	else
	{
		VM_Not_Running_Error();
	}
}
void FsGuiMainCanvas::CD_CDDAStop(FsGuiPopUpMenuItem *)
{
	if(true==IsVMRunning())
	{
		SendVMCommand("CDDASTOP\n");
		VMMustResume=YSTRUE;
	}
	else
	{
		VM_Not_Running_Error();
	}
}

void FsGuiMainCanvas::FD0_SelectImageFile(FsGuiPopUpMenuItem *)
{
	if(true==IsVMRunning())
	{
		auto fdlg=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiFileDialog>();
		fdlg->Initialize();
		fdlg->mode=FsGuiFileDialog::MODE_OPEN;
		fdlg->multiSelect=YSFALSE;
		fdlg->title.Set(L"Open FD0 Image");
		fdlg->fileExtensionArray.Append(L".BIN");
		fdlg->fileExtensionArray.Append(L".D77");
		fdlg->fileExtensionArray.Append(L".RDD");
		fdlg->fileExtensionArray.Append(L".D88");
		fdlg->fileExtensionArray.Append(L".XDF");
		fdlg->defaultFileName=GetDefaultOpenDiskImageFileName();
		fdlg->BindCloseModalCallBack(&THISCLASS::FD0_ImageFileSelected,this);
		AttachModalDialog(fdlg);
	}
	else
	{
		VM_Not_Running_Error();
	}
}
void FsGuiMainCanvas::FD0_ImageFileSelected(FsGuiDialog *dlg,int returnCode)
{
	auto fdlg=dynamic_cast <FsGuiFileDialog *>(dlg);
	if(nullptr!=fdlg && (int)YSOK==returnCode)
	{
		auto fName=fdlg->selectedFileArray[0];
		YsString utf8;
		YsUnicodeToSystemEncoding(utf8,fName);

		std::string cmd="FD0LOAD ";
		cmd.push_back('\"');
		cmd+=utf8.c_str();
		cmd.push_back('\"');
		cmd.push_back('\n');
		SendVMCommand(cmd);
		VMMustResume=YSTRUE;

		FD0_writeProtectMenu->SetCheck(YSFALSE);
		FD0_writeUnprotectMenu->SetCheck(YSTRUE);

		lastSelectedFDFName=fName;
	}
}
void FsGuiMainCanvas::FD0_WriteProtect(FsGuiPopUpMenuItem *)
{
	if(true==IsVMRunning())
	{
		SendVMCommand("FD0WP\n");
		VMMustResume=YSTRUE;

		FD0_writeProtectMenu->SetCheck(YSTRUE);
		FD0_writeUnprotectMenu->SetCheck(YSFALSE);
	}
	else
	{
		VM_Not_Running_Error();
	}
}
void FsGuiMainCanvas::FD0_WriteUnprotect(FsGuiPopUpMenuItem *)
{
	if(true==IsVMRunning())
	{
		SendVMCommand("FD0UP\n");
		VMMustResume=YSTRUE;

		FD0_writeProtectMenu->SetCheck(YSFALSE);
		FD0_writeUnprotectMenu->SetCheck(YSTRUE);
	}
	else
	{
		VM_Not_Running_Error();
	}
}
void FsGuiMainCanvas::FD0_Eject(FsGuiPopUpMenuItem *)
{
	if(true==IsVMRunning())
	{
		SendVMCommand("FD0EJECT\n");
		VMMustResume=YSTRUE;

		FD0_writeProtectMenu->SetCheck(YSFALSE);
		FD0_writeUnprotectMenu->SetCheck(YSTRUE);
	}
	else
	{
		VM_Not_Running_Error();
	}
}

void FsGuiMainCanvas::FD1_SelectImageFile(FsGuiPopUpMenuItem *)
{
	if(true==IsVMRunning())
	{
		auto fdlg=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiFileDialog>();
		fdlg->Initialize();
		fdlg->mode=FsGuiFileDialog::MODE_OPEN;
		fdlg->multiSelect=YSFALSE;
		fdlg->title.Set(L"Open FD1 Image");
		fdlg->fileExtensionArray.Append(L".BIN");
		fdlg->fileExtensionArray.Append(L".D77");
		fdlg->fileExtensionArray.Append(L".RDD");
		fdlg->fileExtensionArray.Append(L".D88");
		fdlg->fileExtensionArray.Append(L".XDF");
		fdlg->defaultFileName=GetDefaultOpenDiskImageFileName();
		fdlg->BindCloseModalCallBack(&THISCLASS::FD1_ImageFileSelected,this);
		AttachModalDialog(fdlg);
	}
	else
	{
		VM_Not_Running_Error();
	}
}
void FsGuiMainCanvas::FD1_ImageFileSelected(FsGuiDialog *dlg,int returnCode)
{
	auto fdlg=dynamic_cast <FsGuiFileDialog *>(dlg);
	if(nullptr!=fdlg && (int)YSOK==returnCode)
	{
		auto fName=fdlg->selectedFileArray[0];
		YsString utf8;
		YsUnicodeToSystemEncoding(utf8,fName);

		std::string cmd="FD1LOAD ";
		cmd.push_back('\"');
		cmd+=utf8.c_str();
		cmd.push_back('\"');
		cmd.push_back('\n');
		SendVMCommand(cmd);
		VMMustResume=YSTRUE;

		FD1_writeProtectMenu->SetCheck(YSFALSE);
		FD1_writeUnprotectMenu->SetCheck(YSTRUE);

		lastSelectedFDFName=fName;
	}
}
void FsGuiMainCanvas::FD1_WriteProtect(FsGuiPopUpMenuItem *)
{
	if(true==IsVMRunning())
	{
		SendVMCommand("FD1WP\n");
		VMMustResume=YSTRUE;

		FD1_writeProtectMenu->SetCheck(YSTRUE);
		FD1_writeUnprotectMenu->SetCheck(YSFALSE);
	}
	else
	{
		VM_Not_Running_Error();
	}
}
void FsGuiMainCanvas::FD1_WriteUnprotect(FsGuiPopUpMenuItem *)
{
	if(true==IsVMRunning())
	{
		SendVMCommand("FD1UP\n");
		VMMustResume=YSTRUE;

		FD1_writeProtectMenu->SetCheck(YSFALSE);
		FD1_writeUnprotectMenu->SetCheck(YSTRUE);
	}
	else
	{
		VM_Not_Running_Error();
	}
}
void FsGuiMainCanvas::FD1_Eject(FsGuiPopUpMenuItem *)
{
	if(true==IsVMRunning())
	{
		SendVMCommand("FD1EJECT\n");
		VMMustResume=YSTRUE;

		FD1_writeProtectMenu->SetCheck(YSFALSE);
		FD1_writeUnprotectMenu->SetCheck(YSTRUE);
	}
	else
	{
		VM_Not_Running_Error();
	}
}



////////////////////////////////////////////////////////////



class FsGuiMainCanvas::GamePortDialog : public FsGuiDialog
{
public:
	int port;
	FsGuiDropList *deviceList;
	FsGuiButton *okBtn,*cancelBtn;
	void Make(int port);
	virtual void OnButtonClick(FsGuiButton *btn);
};

void FsGuiMainCanvas::GamePortDialog::Make(int port)
{
	this->port=port;
	YsString label;
	label.Printf("Game Port %d\n",port);
	AddStaticText(0,FSKEY_NULL,label,YSTRUE);

	deviceList=AddEmptyDropList(0,FSKEY_NULL,"Device Emulation",16,32,32,YSTRUE);
	for(auto sel : selectableGameportDevices)
	{
		deviceList->AddString(GamePortEmulationTypeToHumanReadable(sel).c_str(),YSFALSE);
	}

	okBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"OK",YSTRUE);
	cancelBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"Cancel",YSFALSE);

	Fit();
}
/* virtual */ void FsGuiMainCanvas::GamePortDialog::OnButtonClick(FsGuiButton *btn)
{
	if(btn==okBtn)
	{
		CloseModalDialog(YSOK);
	}
	else if(btn==cancelBtn)
	{
		CloseModalDialog(YSERR);
	}
}

void FsGuiMainCanvas::Device_GamePort0(FsGuiPopUpMenuItem *)
{
	if(true==IsVMRunning())
	{
		auto *dlg=FsGuiDialog::CreateSelfDestructiveDialog<GamePortDialog>();
		dlg->Make(0);
		dlg->BindCloseModalCallBack(&FsGuiMainCanvas::Device_GamePort_DeviceSelected,this);
		AttachModalDialog(dlg);
	}
	else
	{
		VM_Not_Running_Error();
	}
}
void FsGuiMainCanvas::Device_GamePort1(FsGuiPopUpMenuItem *)
{
	if(true==IsVMRunning())
	{
		auto *dlg=FsGuiDialog::CreateSelfDestructiveDialog<GamePortDialog>();
		dlg->Make(1);
		dlg->BindCloseModalCallBack(&FsGuiMainCanvas::Device_GamePort_DeviceSelected,this);
		AttachModalDialog(dlg);
	}
	else
	{
		VM_Not_Running_Error();
	}
}
void FsGuiMainCanvas::Device_GamePort_DeviceSelected(FsGuiDialog *dlgIn,int returnCode)
{
	if((int)YSOK==returnCode)
	{
		auto dlg=dynamic_cast<GamePortDialog *>(dlgIn);
		if(nullptr!=dlg)
		{
			auto sel=dlg->deviceList->GetSelection();
			if(0<=sel)
			{
				auto devStr=dlg->deviceList->GetSelectedString();
				auto emulationType=HumanReadableToGamePortEmulationType(devStr.c_str());

				char str[256];
				sprintf(str,"GAMEPORT %d %s\n",dlg->port,TownsGamePortEmuToStr(emulationType).c_str());
				SendVMCommand(str);
				VMMustResume=YSTRUE;
			}
		}
	}
}

void FsGuiMainCanvas::Device_AutoShot(FsGuiPopUpMenuItem *menu)
{
	if(true==IsVMRunning())
	{
		unsigned int interval[NUM_AUTOSHOT_SPEED]=
		{
			0,
			125,
			83,
			63,
		};
		for(int port=0; port<2; ++port)
		{
			for(int button=0; button<MAX_NUM_BUTTONS; ++button)
			{
				for(int speed=0; speed<NUM_AUTOSHOT_SPEED; ++speed)
				{
					if(menu==padAutoShot[port][button][speed])
					{
						for(int b=0; b<NUM_AUTOSHOT_SPEED; ++b)
						{
							padAutoShot[port][button][b]->SetCheck(YSFALSE);
						}
						menu->SetCheck(YSTRUE);

						char str[256];
						sprintf(str,"AUTOSHOT %d %d %u",port,button,interval[speed]);
						SendVMCommand(str);
						VMMustResume=YSTRUE;
					}
				}
			}
		}
	}
	else
	{
		VM_Not_Running_Error();
	}
}



////////////////////////////////////////////////////////////



void FsGuiMainCanvas::EventLog_StartRecording(FsGuiPopUpMenuItem *)
{
	if(true==IsVMRunning())
	{
		SendVMCommand("ENA EVENTLOG\n");
		VMMustResume=YSTRUE;
	}
	else
	{
		VM_Not_Running_Error();
	}
}
void FsGuiMainCanvas::EventLog_EndRecording(FsGuiPopUpMenuItem *)
{
	if(true==IsVMRunning())
	{
		SendVMCommand("DIS EVENTLOG\n");
		VMMustResume=YSTRUE;
	}
	else
	{
		VM_Not_Running_Error();
	}
}
void FsGuiMainCanvas::EventLog_MakeRepeat(FsGuiPopUpMenuItem *)
{
	if(true==IsVMRunning())
	{
		SendVMCommand("DIS EVENTLOG\n");
		SendVMCommand("MAKEREPEATEVENTLOG\n");
		VMMustResume=YSTRUE;
	}
	else
	{
		VM_Not_Running_Error();
	}
}
void FsGuiMainCanvas::EventLog_Replay(FsGuiPopUpMenuItem *)
{
	if(true==IsVMRunning())
	{
		SendVMCommand("DIS EVENTLOG\n");
		SendVMCommand("PLAYEVT");
		VMMustResume=YSTRUE;
	}
	else
	{
		VM_Not_Running_Error();
	}
}
void FsGuiMainCanvas::EventLog_Stop(FsGuiPopUpMenuItem *)
{
	if(true==IsVMRunning())
	{
		SendVMCommand("STOPEVT\n");
		VMMustResume=YSTRUE;
	}
	else
	{
		VM_Not_Running_Error();
	}
}

void FsGuiMainCanvas::EventLog_Open(FsGuiPopUpMenuItem *)
{
	if(true==IsVMRunning())
	{
		auto fdlg=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiFileDialog>();
		fdlg->Initialize();
		fdlg->mode=FsGuiFileDialog::MODE_OPEN;
		fdlg->multiSelect=YSFALSE;
		fdlg->title.Set(L"Open Event-Log");
		fdlg->fileExtensionArray.Append(L".evt");
		fdlg->defaultFileName=GetDefaultNewEventLogFileName();
		fdlg->BindCloseModalCallBack(&THISCLASS::EventLog_Open_FileSelected,this);
		AttachModalDialog(fdlg);
	}
	else
	{
		VM_Not_Running_Error();
	}
}
void FsGuiMainCanvas::EventLog_Open_FileSelected(FsGuiDialog *dlg,int returnCode)
{
	auto fdlg=dynamic_cast <FsGuiFileDialog *>(dlg);
	if(nullptr!=fdlg && (int)YSOK==returnCode)
	{
		auto fName=fdlg->selectedFileArray[0];

		YsString utf8;
		YsUnicodeToSystemEncoding(utf8,fName);

		lastEventFName=fName;

		SendVMCommand("DIS EVENTLOG\n");

		YsString cmd;
		cmd="LOADEVT \"";
		cmd.Append(utf8);
		cmd.Append("\"");
		cmd.Append("\n");
		SendVMCommand(cmd.data());
		VMMustResume=YSTRUE;

		lastEventFName=fName;
	}
}

void FsGuiMainCanvas::EventLog_Save(FsGuiPopUpMenuItem *)
{
	if(true==IsVMRunning())
	{
		auto fdlg=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiFileDialog>();
		fdlg->Initialize();
		fdlg->mode=FsGuiFileDialog::MODE_SAVE;
		fdlg->multiSelect=YSFALSE;
		fdlg->title.Set(L"Save Event-Log As");
		fdlg->fileExtensionArray.Append(L".evt");
		fdlg->defaultFileName=GetDefaultNewEventLogFileName();
		fdlg->BindCloseModalCallBack(&THISCLASS::EventLog_Save_FileSelected,this);
		AttachModalDialog(fdlg);
	}
	else
	{
		VM_Not_Running_Error();
	}
}
void FsGuiMainCanvas::EventLog_Save_FileSelected(FsGuiDialog *dlg,int returnCode)
{
	auto fdlg=dynamic_cast <FsGuiFileDialog *>(dlg);
	if(nullptr!=fdlg && (int)YSOK==returnCode)
	{
		auto fName=fdlg->selectedFileArray[0];
		if(YSTRUE==YsFileIO::CheckFileExist(fName))
		{
			auto dlg=FsGuiDialog::CreateSelfDestructiveDialog <FsGuiMessageBoxDialogWithPayload<YsWString> >();
			dlg->payload=fName;
			dlg->Make(L"Overwrite Event Log?",L"Are you sure?",L"Yes",L"No");
			dlg->BindCloseModalCallBack(&FsGuiMainCanvas::EventLog_Save_Confirm,this);
			AttachModalDialog(dlg);
		}
		else
		{
			EventLog_Save_Save(fName);
		}
	}
}
void FsGuiMainCanvas::EventLog_Save_Confirm(FsGuiDialog *dlgIn,int returnCode)
{
	auto dlg=dynamic_cast <FsGuiMessageBoxDialogWithPayload<YsWString> *>(dlgIn);
	if(nullptr!=dlg && (int)YSOK==returnCode)
	{
		EventLog_Save_Save(dlg->payload);
	}
}
void FsGuiMainCanvas::EventLog_Save_Save(YsWString fName)
{
	YsString utf8;
	YsUnicodeToSystemEncoding(utf8,fName);

	SendVMCommand("DIS EVENTLOG\n");

	YsString cmd;
	cmd="SAVEEVT \"";
	cmd.Append(utf8);
	cmd.Append("\"");
	cmd.Append("\n");
	SendVMCommand(cmd.data());
	VMMustResume=YSTRUE;

	lastEventFName=fName;
}



////////////////////////////////////////////////////////////



void FsGuiMainCanvas::Help_About(FsGuiPopUpMenuItem *)
{
	auto msgDlg=FsGuiDialog::CreateSelfDestructiveDialog <FsGuiMessageBoxDialog>();
	const wchar_t *msg=
	    L"FM TOWNS Emulator Tsugaru\n"
	    L"Developed by CaptainYS\n"
	    L"http://www.ysflight.com\n"
	    L"PEB01130@nifty.com\n"
	    L"\n";
	msgDlg->Make(L"About Tsugaru",msg,L"OK",nullptr);
	AttachModalDialog(msgDlg);
}
void FsGuiMainCanvas::Help_Help(FsGuiPopUpMenuItem *)
{
	auto msgDlg=FsGuiDialog::CreateSelfDestructiveDialog <FsGuiMessageBoxDialog>();
	msgDlg->Make(
	    L"HELP",
	    L"(To be written)",
	    L"OK",nullptr);
	AttachModalDialog(msgDlg);
}
