#include "profiledlg.h"



void ProfileDialog::Make(void)
{
	const int nShowPath=80;

	ROMDirBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"ROM dir:",YSTRUE);
	ROMDirTxt=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,"",nShowPath,YSFALSE);
	AddStaticText(0,FSKEY_NULL,"(When browsing for the ROM dir, please select one of the ROM files.)",YSTRUE);

	CDImgBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"CD Image:",YSTRUE);
	CDImgTxt=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,"",nShowPath,YSFALSE);

	FDImgBtn[0]=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"FD0:",YSTRUE);
	FDImgTxt[0]=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,"",nShowPath,YSFALSE);

	FDImgBtn[1]=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"FD1:",YSTRUE);
	FDImgTxt[1]=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,"",nShowPath,YSFALSE);

	HDImgBtn[0]=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"HD0:",YSTRUE);
	HDImgTxt[0]=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,"",nShowPath,YSFALSE);

	HDImgBtn[1]=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"HD1:",YSTRUE);
	HDImgTxt[1]=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,"",nShowPath,YSFALSE);

	HDImgBtn[2]=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"HD2:",YSTRUE);
	HDImgTxt[2]=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,"",nShowPath,YSFALSE);

	HDImgBtn[3]=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"HD3:",YSTRUE);
	HDImgTxt[3]=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,"",nShowPath,YSFALSE);

	HDImgBtn[4]=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"HD4:",YSTRUE);
	HDImgTxt[4]=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,"",nShowPath,YSFALSE);

	HDImgBtn[5]=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"HD5:",YSTRUE);
	HDImgTxt[5]=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,"",nShowPath,YSFALSE);

	HDImgBtn[6]=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"HD6:",YSTRUE);
	HDImgTxt[6]=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,"",nShowPath,YSFALSE);

	AddStaticText(0,FSKEY_NULL,"Game Port 0:",YSTRUE);
	gamePortBtn[0][0]=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,"None",YSFALSE);
	gamePortBtn[0][1]=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,"GamePad0",YSFALSE);
	gamePortBtn[0][2]=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,"GamePad1",YSFALSE);
	gamePortBtn[0][3]=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,"Emulation by Keyboard(ASZX and arrow)",YSFALSE);
	gamePortBtn[0][4]=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,"Mouse",YSFALSE);
	SetRadioButtonGroup(5,gamePortBtn[0]);

	AddStaticText(0,FSKEY_NULL,"Game Port 1:",YSTRUE);
	gamePortBtn[1][0]=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,"None",YSFALSE);
	gamePortBtn[1][1]=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,"GamePad0",YSFALSE);
	gamePortBtn[1][2]=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,"GamePad1",YSFALSE);
	gamePortBtn[1][3]=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,"Emulation by Keyboard(ASZX and arrow)",YSFALSE);
	gamePortBtn[1][4]=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,"Mouse",YSFALSE);
	SetRadioButtonGroup(5,gamePortBtn[1]);

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

	runBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"START",YSTRUE);
	autoStartBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,"Auto Start (Applicable to Default Profile only)",YSFALSE);

	Fit();
	SetArrangeType(FSDIALOG_ARRANGE_TOP_LEFT);
}
