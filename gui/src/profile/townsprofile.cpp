#include <string>
#include <sstream>
#include <ysclass.h>

#include "townsprofile.h"
#include "townsdef.h"



TownsProfile::TownsProfile()
{
	CleanUp();
}
void TownsProfile::CleanUp(void)
{
	ROMPath="";
	cdImgFName="";
	cdSpeed=0;
	for(auto &f : fdImgFName)
	{
		f="";
	}
	for(auto &wp : fdImgWriteProtect)
	{
		wp=false;
	}
	for(auto &f : scsiImg)
	{
		f.imageType=SCSIIMAGE_NONE;
		f.imgFName="";
	}

	gamePort[0]=TOWNS_GAMEPORTEMU_PHYSICAL0;
	gamePort[1]=TOWNS_GAMEPORTEMU_MOUSE;

	maxButtonHoldTime[0][0]=0;
	maxButtonHoldTime[0][1]=0;
	maxButtonHoldTime[1][0]=0;
	maxButtonHoldTime[1][1]=0;

	bootKeyComb=BOOT_KEYCOMB_NONE;
	autoStart=false;
	scaling=150;
	damperWireLine=true;
	pretend386DX=false;
	freq=40;
	appSpecificSetting=TOWNS_APPSPECIFIC_NONE;

	memSizeInMB=4;
	mouseIntegrationSpeed=256;
	considerVRAMOffsetInMouseIntegration=true;

	mouseByFlightstickAvailable=false;
	mouseByFlightstickPhysicalId=-1;
	mouseByFlightstickCenterX=320,mouseByFlightstickCenterY=200;
	mouseByFlightstickZeroZoneX=0;
	mouseByFlightstickZeroZoneY=0;
	mouseByFlightstickScaleX=500.0f;
	mouseByFlightstickScaleY=400.0f;

	throttlePhysicalId=-1;
	throttleAxis=2;

	keyboardMode=TOWNS_KEYBOARD_MODE_DEFAULT;
	virtualKeys.resize(MAX_NUM_VIRTUALKEYS);
	for(auto &vk : virtualKeys)
	{
		vk.townsKey="";
		vk.physicalId=-1;
	}

	keyMapFName="";

	catchUpRealTime=true;

	fmVol=-1;
	pcmVol=-1;

	quickScrnShotDir="";
	hostShortCutKeys.clear();

	quickStateSaveFName="";

	pauseResumeKeyLabel="SCROLLLOCK";

	sharedDir.clear();

	RS232CtoTCPAddr="";
}
std::vector <std::string> TownsProfile::Serialize(void) const
{
	std::ostringstream sstream;
	std::vector <std::string> text;

	text.push_back("ROMDIR__ ");
	text.back().push_back('\"');
	text.back()+=ROMPath;
	text.back().push_back('\"');

	text.push_back("CMOSFILE ");
	text.back().push_back('\"');
	text.back()+=CMOSFName;
	text.back().push_back('\"');

	text.push_back("CDIMG___ ");
	text.back().push_back('\"');
	text.back()+=cdImgFName;
	text.back().push_back('\"');

	for(int i=0; i<NUM_FDDRIVES; ++i)
	{
		text.push_back("FDIMG___ ");
		text.back().push_back('0'+i);
		text.back().push_back(' ');
		text.back().push_back('0');
		text.back().push_back(' ');
		text.back().push_back('\"');
		text.back()+=fdImgFName[i];
		text.back().push_back('\"');

		text.push_back("FDWPROT_ ");
		text.back().push_back('0'+i);
		text.back().push_back(' ');
		text.back().push_back('0');
		text.back().push_back(' ');
		text.back().push_back(fdImgWriteProtect[i] ? '1' : '0');
	}
	for(int i=0; i<MAX_NUM_SCSI_DEVICES; ++i)
	{
		text.push_back("SCSIIMG_ ");
		text.back().push_back('0'+i);
		text.back().push_back(' ');
		text.back().push_back('\"');
		if(TownsProfile::SCSIIMAGE_HARDDISK==scsiImg[i].imageType)
		{
			text.back()+=scsiImg[i].imgFName;
		}
		text.back().push_back('\"');
	}
	sstream.str("");
	sstream << "FASTSCSI " << (true==fastSCSI ? 1 : 0);
	text.push_back(sstream.str());

	text.push_back("GAMEPORT 0 ");
	text.back()+=TownsGamePortEmuToStr(gamePort[0]);
	text.push_back("GAMEPORT 1 ");
	text.back()+=TownsGamePortEmuToStr(gamePort[1]);

	for(int i=0; i<2; ++i)
	{
		for(int j=0; j<2; ++j)
		{
			text.push_back("BTNHOLDT 0 0 ");
			text.back()[9]+=i;
			text.back()[11]+=j;

			sstream.str("");
			sstream << maxButtonHoldTime[i][j];
			text.back()+=sstream.str();
		}
	}

	text.push_back("KEYCOMB_ ");
	text.back()+=TownsKeyCombToStr(bootKeyComb);

	text.push_back("AUTOSTAR ");
	text.back()+=(autoStart ? "1" : "0");

	text.push_back("SEPARPRC ");
	text.back()+=(separateProcess ? "1" : "0");

	sstream.str("");
	sstream << "FREQUENC " << freq;
	text.push_back(sstream.str());

	sstream.str("");
	sstream << "USE80387 " << (true==useFPU ? "1" : "0");
	text.push_back(sstream.str());

	sstream.str("");
	sstream << "CATCHUPT " << (true==catchUpRealTime ? 1 : 0);
	text.push_back(sstream.str());

	sstream.str("");
	sstream << "MEMSIZE_ " << memSizeInMB;
	text.push_back(sstream.str());

	sstream.str("");
	sstream << "SCALING_ " << scaling;
	text.push_back(sstream.str());

	sstream.str("");
	sstream << "AUTOSCAL " << (true==autoScaling ? 1 : 0);
	text.push_back(sstream.str());

	sstream.str("");
	sstream << "MTNASPCT " << (true==maintainAspect ? 1 : 0);
	text.push_back(sstream.str());

	// sstream.str("");
	// sstream << "MAXIMIZE " << (true==maximizeOnStartUp ? 1 : 0);
	// text.push_back(sstream.str());

	switch(windowModeOnStartUp)
	{
	case WINDOW_NORMAL:
		text.push_back("WNDWMODE NORMAL");
		break;
	case WINDOW_MAXIMIZE:
		text.push_back("WNDWMODE MAXIMIZE");
		break;
	case WINDOW_FULLSCREEN:
		text.push_back("WNDWMODE FULLSCREEN");
		break;
	}

	sstream.str("");
	sstream << "APPSPEC_ " << TownsAppToStr(appSpecificSetting);
	text.push_back(sstream.str());

	sstream.str("");
	sstream << "RUNAS386 " << (true==pretend386DX ? 1 : 0);
	text.push_back(sstream.str());

	sstream.str("");
	sstream << "MOUSESPD " << mouseIntegrationSpeed;
	text.push_back(sstream.str());

	sstream.str("");
	sstream << "MOUSERNG " << mouseMinX << " " << mouseMinY << " " << mouseMaxX << " " << mouseMaxY;
	text.push_back(sstream.str());

	sstream.str("");
	sstream << "FLTMOUSE " << (mouseByFlightstickAvailable ? 1 : 0);
	text.push_back(sstream.str());

	sstream.str("");
	sstream << "FLTMOSID " << mouseByFlightstickPhysicalId;
	text.push_back(sstream.str());

	sstream.str("");
	sstream << "FLTMOSCT " << mouseByFlightstickCenterX << " " << mouseByFlightstickCenterY;
	text.push_back(sstream.str());

	sstream.str("");
	sstream << "FLTMOSSC " << mouseByFlightstickScaleX << " " << mouseByFlightstickScaleY;
	text.push_back(sstream.str());

	sstream.str("");
	sstream << "FLTMOSZZ " << 100.0f*mouseByFlightstickZeroZoneX;
	text.push_back(sstream.str());


	if(0<=throttlePhysicalId)
	{
		text.push_back("USESCTHR 1");

		sstream.str("");
		sstream << "SCTHRAXS " << throttlePhysicalId << " " << throttleAxis;
		text.push_back(sstream.str());
	}
	else
	{
		text.push_back("USESCTHR 0");
		text.push_back("SCTHRAXS -1 -1");
	}

	if(TOWNS_KEYBOARD_MODE_DEFAULT!=keyboardMode)
	{
		text.push_back("KYBDMODE ");
		text.back()+=TownsKeyboardModeToStr(keyboardMode);
	}

	sstream.str("");
	sstream << "DAMPWIRE " << (damperWireLine ? 1 : 0);
	text.push_back(sstream.str());

	sstream.str("");
	sstream << "SCANLINE15K " << (scanLineEffectIn15KHz ? 1 : 0);
	text.push_back(sstream.str());

	for(auto vk : virtualKeys)
	{
		if(""!=vk.townsKey && 0<=vk.physicalId)
		{
			sstream.str("");
			sstream << "VIRTUKEY " << vk.townsKey << " " << vk.physicalId << " " << vk.button;
			text.push_back(sstream.str());
		}
	}

	if(""!=keyMapFName)
	{
		sstream.str("");
		sstream << "KEYMAPFN " << keyMapFName;
		text.push_back(sstream.str());
	}

	if(0!=cdSpeed)
	{
		sstream.str("");
		sstream << "CDSPEEDX " << cdSpeed;
		text.push_back(sstream.str());
	}

	sstream.str("");
	sstream << "FMVOLUME " << fmVol;
	text.push_back(sstream.str());

	sstream.str("");
	sstream << "PCMVOLUM " << pcmVol;
	text.push_back(sstream.str());

	sstream.str("");
	sstream << "SNDDBLBF " << (maximumSoundDoubleBuffering ? "MAX" : "NORMAL");
	text.push_back(sstream.str());

	sstream.str("");
	sstream << "MIDICARD " << nMidiCards;
	text.push_back(sstream.str());

	if(""!=startUpStateFName)
	{
		sstream.str("");
		sstream << "LOADSTAT " << startUpStateFName;
		text.push_back(sstream.str());
	}

	text.push_back("QSSDIREC ");
	text.back().push_back('\"');
	text.back()+=quickScrnShotDir;
	text.back().push_back('\"');

	for(auto hsc : hostShortCutKeys)
	{
		text.push_back("HOSTSCUT ");
		text.back()+=hsc.hostKey;
		text.back()+=" ";
		text.back()+=(hsc.ctrl ? "1" : "0");
		text.back()+=" ";
		text.back()+=(hsc.shift ? "1" : "0");
		text.back()+=" ";
		text.back().push_back('\"');
		text.back()+=hsc.cmdStr;
		text.back().push_back('\"');
	}

	text.push_back("QSTASAVE ");
	text.back().push_back('\"');
	text.back()+=quickStateSaveFName;
	text.back().push_back('\"');

	text.push_back("PAUSEKEY ");
	text.back()+=pauseResumeKeyLabel;

	for(auto sd : sharedDir)
	{
		text.push_back("SHAREDIR ");
		text.back().push_back('\"');
		text.back()+=sd;
		text.back().push_back('\"');
	}

	if(TOWNSTYPE_UNKNOWN!=townsType)
	{
		text.push_back("TOWNSTYP ");
		text.back()+=TownsTypeToStr(townsType);
	}

	sstream.str("");
	sstream << "SCRNCROP " << scrnShotX0 << " " << scrnShotY0 << " " << scrnShotWid << " " << scrnShotHei;
	text.push_back(sstream.str());

	text.push_back("MAPLOC_X ");
	text.back().push_back('\"');
	text.back()+=mapXYExpression[0];
	text.back().push_back('\"');

	text.push_back("MAPLOC_Y ");
	text.back().push_back('\"');
	text.back()+=mapXYExpression[1];
	text.back().push_back('\"');

	text.push_back("CPUFIDEL ");
	text.back()+=i486DXCommon::FidelityLevelToStr(CPUFidelityLevel);

	text.push_back("RS232TCP ");
	text.back()+=RS232CtoTCPAddr;

	return text;
}
bool TownsProfile::Deserialize(const std::vector <std::string> &text)
{
	bool useThrottleAxis=false;

	CleanUp();
	unsigned int nVirtualKey=0;
	for(auto &cppstr : text)
	{
		YsString str(cppstr.data());
		auto argv=str.Argv();

		if(0==argv.size())
		{
			continue;
		}

		if(0==argv[0].STRCMP("ROMDIR__"))
		{
			if(2<=argv.size())
			{
				ROMPath=argv[1].c_str();
			}
		}
		else if(0==argv[0].STRCMP("CMOSFILE"))
		{
			if(2<=argv.size())
			{
				CMOSFName=argv[1].c_str();
			}
		}
		else if(0==argv[0].STRCMP("CDIMG___"))
		{
			if(2<=argv.size())
			{
				cdImgFName=argv[1].c_str();
			}
		}
		else if(0==argv[0].STRCMP("CDSPEEDX"))
		{
			if(2<=argv.size())
			{
				cdSpeed=argv[1].Atoi();
			}
		}
		else if(0==argv[0].STRCMP("FDIMG___"))
		{
			if(4<=argv.size())
			{
				int drive=argv[1].Atoi();
				int fileNum=argv[2].Atoi();
				if(0<=drive && drive<NUM_FDDRIVES && 0<=fileNum && 0==fileNum)
				{
					fdImgFName[drive]=argv[3].c_str();
				}
			}
		}
		else if(0==argv[0].STRCMP("FDWPROT_"))
		{
			if(4<=argv.size())
			{
				int drive=argv[1].Atoi();
				int fileNum=argv[2].Atoi();
				if(0<=drive && drive<2 && 0<=fileNum && 0==fileNum)
				{
					fdImgWriteProtect[drive]=(0!=argv[3].Atoi());
				}
			}
		}
		else if(0==argv[0].STRCMP("SCSIIMG_"))
		{
			if(3<=argv.size())
			{
				int scsiId=argv[1].Atoi();
				if(0<=scsiId && scsiId<MAX_NUM_SCSI_DEVICES)
				{
					scsiImg[scsiId].imgFName=argv[2].c_str();
					if(""!=scsiImg[scsiId].imgFName)
					{
						scsiImg[scsiId].imageType=TownsProfile::SCSIIMAGE_HARDDISK;
					}
					else
					{
						scsiImg[scsiId].imageType=TownsProfile::SCSIIMAGE_NONE;
					}
				}
			}
		}
		else if (0==argv[0].STRCMP("FASTSCSI"))
		{
			if (2<=argv.size())
			{
				fastSCSI=(0!=argv[1].Atoi());
			}
		}
		else if(0==argv[0].STRCMP("KEYCOMB_"))
		{
			if(2<=argv.size())
			{
				bootKeyComb=TownsStrToKeyComb(argv[1].c_str());
			}
		}
		else if(0==argv[0].STRCMP("AUTOSTAR"))
		{
			if(2<=argv.size())
			{
				autoStart=(0!=argv[1].Atoi());
			}
		}
		else if(0==argv[0].STRCMP("SEPARPRC"))
		{
			if(2<=argv.size())
			{
				separateProcess=(0!=argv[1].Atoi());
			}
		}
		else if(0==argv[0].STRCMP("GAMEPORT"))
		{
			if(3<=argv.size())
			{
				int port=argv[1].Atoi();
				if(0==port || 1==port)
				{
					gamePort[port]=TownsStrToGamePortEmu(argv[2].c_str());
				}
			}
		}
		else if(0==argv[0].STRCMP("BTNHOLDT"))
		{
			if(4<=argv.size())
			{
				int port=argv[1].Atoi()&1;
				int btn=argv[2].Atoi()&1;
				int nanosec=argv[3].Atoi();
				maxButtonHoldTime[port][btn]=nanosec;
			}
		}
		else if(0==argv[0].STRCMP("SCALING_"))
		{
			if(2<=argv.size())
			{
				scaling=argv[1].Atoi();
			}
		}
		else if(0==argv[0].STRCMP("AUTOSCAL"))
		{
			if(2<=argv.size())
			{
				autoScaling=(0!=argv[1].Atoi());
			}
		}
		else if(0==argv[0].STRCMP("MTNASPCT"))
		{
			if(2<=argv.size())
			{
				maintainAspect=(0!=argv[1].Atoi());
			}
		}
		else if(0==argv[0].STRCMP("MAXIMIZE"))
		{
			if(2<=argv.size())
			{
				windowModeOnStartUp=(0!=argv[1].Atoi() ? WINDOW_MAXIMIZE : WINDOW_NORMAL);
			}
		}
		else if(0==argv[0].STRCMP("WNDWMODE"))
		{
			if(2<=argv.size())
			{
				if(0==argv[1].STRCMP("NORMAL"))
				{
					windowModeOnStartUp=WINDOW_NORMAL;
				}
				else if(0==argv[1].STRCMP("MAXIMIZE"))
				{
					windowModeOnStartUp=WINDOW_MAXIMIZE;
				}
				else if(0==argv[1].STRCMP("FULLSCREEN"))
				{
					windowModeOnStartUp=WINDOW_FULLSCREEN;
				}
			}
		}
		else if(0==argv[0].STRCMP("FREQUENC"))
		{
			if(2<=argv.size())
			{
				freq=argv[1].Atoi();
				if(freq<1)
				{
					freq=1;
				}
			}
		}
		else if(0==argv[0].STRCMP("USE80387"))
		{
			if(2<=argv.size())
			{
				if(0!=argv[1].Atoi())
				{
					useFPU=true;
				}
				else
				{
					useFPU=false;
				}
			}
		}
		else if(0==argv[0].STRCMP("MEMSIZE_"))
		{
			if(2<=argv.size())
			{
				memSizeInMB=argv[1].Atoi();
				if(memSizeInMB<1)
				{
					memSizeInMB=1;
				}
				else if(64<memSizeInMB)
				{
					memSizeInMB=64;
				}
			}
		}
		else if(0==argv[0].STRCMP("CATCHUPT"))
		{
			if(2<=argv.size())
			{
				catchUpRealTime=(0!=argv[1].Atoi());
			}
		}
		else if(0==argv[0].STRCMP("APPSPEC_"))
		{
			if(2<=argv.size())
			{
				appSpecificSetting=TownsStrToApp(argv[1].c_str());
			}
		}
		else if(0==argv[0].STRCMP("RUNAS386"))
		{
			if(2<=argv.size())
			{
				pretend386DX=(0!=argv[1].Atoi());
			}
		}
		else if(0==argv[0].STRCMP("MOUSESPD"))
		{
			if(2<=argv.size())
			{
				mouseIntegrationSpeed=argv[1].Atoi();
			}
		}
		else if(0==argv[0].STRCMP("MOUSERNG"))
		{
			if(5<=argv.size())
			{
				mouseMinX=argv[1].Atoi();
				mouseMinY=argv[2].Atoi();
				mouseMaxX=argv[3].Atoi();
				mouseMaxY=argv[4].Atoi();
			}
		}
		else if(0==argv[0].STRCMP("FLTMOUSE"))
		{
			if(2<=argv.size())
			{
				mouseByFlightstickAvailable=(0!=argv[1].Atoi());
			}
		}
		else if(0==argv[0].STRCMP("FLTMOSID"))
		{
			if(2<=argv.size())
			{
				mouseByFlightstickPhysicalId=argv[1].Atoi();
			}
		}
		else if(0==argv[0].STRCMP("FLTMOSCT"))
		{
			if(3<=argv.size())
			{
				mouseByFlightstickCenterX=argv[1].Atoi();
				mouseByFlightstickCenterY=argv[2].Atoi();
			}
		}
		else if(0==argv[0].STRCMP("FLTMOSSC"))
		{
			if(3<=argv.size())
			{
				mouseByFlightstickScaleX=argv[1].Atof();
				mouseByFlightstickScaleY=argv[2].Atof();
			}
		}
		else if(0==argv[0].STRCMP("FLTMOSZZ"))
		{
			if(2<=argv.size())
			{
				mouseByFlightstickZeroZoneX=(float)(argv[1].Atof()/100.0);
				mouseByFlightstickZeroZoneY=(float)(argv[1].Atof()/100.0);
			}
		}
		else if(0==argv[0].STRCMP("USESCTHR"))
		{
			if(2<=argv.size())
			{
				useThrottleAxis=(0!=argv[1].Atoi());
			}
		}
		else if(0==argv[0].STRCMP("SCTHRAXS"))
		{
			if(3<=argv.size())
			{
				throttlePhysicalId=argv[1].Atoi();
				throttleAxis=argv[2].Atoi();
			}
		}
		else if(0==argv[0].STRCMP("VIRTUKEY"))
		{
			if(3<=argv.size() && nVirtualKey<MAX_NUM_VIRTUALKEYS)
			{
				virtualKeys[nVirtualKey].townsKey=argv[1];
				virtualKeys[nVirtualKey].physicalId=argv[2].Atoi();
				virtualKeys[nVirtualKey].button=argv[3].Atoi();
				++nVirtualKey;
			}
		}
		else if(0==argv[0].STRCMP("KEYMAPFN"))
		{
			if(2<=argv.size())
			{
				keyMapFName=argv[1].c_str();
			}
		}
		else if(0==argv[0].STRCMP("KYBDMODE"))
		{
			if(2<=argv.size())
			{
				keyboardMode=TownsStrToKeyboardMode(argv[1].c_str());
			}
		}
		else if(0==argv[0].STRCMP("DAMPWIRE"))
		{
			if(2<=argv.size())
			{
				damperWireLine=(0!=argv[1].Atoi());
			}
		}
		else if (0 == argv[0].STRCMP("SCANLINE15K"))
		{
			if(2<=argv.size())
			{
				scanLineEffectIn15KHz=(0!=argv[1].Atoi());
			}
		}
		else if(0==argv[0].STRCMP("FMVOLUME"))
		{
			if(2<=argv.size())
			{
				fmVol=argv[1].Atoi();
			}
		}
		else if(0==argv[0].STRCMP("PCMVOLUM"))
		{
			if(2<=argv.size())
			{
				pcmVol=argv[1].Atoi();
			}
		}
		else if(0==argv[0].STRCMP("SNDDBLBF"))
		{
			if(2<=argv.size() && 0==argv[1].STRCMP("MAX"))
			{
				maximumSoundDoubleBuffering=true;
			}
			else
			{
				maximumSoundDoubleBuffering=false;
			}
		}
		else if(0==argv[0].STRCMP("MIDICARD"))
		{
			if(2<=argv.size())
			{
				nMidiCards=argv[1].Atoi();
			}
			else
			{
				nMidiCards=0;
			}
		}
		else if(0==argv[0].STRCMP("LOADSTAT"))
		{
			if(2<=argv.size())
			{
				startUpStateFName=argv[1];
			}
		}
		else if(0==argv[0].STRCMP("QSSDIREC"))
		{
			if(2<=argv.size())
			{
				quickScrnShotDir=argv[1].c_str();
			}
		}
		else if(0==argv[0].STRCMP("HOSTSCUT"))
		{
			if(5<=argv.size())
			{
				HostShortCut hsc;
				hsc.hostKey=argv[1].c_str();
				hsc.ctrl=(0!=argv[2].Atoi());
				hsc.shift=(0!=argv[3].Atoi());
				hsc.cmdStr=argv[4].c_str();
				hostShortCutKeys.push_back(hsc);
			}
		}
		else if(0==argv[0].STRCMP("QSTASAVE"))
		{
			if(2<=argv.size())
			{
				quickStateSaveFName=argv[1].c_str();
			}
		}
		else if(0==argv[0].STRCMP("PAUSEKEY"))
		{
			if(2<=argv.size())
			{
				pauseResumeKeyLabel=argv[1].c_str();
			}
		}
		else if(0==argv[0].STRCMP("SHAREDIR"))
		{
			sharedDir.push_back(argv[1].c_str());
		}
		else if(0==argv[0].STRCMP("TOWNSTYP"))
		{
			if(2<=argv.size())
			{
				townsType=StrToTownsType(argv[1].c_str());
			}
		}
		else if(0==argv[0].STRCMP("SCRNCROP"))
		{
			if(5<=argv.size())
			{
				scrnShotX0=argv[1].Atoi();
				scrnShotY0=argv[2].Atoi();
				scrnShotWid=argv[3].Atoi();
				scrnShotHei=argv[4].Atoi();
			}
		}
		else if(0==argv[0].STRCMP("MAPLOC_X"))
		{
			if(2<=argv.size())
			{
				mapXYExpression[0]=argv[1];
			}
		}
		else if(0==argv[0].STRCMP("MAPLOC_Y"))
		{
			if(2<=argv.size())
			{
				mapXYExpression[1]=argv[1];
			}
		}
		else if(0==argv[0].STRCMP("CPUFIDEL") && 2<=argv.size())
		{
			CPUFidelityLevel=i486DXCommon::StrToFidelityLevel(argv[1].c_str());
		}
		else if(0==argv[0].STRCMP("RS232TCP"))
		{
			if(2<=argv.size())
			{
				RS232CtoTCPAddr=argv[1];
			}
			else
			{
				RS232CtoTCPAddr="";
			}
		}
		else
		{
			errorMsg="Unrecognized keyword:";
			errorMsg+=argv[0].c_str();
			return false;
		}
	}

	if(true!=useThrottleAxis)
	{
		throttlePhysicalId=-1;
	}

	errorMsg="";
	return true;
}
std::vector <std::string> TownsProfile::MakeArgv(void) const
{
	std::ostringstream sstream;
	std::vector <std::string> argv;

	argv.push_back("Tsugaru_CUI.exe");

	argv.push_back(ROMPath);
	if(0<argv.back().size() && (argv.back().back()=='/' || argv.back().back()=='\\'))
	{
		argv.back().pop_back();
	}

	if(""!=CMOSFName)
	{
		argv.push_back("-CMOS");
		argv.push_back(CMOSFName);
	}

	argv.push_back("-WINDOWSHIFT");
	argv.push_back("-FORCEQUITONPOFF");

	if(""!=cdImgFName)
	{
		argv.push_back("-CD");
		argv.push_back(cdImgFName);
	}

	if(""!=fdImgFName[0])
	{
		argv.push_back("-FD0");
		argv.push_back(fdImgFName[0]);
	}
	if(""!=fdImgFName[1])
	{
		argv.push_back("-FD1");
		argv.push_back(fdImgFName[1]);
	}

	for(int scsiId=0; scsiId<MAX_NUM_SCSI_DEVICES; ++scsiId)
	{
		if(""!=scsiImg[scsiId].imgFName)
		{
			if(TownsProfile::SCSIIMAGE_HARDDISK==scsiImg[scsiId].imageType)
			{
				argv.push_back("-HD");
				argv.back().push_back('0'+scsiId);
				argv.push_back(scsiImg[scsiId].imgFName);
			}
		}
	}

	if(true==fastSCSI)
	{
		argv.push_back("-FASTSCSI");
	}

	if(BOOT_KEYCOMB_NONE!=bootKeyComb)
	{
		argv.push_back("-BOOTKEY");
		argv.push_back(TownsKeyCombToStr(bootKeyComb));
	}

	for(int i=0; i<2; ++i)
	{
		argv.push_back("-GAMEPORT");
		argv.back().push_back('0'+i);
		argv.push_back(TownsGamePortEmuToStr(gamePort[i]));
	}

	for(int i=0; i<2; ++i)
	{
		for(int j=0; j<2; ++j)
		{
			unsigned int millisec=(maxButtonHoldTime[i][j]/1000000);
			if(0!=millisec)
			{
				argv.push_back("-BUTTONHOLDTIME0");
				argv.back().back()+=i;

				argv.push_back("0");
				argv.back().back()+=j;

				sstream.str("");
				sstream << millisec;
				argv.push_back(sstream.str());
			}
		}
	}

	if(100!=scaling)
	{
		auto screenScalingFix=scaling;
		if(screenScalingFix<30)
		{
			screenScalingFix=30;
		}
		else if(500<screenScalingFix)
		{
			screenScalingFix=400;
		}
		argv.push_back("-SCALE");
		sstream.str("");
		sstream << screenScalingFix;
		argv.push_back(sstream.str());
	}
	if(true==autoScaling)
	{
		argv.push_back("-AUTOSCALE");
	}
	if(true==maintainAspect)
	{
		argv.push_back("-MAINTAINASPECT");
	}
	else
	{
		argv.push_back("-FREEASPECT");
	}
	switch(windowModeOnStartUp)
	{
	case WINDOW_MAXIMIZE:
		argv.push_back("-MAXIMIZE");
		break;
	case WINDOW_FULLSCREEN:
		argv.push_back("-FULLSCREEN");
		break;
	}

	if(TOWNSTYPE_UNKNOWN!=townsType)
	{
		argv.push_back("-TOWNSTYPE");
		argv.push_back(TownsTypeToStr(townsType));
	}

	if(1<=freq)
	{
		argv.push_back("-FREQ");
		sstream.str("");
		sstream << freq;
		argv.push_back(sstream.str());
	}

	if(true==useFPU)
	{
		argv.push_back("-USEFPU");
	}
	else
	{
		argv.push_back("-DONTUSEFPU");
	}

	if(true!=catchUpRealTime)
	{
		argv.push_back("-NOCATCHUPREALTIME");
	}

	if(0!=memSizeInMB)
	{
		argv.push_back("-MEMSIZE");
		sstream.str("");
		sstream << (memSizeInMB);
		argv.push_back(sstream.str());
	}

	// Reminder to myself: App-Specific Augmentation must come at the very end.
	if(TOWNS_APPSPECIFIC_NONE!=appSpecificSetting)
	{
		argv.push_back("-APP");
		argv.push_back(TownsAppToStr(appSpecificSetting));
	}

	if(true==pretend386DX)
	{
		argv.push_back("-PRETEND386DX");
	}

	argv.push_back("-MOUSEINTEGSPD");
	sstream.str("");
	sstream << mouseIntegrationSpeed;
	argv.push_back(sstream.str());

	argv.push_back("-MOUSEINTEGVRAMOFFSET");
	argv.push_back(considerVRAMOffsetInMouseIntegration ? "1" : "0");


	if(true==mouseByFlightstickAvailable)
	{
		argv.push_back("-FLIGHTMOUSE");

		sstream.str("");
		sstream << mouseByFlightstickPhysicalId;
		argv.push_back(sstream.str());

		sstream.str("");
		sstream << mouseByFlightstickCenterX;
		argv.push_back(sstream.str());

		sstream.str("");
		sstream << mouseByFlightstickCenterY;
		argv.push_back(sstream.str());

		sstream.str("");
		sstream << mouseByFlightstickScaleX;
		argv.push_back(sstream.str());

		sstream.str("");
		sstream << mouseByFlightstickScaleY;
		argv.push_back(sstream.str());

		sstream.str("");
		sstream << 100.0f*mouseByFlightstickZeroZoneX;
		argv.push_back(sstream.str());
	}

	if(0<=throttlePhysicalId)
	{
		if(TOWNS_APPSPECIFIC_WINGCOMMANDER1==appSpecificSetting ||
		   TOWNS_APPSPECIFIC_WINGCOMMANDER2==appSpecificSetting)
		{
			argv.push_back("-WCTHR");
		}
		else
		{
			argv.push_back("-STCMTHR");
		}

		sstream.str("");
		sstream << throttlePhysicalId;
		argv.push_back(sstream.str());

		sstream.str("");
		sstream << throttleAxis;
		argv.push_back(sstream.str());
	}

	if(TOWNS_KEYBOARD_MODE_DEFAULT!=keyboardMode)
	{
		argv.push_back("-KEYBOARD");
		argv.push_back(TownsKeyboardModeToStr(keyboardMode));
	}

	if(true==damperWireLine)
	{
		argv.push_back("-DAMPERWIRELINE");
	}

	if(true==scanLineEffectIn15KHz)
	{
		argv.push_back("-SCANLINE15K");
	}

	for(auto vk : virtualKeys)
	{
		if(0<=vk.physicalId && ""!=vk.townsKey)
		{
			argv.push_back("-VIRTKEY");
			argv.push_back(vk.townsKey);

			sstream.str("");
			sstream << vk.physicalId;
			argv.push_back(sstream.str());

			sstream.str("");
			sstream << vk.button;
			argv.push_back(sstream.str());
		}
	}

	if(0!=cdSpeed)
	{
		argv.push_back("-CDSPEED");

		sstream.str("");
		sstream << cdSpeed;
		argv.push_back(sstream.str());
	}

	if(""!=startUpStateFName)
	{
		argv.push_back("-LOADSTATE");
		argv.push_back(startUpStateFName);
	}

	if(""!=quickScrnShotDir)
	{
		argv.push_back("-QUICKSSDIR");
		argv.push_back(quickScrnShotDir);
	}
	for(auto hsc : hostShortCutKeys)
	{
		argv.push_back("-HOSTSHORTCUT");
		argv.push_back(hsc.hostKey);
		argv.push_back(hsc.ctrl ? "1" : "0");
		argv.push_back(hsc.shift ? "1" : "0");
		argv.push_back(hsc.cmdStr);
	}

	if(""!=quickStateSaveFName)
	{
		argv.push_back("-QUICKSTATESAVE");
		argv.push_back(quickStateSaveFName);
	}

	if(""!=pauseResumeKeyLabel)
	{
		argv.push_back("-PAUSEKEY");
		argv.push_back(pauseResumeKeyLabel);
	}

	argv.push_back("-MOUSERANGE");

	sstream.str("");
	sstream << mouseMinX;
	argv.push_back(sstream.str());

	sstream.str("");
	sstream << mouseMinY;
	argv.push_back(sstream.str());

	sstream.str("");
	sstream << mouseMaxX;
	argv.push_back(sstream.str());

	sstream.str("");
	sstream << mouseMaxY;
	argv.push_back(sstream.str());

	for(auto sd : sharedDir)
	{
		argv.push_back("-SHAREDIR");
		argv.push_back(sd);
	}


	argv.push_back("-SSCROP");
	sstream.str("");
	sstream << scrnShotX0;
	argv.push_back(sstream.str());

	sstream.str("");
	sstream << scrnShotY0;
	argv.push_back(sstream.str());

	sstream.str("");
	sstream << scrnShotWid;
	argv.push_back(sstream.str());

	sstream.str("");
	sstream << scrnShotHei;
	argv.push_back(sstream.str());

	if(""!=mapXYExpression[0])
	{
		argv.push_back("-MAPX");
		argv.push_back(mapXYExpression[0]);
	}
	if(""!=mapXYExpression[1])
	{
		argv.push_back("-MAPY");
		argv.push_back(mapXYExpression[1]);
	}
	if(""!=keyMapFName)
	{
		argv.push_back("-KEYMAP");
		argv.push_back(keyMapFName);
	}

	if(i486DXCommon::HIGH_FIDELITY==CPUFidelityLevel)
	{
		argv.push_back("-HIGHFIDELITY");
	}

	if(""!=RS232CtoTCPAddr)
	{
		argv.push_back("-RSTCP");
		argv.push_back(RS232CtoTCPAddr);
	}

	if(true==maximumSoundDoubleBuffering)
	{
		argv.push_back("-MAXSNDDBLBUF");
	}

	argv.push_back("-MIDI");
	sstream.str("");
	sstream << nMidiCards;
	argv.push_back(sstream.str());

	for(auto p : specialPath)
	{
		argv.push_back("-SPECIALPATH");
		argv.push_back(p.first);
		argv.push_back(p.second);
	}

	return argv;
}

void TownsProfile::MakeRelativePath(std::string baseDir,std::string alias)
{
	MakeRelative(startUpStateFName,baseDir,alias);

	MakeRelative(ROMPath,baseDir,alias);
	MakeRelative(CMOSFName,baseDir,alias);
	for(auto &fName : fdImgFName)
	{
		MakeRelative(fName,baseDir,alias);
	}
	MakeRelative(cdImgFName,baseDir,alias);
	MakeRelative(memCardImgFName,baseDir,alias);
	MakeRelative(startUpScriptFName,baseDir,alias);
	MakeRelative(symbolFName,baseDir,alias);
	MakeRelative(playbackEventLogFName,baseDir,alias);
	MakeRelative(keyMapFName,baseDir,alias);

	for(auto &fName : sharedDir)
	{
		MakeRelative(fName,baseDir,alias);
	}
	for(auto &fName : fdSearchPaths)
	{
		MakeRelative(fName,baseDir,alias);
	}
	for(auto &fName : cdSearchPaths)
	{
		MakeRelative(fName,baseDir,alias);
	}

	for(auto &s : scsiImg)
	{
		MakeRelative(s.imgFName,baseDir,alias);
	}

	for(auto &s : toSend)
	{
		MakeRelative(s.hostFName,baseDir,alias);
	}

	MakeRelative(quickScrnShotDir,baseDir,alias);
	MakeRelative(quickStateSaveFName,baseDir,alias);
}

void TownsProfile::MakeRelative(std::string &fName,std::string baseDir,std::string alias)
{
	auto src=fName;
	for(auto &c : src)
	{
		if('\\'==c)
		{
			c='/';
		}
	}
	for(auto &c : baseDir)
	{
		if('\\'==c)
		{
			c='/';
		}
	}

	if(0<baseDir.size() && '/'==baseDir.back())
	{
		baseDir.pop_back();
	}

	// Special case baseDir==src
	if(baseDir==src || (baseDir+"/")==src)
	{
		fName=alias;
		return;
	}

	// (1) src is longer or equal to than baseDir.
	// (2) baseDir matches the first part of src, and
	// (3) subsequent char is '/'.

	// (1)
	if(src.size()<=baseDir.size())
	{
		return;
	}

	// (2)
	auto chopOff=src;
	chopOff.resize(baseDir.size());
	if(baseDir!=chopOff)
	{
		return;
	}

	// (3)
	if(src[baseDir.size()]!='/')
	{
		return;
	}

	if(0<=alias.size() && alias.back()=='/')
	{
		alias.pop_back();
	}

	alias.insert(alias.end(),src.begin()+baseDir.size(),src.end());
	std::swap(fName,alias);
}
