#include <string>
#include <sstream>
#include <ysclass.h>

#include "townsprofile.h"



TownsProfile::TownsProfile()
{
	CleanUp();
}
void TownsProfile::CleanUp(void)
{
	ROMDir="";
	CDImgFile="";
	for(auto &f : FDImgFile[0])
	{
		f="";
	}
	for(auto &f : FDImgFile[1])
	{
		f="";
	}
	for(auto &wp : FDWriteProtect[0])
	{
		wp=false;
	}
	for(auto &wp : FDWriteProtect[1])
	{
		wp=false;
	}
	for(auto &f : SCSIImgFile)
	{
		f="";
	}
	bootKeyComb=BOOT_KEYCOMB_NONE;
	autoStart=false;
	screenScaling=150;
	freq=25;
	appSpecificAugmentation=TOWNS_APPSPECIFIC_NONE;
}
std::vector <std::string> TownsProfile::Serialize(void) const
{
	std::ostringstream sstream;
	std::vector <std::string> text;

	text.push_back("ROMDIR__ ");
	text.back().push_back('\"');
	text.back()+=ROMDir;
	text.back().push_back('\"');

	text.push_back("CDIMG___ ");
	text.back().push_back('\"');
	text.back()+=CDImgFile;
	text.back().push_back('\"');

	for(int i=0; i<2; ++i)
	{
		for(int j=0; j<NUM_STANDBY_FDIMG; ++j)
		{
			text.push_back("FDIMG___ ");
			text.back().push_back('0'+i);
			text.back().push_back(' ');
			text.back().push_back('0'+j);
			text.back().push_back(' ');
			text.back().push_back('\"');
			text.back()+=FDImgFile[i][j];
			text.back().push_back('\"');

			text.push_back("FDWPROT_ ");
			text.back().push_back('0'+i);
			text.back().push_back(' ');
			text.back().push_back('0'+j);
			text.back().push_back(' ');
			text.back().push_back(FDWriteProtect[i][j] ? '1' : '0');
		}
	}
	for(int i=0; i<MAX_NUM_SCSI_DEVICE; ++i)
	{
		text.push_back("SCSIIMG_ ");
		text.back().push_back('0'+i);
		text.back().push_back(' ');
		text.back().push_back('\"');
		text.back()+=SCSIImgFile[i];
		text.back().push_back('\"');
	}

	text.push_back("GAMEPORT 0 ");
	text.back()+=TownsGamePortEmuToStr(gamePort[0]);
	text.push_back("GAMEPORT 1 ");
	text.back()+=TownsGamePortEmuToStr(gamePort[1]);

	text.push_back("KEYCOMB_ ");
	text.back()+=TownsKeyCombToStr(bootKeyComb);

	text.push_back("AUTOSTAR ");
	text.back()+=(autoStart ? "1" : "0");

	sstream.str("");
	sstream << "FREQUENC " << freq;
	text.push_back(sstream.str());

	sstream.str("");
	sstream << "CATCHUPT " << (true==catchUpRealTime ? 1 : 0);
	text.push_back(sstream.str());

	sstream.str("");
	sstream << "MEMSIZE_ " << memSizeInMB;
	text.push_back(sstream.str());

	sstream.str("");
	sstream << "SCALING_ " << screenScaling;
	text.push_back(sstream.str());

	sstream.str("");
	sstream << "APPSPEC_ " << TownsAppToStr(appSpecificAugmentation);
	text.push_back(sstream.str());

	sstream.str("");
	sstream << "RUNAS386 " << (true==pretend386DX ? 1 : 0);
	text.push_back(sstream.str());

	sstream.str("");
	sstream << "MOUSESPD " << mouseIntegrationSpeed;
	text.push_back(sstream.str());

	return text;
}
bool TownsProfile::Deserialize(const std::vector <std::string> &text)
{
	CleanUp();
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
				ROMDir=argv[1].c_str();
			}
		}
		else if(0==argv[0].STRCMP("CDIMG___"))
		{
			if(2<=argv.size())
			{
				CDImgFile=argv[1].c_str();
			}
		}
		else if(0==argv[0].STRCMP("FDIMG___"))
		{
			if(4<=argv.size())
			{
				int drive=argv[1].Atoi();
				int fileNum=argv[2].Atoi();
				if(0<=drive && drive<2 && 0<=fileNum && fileNum<NUM_STANDBY_FDIMG)
				{
					FDImgFile[drive][fileNum]=argv[3].c_str();
				}
			}
		}
		else if(0==argv[0].STRCMP("FDWPROT_"))
		{
			if(4<=argv.size())
			{
				int drive=argv[1].Atoi();
				int fileNum=argv[2].Atoi();
				if(0<=drive && drive<2 && 0<=fileNum && fileNum<NUM_STANDBY_FDIMG)
				{
					FDWriteProtect[drive][fileNum]=(0!=argv[3].Atoi());
				}
			}
		}
		else if(0==argv[0].STRCMP("SCSIIMG_"))
		{
			if(3<=argv.size())
			{
				int scsiId=argv[1].Atoi();
				if(0<=scsiId && scsiId<MAX_NUM_SCSI_DEVICE)
				{
					SCSIImgFile[scsiId]=argv[2].c_str();
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
		else if(0==argv[0].STRCMP("SCALING_"))
		{
			if(2<=argv.size())
			{
				screenScaling=argv[1].Atoi();
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
				appSpecificAugmentation=TownsStrToApp(argv[1].c_str());
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
		else
		{
			errorMsg="Unrecognized keyword:";
			errorMsg+=argv[0].c_str();
			return false;
		}
	}
	errorMsg="";
	return true;
}
std::vector <std::string> TownsProfile::MakeArgv(void) const
{
	std::ostringstream sstream;
	std::vector <std::string> argv;

	argv.push_back("Tsugaru_CUI.exe");

	argv.push_back(ROMDir);
	if(0<argv.back().size() && (argv.back().back()=='/' || argv.back().back()=='\\'))
	{
		argv.back().pop_back();
	}

	argv.push_back("-WINDOWSHIFT");

	if(""!=CDImgFile)
	{
		argv.push_back("-CD");
		argv.push_back(CDImgFile);
	}

	if(""!=FDImgFile[0][0])
	{
		argv.push_back("-FD0");
		argv.push_back(FDImgFile[0][0]);
	}
	if(""!=FDImgFile[1][0])
	{
		argv.push_back("-FD1");
		argv.push_back(FDImgFile[1][0]);
	}

	for(int scsiId=0; scsiId<MAX_NUM_SCSI_DEVICE; ++scsiId)
	{
		if(""!=SCSIImgFile[scsiId])
		{
			argv.push_back("-HD");
			argv.back().push_back('0'+scsiId);
			argv.push_back(SCSIImgFile[scsiId]);
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

	if(100!=screenScaling)
	{
		auto screenScalingFix=screenScaling;
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

	if(1<=freq)
	{
		argv.push_back("-FREQ");
		sstream.str("");
		sstream << freq;
		argv.push_back(sstream.str());
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
	if(TOWNS_APPSPECIFIC_NONE!=appSpecificAugmentation)
	{
		argv.push_back("-APP");
		argv.push_back(TownsAppToStr(appSpecificAugmentation));
	}

	if(true==pretend386DX)
	{
		argv.push_back("-PRETEND386DX");
	}

	argv.push_back("-MOUSEINTEGSPD");
	sstream.str("");
	sstream << mouseIntegrationSpeed;
	argv.push_back(sstream.str());

	return argv;
}
