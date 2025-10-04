#include <iostream>
#include <string>
#include <set>
#include "profiledlg.h"
#include "fsguiapp.h"
#include "ysport.h"

#include "ym2612.h"



static std::string hotKeyAssignable[]=
{
	"NULL",
	"SPACE",
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"A",
	"B",
	"C",
	"D",
	"E",
	"F",
	"G",
	"H",
	"I",
	"J",
	"K",
	"L",
	"M",
	"N",
	"O",
	"P",
	"Q",
	"R",
	"S",
	"T",
	"U",
	"V",
	"W",
	"X",
	"Y",
	"Z",
	"ESC",
	"F1",
	"F2",
	"F3",
	"F4",
	"F5",
	"F6",
	"F7",
	"F8",
	"F9",
	"F10",
	"F11",
	"F12",
	"PRINTSCRN",
	"CAPSLOCK",
	"SCROLLLOCK",
	"PAUSEBREAK",
	"BS",
	"TAB",
	"ENTER",
	"SHIFT",
	"CTRL",
	"ALT",
	"INS",
	"DEL",
	"HOME",
	"END",
	"PAGEUP",
	"PAGEDOWN",
	"UP",
	"DOWN",
	"LEFT",
	"RIGHT",
	"NUMLOCK",
	"TILDA",
	"MINUS",
	"PLUS",
	"LBRACKET",
	"RBRACKET",
	"BACKSLASH",
	"SEMICOLON",
	"SINGLEQUOTE",
	"COMMA",
	"DOT",
	"SLASH",
};

static std::string hotKeyFunc[][2]=
{
	{"None",""},
	{"Quick Screenshot","QSS"},
	{"Quick Page 0 Screenshot","QSS 0"},
	{"Quick Page 1 Screenshot","QSS 1"},
	{"Quick State Save","QSAVE"},
	{"Quick State Load","QLOAD"},
};




ProfileDialog::ProfileDialog(FsGuiMainCanvas *canvasPtr)
{
	this->canvasPtr=canvasPtr;
}

void ProfileDialog::Make(const UiText &ui)
{
	const int nShowPath=PATH_SHOW;

	this->ui=ui;

	profileFNameTxt=AddStaticText(0,FSKEY_NULL,ui("/profile","PROFILE"),YSFALSE);

	tab=AddTabControl(0,FSKEY_NULL,YSTRUE);

	int mainTabId=0;
	{
		auto tabId=AddTab(tab,ui("/profile/main","Main"));
		mainTabId=tabId;
		BeginAddTabItem(tab,tabId);

		ROMDirBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,ui("/profile/main/romdir","ROM dir:"),YSTRUE);
		ROMDirTxt=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,"",nShowPath,YSFALSE);
		ROMDirTxt->SetLengthLimit(PATH_LENGTH);
		AddStaticText(0,FSKEY_NULL,
		    ui("/profile/main/browsing_inst","(When browsing for the ROM dir, please select one of the ROM files.)"),YSTRUE);

		CMOSFileBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,ui("/profile/main/cmosfile","CMOS RAM Image:"),YSTRUE);
		CMOSFileTxt=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,"",nShowPath,YSFALSE);
		ROMDirTxt->SetLengthLimit(PATH_LENGTH);
		AddStaticText(0,FSKEY_NULL,
		    ui("/profile/main/cmos_inst","(If you leave it blank, Tsugaru will use the default location.)"),YSTRUE);

		CPUFreqTxt=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,ui("/profile/main/cpufreq","CPU Speed (MHz)"),4,YSTRUE);
		CPUFreqTxt->SetInteger(25);
		AddStaticText(0,FSKEY_NULL,
		    ui("/profile/main/freq_info","(Too-fast frequency rather slows down VM)"),YSFALSE);

		RAMSizeTxt=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,ui("/profile/main/ramsize","RAM (MB)"),4,YSFALSE);
		RAMSizeTxt->SetInteger(4);
		FPUBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,ui("/profile/main/enable387","Enable 80387 FPU"),YSFALSE);

		AddStaticText(0,FSKEY_NULL,ui("/profile/main/cpufidelity","CPU Fidelity:"),YSTRUE);
		CPUFidelityDrp=AddEmptyDropList(0,FSKEY_NULL,"",8,20,20,YSFALSE);
		CPUFidelityDrp->AddString(i486DXCommon::FidelityLevelToStr(i486DXCommon::MID_FIDELITY).c_str(),YSTRUE);
		CPUFidelityDrp->AddString(i486DXCommon::FidelityLevelToStr(i486DXCommon::HIGH_FIDELITY).c_str(),YSFALSE);
		CPUFidelityHelpBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,ui("/profile/main/whatsfidelity","What's this?"),YSFALSE);
		pretend386DXBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,ui("/profile/main/pretend386dx","Pretend 80386DX (for TownsOS V2.1 L10 etc.)"),YSFALSE);

		CDImgBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,ui("/profile/main/cdimage","CD Image:"),YSTRUE);
		CDImgTxt=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,"",nShowPath,YSFALSE);
		CDImgTxt->SetLengthLimit(PATH_LENGTH);

		AddStaticText(0,FSKEY_NULL,ui("/profile/main/cdspeed","CD Speed"),YSTRUE);
		CDSpeedDrp=AddEmptyDropList(0,FSKEY_NULL,"",16,8,8,YSFALSE);
		CDSpeedDrp->AddString("Default",YSTRUE);
		CDSpeedDrp->AddString("1X",YSFALSE);
		CDSpeedDrp->AddString("2X",YSFALSE);
		CDSpeedDrp->AddString("3X",YSFALSE);
		CDSpeedDrp->AddString("4X",YSFALSE);
		CDSpeedDrp->AddString("5X",YSFALSE);
		CDSpeedDrp->AddString("6X",YSFALSE);
		CDSpeedDrp->AddString("7X",YSFALSE);
		CDSpeedDrp->AddString("8X",YSFALSE);

		FDImgBtn[0]=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"FD0:",YSTRUE);
		FDWriteProtBtn[0]=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,ui("/profile/main/writeprotect","Write Protect"),YSFALSE);
		FDImgTxt[0]=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,"",nShowPath-24,YSFALSE);
		FDImgTxt[0]->SetLengthLimit(PATH_LENGTH);

		FDImgBtn[1]=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"FD1:",YSTRUE);
		FDWriteProtBtn[1]=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,ui("/profile/main/writeprotect","Write Protect"),YSFALSE);
		FDImgTxt[1]=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,"",nShowPath-24,YSFALSE);
		FDImgTxt[1]->SetLengthLimit(PATH_LENGTH);

		scrnScaleTxt=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,ui("/profile/main/scaling","Scaling(%):"),4,YSTRUE);
		scrnScaleTxt->SetLengthLimit(PATH_LENGTH);
		scrnScaleTxt->SetInteger(100);

		scrnAutoScaleBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,ui("/profile/main/autoscaling","Auto Scaling (Fit to Window Size)"),YSTRUE);
		scrnModeDrp=AddEmptyDropList(0,FSKEY_NULL,"",4,20,20,YSFALSE);
		scrnModeDrp->AddString("NORMAL WINDOW",YSTRUE);
		scrnModeDrp->AddString("MAXIMIZE",YSFALSE);
		scrnModeDrp->AddString("FULL SCREEN",YSFALSE);
		scrnMaintainAspectBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,ui("/profile/main/maintainaspect","Maintain Aspect Ratio"),YSFALSE);

		AddStaticText(0,FSKEY_NULL,ui("/profile/main/model","Model:"),YSTRUE);

		townsTypeDrp=AddEmptyDropList(0,FSKEY_NULL,"",20,20,20,YSFALSE);
		townsTypeDrp->AddString("Unspecified",YSTRUE);
		townsTypeDrp->AddString(TownsTypeToStr(TOWNSTYPE_MODEL1_2).c_str(),YSFALSE);
		townsTypeDrp->AddString(TownsTypeToStr(TOWNSTYPE_1F_2F).c_str(),YSFALSE);
		townsTypeDrp->AddString(TownsTypeToStr(TOWNSTYPE_10F_20F).c_str(),YSFALSE);
		townsTypeDrp->AddString(TownsTypeToStr(TOWNSTYPE_2_UX).c_str(),YSFALSE);
		townsTypeDrp->AddString(TownsTypeToStr(TOWNSTYPE_2_CX).c_str(),YSFALSE);
		townsTypeDrp->AddString(TownsTypeToStr(TOWNSTYPE_2_UG).c_str(),YSFALSE);
		townsTypeDrp->AddString(TownsTypeToStr(TOWNSTYPE_2_HG).c_str(),YSFALSE);
		townsTypeDrp->AddString(TownsTypeToStr(TOWNSTYPE_2_HR).c_str(),YSFALSE);
		townsTypeDrp->AddString(TownsTypeToStr(TOWNSTYPE_2_UR).c_str(),YSFALSE);
		townsTypeDrp->AddString(TownsTypeToStr(TOWNSTYPE_2_MA).c_str(),YSFALSE);
		townsTypeDrp->AddString(TownsTypeToStr(TOWNSTYPE_2_MX).c_str(),YSFALSE);
		townsTypeDrp->AddString(TownsTypeToStr(TOWNSTYPE_2_ME).c_str(),YSFALSE);
		townsTypeDrp->AddString(TownsTypeToStr(TOWNSTYPE_2_MF_FRESH).c_str(),YSFALSE);
		townsTypeDrp->AddString(TownsTypeToStr(TOWNSTYPE_2_HC).c_str(),YSFALSE);
		townsTypeDrp->AddString(TownsTypeToStr(TOWNSTYPE_MARTY).c_str(),YSFALSE);

		scanLineEffectIn15KHzBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,
		    ui("/profile/main/scanline","Scanline Effect (Only 15khz Screen)"),YSFALSE);
		damperWireLineBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,
		    ui("/profile/main/damperwireline","Render Damper-Wire Line (to make you nostalgic)"),YSFALSE);

		startUpStateFNameBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,ui("/profile/main/loadstate","Load VM State"),YSTRUE);
		startUpStateFNameTxt=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,"",nShowPath,YSFALSE);

		EndAddTabItem();
	}

	{
		auto tabId=AddTab(tab,ui("/profile/rs232c","RS232C"));
		BeginAddTabItem(tab,tabId);

		AddStaticText(0,FSKEY_NULL,ui("/profile/rs232c/tcpfwd","RS232C to TCP/IP Forwarding."),YSTRUE);
		RS232CtoTCPTxt=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,ui("/profile/rs232c/tcpfwd_addr","IPAddress:PORT"),32,YSTRUE);
		AddStaticText(0,FSKEY_NULL,
		    ui("/profile/rs232c/tcpfwd_warning",
			"!!CAUTION!! TRANSMISSION IS NOT ENCRYPTED!!!!\n"
			"!!CAUTION!! DO NOT SEND SENSITIVE AND/OR PERSONAL INFORMATION OVER TCP/IP FORWARDING!!!!\n"
			"!!CAUTION!! I WILL NOT TAKE RESPONSIBILITY FOR ANY DAMAGE CAUSED BY COMPROMISED INFORMATION!!!!")
			,YSTRUE);

		EndAddTabItem();
	}

	{
		auto tabId=AddTab(tab,ui("/profile/scsi","SCSI"));
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

		fastSCSIBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,
		   ui("/profile/scsi/fast","Fast SCSI (Faster HDD access but may less compatibility)"),YSTRUE);

		EndAddTabItem();
	}

	{
		auto tabId=AddTab(tab,ui("/profile/gameport","GamePort"));
		BeginAddTabItem(tab,tabId);

		for(int i=0; i<2; ++i)
		{
			std::string label=ui("/profile/gameport/port","Game Port ");
			label.push_back('0'+i);
			label.push_back(':');

			AddStaticText(0,FSKEY_NULL,label.c_str(),YSTRUE);
			gamePortDrp[i]=AddEmptyDropList(0,FSKEY_NULL,"",16,40,40,YSTRUE);
			for(auto label : FsGuiMainCanvas::GetSelectableGamePortEmulationTypeHumanReadable())
			{
				gamePortDrp[i]->AddString(label.c_str(),YSFALSE);
			}

			AddStaticText(0,FSKEY_NULL,ui("/profile/gameport/maxbtnholdtime","Max Button-Hold Time(ms)"),YSTRUE);
			maxButtonHoldTimeTxt[i][0]=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,L"Btn0:",4,YSFALSE);
			maxButtonHoldTimeTxt[i][1]=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,L"Btn1:",4,YSFALSE);
		}

		AddStaticText(0,FSKEY_NULL,
		    ui("/profile/gameport/maxbtnholdtime_info",
			"Max Button-Hold Time will help you make a single click.  Some games are virtually unplayable\n"
			"because you end up making unwanted menu selection unless you really release the button quickly\n"
			"after pressing. By setting non-zero value in Max Button-Hold Time, Tsugaru virtually releases\n"
			"the mouse/pad button after the specified time.")
			,
			YSTRUE);

		gamePortDrp[0]->SelectByString(FsGuiMainCanvas::GamePortEmulationTypeToHumanReadable(TOWNS_GAMEPORTEMU_PHYSICAL0).c_str());
		gamePortDrp[1]->SelectByString(FsGuiMainCanvas::GamePortEmulationTypeToHumanReadable(TOWNS_GAMEPORTEMU_MOUSE).c_str());

		AddStaticText(0,FSKEY_NULL,ui("/profile/gameport/mouse_integ_speed","Mouse-Integration Speed"),YSTRUE);
		AddStaticText(0,FSKEY_NULL,ui("/profile/gameport/mouse_integ_info","(Try slowing down if mouse-cursor jumps around)"),YSTRUE);
		AddStaticText(0,FSKEY_NULL,ui("/profile/gameport/slow","Slow"),YSTRUE);
		mouseIntegSpdSlider=AddHorizontalSlider(0,FSKEY_NULL,20,32.0,256.0,YSFALSE);
		AddStaticText(0,FSKEY_NULL,ui("/profile/gameport/fast","Fast"),YSFALSE);

		mouseIntegConsiderVRAMOffsetBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,ui("/profile/gameport/considervramoffset","Consider VRAM Offset"),YSFALSE);
		AddStaticText(0,FSKEY_NULL,"MinX:",YSTRUE);
		mouseMinXTxt=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,"",5,YSFALSE);
		AddStaticText(0,FSKEY_NULL,"MinY:",YSFALSE);
		mouseMinYTxt=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,"",5,YSFALSE);
		AddStaticText(0,FSKEY_NULL,"MaxX:",YSFALSE);
		mouseMaxXTxt=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,"",5,YSFALSE);
		AddStaticText(0,FSKEY_NULL,"MaxY:",YSFALSE);
		mouseMaxYTxt=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,"",5,YSFALSE);

		EndAddTabItem();
	}

	{
		auto tabId=AddTab(tab,ui("/profile/keyboard1","Keyboard1"));
		BeginAddTabItem(tab,tabId);

		AddStaticText(0,FSKEY_NULL,
		    ui("/profile/keyboard1/mode",
		    "Keyboard Mode (TRANS1:ESC->ESC+BREAK, TRANS2:ESC->ESC, TRANS3:ESC->BREAK):"),YSTRUE);
		keyboardModeDrp=AddEmptyDropList(0,FSKEY_NULL,"",20,20,20,YSFALSE);
		for(int i=0; i<TOWNS_KEYBOARD_MODE_NUM_MODES; ++i)
		{
			keyboardModeDrp->AddString(TownsKeyboardModeToStr(i).c_str(),YSFALSE);
		}

		AddStaticText(0,FSKEY_NULL,ui("/profile/keyboard1/virtualkey","Virtual Keys:"),YSTRUE);
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

			AddStaticText(0,FSKEY_NULL,ui("/profile/keyboard1/joistickid","JoystickID:"),YSFALSE);
			virtualKeyPhysIdDrp[row]=AddEmptyDropList(0,FSKEY_NULL,"",8,4,4,YSFALSE);
			virtualKeyPhysIdDrp[row]->AddString("0",YSTRUE);
			virtualKeyPhysIdDrp[row]->AddString("1",YSFALSE);
			virtualKeyPhysIdDrp[row]->AddString("2",YSFALSE);
			virtualKeyPhysIdDrp[row]->AddString("3",YSFALSE);
			virtualKeyPhysIdDrp[row]->AddString("4",YSFALSE);
			virtualKeyPhysIdDrp[row]->AddString("5",YSFALSE);
			virtualKeyPhysIdDrp[row]->AddString("6",YSFALSE);
			virtualKeyPhysIdDrp[row]->AddString("7",YSFALSE);

			AddStaticText(0,FSKEY_NULL,ui("/profile/keyboard1/button","Button:"),YSFALSE);
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
		auto tabId=AddTab(tab,ui("/profile/keyboard2","Keyboard2"));
		BeginAddTabItem(tab,tabId);
		
		AddStaticText(0,FSKEY_NULL,ui("/profile/keyboard1/virtualkey","Virtual Keys:"),YSTRUE);
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

			AddStaticText(0,FSKEY_NULL,ui("/profile/keyboard1/joistickid","JoystickID:"),YSFALSE);
			virtualKeyPhysIdDrp[row]=AddEmptyDropList(0,FSKEY_NULL,"",8,4,4,YSFALSE);
			virtualKeyPhysIdDrp[row]->AddString("0",YSTRUE);
			virtualKeyPhysIdDrp[row]->AddString("1",YSFALSE);
			virtualKeyPhysIdDrp[row]->AddString("2",YSFALSE);
			virtualKeyPhysIdDrp[row]->AddString("3",YSFALSE);
			virtualKeyPhysIdDrp[row]->AddString("4",YSFALSE);
			virtualKeyPhysIdDrp[row]->AddString("5",YSFALSE);
			virtualKeyPhysIdDrp[row]->AddString("6",YSFALSE);
			virtualKeyPhysIdDrp[row]->AddString("7",YSFALSE);

			AddStaticText(0,FSKEY_NULL,ui("/profile/keyboard1/button","Button:"),YSFALSE);
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

		AddStaticText(0,FSKEY_NULL,ui("/profile/keyboard2/keymappingfile","Key-Mapping File (See File->New for Generating a Template)"),YSTRUE);
		selectKeyMapFileBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,ui("/browse","Browse"),YSTRUE);
		keyMapFileTxt=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,"",nShowPath,YSFALSE);

		EndAddTabItem();
	}

	{
		auto tabId=AddTab(tab,ui("/profile/sound","Sound"));
		BeginAddTabItem(tab,tabId);

		AddStaticText(0,FSKEY_NULL,ui("/profile/sound/fmvolume","FM Volume"),YSTRUE);
		fmVolumeDefaultBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,ui("/setdefault","Set Default"),YSFALSE);
		fmVolumeText=AddStaticText(0,FSKEY_NULL,"00000",YSFALSE);
		fmVolumeSlider=AddHorizontalSlider(0,FSKEY_NULL,32,0.0,8192.0,YSFALSE);

		AddStaticText(0,FSKEY_NULL,ui("/profile/sound/pcmvolume","PCM Volume"),YSTRUE);
		pcmVolumeDefaultBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,ui("/setdefault","Set Default"),YSFALSE);
		pcmVolumeText=AddStaticText(0,FSKEY_NULL,"00000",YSFALSE);
		pcmVolumeSlider=AddHorizontalSlider(0,FSKEY_NULL,32,0.0,8192.0,YSFALSE);

		maxSoundDoubldBufBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,ui("/profile/sound/maxdoublebuf","Maximum Double Buffering"),YSTRUE);

		AddStaticText(0,FSKEY_NULL,ui("/profile/sound/num_midi_cards","Number of MIDI Cards"),YSTRUE);
		numMidiCardsDrp=AddEmptyDropList(0,FSKEY_NULL,"",8,4,4,YSFALSE);
		numMidiCardsDrp->AddString("0",YSTRUE);
		numMidiCardsDrp->AddString("1",YSFALSE);
		numMidiCardsDrp->AddString("2",YSFALSE);
		numMidiCardsDrp->AddString("3",YSFALSE);
		numMidiCardsDrp->AddString("4",YSFALSE);

		AddStaticText(0,FSKEY_NULL,
			"At this time, MIDI audio is only available in Windows.\n"
			"Enabling MIDI in non-Windows environment only enables MIDI I/O\n"
			"but does not play music.",YSTRUE);

		EndAddTabItem();
	}

	{
		auto tabId=AddTab(tab,ui("/profile/boot","Boot"));
		BeginAddTabItem(tab,tabId);

		AddStaticText(0,FSKEY_NULL,ui("/profile/boot/option","Boot Option:"),YSTRUE);
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
		bootKeyBtn[15]=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,"T",YSFALSE);
		bootKeyBtn[16]=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,"N",YSFALSE);
		SetRadioButtonGroup(sizeof(bootKeyBtn)/sizeof(bootKeyBtn[0]),bootKeyBtn);

		EndAddTabItem();
	}

	{
		auto tabId=AddTab(tab,ui("/profile/advanced","Advanced"));
		BeginAddTabItem(tab,tabId);
		catchUpRealTimeBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,"Catch Up Real Time (Recommended OFF for ChaseHQ)",YSTRUE);

		AddStaticText(0,FSKEY_NULL,L"Crop Screenshot:",YSTRUE);
		scrnShotCropTxt[0]=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,"X0",5,YSFALSE);
		scrnShotCropTxt[1]=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,"Y0",5,YSFALSE);
		scrnShotCropTxt[2]=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,"WID",5,YSFALSE);
		scrnShotCropTxt[3]=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,"HEI",5,YSFALSE);

		AddStaticText(0,FSKEY_NULL,L"Map Location:",YSTRUE);
		mapXYExpressionTxt[0]=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,"X:",16,YSTRUE);
		mapXYExpressionTxt[1]=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,"Y:",16,YSFALSE);

		AddStaticText(0,FSKEY_NULL,
			"If the physical addresses of the memory bytes where the map coordinates\n"
			"are written are known, you can set map location expressions in\n"
			"here.  Then X and Y coordinates are automatically added to the\n"
			"quick-screenshot file name, so that another program can locate\n"
			"the screenshot at the correct location.\n"
			"Eg.  WORD:0x7082F4  If map coordinate is written as word at physical\n"
			"     address 0x7082F4."
			,
			YSTRUE);

		EndAddTabItem();
	}

	{
		auto tabId=AddTab(tab,ui("/profile/mouse_by_flightstick","Mouse by Flight Joystick"));
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
		    ui("/profile/mouse_by_flightstick/info",
			"Games like Wing Commander and Strike Commander let you control your fighter by mouse.\n"
			"This feature translates joystick to mouse coordinate and let you fly using a flight joystick device.\n"
			"Need to be used with Application-Specific Augmentation if the application is not\n"
			"using TBIOS for reading mouse.")
		,YSTRUE);


		strikeCommanderThrottleEnableBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,
		  ui("/profile/mouse_by_flightstick/throttle_for_stcm","Use Throttle Axis for Strike Commander/Wing Commander"),YSTRUE);

		AddStaticText(0,FSKEY_NULL,ui("/joystickid","JoystickID:"),YSTRUE);
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

		AddStaticText(0,FSKEY_NULL,ui("/axis","Axis:"),YSFALSE);
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
		auto tabId=AddTab(tab,ui("/profile/application","Application"));
		BeginAddTabItem(tab,tabId);

		AddStaticText(0,FSKEY_NULL,ui("/profile/application","Application-Specific Augmentation"),YSTRUE);
		appSpecificAugDrp=AddEmptyDropList(0,FSKEY_NULL,"",10,40,40,YSTRUE);
		appSpecificAugDrp->AddString(TownsAppToStr(TOWNS_APPSPECIFIC_NONE).c_str(),YSTRUE);

		std::set <std::string> apps;
		for(unsigned int i=0; i<TOWNS_NUM_APPSPECIFIC; ++i)
		{
			if(TOWNS_APPSPECIFIC_NONE!=i)
			{
				apps.insert(TownsAppToStr(i));
			}
		}
		for(auto s : apps)
		{
			appSpecificAugDrp->AddString(s.c_str(),YSFALSE);
		}

		appSpecificExplanation=AddStaticText(0,FSKEY_NULL,L"",YSTRUE);

		EndAddTabItem();
	}

	{
		auto tabId=AddTab(tab,ui("/profile/hotkeys","Hot Keys"));
		BeginAddTabItem(tab,tabId);

		AddStaticText(0,FSKEY_NULL,ui("/profile/hotkeys/pauseresume","Pause/Resume(MENU on/off):"),YSTRUE);
		pauseResumeKeyDrp=AddEmptyDropList(0,FSKEY_NULL,"",8,12,12,YSFALSE);
		for(auto str : hotKeyAssignable)
		{
			pauseResumeKeyDrp->AddString(str.c_str(),YSFALSE);
		}

		for(int i=0; i<MAX_NUM_HOST_SHORTCUT; ++i)
		{
			hostShortCutKeyLabelDrp[i]=AddEmptyDropList(0,FSKEY_NULL,"",8,6,8,(0==i%2 ? YSTRUE : YSFALSE));
			for(auto str : hotKeyAssignable)
			{
				hostShortCutKeyLabelDrp[i]->AddString(str.c_str(),YSFALSE);
			}
			hostShortCutKeyLabelDrp[i]->Select(0);
			hostShortCutKeyCtrlBtn[i]=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,"CTRL",YSFALSE);
			hostShortCutKeyShiftBtn[i]=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,"SHIFT",YSFALSE);
			hostShortCutFunctionDrp[i]=AddEmptyDropList(0,FSKEY_NULL,"",8,14,16,YSFALSE);
			for(auto pair : hotKeyFunc)
			{
				hostShortCutFunctionDrp[i]->AddString(pair[0].c_str(),YSFALSE);
			}
			hostShortCutFunctionDrp[i]->Select(0);
		}

		AddStaticText(0,FSKEY_NULL,ui("/profile/hotkeys/qssdir","Quick Screen Shot Dir:"),YSTRUE);
		quickSsDirBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,ui("/browse","Browse"),YSTRUE);
		quickSsDirTxt=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,"",nShowPath,YSFALSE);;

		AddStaticText(0,FSKEY_NULL,ui("/profile/hotkeys/qssname","Quick State-Save File Name:"),YSTRUE);
		quickStateSaveFNameBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,ui("/browse","Browse"),YSTRUE);
		quickStateSaveFNameTxt=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,"",nShowPath,YSFALSE);

		EndAddTabItem();
	}

	{
		auto tabId=AddTab(tab,ui("/profile/tgdrv","TGDRV"));
		BeginAddTabItem(tab,tabId);

		for(int i=0; i<MAX_NUM_SHARED_DIR; ++i)
		{
			browseShareDirBtn[i]=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,ui("/browse","Browse"),YSTRUE);
			shareDirTxt[i]=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,"",nShowPath,YSFALSE);;
		}

		AddStaticText(0,FSKEY_NULL,
		    ui("/profile/tgdrv/info",
			"TGDRV enables VM-HOST file sharing.  Specify directories above to share,\n"
			"and run TGDRV.COM in the utility disk (TsugaruUtil.D77) in the VM.  Then\n"
			"shared directory will show up as a virtual drive.\n"
			"\n"
			"Towns MENU V2.1 is required.  Drive will be available, but will not be\n"
			"visible from Towns MENU V1.1\n"
			"\n"
			"Browse: Click on a file in the shared directory.  Can be a dummy file.\n")
			,YSTRUE);

		EndAddTabItem();
	}

	tab->SelectCurrentTab(mainTabId);

	runBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,ui("/profile/start","START!"),YSTRUE);
	autoStartBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,ui("/profile/autostart","Auto Start (Start VM as soon as the profile is loaded)"),YSFALSE);
	separateProcBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,ui("/profile/separateprocess","Separate Process"),YSFALSE);

	if(true!=canvasPtr->separateProcess)
	{
// macOS doen't send SCROLL_LOCK key code to the application.  WTF.
#ifndef __APPLE__
		AddStaticText(0,FSKEY_NULL,
		    ui("/profile/menu_info",
			"Press SCROLL_LOCK or Click on the lower-right [MENU] icon\n"
			"to come back to the GUI from the VM."),
			YSTRUE);
#else
		AddStaticText(0,FSKEY_NULL,
		    ui("/profile/menu_info_apple",
			"Click on the lower-right [MENU] icon\n"
			"to come back to the GUI from the VM."),
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
		default:
		case TOWNS_APPSPECIFIC_NONE:
			appSpecificExplanation->SetText("");
			break;
		case TOWNS_APPSPECIFIC_WINGCOMMANDER1:
			appSpecificExplanation->SetText(
				"Wing Commander, Origin\n"
			    "Mouse Integration.\n"
				"Wing Commander 1 for FM TOWNS uses its own function\n"
				"instead of the Mouse BIOS to read mouse status.\n"
				"To send mouse data correctly, Tsugasu needs to\n"
				"know that it is running Wing Commander 1."
			);
			break;
		case TOWNS_APPSPECIFIC_WINGCOMMANDER2:
			appSpecificExplanation->SetText(
				"Wing Commander 2, Origin\n"
			    "Mouse Integration.\n"
				"Wing Commander 2 for FM TOWNS uses its own function\n"
				"instead of the Mouse BIOS to read mouse status.\n"
				"To send mouse data correctly, Tsugasu needs to\n"
				"know that it is running Wing Commander 2."
			);
			break;
		case TOWNS_APPSPECIFIC_STRIKECOMMANDER:
			appSpecificExplanation->SetText(
				"Strike Commander, Origin\n"
			    "Mouse Integration.\n"
				"Strike Commander uses hybrid of own function and TBIOS\n"
				"for reading mouse.\n"
				"Mouse Integration is activated once you press an arrow key.\n"
				"Also minimum RAM size is set to 8.\n"
			);
			break;
		case TOWNS_APPSPECIFIC_SUPERDAISEN:
			appSpecificExplanation->SetText(
				"Super Strategic Confrontation (Super Daisenryaku), Systemsoft\n"
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
				"Lemmings, Psygnosis\n"
				"Mouse Integration.\n"
				"Like Wing Commander 1, Lemmings uses its own\n"
				"mouse-reading function.  This option enables\n"
				"control by mouse in Lemmings.\n"
				"Cursor trembles a little bit.\n"
			);
			break;
		case TOWNS_APPSPECIFIC_LEMMINGS2:
			appSpecificExplanation->SetText(
				"Lemmings2, Psygnosis\n"
				"Mouse Integration.\n"
				"Like Wing Commander 1, Lemmings uses its own\n"
				"mouse-reading function.  This option enables\n"
				"control by mouse in Lemmings.\n"
				"Cursor trembles a little bit.\n"
			);
			break;
		case TOWNS_APPSPECIFIC_DAIKOUKAIJIDAI:
			appSpecificExplanation->SetText(
				"Uncharted Warters (DAIKOUKAI JIDAI), KOEI\n"
				"With 99.99% confidence, I believe DaikoukaiJidai (The Age of Navigation)\n"
				"for FM TOWNS nullifies a pointer after winning a battle, but never\n"
				"set a valid pointer before using it next time.  As a result, no \n"
				"pirates appears after a victory, or in the worst case the program\n"
				"crashes.  This patch prevents the pointer-destruction.\n"
				"This patch also replaces busy-wait with timer (I/O 6Ch).\n"
				"This patch also enables keyboard shortcuts.\n"
			);
			break;
		case TOWNS_APPSPECIFIC_RASHINBAN:
			appSpecificExplanation->SetText(
				"Golden Compass (Ougon No Rashinban), Riverhill Soft\n"
				"Mouse Integration.\n"
				"Also connect mouse to game port 0.\n"
			);
			break;
		case TOWNS_APPSPECIFIC_ORGEL:
			appSpecificExplanation->SetText(
				"Orgel (DAPS ver, not DCCS ver), Data West\n"
				"CD-ROM 1x Read time, seek time emulation.\n"
				"CD drive reading too fast causes audio issues,\n"
				"and freezes when playing DAPS replay.\n"
			);
			break;
		case TOWNS_APPSPECIFIC_ASUKA120:
			appSpecificExplanation->SetText(
				"Asuka 120% (Excellent) Burning Fest, Family Soft\n"
				"Disconected the Mouse.\n"
				"Asuka 120% is freezes in the battle screen\n"
				"if the mouse connected to game ports.\n"
			);
			break;
		}
	}
}

void ProfileDialog::OnSliderPositionChange(FsGuiSlider *slider,const double &prevPos,const double &prevValue)
{
	if(slider==fmVolumeSlider)
	{
		UpdateFMVolumeText();
	}
	else if(slider==pcmVolumeSlider)
	{
		UpdatePCMVolumeText();
	}
}

/* virtual */ void ProfileDialog::OnButtonClick(FsGuiButton *btn)
{
	if(ROMDirBtn==btn)
	{
		YsString defDir=ROMDirTxt->GetString();
		if(0==defDir.Strcmp(""))
		{
			YsSpecialPath::GetUserDir(defDir);
		}

		YsString def;
		def.MakeFullPathName(defDir,"FMT_SYS.ROM");

		auto fdlg=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiFileDialog>();
		fdlg->Initialize();
		fdlg->mode=FsGuiFileDialog::MODE_OPEN;
		fdlg->multiSelect=YSFALSE;
		fdlg->title.Set(L"Select a ROM file (like FMT_SYS.ROM)");
		fdlg->fileExtensionArray.Append(L".ROM");
		fdlg->defaultFileName.SetUTF8String(def);
		AttachModalDialog(fdlg);
		fdlg->CallOnCloseModal([=,this](int returnCode)
		{
			if((int)YSOK==returnCode)
			{
				YsWString ful(fdlg->selectedFileArray[0]);
				YsWString pth,fil;
				ful.SeparatePathFile(pth,fil);
				ROMDirTxt->SetText(pth);
			}
		});
	}
	if(CMOSFileBtn==btn)
	{
		std::vector <const wchar_t *> extList={L".BIN"};
		BrowseSaveAs(L"CMOS RAM Image",CMOSFileTxt,extList);
	}
	if(CDImgBtn==btn)
	{
		std::vector <const wchar_t *> extList={L".CUE",L".ISO",L".MDS",L".CCD"};
		Browse(L"CD Image",CDImgTxt,GetDefaultCDImageFileName(),extList);
	}
	if(startUpStateFNameBtn==btn)
	{
		std::vector <const wchar_t *> extList={L".TState"};
		Browse(L"Start-Up VM State",startUpStateFNameTxt,GetDefaultStateFileName(),extList);
	}
	for(int i=0; i<2; ++i)
	{
		if(FDImgBtn[i]==btn)
		{
			YsWString label(L"Floppy Drive ");
			label.push_back('0'+i);
			std::vector <const wchar_t *> extList={L".BIN",L".D77",L".RDD",L".D88",L".XDF"};
			Browse(label,FDImgTxt[i],GetDefaultFDImageFileName(i),extList);
		}
	}
	for(int i=0; i<sizeof(HDImgBtn)/sizeof(HDImgBtn[0]); ++i)
	{
		if(HDImgBtn[i]==btn)
		{
			YsWString label(L"SCSI HDD ");
			label.push_back('0'+i);
			std::vector <const wchar_t *> extList={L".BIN",L".HDD",L".HDM",L".HDI",L".H0",L".H1",L".H2",L".H3"};
			Browse(label,HDImgTxt[i],GetDefaultHDImageFileName(i),extList);
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
	if(fmVolumeDefaultBtn==btn)
	{
		SetDefaultFMVolume();
	}
	if(pcmVolumeDefaultBtn==btn)
	{
		SetDefaultPCMVolume();
	}
	if(quickSsDirBtn==btn)
	{
		std::vector <const wchar_t *> extList={L".png"};
		BrowseDir(L"Select A Quick Screenshot File (Can be dummy or new png)",quickSsDirTxt,extList);
	}
	if(quickStateSaveFNameBtn==btn)
	{
		std::vector <const wchar_t *> extList={L".TState"};
		BrowseSaveAs(L"Select A Quick State-Save File",quickStateSaveFNameTxt,extList);
	}
	if(CPUFidelityHelpBtn==btn)
	{
		ExplainCPUFidelity();
	}
	for(int i=0; i<MAX_NUM_SHARED_DIR; ++i)
	{
		if(browseShareDirBtn[i]==btn)
		{
			std::vector <const wchar_t *> extList={L"*.*"};
			BrowseDir(L"Select A File in the Shared Directory (Can be dummy file)",shareDirTxt[i],extList);
		}
	}
	if(selectKeyMapFileBtn==btn)
	{
		std::vector <const wchar_t *> extList={L".txt"};
		BrowseSaveAs(L"Select A Key-Mapping File",keyMapFileTxt,extList);
	}
	if(YSTRUE==scrnAutoScaleBtn->GetCheck())
	{
		scrnMaintainAspectBtn->Enable();
	}
	else
	{
		scrnMaintainAspectBtn->Disable();
	}

}

YsString ProfileDialog::GetDefaultCDImageFileName(void)  const
{
	YsString str=CDImgTxt->GetString();
	if(0==str.Strcmp(""))
	{
		str=MakeUpAnyImageFileName("*.CUE");
	}
	return str;
}
YsString ProfileDialog::GetDefaultStateFileName(void)    const
{
	YsString str=startUpStateFNameTxt->GetString();
	if(0==str.Strcmp(""))
	{
		str=MakeUpAnyImageFileName("*.tstate");
	}
	return str;
}
YsString ProfileDialog::GetDefaultFDImageFileName(int i) const
{
	YsString str;
	for(auto txt : FDImgTxt)
	{
		if(nullptr!=txt)
		{
			str=txt->GetString();
			if(0!=str.Strcmp(""))
			{
				break;
			}
		}
	}
	if(0==str.Strcmp(""))
	{
		str=MakeUpAnyImageFileName("*.D77");
	}
	return str;
}
YsString ProfileDialog::GetDefaultHDImageFileName(int i) const
{
	YsString str;
	for(auto txt : HDImgTxt)
	{
		if(nullptr!=txt)
		{
			str=txt->GetString();
			if(0!=str.Strcmp(""))
			{
				break;
			}
		}
	}
	if(0==str.Strcmp(""))
	{
		str=MakeUpAnyImageFileName("*.H0");
	}
	return str;
}
YsString ProfileDialog::MakeUpAnyImageFileName(YsString filePart) const
{
	YsString ful=canvasPtr->lastSelectedProfileFName.GetUTF8String();
	if(0==ful.Strcmp(""))
	{
		ful=CDImgTxt->GetString();
	}
	if(0==ful.Strcmp(""))
	{
		for(auto txt : FDImgTxt)
		{
			if(nullptr!=txt && 0==ful.Strcmp(""))
			{
				ful=txt->GetString();
			}
		}
	}
	if(0==ful.Strcmp(""))
	{
		for(auto txt : HDImgTxt)
		{
			if(nullptr!=txt && 0==ful.Strcmp(""))
			{
				ful=txt->GetString();
			}
		}
	}
	if(0!=ful.Strcmp(""))
	{
		YsString str,path,file;
		ful.SeparatePathFile(path,file);
		str.MakeFullPathName(path,filePart);
		return str;
	}
	return "";
}

void ProfileDialog::Browse(const wchar_t label[],FsGuiTextBox *txt,YsString dflt,std::vector <const wchar_t *> extList)
{
	nowBrowsingTxt=txt;

	YsString def=dflt;
	if(0==def.Strcmp(""))
	{
		def=ROMDirTxt->GetString();
	}
	if(0==def.Strcmp(""))
	{
		YsSpecialPath::GetUserDir(def);
	}

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
	AttachModalDialog(fdlg);

	fdlg->CallOnCloseModal([=,this](int returnCode)
	{
		if((int)YSOK==returnCode)
		{
			nowBrowsingTxt->SetText(fdlg->selectedFileArray[0]);
		}
	});
}

void ProfileDialog::BrowseSaveAs(const wchar_t label[],FsGuiTextBox *txt,std::vector <const wchar_t *> extList)
{
	nowBrowsingTxt=txt;

	YsString def=ROMDirTxt->GetString();

	auto fdlg=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiFileDialog>();
	fdlg->Initialize();
	fdlg->mode=FsGuiFileDialog::MODE_SAVE;
	fdlg->multiSelect=YSFALSE;
	fdlg->title.Set(label);
	for(auto ext : extList)
	{
		fdlg->fileExtensionArray.Append(ext);
	}
	fdlg->defaultFileName.SetUTF8String(def);
	fdlg->BindCloseModalCallBack(&THISCLASS::OnSelectFileSaveAs,this);
	AttachModalDialog(fdlg);
}
void ProfileDialog::OnSelectFileSaveAs(FsGuiDialog *dlg,int returnCode)
{
	auto fdlg=dynamic_cast <FsGuiFileDialog *>(dlg);
	if((int)YSOK==returnCode && nullptr!=fdlg)
	{
		nowBrowsingTxt->SetText(fdlg->selectedFileArray[0]);
	}
}

void ProfileDialog::BrowseDir(const wchar_t label[],FsGuiTextBox *txt,std::vector <const wchar_t *> extList)
{
	nowBrowsingTxt=txt;

	YsString def=ROMDirTxt->GetString();

	auto fdlg=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiFileDialog>();
	fdlg->Initialize();
	fdlg->mode=FsGuiFileDialog::MODE_SAVE;
	fdlg->multiSelect=YSFALSE;
	fdlg->title.Set(label);
	for(auto ext : extList)
	{
		fdlg->fileExtensionArray.Append(ext);
	}
	fdlg->defaultFileName.SetUTF8String(def);
	fdlg->BindCloseModalCallBack(&THISCLASS::OnSelectDir,this);
	AttachModalDialog(fdlg);
}
void ProfileDialog::OnSelectDir(FsGuiDialog *dlg,int returnCode)
{
	auto fdlg=dynamic_cast <FsGuiFileDialog *>(dlg);
	if((int)YSOK==returnCode && nullptr!=fdlg)
	{
		auto ful=fdlg->selectedFileArray[0];
		YsWString dir,nam;
		ful.SeparatePathFile(dir,nam);
		nowBrowsingTxt->SetText(dir);
	}
}

void ProfileDialog::ToSystemEncoding(TownsProfile &profile)
{
	ToSystemEncoding(profile.ROMPath);

	ToSystemEncoding(profile.CMOSFName);

	ToSystemEncoding(profile.cdImgFName);

	ToSystemEncoding(profile.fdImgFName[0]);

	ToSystemEncoding(profile.fdImgFName[1]);

	for(int i=0; i<TownsProfile::MAX_NUM_SCSI_DEVICES; ++i)
	{
		ToSystemEncoding(profile.scsiImg[i].imgFName);
	}

	ToSystemEncoding(profile.keyMapFName);

	ToSystemEncoding(profile.startUpStateFName);

	ToSystemEncoding(profile.quickScrnShotDir);

	ToSystemEncoding(profile.quickStateSaveFName);

	for(auto &dir : profile.sharedDir)
	{
		ToSystemEncoding(dir);
	}

	for(auto &p : profile.specialPath)
	{
		ToSystemEncoding(p.second);
	}
}

/* static */ void ProfileDialog::ToSystemEncoding(std::string &str)
{
	YsWString wStr;
	wStr.SetUTF8String(str.data());

	YsString sysEnc;
	YsUnicodeToSystemEncoding(sysEnc,wStr);

	str=sysEnc.c_str();
}


TownsProfile ProfileDialog::GetProfile(void) const
{
	TownsProfile profile;
	YsString utf8;

	profile.ROMPath=ROMDirTxt->GetWString().GetUTF8String();

	profile.CMOSFName=CMOSFileTxt->GetWString().GetUTF8String();

	profile.freq=CPUFreqTxt->GetInteger();
	profile.useFPU=(YSTRUE==FPUBtn->GetCheck());
	profile.memSizeInMB=RAMSizeTxt->GetInteger();

	profile.cdImgFName=CDImgTxt->GetWString().GetUTF8String();

	profile.fdImgFName[0]=FDImgTxt[0]->GetWString().GetUTF8String();

	profile.fdImgWriteProtect[0]=(YSTRUE==FDWriteProtBtn[0]->GetCheck());

	profile.fdImgFName[1]=FDImgTxt[1]->GetWString().GetUTF8String();

	profile.fdImgWriteProtect[1]=(YSTRUE==FDWriteProtBtn[1]->GetCheck());
	for(int i=0; i<TownsProfile::MAX_NUM_SCSI_DEVICES; ++i)
	{
		profile.scsiImg[i].imgFName=HDImgTxt[i]->GetWString().GetUTF8String();
		if(""!=profile.scsiImg[i].imgFName)
		{
			profile.scsiImg[i].imageType=TownsProfile::SCSIIMAGE_HARDDISK;
		}
		else
		{
			profile.scsiImg[i].imageType=TownsProfile::SCSIIMAGE_NONE;
		}
	}
	profile.fastSCSI=(YSTRUE==fastSCSIBtn->GetCheck());

	for(int gameport=0; gameport<2; ++gameport)
	{
		auto sel=gamePortDrp[gameport]->GetSelectedString();
		profile.gamePort[gameport]=FsGuiMainCanvas::HumanReadableToGamePortEmulationType(sel.c_str());

		profile.maxButtonHoldTime[gameport][0]=maxButtonHoldTimeTxt[gameport][0]->GetInteger();
		profile.maxButtonHoldTime[gameport][1]=maxButtonHoldTimeTxt[gameport][1]->GetInteger();
		profile.maxButtonHoldTime[gameport][0]*=1000000;
		profile.maxButtonHoldTime[gameport][1]*=1000000;
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
	profile.considerVRAMOffsetInMouseIntegration=(mouseIntegConsiderVRAMOffsetBtn->GetCheck()==YSTRUE);

	profile.mouseMinX=mouseMinXTxt->GetInteger();
	profile.mouseMinY=mouseMinYTxt->GetInteger();
	profile.mouseMaxX=mouseMaxXTxt->GetInteger();
	profile.mouseMaxY=mouseMaxYTxt->GetInteger();

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

	profile.townsType=StrToTownsType(townsTypeDrp->GetSelectedString().c_str());

	profile.pretend386DX=(YSTRUE==pretend386DXBtn->GetCheck());

	profile.autoStartOnLoad=(YSTRUE==autoStartBtn->GetCheck());

	profile.scaling=scrnScaleTxt->GetInteger();
	profile.autoScaling=(YSTRUE==scrnAutoScaleBtn->GetCheck());
	profile.maintainAspect=(YSTRUE==scrnMaintainAspectBtn->GetCheck());
	switch(scrnModeDrp->GetSelection())
	{
	case 0:
		profile.windowModeOnStartUp=TownsStartParameters::WINDOW_NORMAL;
		break;
	case 1:
		profile.windowModeOnStartUp=TownsStartParameters::WINDOW_MAXIMIZE;
		break;
	case 2:
		profile.windowModeOnStartUp=TownsStartParameters::WINDOW_FULLSCREEN;
		break;
	}

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


	profile.scanLineEffectIn15KHz=(YSTRUE==scanLineEffectIn15KHzBtn->GetCheck());
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

	profile.keyMapFName=keyMapFileTxt->GetWString().GetUTF8String();

	profile.fmVol=(int)fmVolumeSlider->GetScaledValue();
	if(YM2612::WAVE_OUTPUT_AMPLITUDE_MAX_DEFAULT==profile.fmVol)
	{
		profile.fmVol=-1;
	}
	profile.pcmVol=(int)pcmVolumeSlider->GetScaledValue();
	if(RF5C68::WAVE_OUTPUT_AMPLITUDE_MAX_DEFAULT==profile.pcmVol)
	{
		profile.pcmVol=-1;
	}
	profile.maximumSoundDoubleBuffering=(YSTRUE==maxSoundDoubldBufBtn->GetCheck());

	profile.nMidiCards=numMidiCardsDrp->GetSelection();

	profile.cdSpeed=CDSpeedDrp->GetSelection();
	if(profile.cdSpeed<0)
	{
		profile.cdSpeed=0;
	}

	profile.startUpStateFName=startUpStateFNameTxt->GetWString().GetUTF8String();

	profile.quickScrnShotDir=quickSsDirTxt->GetWString().GetUTF8String();
	for(int i=0; i<MAX_NUM_HOST_SHORTCUT; ++i)
	{
		auto selHostKey=hostShortCutKeyLabelDrp[i]->GetSelection();
		auto selFunc=hostShortCutFunctionDrp[i]->GetSelection();
		if(0!=selHostKey && 0!=selFunc)
		{
			TownsStartParameters::HostShortCut hsc;
			hsc.hostKey=hostShortCutKeyLabelDrp[i]->GetSelectedString().c_str();
			for(auto pair : hotKeyFunc)
			{
				std::string label=hostShortCutFunctionDrp[i]->GetSelectedString().c_str();
				if(pair[0]==label)
				{
					hsc.cmdStr=pair[1];
					break;
				}
			}
			hsc.ctrl=(YSTRUE==hostShortCutKeyCtrlBtn[i]->GetCheck());
			hsc.shift=(YSTRUE==hostShortCutKeyShiftBtn[i]->GetCheck());
			profile.hostShortCutKeys.push_back(hsc);
		}
	}

	profile.quickStateSaveFName=quickStateSaveFNameTxt->GetWString().GetUTF8String();

	profile.pauseResumeKeyLabel=pauseResumeKeyDrp->GetSelectedString().data();

	profile.sharedDir.clear();
	for(int i=0; i<MAX_NUM_SHARED_DIR; ++i)
	{
		std::string str=shareDirTxt[i]->GetWString().GetUTF8String().c_str();
		if(""!=str)
		{
			profile.sharedDir.push_back(str);
		}
	}

	profile.scrnShotX0=scrnShotCropTxt[0]->GetInteger();
	profile.scrnShotY0=scrnShotCropTxt[1]->GetInteger();
	profile.scrnShotWid=scrnShotCropTxt[2]->GetInteger();
	profile.scrnShotHei=scrnShotCropTxt[3]->GetInteger();
	profile.mapXYExpression[0]=mapXYExpressionTxt[0]->GetString().c_str();
	profile.mapXYExpression[1]=mapXYExpressionTxt[1]->GetString().c_str();

	profile.CPUFidelityLevel=i486DXCommon::StrToFidelityLevel(CPUFidelityDrp->GetSelectedString().data());

	profile.RS232CtoTCPAddr=RS232CtoTCPTxt->GetString().c_str();

	return profile;
}
void ProfileDialog::SetProfile(const TownsProfile &profile)
{
	YsWString str;

	str.SetUTF8String(profile.ROMPath.data());
	ROMDirTxt->SetText(str);

	str.SetUTF8String(profile.CMOSFName.data());
	CMOSFileTxt->SetText(str);

	CPUFreqTxt->SetInteger(profile.freq);
	FPUBtn->SetCheck(true==profile.useFPU ? YSTRUE : YSFALSE);

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
	fastSCSIBtn->SetCheck(profile.fastSCSI ? YSTRUE : YSFALSE);

	for(int gameport=0; gameport<2; ++gameport)
	{
		gamePortDrp[gameport]->SelectByString(FsGuiMainCanvas::GamePortEmulationTypeToHumanReadable(profile.gamePort[gameport]).c_str());
		maxButtonHoldTimeTxt[gameport][0]->SetInteger(profile.maxButtonHoldTime[gameport][0]/1000000);
		maxButtonHoldTimeTxt[gameport][1]->SetInteger(profile.maxButtonHoldTime[gameport][1]/1000000);
	}

	mouseIntegSpdSlider->SetPositionByScaledValue((double)profile.mouseIntegrationSpeed);
	mouseIntegConsiderVRAMOffsetBtn->SetCheck(profile.considerVRAMOffsetInMouseIntegration ? YSTRUE : YSFALSE);

	mouseMinXTxt->SetInteger(profile.mouseMinX);
	mouseMinYTxt->SetInteger(profile.mouseMinY);
	mouseMaxXTxt->SetInteger(profile.mouseMaxX);
	mouseMaxYTxt->SetInteger(profile.mouseMaxY);


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

	if(TOWNSTYPE_UNKNOWN==profile.townsType)
	{
		townsTypeDrp->Select(0);
	}
	else
	{
		townsTypeDrp->SelectByString(TownsTypeToStr(profile.townsType).c_str());
	}

	if(true==profile.pretend386DX)
	{
		pretend386DXBtn->SetCheck(YSTRUE);
	}
	else
	{
		pretend386DXBtn->SetCheck(YSFALSE);
	}

	if(true==profile.autoStartOnLoad)
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
	scrnMaintainAspectBtn->SetCheck(true==profile.maintainAspect ? YSTRUE : YSFALSE);
	if(true!=profile.autoScaling)
	{
		scrnMaintainAspectBtn->Disable();
	}
	else
	{
		scrnMaintainAspectBtn->Enable();
	}
	switch(profile.windowModeOnStartUp)
	{
	case TownsStartParameters::WINDOW_NORMAL:
		scrnModeDrp->Select(0);
		break;
	case TownsStartParameters::WINDOW_MAXIMIZE:
		scrnModeDrp->Select(1);
		break;
	case TownsStartParameters::WINDOW_FULLSCREEN:
		scrnModeDrp->Select(2);
		break;
	}
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

	scanLineEffectIn15KHzBtn->SetCheck(profile.scanLineEffectIn15KHz ? YSTRUE : YSFALSE);

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

	str.SetUTF8String(profile.keyMapFName.data());
	keyMapFileTxt->SetText(str);


	if(profile.fmVol<0)
	{
		SetDefaultFMVolume();
	}
	else
	{
		fmVolumeSlider->SetPositionByScaledValue(profile.fmVol);
		UpdateFMVolumeText();
	}
	if(profile.pcmVol<0)
	{
		SetDefaultPCMVolume();
	}
	else
	{
		pcmVolumeSlider->SetPositionByScaledValue(profile.pcmVol);
		UpdatePCMVolumeText();
	}
	maxSoundDoubldBufBtn->SetCheck(profile.maximumSoundDoubleBuffering ? YSTRUE : YSFALSE);

	numMidiCardsDrp->Select(profile.nMidiCards);


	CDSpeedDrp->Select(profile.cdSpeed);

	str.SetUTF8String(profile.startUpStateFName.data());
	startUpStateFNameTxt->SetText(str);



	str.SetUTF8String(profile.quickScrnShotDir.data());
	quickSsDirTxt->SetText(str);

	for(int i=0; i<MAX_NUM_HOST_SHORTCUT && i<profile.hostShortCutKeys.size(); ++i)
	{
		hostShortCutKeyLabelDrp[i]->Select(0);
		hostShortCutFunctionDrp[i]->Select(0);
	}
	for(int i=0; i<MAX_NUM_HOST_SHORTCUT && i<profile.hostShortCutKeys.size(); ++i)
	{
		auto hsc=profile.hostShortCutKeys[i];

		hostShortCutKeyLabelDrp[i]->SelectByString(hsc.hostKey.c_str(),YSFALSE);
		hostShortCutKeyCtrlBtn[i]->SetCheck(hsc.ctrl ? YSTRUE : YSFALSE);
		hostShortCutKeyShiftBtn[i]->SetCheck(hsc.shift ? YSTRUE : YSFALSE);

		for(auto pair : hotKeyFunc)
		{
			if(pair[1]==hsc.cmdStr)
			{
				hostShortCutFunctionDrp[i]->SelectByString(pair[0].c_str(),YSFALSE);
				break;
			}
		}
	}

	str.SetUTF8String(profile.quickStateSaveFName.data());
	quickStateSaveFNameTxt->SetText(str);

	pauseResumeKeyDrp->SelectByString(profile.pauseResumeKeyLabel.c_str(),YSFALSE);

	for(int i=0; i<MAX_NUM_SHARED_DIR; ++i)
	{
		if(i<profile.sharedDir.size())
		{
			str.SetUTF8String(profile.sharedDir[i].data());
			shareDirTxt[i]->SetText(str);
		}
		else
		{
			shareDirTxt[i]->SetText("");
		}
	}

	scrnShotCropTxt[0]->SetInteger(profile.scrnShotX0);
	scrnShotCropTxt[1]->SetInteger(profile.scrnShotY0);
	scrnShotCropTxt[2]->SetInteger(profile.scrnShotWid);
	scrnShotCropTxt[3]->SetInteger(profile.scrnShotHei);
	mapXYExpressionTxt[0]->SetText(profile.mapXYExpression[0].c_str());
	mapXYExpressionTxt[1]->SetText(profile.mapXYExpression[1].c_str());

	CPUFidelityDrp->SelectByString(i486DXCommon::FidelityLevelToStr(profile.CPUFidelityLevel).c_str());

	RS232CtoTCPTxt->SetText(profile.RS232CtoTCPAddr.c_str());
}

void ProfileDialog::SetDefaultFMVolume(void)
{
	fmVolumeSlider->SetPositionByScaledValue(YM2612::WAVE_OUTPUT_AMPLITUDE_MAX_DEFAULT);
	UpdateFMVolumeText();
}
void ProfileDialog::SetDefaultPCMVolume(void)
{
	pcmVolumeSlider->SetPositionByScaledValue(RF5C68::WAVE_OUTPUT_AMPLITUDE_MAX_DEFAULT);
	UpdatePCMVolumeText();
}
void ProfileDialog::UpdateFMVolumeText(void)
{
	auto scaled=fmVolumeSlider->GetScaledValue();
	YsString text;
	text.Printf("%d",(int)scaled);
	fmVolumeText->SetText(text);
}
void ProfileDialog::UpdatePCMVolumeText(void)
{
	auto scaled=pcmVolumeSlider->GetScaledValue();
	YsString text;
	text.Printf("%d",(int)scaled);
	pcmVolumeText->SetText(text);
}
void ProfileDialog::ExplainCPUFidelity(void)
{
	FsGuiMessageBoxDialog *dlg=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiMessageBoxDialog>();
	dlg->Make(
		"About CPU Fidelity",
		ui("/profile/main/cpufidelity_info",
		"Medium-Fidelity CPU core emulates x86 features necessary for majority of FM TOWNS applications\n"
		"including TownsOS and DOS6 and Fractal Engines.\n"
		"It skips exception handling that are not used in those applications.\n"
		"\n"
		"High-Fidelity CPU core emulates more x86 features necessary for installing and running Windows 3.1, Windows 95, and Linux.\n"
		"\n"
		"Since High-Fidelity CPU needs to check and handle more exceptions, it is slower than Medium-Fidelity\n"
		"CPU core.\n"
		"\n"
		"Windows 3.1, Windows 95, Linux, and OSASK requires High-Fidelity CPU core.\n"),
		"OK","");
	AttachModalDialog(dlg);
}
