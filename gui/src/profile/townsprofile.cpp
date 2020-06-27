#include <ysclass.h>

#include "townsprofile.h"



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
	for(auto &f : SCSIImgFile)
	{
		f="";
	}
	bootKeyComb=BOOT_KEYCOMB_NONE;
	autoStart=false;
}
std::vector <std::string> TownsProfile::Serialize(void) const
{
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
	std::vector <std::string> argv;

	argv.push_back("Tsugaru_CUI.exe");

	argv.push_back(ROMDir);

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

	return argv;
}
