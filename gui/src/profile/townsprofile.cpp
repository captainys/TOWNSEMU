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

	catchUpRealTime=true;

	fmVol=-1;
	pcmVol=-1;

	quickScrnShotDir="";
	hostShortCutKeys.clear();

	quickStateSaveFName="";

	pauseResumeKeyLabel="SCROLLLOCK";
}
std::vector <std::string> TownsProfile::Serialize(void) const
{
	std::ostringstream sstream;
	std::vector <std::string> text;

	text.push_back("ROMDIR__ ");
	text.back().push_back('\"');
	text.back()+=ROMPath;
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
	sstream << "MAXIMIZE " << (true==maximizeOnStartUp ? 1 : 0);
	text.push_back(sstream.str());

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

	for(auto vk : virtualKeys)
	{
		if(""!=vk.townsKey && 0<=vk.physicalId)
		{
			sstream.str("");
			sstream << "VIRTUKEY " << vk.townsKey << " " << vk.physicalId << " " << vk.button;
			text.push_back(sstream.str());
		}
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
		else if(0==argv[0].STRCMP("MAXIMIZE"))
		{
			if(2<=argv.size())
			{
				maximizeOnStartUp=(0!=argv[1].Atoi());
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
	if(true==maximizeOnStartUp)
	{
		argv.push_back("-MAXIMIZE");
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

	return argv;
}
