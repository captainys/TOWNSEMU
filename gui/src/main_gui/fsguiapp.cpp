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
			auto subSubMenu=subMenu->AddTextItem(0,FSKEY_N,L"New")->AddSubMenu();
			subSubMenu->AddTextItem(0,FSKEY_NULL,L"1232KB Floppy Disk Image")->BindCallBack(&THISCLASS::File_New_1232KB,this);
			subSubMenu->AddTextItem(0,FSKEY_NULL,L"1440KB Floppy Disk Image")->BindCallBack(&THISCLASS::File_New_1440KB,this);
			subSubMenu->AddTextItem(0,FSKEY_NULL,L"640KB Floppy Disk Image")->BindCallBack(&THISCLASS::File_New_640KB,this);
			subSubMenu->AddTextItem(0,FSKEY_NULL,L"720KB Floppy Disk Image")->BindCallBack(&THISCLASS::File_New_720KB,this);
			subSubMenu->AddTextItem(0,FSKEY_NULL,L"Hard-Disk Image")->BindCallBack(&THISCLASS::File_New_HDD,this);
		}

		subMenu->AddTextItem(0,FSKEY_X,L"Exit")->BindCallBack(&THISCLASS::File_Exit,this);
	}

	{
		auto *subMenu=mainMenu->AddTextItem(0,FSKEY_T,L"FM TOWNS")->GetSubMenu();
		subMenu->AddTextItem(0,FSKEY_S,L"Start Virtual Machine")->BindCallBack(&THISCLASS::VM_Start,this);
		subMenu->AddTextItem(0,FSKEY_NULL,L"Start and Close GUI")->BindCallBack(&THISCLASS::VM_StartAndCloseGUI,this);
		subMenu->AddTextItem(0,FSKEY_Q,L"Power Off")->BindCallBack(&THISCLASS::VM_PowerOff,this);
		subMenu->AddTextItem(0,FSKEY_P,L"Pause")->BindCallBack(&THISCLASS::VM_Pause,this);
		subMenu->AddTextItem(0,FSKEY_R,L"Resume")->BindCallBack(&THISCLASS::VM_Resume,this);

		{
			auto *freqSubMenu=subMenu->AddTextItem(0,FSKEY_F,L"CPU Frequency")->AddSubMenu();
			freqSubMenu->AddTextItem(0,FSKEY_1,L"1MHz")->BindCallBack(&THISCLASS::VM_1MHz,this);
			freqSubMenu->AddTextItem(0,FSKEY_4,L"4MHz")->BindCallBack(&THISCLASS::VM_4MHz,this);
			freqSubMenu->AddTextItem(0,FSKEY_8,L"8MHz")->BindCallBack(&THISCLASS::VM_8MHz,this);
			freqSubMenu->AddTextItem(0,FSKEY_2,L"12MHz")->BindCallBack(&THISCLASS::VM_12MHz,this);
			freqSubMenu->AddTextItem(0,FSKEY_6,L"16MHz")->BindCallBack(&THISCLASS::VM_16MHz,this);
			freqSubMenu->AddTextItem(0,FSKEY_5,L"25MHz")->BindCallBack(&THISCLASS::VM_25MHz,this);
		}

		{
			auto *keyboardSubMenu=subMenu->AddTextItem(0,FSKEY_K,L"Keyboard Mode")->AddSubMenu();
			keyboardSubMenu->AddTextItem(0,FSKEY_D,L"Direct (for Gaming)")->BindCallBack(&THISCLASS::VM_Keyboard_Direct,this);
			keyboardSubMenu->AddTextItem(0,FSKEY_T,L"Translation (for Typing)")->BindCallBack(&THISCLASS::VM_Keyboard_Translation,this);
		}
	}

	{
		auto *subMenu=mainMenu->AddTextItem(0,FSKEY_C,L"CD-ROM")->GetSubMenu();
		subMenu->AddTextItem(0,FSKEY_S,L"Select CD Image")->BindCallBack(&THISCLASS::CD_SelectImageFile,this);
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

	{
		int key;
		while(FSKEY_NULL!=(key=FsInkey()))
		{
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

	YsGLSLUsePlain2DRenderer(YsGLSLSharedPlain2DRenderer());
	YsGLSLUseWindowCoordinateInPlain2DDrawing(YsGLSLSharedPlain2DRenderer(),YSTRUE);
	YsGLSLEndUsePlain2DRenderer(YsGLSLSharedPlain2DRenderer());

	glDisable(GL_DEPTH_TEST);

	glUseProgram(0);
	FsGuiCanvas::Show();

	FsSwapBuffers();
}



void FsGuiMainCanvas::Run(void)
{
	// Warn if the VM is already running.
	ReallyRun();
}

bool FsGuiMainCanvas::ReallyRun(bool usePipe)
{
	if(subproc.SubprocRunning())
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
	return true;
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
		utf8.EncodeUTF8(ful.data());
		return utf8.c_str();
	}

	YsWString pth,fil;
	YsSpecialPath::GetProgramFileNameW().SeparatePathFile(pth,fil);
	ful.MakeFullPathName(pth,exeFile);
	if(YSTRUE==YsFileIO::CheckFileExist(ful))
	{
		YsString utf8;
		utf8.EncodeUTF8(ful.data());
		return utf8.c_str();
	}

	return "";
}

std::string FsGuiMainCanvas::GetCMOSFileName(void) const
{
	YsWString ful;
	ful.MakeFullPathName(GetTsugaruProfileDir(),L"CMOS.DAT");
	YsString utf8;
	utf8.EncodeUTF8(ful.data());
	return utf8.c_str();
}

std::vector <YsWString> FsGuiMainCanvas::CheckMissingROMFiles(void) const
{
	std::vector <YsWString> missing;
	const YsWString ROMFName[]=
	{
		L"FMT_DIC.ROM",
		L"FMT_DOS.ROM",
		L"FMT_F20.ROM",
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
void FsGuiMainCanvas::File_OpenProfile_FileSelected(FsGuiDialog *dlg,int returnCode)
{
	auto fdlg=dynamic_cast <FsGuiFileDialog *>(dlg);
	if(nullptr!=fdlg && (int)YSOK==returnCode)
	{
		profileDlg->profileFNameTxt->SetText(fdlg->selectedFileArray[0]);
		LoadProfile(fdlg->selectedFileArray[0]);
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
	fdlg->defaultFileName=profileDlg->profileFNameTxt->GetWText();
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
	fdlg->defaultFileName=profileDlg->profileFNameTxt->GetWText();
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
	fdlg->defaultFileName=profileDlg->profileFNameTxt->GetWText();
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
	fdlg->defaultFileName=profileDlg->profileFNameTxt->GetWText();
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
			fdlg->fileExtensionArray.Append(L".HDI");
			fdlg->fileExtensionArray.Append(L".HD");
			fdlg->defaultFileName=profileDlg->profileFNameTxt->GetWText();
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
				if(0==ext.STRCMP(L".D77"))
				{
					D77File d77;
					d77.SetRawBinary(img);
					img=d77.MakeD77Image();
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
	if(true==subproc.SubprocRunning())
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
		subproc.Send("Q\n");
	}
}
void FsGuiMainCanvas::VM_Pause(FsGuiPopUpMenuItem *)
{
	if(true==subproc.SubprocRunning())
	{
		subproc.Send("PAU\n");
	}
	else
	{
		VM_Not_Running_Error();
	}
}
void FsGuiMainCanvas::VM_Resume(FsGuiPopUpMenuItem *)
{
	if(true==subproc.SubprocRunning())
	{
		subproc.Send("RUN\n");
	}
	else
	{
		VM_Not_Running_Error();
	}
}
void FsGuiMainCanvas::VM_1MHz(FsGuiPopUpMenuItem *)
{
	if(true==subproc.SubprocRunning())
	{
		subproc.Send("FREQ 1\n");
	}
	else
	{
		VM_Not_Running_Error();
	}
}
void FsGuiMainCanvas::VM_4MHz(FsGuiPopUpMenuItem *)
{
	if(true==subproc.SubprocRunning())
	{
		subproc.Send("FREQ 4\n");
	}
	else
	{
		VM_Not_Running_Error();
	}
}
void FsGuiMainCanvas::VM_8MHz(FsGuiPopUpMenuItem *)
{
	if(true==subproc.SubprocRunning())
	{
		subproc.Send("FREQ 8\n");
	}
	else
	{
		VM_Not_Running_Error();
	}
}
void FsGuiMainCanvas::VM_12MHz(FsGuiPopUpMenuItem *)
{
	if(true==subproc.SubprocRunning())
	{
		subproc.Send("FREQ 12\n");
	}
	else
	{
		VM_Not_Running_Error();
	}
}
void FsGuiMainCanvas::VM_16MHz(FsGuiPopUpMenuItem *)
{
	if(true==subproc.SubprocRunning())
	{
		subproc.Send("FREQ 16\n");
	}
	else
	{
		VM_Not_Running_Error();
	}
}
void FsGuiMainCanvas::VM_25MHz(FsGuiPopUpMenuItem *)
{
	if(true==subproc.SubprocRunning())
	{
		subproc.Send("FREQ 25\n");
	}
	else
	{
		VM_Not_Running_Error();
	}
}

void FsGuiMainCanvas::VM_Keyboard_Direct(FsGuiPopUpMenuItem *)
{
	if(true==subproc.SubprocRunning())
	{
		subproc.Send("KEYBOARD DIRECT\n");
	}
	else
	{
		VM_Not_Running_Error();
	}
}
void FsGuiMainCanvas::VM_Keyboard_Translation(FsGuiPopUpMenuItem *)
{
	if(true==subproc.SubprocRunning())
	{
		subproc.Send("KEYBOARD TRANS\n");
	}
	else
	{
		VM_Not_Running_Error();
	}
}

////////////////////////////////////////////////////////////

void FsGuiMainCanvas::CD_SelectImageFile(FsGuiPopUpMenuItem *)
{
	if(true==subproc.SubprocRunning())
	{
		auto fdlg=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiFileDialog>();
		fdlg->Initialize();
		fdlg->mode=FsGuiFileDialog::MODE_OPEN;
		fdlg->multiSelect=YSFALSE;
		fdlg->title.Set(L"Open CD Image");
		fdlg->fileExtensionArray.Append(L".CUE");
		fdlg->fileExtensionArray.Append(L".ISO");
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
		subproc.Send(cmd);
	}
}

void FsGuiMainCanvas::CD_Eject(FsGuiPopUpMenuItem *)
{
	if(true==subproc.SubprocRunning())
	{
	}
	else
	{
		VM_Not_Running_Error();
	}
}

void FsGuiMainCanvas::FD0_SelectImageFile(FsGuiPopUpMenuItem *)
{
	if(true==subproc.SubprocRunning())
	{
		auto fdlg=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiFileDialog>();
		fdlg->Initialize();
		fdlg->mode=FsGuiFileDialog::MODE_OPEN;
		fdlg->multiSelect=YSFALSE;
		fdlg->title.Set(L"Open FD0 Image");
		fdlg->fileExtensionArray.Append(L".BIN");
		// fdlg->fileExtensionArray.Append(L".D77");
		fdlg->defaultFileName=profileDlg->FDImgTxt[0][0]->GetWString();
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
		subproc.Send(cmd);

		FD0_writeProtectMenu->SetCheck(YSFALSE);
		FD0_writeUnprotectMenu->SetCheck(YSTRUE);
	}
}
void FsGuiMainCanvas::FD0_WriteProtect(FsGuiPopUpMenuItem *)
{
	if(true==subproc.SubprocRunning())
	{
		subproc.Send("FD0WP\n");
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
	if(true==subproc.SubprocRunning())
	{
		subproc.Send("FD0UP\n");
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
	if(true==subproc.SubprocRunning())
	{
		subproc.Send("FD0EJECT\n");
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
	if(true==subproc.SubprocRunning())
	{
		auto fdlg=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiFileDialog>();
		fdlg->Initialize();
		fdlg->mode=FsGuiFileDialog::MODE_OPEN;
		fdlg->multiSelect=YSFALSE;
		fdlg->title.Set(L"Open FD1 Image");
		fdlg->fileExtensionArray.Append(L".BIN");
		// fdlg->fileExtensionArray.Append(L".D77");
		fdlg->defaultFileName=profileDlg->FDImgTxt[1][0]->GetWString();
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
		subproc.Send(cmd);

		FD1_writeProtectMenu->SetCheck(YSFALSE);
		FD1_writeUnprotectMenu->SetCheck(YSTRUE);
	}
}
void FsGuiMainCanvas::FD1_WriteProtect(FsGuiPopUpMenuItem *)
{
	if(true==subproc.SubprocRunning())
	{
		subproc.Send("FD1WP\n");
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
	if(true==subproc.SubprocRunning())
	{
		subproc.Send("FD1UP\n");
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
	if(true==subproc.SubprocRunning())
	{
		subproc.Send("FD1EJECT\n");
		FD1_writeProtectMenu->SetCheck(YSFALSE);
		FD1_writeUnprotectMenu->SetCheck(YSTRUE);
	}
	else
	{
		VM_Not_Running_Error();
	}
}
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
