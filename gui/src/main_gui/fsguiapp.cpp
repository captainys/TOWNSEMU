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



// If you want to use fixed-function pipeline of OpenGL 1.x,
//   (1) comment out the following line, and
//   (2) change linking libraries in CMakeLists.txt from:
//          fsguilib_gl2 fsgui3d_gl2
//       to
//          fsguilib_gl1 fsgui3d_gl1
#define DONT_USE_FIXED_FUNCTION_PIPELINE



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
		auto *fileSubMenu=mainMenu->AddTextItem(0,FSKEY_F,L"File")->GetSubMenu();
		fileSubMenu->AddTextItem(0,FSKEY_NULL,L"Save as Default")->BindCallBack(&THISCLASS::File_SaveDefaultProfile,this);
		fileSubMenu->AddTextItem(0,FSKEY_NULL,L"Reload Default")->BindCallBack(&THISCLASS::File_ReloadDefaultProfile,this);
		fileSubMenu->AddTextItem(0,FSKEY_X,L"Exit")->BindCallBack(&THISCLASS::File_Exit,this);
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
	glOrtho(0.0,(double)viewport[2],(double)viewport[3],0.0,-1.0,1.0);
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

void FsGuiMainCanvas::ReallyRun(void)
{
	auto profile=profileDlg->GetProfile();
	auto argv=profile.MakeArgv();
	for(auto arg : argv)
	{
		std::cout << arg << std::endl;
	}
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

	YsFileIO::File fp(fName,"r");
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

YsWString FsGuiMainCanvas::GetDefaultProfileFileName(void) const
{
	YsWString ful;
	ful.MakeFullPathName(YsSpecialPath::GetUserDocDirW(),L"Tsugaru_Default.txt");
	return ful;
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

////////////////////////////////////////////////////////////

