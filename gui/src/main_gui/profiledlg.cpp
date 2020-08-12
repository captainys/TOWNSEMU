#include <iostream>
#include "profiledlg.h"
#include "fsguiapp.h"



ProfileDialog::ProfileDialog(FsGuiMainCanvas *canvasPtr)
{
	this->canvasPtr=canvasPtr;
}

void ProfileDialog::Make(void)
{
	const int nShowPath=PATH_SHOW;

	profileFNameTxt=AddStaticText(0,FSKEY_NULL,L"PROFILE",YSFALSE);

	tab=AddTabControl(0,FSKEY_NULL,YSTRUE);

	int mainTabId=0;
	{
		auto tabId=AddTab(tab,"Main");
		mainTabId=tabId;
		BeginAddTabItem(tab,tabId);

		ROMDirBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"ROM dir:",YSTRUE);
		ROMDirTxt=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,"",nShowPath,YSFALSE);
		ROMDirTxt->SetLengthLimit(PATH_LENGTH);
		AddStaticText(0,FSKEY_NULL,"(When browsing for the ROM dir, please select one of the ROM files.)",YSTRUE);

		CPUFreqTxt=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,"CPU Speed (MHz)",4,YSTRUE);
		CPUFreqTxt->SetInteger(25);
		AddStaticText(0,FSKEY_NULL,"(Too-fast frequency rather slows down VM)",YSFALSE);

		RAMSizeTxt=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,"RAM (MB)",4,YSFALSE);
		RAMSizeTxt->SetInteger(4);

		CDImgBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"CD Image:",YSTRUE);
		CDImgTxt=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,"",nShowPath,YSFALSE);
		CDImgTxt->SetLengthLimit(PATH_LENGTH);

		FDImgBtn[0][0]=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"FD0:",YSTRUE);
		FDWriteProtBtn[0][0]=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,"Write Protect",YSFALSE);
		FDImgTxt[0][0]=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,"",nShowPath-24,YSFALSE);
		FDImgTxt[0][0]->SetLengthLimit(PATH_LENGTH);

		FDImgBtn[1][0]=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"FD1:",YSTRUE);
		FDWriteProtBtn[1][0]=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,"Write Protect",YSFALSE);
		FDImgTxt[1][0]=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,"",nShowPath-24,YSFALSE);
		FDImgTxt[1][0]->SetLengthLimit(PATH_LENGTH);

		scrnScaleTxt=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,L"Scaling(%):",4,YSTRUE);
		scrnScaleTxt->SetLengthLimit(PATH_LENGTH);
		scrnScaleTxt->SetInteger(100);

		pretend386DXBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,"Pretend 80386DX",YSTRUE);

		EndAddTabItem();
	}

	{
		auto tabId=AddTab(tab,"SCSI");
		BeginAddTabItem(tab,tabId);

		HDImgBtn[0]=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"HD0:",YSTRUE);
		HDImgTxt[0]=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,"",nShowPath,YSFALSE);
		HDImgTxt[0]->SetLengthLimit(PATH_LENGTH);

		HDImgBtn[1]=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"HD1:",YSTRUE);
		HDImgTxt[1]=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,"",nShowPath,YSFALSE);
		HDImgTxt[1]->SetLengthLimit(PATH_LENGTH);

		HDImgBtn[2]=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"HD2:",YSTRUE);
		HDImgTxt[2]=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,"",nShowPath,YSFALSE);
		HDImgTxt[2]->SetLengthLimit(PATH_LENGTH);

		HDImgBtn[3]=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"HD3:",YSTRUE);
		HDImgTxt[3]=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,"",nShowPath,YSFALSE);
		HDImgTxt[3]->SetLengthLimit(PATH_LENGTH);

		HDImgBtn[4]=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"HD4:",YSTRUE);
		HDImgTxt[4]=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,"",nShowPath,YSFALSE);
		HDImgTxt[4]->SetLengthLimit(PATH_LENGTH);

		HDImgBtn[5]=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"HD5:",YSTRUE);
		HDImgTxt[5]=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,"",nShowPath,YSFALSE);
		HDImgTxt[5]->SetLengthLimit(PATH_LENGTH);

		HDImgBtn[6]=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"HD6:",YSTRUE);
		HDImgTxt[6]=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,"",nShowPath,YSFALSE);
		HDImgTxt[6]->SetLengthLimit(PATH_LENGTH);

		EndAddTabItem();
	}

	{
		auto tabId=AddTab(tab,"GamePort");
		BeginAddTabItem(tab,tabId);

		AddStaticText(0,FSKEY_NULL,"Game Port 0:",YSTRUE);
		gamePortBtn[0][0]=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,"None",YSFALSE);
		gamePortBtn[0][1]=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,"GamePad0",YSFALSE);
		gamePortBtn[0][2]=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,"GamePad1",YSFALSE);
		gamePortBtn[0][3]=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,"Emulation by Keyboard(ASZX and arrow)",YSFALSE);
		gamePortBtn[0][4]=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,"Mouse",YSFALSE);
		SetRadioButtonGroup(5,gamePortBtn[0]);
		gamePortBtn[0][1]->SetCheck(YSTRUE);

		AddStaticText(0,FSKEY_NULL,"Game Port 1:",YSTRUE);
		gamePortBtn[1][0]=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,"None",YSFALSE);
		gamePortBtn[1][1]=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,"GamePad0",YSFALSE);
		gamePortBtn[1][2]=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,"GamePad1",YSFALSE);
		gamePortBtn[1][3]=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,"Emulation by Keyboard(ASZX and arrow)",YSFALSE);
		gamePortBtn[1][4]=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,"Mouse",YSFALSE);
		SetRadioButtonGroup(5,gamePortBtn[1]);
		gamePortBtn[1][4]->SetCheck(YSTRUE);

		EndAddTabItem();
	}

	{
		auto tabId=AddTab(tab,"Boot");
		BeginAddTabItem(tab,tabId);

		AddStaticText(0,FSKEY_NULL,"Boot Option:",YSTRUE);
		bootKeyBtn[ 0]=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,"None",YSFALSE);
		bootKeyBtn[ 1]=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,"CD",YSFALSE);
		bootKeyBtn[ 2]=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,"F0",YSFALSE);
		bootKeyBtn[ 3]=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,"F1",YSFALSE);
		bootKeyBtn[ 4]=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,"F2",YSFALSE);
		bootKeyBtn[ 5]=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,"F3",YSFALSE);
		bootKeyBtn[ 6]=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,"H0",YSFALSE);
		bootKeyBtn[ 7]=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,"H1",YSFALSE);
		bootKeyBtn[ 8]=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,"H2",YSFALSE);
		bootKeyBtn[ 9]=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,"H3",YSFALSE);
		bootKeyBtn[10]=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,"H4",YSFALSE);
		bootKeyBtn[11]=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,"ICM",YSFALSE);
		bootKeyBtn[12]=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,"DEBUG",YSFALSE);
		bootKeyBtn[13]=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,"PAD_A",YSFALSE);
		bootKeyBtn[14]=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,"PAD_B",YSFALSE);
		SetRadioButtonGroup(sizeof(bootKeyBtn)/sizeof(bootKeyBtn[0]),bootKeyBtn);

		EndAddTabItem();
	}

	{
		auto tabId=AddTab(tab,"Advanced");
		BeginAddTabItem(tab,tabId);
		catchUpRealTimeBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,"Catch Up Real Time (Recommended OFF for ChaseHQ)",YSTRUE);
		EndAddTabItem();
	}

	{
		auto tabId=AddTab(tab,"Application");
		BeginAddTabItem(tab,tabId);

		AddStaticText(0,FSKEY_NULL,L"Application-Specific Augmentation",YSTRUE);
		appSpecificAugDrp=AddEmptyDropList(0,FSKEY_NULL,"",10,40,40,YSTRUE);
		appSpecificAugDrp->AddString(TownsAppToStr(TOWNS_APPSPECIFIC_NONE).c_str(),YSTRUE);
		appSpecificAugDrp->AddString(TownsAppToStr(TOWNS_APPSPECIFIC_LEMMINGS).c_str(),YSFALSE);
		appSpecificAugDrp->AddString(TownsAppToStr(TOWNS_APPSPECIFIC_LEMMINGS2).c_str(),YSFALSE);
		appSpecificAugDrp->AddString(TownsAppToStr(TOWNS_APPSPECIFIC_STRIKECOMMANDER).c_str(),YSFALSE);
		appSpecificAugDrp->AddString(TownsAppToStr(TOWNS_APPSPECIFIC_SUPERDAISEN).c_str(),YSFALSE);
		appSpecificAugDrp->AddString(TownsAppToStr(TOWNS_APPSPECIFIC_WINGCOMMANDER1).c_str(),YSFALSE);

		appSpecificExplanation=AddStaticText(0,FSKEY_NULL,L"",YSTRUE);

		EndAddTabItem();
	}

	tab->SelectCurrentTab(mainTabId);

	runBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"START",YSTRUE);
	autoStartBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,"Auto Start (Start VM as soon as the profile is loaded)",YSFALSE);

	Fit();
	SetArrangeType(FSDIALOG_ARRANGE_TOP_LEFT);
}

/* virtual */ void ProfileDialog::OnDropListSelChange(FsGuiDropList *drp,int prevSel)
{
	if(appSpecificAugDrp==drp)
	{
		auto str=appSpecificAugDrp->GetSelectedString();
		auto app=TownsStrToApp(str.c_str());
		switch(app)
		{
		case TOWNS_APPSPECIFIC_NONE:
			appSpecificExplanation->SetText("");
			break;
		case TOWNS_APPSPECIFIC_WINGCOMMANDER1:
			appSpecificExplanation->SetText(
			    "Mouse Integration for Wing Commander 1\n"
				"Wing Commander 1 for FM TOWNS uses its own function\n"
				"instead of the Mouse BIOS to read mouse status.\n"
				"To send mouse data correctly, Tsugasu needs to\n"
				"know that it is running Wing Commander 1."
			);
			break;
		case TOWNS_APPSPECIFIC_STRIKECOMMANDER:
			appSpecificExplanation->SetText(
			    "Mouse Integration for Strike Commander\n"
				"Strike Commander uses hybrid of own function and TBIOS\n"
				"for reading mouse.\n"
				"Mouse Integration is activated once you press an arrow key.\n"
				"Also minimum RAM size is set to 8.\n"
			);
			break;
		case TOWNS_APPSPECIFIC_SUPERDAISEN:
			appSpecificExplanation->SetText(
				"Super Daisenryaku for FM TOWNS is, I believe,\n"
				"the best port among all other ports.  However,\n"
				"it has a usability issue at higher CPU frequency.\n"
				"It scrolls too fast.  To counter this issue,\n"
				"Tsugaru can slow down the CPU to 2MHz while\n"
				"mouse left button is held down.\n"
			);
			break;
		case TOWNS_APPSPECIFIC_LEMMINGS:
			appSpecificExplanation->SetText(
				"Mouse Integration for Lemmings\n"
				"Like Wing Commander 1, Lemmings uses its own\n"
				"mouse-reading function.  This option enables\n"
				"control by mouse in Lemmings.\n"
				"Cursor trembles a little bit.\n"
			);
			break;
		case TOWNS_APPSPECIFIC_LEMMINGS2:
			appSpecificExplanation->SetText(
				"Mouse Integration for Lemmings2\n"
				"Like Wing Commander 1, Lemmings uses its own\n"
				"mouse-reading function.  This option enables\n"
				"control by mouse in Lemmings.\n"
				"Cursor trembles a little bit.\n"
			);
			break;
		}
	}
}

/* virtual */ void ProfileDialog::OnButtonClick(FsGuiButton *btn)
{
	if(ROMDirBtn==btn)
	{
		YsString def;
		def.MakeFullPathName(ROMDirTxt->GetString(),"FMT_SYS.ROM");

		auto fdlg=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiFileDialog>();
		fdlg->Initialize();
		fdlg->mode=FsGuiFileDialog::MODE_OPEN;
		fdlg->multiSelect=YSFALSE;
		fdlg->title.Set(L"Select a ROM file (like FMT_SYS.ROM)");
		fdlg->fileExtensionArray.Append(L".ROM");
		fdlg->defaultFileName.SetUTF8String(def);
		fdlg->BindCloseModalCallBack(&THISCLASS::OnSelectROMFile,this);
		AttachModalDialog(fdlg);
	}
	if(CDImgBtn==btn)
	{
		Browse(L"CD Image",CDImgTxt,L".CUE",L".ISO");
	}
	for(int i=0; i<2; ++i)
	{
		if(FDImgBtn[i][0]==btn)
		{
			YsWString label(L"Floppy Drive ");
			label.push_back('0'+i);
			Browse(label,FDImgTxt[i][0],L".BIN",nullptr); // ,L".D77"
		}
	}
	for(int i=0; i<sizeof(HDImgBtn)/sizeof(HDImgBtn[0]); ++i)
	{
		if(HDImgBtn[i]==btn)
		{
			YsWString label(L"SCSI HDD ");
			label.push_back('0'+i);
			Browse(label,HDImgTxt[i],L".BIN",L".HDD");
		}
	}
	if(runBtn==btn)
	{
		canvasPtr->Run();
	}
}

void ProfileDialog::OnSelectROMFile(FsGuiDialog *dlg,int returnCode)
{
	auto fdlg=dynamic_cast <FsGuiFileDialog *>(dlg);
	if((int)YSOK==returnCode && nullptr!=fdlg)
	{
		YsWString ful(fdlg->selectedFileArray[0]);
		YsWString pth,fil;
		ful.SeparatePathFile(pth,fil);
		ROMDirTxt->SetText(pth);
	}
}
void ProfileDialog::Browse(const wchar_t label[],FsGuiTextBox *txt,const wchar_t ext0[],const wchar_t ext1[])
{
	nowBrowsingTxt=txt;

	YsString def=ROMDirTxt->GetString();

	auto fdlg=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiFileDialog>();
	fdlg->Initialize();
	fdlg->mode=FsGuiFileDialog::MODE_OPEN;
	fdlg->multiSelect=YSFALSE;
	fdlg->title.Set(label);
	fdlg->fileExtensionArray.Append(ext0);
	if(nullptr!=ext1)
	{
		fdlg->fileExtensionArray.Append(ext1);
	}
	fdlg->defaultFileName.SetUTF8String(def);
	fdlg->BindCloseModalCallBack(&THISCLASS::OnSelectFile,this);
	AttachModalDialog(fdlg);
}
void ProfileDialog::OnSelectFile(FsGuiDialog *dlg,int returnCode)
{
	auto fdlg=dynamic_cast <FsGuiFileDialog *>(dlg);
	if((int)YSOK==returnCode && nullptr!=fdlg)
	{
		nowBrowsingTxt->SetText(fdlg->selectedFileArray[0]);
	}
}

TownsProfile ProfileDialog::GetProfile(void) const
{
	TownsProfile profile;

	profile.ROMDir=ROMDirTxt->GetString().data();
	profile.freq=CPUFreqTxt->GetInteger();
	profile.memSizeInMB=RAMSizeTxt->GetInteger();
	profile.CDImgFile=CDImgTxt->GetString().data();
	profile.FDImgFile[0][0]=FDImgTxt[0][0]->GetString().data();
	profile.FDWriteProtect[0][0]=(YSTRUE==FDWriteProtBtn[0][0]->GetCheck());
	profile.FDImgFile[1][0]=FDImgTxt[1][0]->GetString().data();
	profile.FDWriteProtect[1][0]=(YSTRUE==FDWriteProtBtn[1][0]->GetCheck());
	profile.SCSIImgFile[0]=HDImgTxt[0]->GetString().data();
	profile.SCSIImgFile[1]=HDImgTxt[1]->GetString().data();
	profile.SCSIImgFile[2]=HDImgTxt[2]->GetString().data();
	profile.SCSIImgFile[3]=HDImgTxt[3]->GetString().data();
	profile.SCSIImgFile[4]=HDImgTxt[4]->GetString().data();
	profile.SCSIImgFile[5]=HDImgTxt[5]->GetString().data();

	for(int gameport=0; gameport<2; ++gameport)
	{
		for(int i=0; i<NUM_GAMEPORT_CHOICE; ++i)
		{
			if(YSTRUE==gamePortBtn[gameport][i]->GetCheck())
			{
				profile.gamePort[gameport]=gamePortChoice[i];
			}
		}
	}

	profile.bootKeyComb=BOOT_KEYCOMB_NONE;
	for(int i=0; i<sizeof(bootKeyBtn)/sizeof(bootKeyBtn[0]); ++i)
	{
		if(YSTRUE==bootKeyBtn[i]->GetCheck())
		{
			profile.bootKeyComb=i;
			break;
		}
	}

	profile.catchUpRealTime=(YSTRUE==catchUpRealTimeBtn->GetCheck());

	profile.pretend386DX=(YSTRUE==pretend386DXBtn->GetCheck());

	profile.autoStart=(YSTRUE==autoStartBtn->GetCheck());

	profile.screenScaling=scrnScaleTxt->GetInteger();

	profile.appSpecificAugmentation=TownsStrToApp(appSpecificAugDrp->GetSelectedString().c_str());

	return profile;
}
void ProfileDialog::SetProfile(const TownsProfile &profile)
{
	YsWString str;

	str.SetUTF8String(profile.ROMDir.data());
	ROMDirTxt->SetText(str);

	CPUFreqTxt->SetInteger(profile.freq);

	RAMSizeTxt->SetInteger(profile.memSizeInMB);

	str.SetUTF8String(profile.CDImgFile.data());
	CDImgTxt->SetText(str);

	str.SetUTF8String(profile.FDImgFile[0][0].data());
	FDImgTxt[0][0]->SetText(str);
	FDWriteProtBtn[0][0]->SetCheck(true==profile.FDWriteProtect[0][0] ? YSTRUE : YSFALSE);

	str.SetUTF8String(profile.FDImgFile[1][0].data());
	FDImgTxt[1][0]->SetText(str);
	FDWriteProtBtn[1][0]->SetCheck(true==profile.FDWriteProtect[1][0] ? YSTRUE : YSFALSE);

	for(int i=0; i<TownsProfile::MAX_NUM_SCSI_DEVICE; ++i)
	{
		str.SetUTF8String(profile.SCSIImgFile[i].data());
		HDImgTxt[i]->SetText(str);
	}

	for(int gameport=0; gameport<2; ++gameport)
	{
		for(int i=0; i<NUM_GAMEPORT_CHOICE; ++i)
		{
			if(gamePortChoice[i]==profile.gamePort[gameport])
			{
				gamePortBtn[gameport][i]->SetCheck(YSTRUE);
			}
			else
			{
				gamePortBtn[gameport][i]->SetCheck(YSFALSE);
			}
		}
	}

	for(int i=0; i<sizeof(bootKeyBtn)/sizeof(bootKeyBtn[0]); ++i)
	{
		if(i==profile.bootKeyComb)
		{
			bootKeyBtn[i]->SetCheck(YSTRUE);
		}
		else
		{
			bootKeyBtn[i]->SetCheck(YSFALSE);
		}
	}

	if(true==profile.catchUpRealTime)
	{
		catchUpRealTimeBtn->SetCheck(YSTRUE);
	}
	else
	{
		catchUpRealTimeBtn->SetCheck(YSFALSE);
	}

	if(true==profile.pretend386DX)
	{
		pretend386DXBtn->SetCheck(YSTRUE);
	}
	else
	{
		pretend386DXBtn->SetCheck(YSFALSE);
	}

	if(true==profile.autoStart)
	{
		autoStartBtn->SetCheck(YSTRUE);
	}
	else
	{
		autoStartBtn->SetCheck(YSFALSE);
	}

	scrnScaleTxt->SetInteger(profile.screenScaling);

	appSpecificAugDrp->SelectByString(TownsAppToStr(profile.appSpecificAugmentation).c_str(),YSTRUE);

}
