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

		FDImgBtn[0]=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"FD0:",YSTRUE);
		FDWriteProtBtn[0]=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,"Write Protect",YSFALSE);
		FDImgTxt[0]=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,"",nShowPath-24,YSFALSE);
		FDImgTxt[0]->SetLengthLimit(PATH_LENGTH);

		FDImgBtn[1]=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"FD1:",YSTRUE);
		FDWriteProtBtn[1]=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,"Write Protect",YSFALSE);
		FDImgTxt[1]=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,"",nShowPath-24,YSFALSE);
		FDImgTxt[1]->SetLengthLimit(PATH_LENGTH);

		scrnScaleTxt=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,L"Scaling(%):",4,YSTRUE);
		scrnScaleTxt->SetLengthLimit(PATH_LENGTH);
		scrnScaleTxt->SetInteger(100);

		scrnAutoScaleBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,"Auto Scaling (Fit to Window Size)",YSTRUE);
		scrnMaximizeBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,"Maximize on Start Up",YSFALSE);

		pretend386DXBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,"Pretend 80386DX",YSTRUE);

		damperWireLineBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,"Render Damper-Wire Line (to make you nostalgic)",YSFALSE);

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

		for(int i=0; i<2; ++i)
		{
			std::string label="Game Port ";
			label.push_back('0'+i);
			label.push_back(':');

			AddStaticText(0,FSKEY_NULL,label.c_str(),YSTRUE);
			gamePortDrp[i]=AddEmptyDropList(0,FSKEY_NULL,"",20,40,40,YSTRUE);
			gamePortDrp[i]->AddString("None",YSTRUE);
			gamePortDrp[i]->AddString("GamePad0",YSFALSE);
			gamePortDrp[i]->AddString("GamePad1",YSFALSE);
			gamePortDrp[i]->AddString("GamePad0 (Analog Axes as Direction)",YSFALSE);
			gamePortDrp[i]->AddString("GamePad1 (Analog Axes as Direction)",YSFALSE);
			gamePortDrp[i]->AddString("Emulation by Keyboard(Arrow and ASZX)",YSFALSE);
			gamePortDrp[i]->AddString("Mouse",YSFALSE);
			gamePortDrp[i]->AddString("Mouse by Key (Arrow and ZX)",YSTRUE);
			gamePortDrp[i]->AddString("Mouse by NumPad (NUMKEY and /*)",YSFALSE);
			gamePortDrp[i]->AddString("Mouse by Pad0",YSFALSE);
			gamePortDrp[i]->AddString("Mouse by Pad1",YSFALSE);
			gamePortDrp[i]->AddString("Mouse by Analog0",YSFALSE);
			gamePortDrp[i]->AddString("Mouse by Analog1",YSFALSE);
		}
		gamePortDrp[0]->Select(1);
		gamePortDrp[1]->Select(6);

		AddStaticText(0,FSKEY_NULL,"Mouse-Integration Speed",YSTRUE);
		AddStaticText(0,FSKEY_NULL,"(Try slowing down if mouse-cursor jumps around)",YSTRUE);
		AddStaticText(0,FSKEY_NULL,"Slow",YSTRUE);
		mouseIntegSpdSlider=AddHorizontalSlider(0,FSKEY_NULL,20,32.0,256.0,YSFALSE);
		AddStaticText(0,FSKEY_NULL,"Fast",YSFALSE);

		EndAddTabItem();
	}

	{
		auto tabId=AddTab(tab,"Keyboard1");
		BeginAddTabItem(tab,tabId);

		AddStaticText(0,FSKEY_NULL,"Keyboard Mode (TRANS1:ESC->ESC+BREAK, TRANS2:ESC->ESC, TRANS3:ESC->BREAK):",YSTRUE);
		keyboardModeDrp=AddEmptyDropList(0,FSKEY_NULL,"",20,20,20,YSFALSE);
		for(int i=0; i<TOWNS_KEYBOARD_MODE_NUM_MODES; ++i)
		{
			keyboardModeDrp->AddString(TownsKeyboardModeToStr(i).c_str(),YSFALSE);
		}

		AddStaticText(0,FSKEY_NULL,"Virtual Keys:",YSTRUE);
		for(int row=0; row<10; ++row)
		{
			virtualKeyTownsKeyDrp[row]=AddEmptyDropList(0,FSKEY_NULL,"",20,20,20,YSTRUE);
			virtualKeyTownsKeyDrp[row]->AddString("(None)",YSTRUE);
			for(int townsKey=0; townsKey<256; ++townsKey)
			{
				auto str=TownsKeyCodeToStr(townsKey);
				auto reverse=TownsStrToKeyCode(str);
				if(TOWNS_JISKEY_NULL!=reverse)
				{
					virtualKeyTownsKeyDrp[row]->AddString(str.c_str(),YSFALSE);
				}
			}

			AddStaticText(0,FSKEY_NULL,"JoystickID:",YSFALSE);
			virtualKeyPhysIdDrp[row]=AddEmptyDropList(0,FSKEY_NULL,"",8,4,4,YSFALSE);
			virtualKeyPhysIdDrp[row]->AddString("0",YSTRUE);
			virtualKeyPhysIdDrp[row]->AddString("1",YSFALSE);
			virtualKeyPhysIdDrp[row]->AddString("2",YSFALSE);
			virtualKeyPhysIdDrp[row]->AddString("3",YSFALSE);
			virtualKeyPhysIdDrp[row]->AddString("4",YSFALSE);
			virtualKeyPhysIdDrp[row]->AddString("5",YSFALSE);
			virtualKeyPhysIdDrp[row]->AddString("6",YSFALSE);
			virtualKeyPhysIdDrp[row]->AddString("7",YSFALSE);

			AddStaticText(0,FSKEY_NULL,"Button:",YSFALSE);
			virtualKeyButtonDrp[row]=AddEmptyDropList(0,FSKEY_NULL,"",8,4,4,YSFALSE);
			virtualKeyButtonDrp[row]->AddString("0",YSTRUE);
			virtualKeyButtonDrp[row]->AddString("1",YSFALSE);
			virtualKeyButtonDrp[row]->AddString("2",YSFALSE);
			virtualKeyButtonDrp[row]->AddString("3",YSFALSE);
			virtualKeyButtonDrp[row]->AddString("4",YSFALSE);
			virtualKeyButtonDrp[row]->AddString("5",YSFALSE);
			virtualKeyButtonDrp[row]->AddString("6",YSFALSE);
			virtualKeyButtonDrp[row]->AddString("7",YSFALSE);
			virtualKeyButtonDrp[row]->AddString("8",YSTRUE);
			virtualKeyButtonDrp[row]->AddString("9",YSFALSE);
			virtualKeyButtonDrp[row]->AddString("10",YSFALSE);
			virtualKeyButtonDrp[row]->AddString("11",YSFALSE);
			virtualKeyButtonDrp[row]->AddString("12",YSFALSE);
			virtualKeyButtonDrp[row]->AddString("13",YSFALSE);
			virtualKeyButtonDrp[row]->AddString("14",YSFALSE);
			virtualKeyButtonDrp[row]->AddString("15",YSFALSE);
		}

		EndAddTabItem();
	}
	
	{
		auto tabId=AddTab(tab,"Keyboard2");
		BeginAddTabItem(tab,tabId);
		
		AddStaticText(0,FSKEY_NULL,"Virtual Keys:",YSTRUE);
		for(int row=10; row<TownsProfile::MAX_NUM_VIRTUALKEYS; ++row)
		{
			virtualKeyTownsKeyDrp[row]=AddEmptyDropList(0,FSKEY_NULL,"",20,20,20,YSTRUE);
			virtualKeyTownsKeyDrp[row]->AddString("(None)",YSTRUE);
			for(int townsKey=0; townsKey<256; ++townsKey)
			{
				auto str=TownsKeyCodeToStr(townsKey);
				auto reverse=TownsStrToKeyCode(str);
				if(TOWNS_JISKEY_NULL!=reverse)
				{
					virtualKeyTownsKeyDrp[row]->AddString(str.c_str(),YSFALSE);
				}
			}

			AddStaticText(0,FSKEY_NULL,"JoystickID:",YSFALSE);
			virtualKeyPhysIdDrp[row]=AddEmptyDropList(0,FSKEY_NULL,"",8,4,4,YSFALSE);
			virtualKeyPhysIdDrp[row]->AddString("0",YSTRUE);
			virtualKeyPhysIdDrp[row]->AddString("1",YSFALSE);
			virtualKeyPhysIdDrp[row]->AddString("2",YSFALSE);
			virtualKeyPhysIdDrp[row]->AddString("3",YSFALSE);
			virtualKeyPhysIdDrp[row]->AddString("4",YSFALSE);
			virtualKeyPhysIdDrp[row]->AddString("5",YSFALSE);
			virtualKeyPhysIdDrp[row]->AddString("6",YSFALSE);
			virtualKeyPhysIdDrp[row]->AddString("7",YSFALSE);

			AddStaticText(0,FSKEY_NULL,"Button:",YSFALSE);
			virtualKeyButtonDrp[row]=AddEmptyDropList(0,FSKEY_NULL,"",8,4,4,YSFALSE);
			virtualKeyButtonDrp[row]->AddString("0",YSTRUE);
			virtualKeyButtonDrp[row]->AddString("1",YSFALSE);
			virtualKeyButtonDrp[row]->AddString("2",YSFALSE);
			virtualKeyButtonDrp[row]->AddString("3",YSFALSE);
			virtualKeyButtonDrp[row]->AddString("4",YSFALSE);
			virtualKeyButtonDrp[row]->AddString("5",YSFALSE);
			virtualKeyButtonDrp[row]->AddString("6",YSFALSE);
			virtualKeyButtonDrp[row]->AddString("7",YSFALSE);
			virtualKeyButtonDrp[row]->AddString("8",YSTRUE);
			virtualKeyButtonDrp[row]->AddString("9",YSFALSE);
			virtualKeyButtonDrp[row]->AddString("10",YSFALSE);
			virtualKeyButtonDrp[row]->AddString("11",YSFALSE);
			virtualKeyButtonDrp[row]->AddString("12",YSFALSE);
			virtualKeyButtonDrp[row]->AddString("13",YSFALSE);
			virtualKeyButtonDrp[row]->AddString("14",YSFALSE);
			virtualKeyButtonDrp[row]->AddString("15",YSFALSE);
		}

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
		auto tabId=AddTab(tab,"Mouse by Flight Joystick");
		BeginAddTabItem(tab,tabId);

		flightMouseEnableBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,"Use Mouse by Flight Joystick",YSTRUE);

		AddStaticText(0,FSKEY_NULL,"JoystickID:",YSTRUE);
		flightMousePhysIdDrp=AddEmptyDropList(0,FSKEY_NULL,"",8,4,4,YSFALSE);
		flightMousePhysIdDrp->AddString("0",YSTRUE);
		flightMousePhysIdDrp->AddString("1",YSFALSE);
		flightMousePhysIdDrp->AddString("2",YSFALSE);
		flightMousePhysIdDrp->AddString("3",YSFALSE);
		flightMousePhysIdDrp->AddString("4",YSFALSE);
		flightMousePhysIdDrp->AddString("5",YSFALSE);
		flightMousePhysIdDrp->AddString("6",YSFALSE);
		flightMousePhysIdDrp->AddString("7",YSFALSE);

		AddStaticText(0,FSKEY_NULL,"CX:",YSTRUE);
		flightMouseCenterXTxt=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,"",5,YSFALSE);
		AddStaticText(0,FSKEY_NULL,"CY:",YSFALSE);
		flightMouseCenterYTxt=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,"",5,YSFALSE);

		AddStaticText(0,FSKEY_NULL,"ScaleX:",YSFALSE);
		flightMouseScaleXTxt=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,"",5,YSFALSE);
		AddStaticText(0,FSKEY_NULL,"ScaleY:",YSFALSE);
		flightMouseScaleYTxt=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,"",5,YSFALSE);

		AddStaticText(0,FSKEY_NULL,"DeadZone(%):",YSFALSE);
		flightMouseDeadZoneTxt=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,"",5,YSFALSE);

		AddStaticText(0,FSKEY_NULL,"Recommended:",YSTRUE);
		flightMouseWC1=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"Wing Commander",YSFALSE);
		flightMouseWC2=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"Wing Commander2",YSFALSE);
		flightMouseSTCM=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"Strike Commander",YSFALSE);

		AddStaticText(0,FSKEY_NULL,
			"Games like Wing Commander and Strike Commander let you control your fighter by mouse.\n"
			"This feature translates joystick to mouse coordinate and let you fly using a flight joystick device.\n"
			"Need to be used with Application-Specific Augmentation if the application is not\n"
			"using TBIOS for reading mouse."
		,YSTRUE);


		strikeCommanderThrottleEnableBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,"Use Throttle Axis for Strike Commander/Wing Commander",YSTRUE);

		AddStaticText(0,FSKEY_NULL,"JoystickID:",YSTRUE);
		strikeCommanderThrottlePhysIdDrp=AddEmptyDropList(0,FSKEY_NULL,"",8,4,4,YSFALSE);
		strikeCommanderThrottlePhysIdDrp->AddString("0",YSTRUE);
		strikeCommanderThrottlePhysIdDrp->AddString("1",YSFALSE);
		strikeCommanderThrottlePhysIdDrp->AddString("2",YSFALSE);
		strikeCommanderThrottlePhysIdDrp->AddString("3",YSFALSE);
		strikeCommanderThrottlePhysIdDrp->AddString("4",YSFALSE);
		strikeCommanderThrottlePhysIdDrp->AddString("5",YSFALSE);
		strikeCommanderThrottlePhysIdDrp->AddString("6",YSFALSE);
		strikeCommanderThrottlePhysIdDrp->AddString("7",YSFALSE);
		strikeCommanderThrottlePhysIdDrp->Disable();

		AddStaticText(0,FSKEY_NULL,"Axis:",YSFALSE);
		strikeCommanderThrottleAxisDrp=AddEmptyDropList(0,FSKEY_NULL,"",8,4,4,YSFALSE);
		strikeCommanderThrottleAxisDrp->AddString("0",YSFALSE);
		strikeCommanderThrottleAxisDrp->AddString("1",YSFALSE);
		strikeCommanderThrottleAxisDrp->AddString("2",YSTRUE);
		strikeCommanderThrottleAxisDrp->AddString("3",YSFALSE);
		strikeCommanderThrottleAxisDrp->AddString("4",YSFALSE);
		strikeCommanderThrottleAxisDrp->AddString("5",YSFALSE);
		strikeCommanderThrottleAxisDrp->AddString("6",YSFALSE);
		strikeCommanderThrottleAxisDrp->AddString("7",YSFALSE);
		strikeCommanderThrottleAxisDrp->Disable();

		AddStaticText(0,FSKEY_NULL,
			"This option translates physical throttle axis movement to 1,2,3,...,9,0 keys."
		,YSTRUE);

		EndAddTabItem();
	}

	{
		auto tabId=AddTab(tab,"Application");
		BeginAddTabItem(tab,tabId);

		AddStaticText(0,FSKEY_NULL,L"Application-Specific Augmentation",YSTRUE);
		appSpecificAugDrp=AddEmptyDropList(0,FSKEY_NULL,"",10,40,40,YSTRUE);
		appSpecificAugDrp->AddString(TownsAppToStr(TOWNS_APPSPECIFIC_NONE).c_str(),YSTRUE);
		appSpecificAugDrp->AddString(TownsAppToStr(TOWNS_APPSPECIFIC_AIRWARRIOR_V2).c_str(),YSTRUE);
		appSpecificAugDrp->AddString(TownsAppToStr(TOWNS_APPSPECIFIC_AMARANTH3).c_str(),YSFALSE);
		appSpecificAugDrp->AddString(TownsAppToStr(TOWNS_APPSPECIFIC_BRANDISH).c_str(),YSFALSE);
		appSpecificAugDrp->AddString(TownsAppToStr(TOWNS_APPSPECIFIC_LEMMINGS).c_str(),YSFALSE);
		appSpecificAugDrp->AddString(TownsAppToStr(TOWNS_APPSPECIFIC_LEMMINGS2).c_str(),YSFALSE);
		appSpecificAugDrp->AddString(TownsAppToStr(TOWNS_APPSPECIFIC_OPERATIONWOLF).c_str(),YSFALSE);
		appSpecificAugDrp->AddString(TownsAppToStr(TOWNS_APPSPECIFIC_STRIKECOMMANDER).c_str(),YSFALSE);
		appSpecificAugDrp->AddString(TownsAppToStr(TOWNS_APPSPECIFIC_SUPERDAISEN).c_str(),YSFALSE);
		appSpecificAugDrp->AddString(TownsAppToStr(TOWNS_APPSPECIFIC_ULTIMAUNDERWORLD).c_str(),YSFALSE);
		appSpecificAugDrp->AddString(TownsAppToStr(TOWNS_APPSPECIFIC_WINGCOMMANDER1).c_str(),YSFALSE);
		appSpecificAugDrp->AddString(TownsAppToStr(TOWNS_APPSPECIFIC_WINGCOMMANDER2).c_str(),YSFALSE);
		appSpecificAugDrp->AddString(TownsAppToStr(TOWNS_APPSPECIFIC_DUNGEONMASTER_JP).c_str(),YSFALSE);
		appSpecificAugDrp->AddString(TownsAppToStr(TOWNS_APPSPECIFIC_DUNGEONMASTER_EN).c_str(),YSFALSE);

		appSpecificExplanation=AddStaticText(0,FSKEY_NULL,L"",YSTRUE);

		EndAddTabItem();
	}

	tab->SelectCurrentTab(mainTabId);

	runBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"START",YSTRUE);
	autoStartBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,"Auto Start (Start VM as soon as the profile is loaded)",YSFALSE);
	separateProcBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,"Separate Process",YSFALSE);

	if(true!=canvasPtr->separateProcess)
	{
// macOS doen't send SCROLL_LOCK key code to the application.  WTF.
#ifndef __APPLE__
		AddStaticText(0,FSKEY_NULL,
			"Press SCROLL_LOCK or Click on the lower-right [MENU] icon\n"
			"to come back to the GUI from the VM.",
			YSTRUE);
#else
		AddStaticText(0,FSKEY_NULL,
			"Click on the lower-right [MENU] icon\n"
			"to come back to the GUI from the VM.",
			YSTRUE);
#endif
	}

	Fit();
	SetArrangeType(FSDIALOG_ARRANGE_TOP_LEFT);


	TownsProfile defaultProfile;
	SetProfile(defaultProfile);
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
		case TOWNS_APPSPECIFIC_WINGCOMMANDER2:
			appSpecificExplanation->SetText(
			    "Mouse Integration for Wing Commander 2\n"
				"Wing Commander 2 for FM TOWNS uses its own function\n"
				"instead of the Mouse BIOS to read mouse status.\n"
				"To send mouse data correctly, Tsugasu needs to\n"
				"know that it is running Wing Commander 2."
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
		std::vector <const wchar_t *> extList={L".CUE",L".ISO"};
		Browse(L"CD Image",CDImgTxt,extList);
	}
	for(int i=0; i<2; ++i)
	{
		if(FDImgBtn[i]==btn)
		{
			YsWString label(L"Floppy Drive ");
			label.push_back('0'+i);
			std::vector <const wchar_t *> extList={L".BIN",L".D77",L".D88",L".XDF"};
			Browse(label,FDImgTxt[i],extList);
		}
	}
	for(int i=0; i<sizeof(HDImgBtn)/sizeof(HDImgBtn[0]); ++i)
	{
		if(HDImgBtn[i]==btn)
		{
			YsWString label(L"SCSI HDD ");
			label.push_back('0'+i);
			std::vector <const wchar_t *> extList={L".BIN",L".HDD",L".HDM",L".HDI",L".H0",L".H1",L".H2",L".H3"};
			Browse(label,HDImgTxt[i],extList);
		}
	}
	if(runBtn==btn)
	{
		canvasPtr->Run();
	}

	if(flightMouseWC1==btn)
	{
		flightMouseEnableBtn->SetCheck(YSTRUE);
		flightMouseCenterXTxt->SetInteger(320);
		flightMouseCenterYTxt->SetInteger(135);
		flightMouseScaleXTxt->SetInteger(400);
		flightMouseScaleYTxt->SetInteger(300);
		appSpecificAugDrp->SelectByString(TownsAppToStr(TOWNS_APPSPECIFIC_WINGCOMMANDER1).c_str());
	}
	if(flightMouseWC2==btn)
	{
		flightMouseEnableBtn->SetCheck(YSTRUE);
		flightMouseCenterXTxt->SetInteger(320);
		flightMouseCenterYTxt->SetInteger(135);
		flightMouseScaleXTxt->SetInteger(400);
		flightMouseScaleYTxt->SetInteger(300);
		appSpecificAugDrp->SelectByString(TownsAppToStr(TOWNS_APPSPECIFIC_WINGCOMMANDER2).c_str());
	}
	if(flightMouseSTCM==btn)
	{
		flightMouseEnableBtn->SetCheck(YSTRUE);
		flightMouseCenterXTxt->SetInteger(320);
		flightMouseCenterYTxt->SetInteger(200);
		flightMouseScaleXTxt->SetInteger(400);
		flightMouseScaleYTxt->SetInteger(300);
		appSpecificAugDrp->SelectByString(TownsAppToStr(TOWNS_APPSPECIFIC_STRIKECOMMANDER).c_str());
	}
	if(strikeCommanderThrottleEnableBtn==btn)
	{
		if(YSTRUE==strikeCommanderThrottleEnableBtn->GetCheck())
		{
			strikeCommanderThrottlePhysIdDrp->Enable();
			strikeCommanderThrottleAxisDrp->Enable();
		}
		else
		{
			strikeCommanderThrottlePhysIdDrp->Disable();
			strikeCommanderThrottleAxisDrp->Disable();
		}
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
void ProfileDialog::Browse(const wchar_t label[],FsGuiTextBox *txt,std::vector <const wchar_t *> extList)
{
	nowBrowsingTxt=txt;

	YsString def=ROMDirTxt->GetString();

	auto fdlg=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiFileDialog>();
	fdlg->Initialize();
	fdlg->mode=FsGuiFileDialog::MODE_OPEN;
	fdlg->multiSelect=YSFALSE;
	fdlg->title.Set(label);
	for(auto ext : extList)
	{
		fdlg->fileExtensionArray.Append(ext);
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

	profile.ROMPath=ROMDirTxt->GetString().data();
	profile.freq=CPUFreqTxt->GetInteger();
	profile.memSizeInMB=RAMSizeTxt->GetInteger();
	profile.cdImgFName=CDImgTxt->GetString().data();
	profile.fdImgFName[0]=FDImgTxt[0]->GetString().data();
	profile.fdImgWriteProtect[0]=(YSTRUE==FDWriteProtBtn[0]->GetCheck());
	profile.fdImgFName[1]=FDImgTxt[1]->GetString().data();
	profile.fdImgWriteProtect[1]=(YSTRUE==FDWriteProtBtn[1]->GetCheck());
	for(int i=0; i<TownsProfile::MAX_NUM_SCSI_DEVICES; ++i)
	{
		profile.scsiImg[i].imgFName=HDImgTxt[i]->GetString().data();
		if(""!=profile.scsiImg[i].imgFName)
		{
			profile.scsiImg[i].imageType=TownsProfile::SCSIIMAGE_HARDDISK;
		}
		else
		{
			profile.scsiImg[i].imageType=TownsProfile::SCSIIMAGE_NONE;
		}
	}

	for(int gameport=0; gameport<2; ++gameport)
	{
		auto sel=gamePortDrp[gameport]->GetSelection();
		profile.gamePort[gameport]=gamePortChoice[sel];
	}

	profile.mouseIntegrationSpeed=(int)(mouseIntegSpdSlider->GetScaledValue());
	if(profile.mouseIntegrationSpeed<32)
	{
		profile.mouseIntegrationSpeed=32;
	}
	else if(256<profile.mouseIntegrationSpeed)
	{
		profile.mouseIntegrationSpeed=256;
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

	profile.scaling=scrnScaleTxt->GetInteger();
	profile.autoScaling=(YSTRUE==scrnAutoScaleBtn->GetCheck());
	profile.maximizeOnStartUp=(YSTRUE==scrnMaximizeBtn->GetCheck());

	profile.appSpecificSetting=TownsStrToApp(appSpecificAugDrp->GetSelectedString().c_str());


	profile.mouseByFlightstickAvailable=(YSTRUE==flightMouseEnableBtn->GetCheck());
	profile.mouseByFlightstickPhysicalId=flightMousePhysIdDrp->GetSelection();
	profile.mouseByFlightstickCenterX=flightMouseCenterXTxt->GetInteger();
	profile.mouseByFlightstickCenterY=flightMouseCenterYTxt->GetInteger();
	profile.mouseByFlightstickScaleX=flightMouseScaleXTxt->GetRealNumber();
	profile.mouseByFlightstickScaleY=flightMouseScaleYTxt->GetRealNumber();
	profile.mouseByFlightstickZeroZoneX=(float)flightMouseDeadZoneTxt->GetInteger()/100.0;
	profile.mouseByFlightstickZeroZoneY=(float)flightMouseDeadZoneTxt->GetInteger()/100.0;


	if(YSTRUE==strikeCommanderThrottleEnableBtn->GetCheck())
	{
		profile.throttlePhysicalId=strikeCommanderThrottlePhysIdDrp->GetSelection();
		profile.throttleAxis=strikeCommanderThrottleAxisDrp->GetSelection();
	}
	else
	{
		profile.throttlePhysicalId=-1;
	}


	profile.damperWireLine=(YSTRUE==damperWireLineBtn->GetCheck());
	profile.separateProcess=(YSTRUE==separateProcBtn->GetCheck());

	profile.keyboardMode=TownsStrToKeyboardMode(keyboardModeDrp->GetSelectedString().c_str());
	for(int row=0; row<TownsProfile::MAX_NUM_VIRTUALKEYS; ++row)
	{
		auto townsKeyStr=virtualKeyTownsKeyDrp[row]->GetSelectedString();
		profile.virtualKeys[row].townsKey=townsKeyStr.c_str();
		profile.virtualKeys[row].physicalId=virtualKeyPhysIdDrp[row]->GetSelection();
		profile.virtualKeys[row].button=virtualKeyButtonDrp[row]->GetSelection();
	}


	return profile;
}
void ProfileDialog::SetProfile(const TownsProfile &profile)
{
	YsWString str;

	str.SetUTF8String(profile.ROMPath.data());
	ROMDirTxt->SetText(str);

	CPUFreqTxt->SetInteger(profile.freq);

	RAMSizeTxt->SetInteger(profile.memSizeInMB);

	str.SetUTF8String(profile.cdImgFName.data());
	CDImgTxt->SetText(str);

	str.SetUTF8String(profile.fdImgFName[0].data());
	FDImgTxt[0]->SetText(str);
	FDWriteProtBtn[0]->SetCheck(true==profile.fdImgWriteProtect[0] ? YSTRUE : YSFALSE);

	str.SetUTF8String(profile.fdImgFName[1].data());
	FDImgTxt[1]->SetText(str);
	FDWriteProtBtn[1]->SetCheck(true==profile.fdImgWriteProtect[1] ? YSTRUE : YSFALSE);

	for(int i=0; i<TownsProfile::MAX_NUM_SCSI_DEVICES; ++i)
	{
		if(TownsProfile::SCSIIMAGE_HARDDISK==profile.scsiImg[i].imageType)
		{
			str.SetUTF8String(profile.scsiImg[i].imgFName.data());
			HDImgTxt[i]->SetText(str);
		}
	}

	for(int gameport=0; gameport<2; ++gameport)
	{
		for(int i=0; i<NUM_GAMEPORT_CHOICE; ++i)
		{
			if(gamePortChoice[i]==profile.gamePort[gameport])
			{
				gamePortDrp[gameport]->Select(i);
			}
		}
	}

	mouseIntegSpdSlider->SetPositionByScaledValue((double)profile.mouseIntegrationSpeed);


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

	if(true==profile.separateProcess)
	{
		separateProcBtn->SetCheck(YSTRUE);
	}
	else
	{
		separateProcBtn->SetCheck(YSFALSE);
	}

	scrnAutoScaleBtn->SetCheck(true==profile.autoScaling ? YSTRUE : YSFALSE);
	scrnMaximizeBtn->SetCheck(true==profile.maximizeOnStartUp ? YSTRUE : YSFALSE);
	scrnScaleTxt->SetInteger(profile.scaling);

	appSpecificAugDrp->SelectByString(TownsAppToStr(profile.appSpecificSetting).c_str(),YSTRUE);

	flightMouseEnableBtn->SetCheck(profile.mouseByFlightstickAvailable ? YSTRUE : YSFALSE);
	flightMousePhysIdDrp->Select(profile.mouseByFlightstickPhysicalId);
	flightMouseCenterXTxt->SetInteger(profile.mouseByFlightstickCenterX);
	flightMouseCenterYTxt->SetInteger(profile.mouseByFlightstickCenterY);
	flightMouseScaleXTxt->SetInteger(profile.mouseByFlightstickScaleX);
	flightMouseScaleYTxt->SetInteger(profile.mouseByFlightstickScaleY);
	flightMouseDeadZoneTxt->SetInteger((int)(100.0f*profile.mouseByFlightstickZeroZoneX));

	if(0<=profile.throttlePhysicalId)
	{
		strikeCommanderThrottleEnableBtn->SetCheck(YSTRUE);
		strikeCommanderThrottlePhysIdDrp->Select(profile.throttlePhysicalId);
		strikeCommanderThrottleAxisDrp->Select(profile.throttleAxis);
		strikeCommanderThrottlePhysIdDrp->Enable();
		strikeCommanderThrottleAxisDrp->Enable();
	}
	else
	{
		strikeCommanderThrottleEnableBtn->SetCheck(YSFALSE);
		strikeCommanderThrottlePhysIdDrp->Disable();
		strikeCommanderThrottleAxisDrp->Disable();
	}

	damperWireLineBtn->SetCheck(profile.damperWireLine ? YSTRUE : YSFALSE);

	keyboardModeDrp->SelectByString(TownsKeyboardModeToStr(profile.keyboardMode).c_str());
	for(int row=0; row<TownsProfile::MAX_NUM_VIRTUALKEYS; ++row)
	{
		auto townsKeyStr=profile.virtualKeys[row].townsKey;
		virtualKeyTownsKeyDrp[row]->Select(0);
		virtualKeyTownsKeyDrp[row]->SelectByString(townsKeyStr.c_str(),YSFALSE);

		virtualKeyPhysIdDrp[row]->Select(profile.virtualKeys[row].physicalId);
		virtualKeyButtonDrp[row]->Select(profile.virtualKeys[row].button);
	}
}
