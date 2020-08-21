/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include <iostream>
#include <fstream>

#include "townsargv.h"
#include "cpputil.h"
#include "diskimg.h"
#include "d77.h"



TownsStartParameters::TownsStartParameters()
{
	gamePort[0]=TOWNS_GAMEPORTEMU_PHYSICAL0;
	gamePort[1]=TOWNS_GAMEPORTEMU_MOUSE;
}



////////////////////////////////////////////////////////////



TownsARGV::TownsARGV()
{
}
void TownsARGV::PrintHelp(void) const
{
	std::cout << "Usage:" << std::endl;
	std::cout << "-HELP,-H,-?" << std::endl;
	std::cout << "  Print Help." << std::endl;
	std::cout << "-SCALE X" << std::endl;
	std::cout << "  Screen scaling X percent." << std::endl;
	std::cout << "-PAUSE" << std::endl;
	std::cout << "  Machine state is PAUSE on start up." << std::endl;
	std::cout << "-SHAREDDIR path" << std::endl;
	std::cout << "  Directory visible in the VM via VNDRV." << std::endl;
	std::cout << "-DEBUG,-DEBUGGER" << std::endl;
	std::cout << "  Start the machine with debugger enabled." << std::endl;
	std::cout << "-UNITTEST" << std::endl;
	std::cout << "  Let it run automatically to the end without taking control commands." << std::endl;
	std::cout << "-FREQ frequency_in_MHz" << std::endl;
	std::cout << "  Specify CPU frequency in Megahertz." << std::endl;
	std::cout << "-MEMSIZE memory_size_in_MB" << std::endl;
	std::cout << "  Specify main RAM size in mega bytes.  Max 64." << std::endl;
	std::cout << "-NOWAIT" << std::endl;
	std::cout << "  VM never waits for real time when VM time runs ahead of the real time." << std::endl;
	std::cout << "-YESWAIT" << std::endl;
	std::cout << "  VM always waits for real time when VM time runs ahead of the real time." << std::endl;
	std::cout << "-NOWAITBOOT" << std::endl;
	std::cout << "  No wait during the memory test, and then VM waits for real time when" << std::endl;
	std::cout << "  VM time runs ahead of the real time." << std::endl;
	std::cout << "-NOCATCHUPREALTIME" << std::endl;
	std::cout << "  By default, when VM lags behind the real time, the VM timer is fast-forwarded to catch" << std::endl;
	std::cout << "  up with the real time.  This will play YM2612 BGMs roughly correct timing." << std::endl;
	std::cout << "  However, it may break time-sensitive applications.  Like ChaseHQ flickers." << std::endl;
	std::cout << "  To prevent such break down, this option will prevent VM timer from fast-forwarded" << std::endl;
	std::cout << "  when VM lags.  In return, the execution may become slower." << std::endl;
	std::cout << "-CMOS filename" << std::endl;
	std::cout << "  Specify CMOS (BIOS settings) file.  If you specify this option," << std::endl;
	std::cout << "  CMOS settings will be saved automatically when closing the program." << std::endl;
	std::cout << "-KEYMAP filename" << std::endl;
	std::cout << "  Load Key-Mapping file." << std::endl;
	std::cout << "-DONTAUTOSAVECMOS" << std::endl;
	std::cout << "  This option will prevent VM from saving CMOS file on exit." << std::endl;
	std::cout << "-FD0 filename" << std::endl;
	std::cout << "  Floppy disk image file name for Drive A." << std::endl;
	std::cout << "-FD1 filename" << std::endl;
	std::cout << "  Floppy disk image file name for Drive B." << std::endl;
	std::cout << "-FD0WP,-FD1WP" << std::endl;
	std::cout << "  Write protect floppy disk." << std::endl;
	std::cout << "-FD0UP,-FD1UP" << std::endl;
	std::cout << "  Write un-protect floppy disk." << std::endl;
	std::cout << "-CD image-file-name" << std::endl;
	std::cout << "  CD-ROM image file name for the internal drive. ISO or CUE." << std::endl;
	std::cout << "-GAMEPORT0 KEY|PHYSx|ANAx|NONE" << std::endl;
	std::cout << "-GAMEPORT1 KEY|PHYSx|ANAx|NONE" << std::endl;
	std::cout << "  Specify game-port emulation.  By keyboard (Arrow,Z,X,A,S), or physical gamepad." << std::endl;
	std::cout << "  PHYS0,PHYS1,PHYS2,PHYS3 use physical game pad direction button (or hat switch) as up/down/left/right." << std::endl;
	std::cout << "  ANA0,ANA1,ANA2,ANA3 use physical game pad analog stick as up/down/left/right." << std::endl;
	std::cout << "  KEYMOUSE use arrow keys and ZX keys for mouse cursor and buttons." << std::endl;
	std::cout << "  NUMPADMOUSE use NUMPAD number keys and /* keys for mouse cursor and buttons." << std::endl;
	std::cout << "  PHYS0MOUSE,PHYS1MOUSE,PHYS2MOUSE,PHYS3MOUSE use physical game pad digital axis for mouse." << std::endl;
	std::cout << "  ANA0MOUSE,ANA1MOUSE,ANA2MOUSE,ANA3MOUSE use physical game pad analog axis for mouse." << std::endl;
	std::cout << "-HD0 image-file-name" << std::endl;
	std::cout << "  Hard-disk image file name.  Can be -HDx (0<=x<=6)" << std::endl;
	std::cout << "-SCSICD0 image-file-name" << std::endl;
	std::cout << "  SCSI CD-ROM image file name.  Can be -SCSICDx (0<=x<=6)" << std::endl;
	std::cout << "-SYM filename" << std::endl;
	std::cout << "  Specify symbol file name." << std::endl;
	std::cout << "-EVTLOG filename" << std::endl;
	std::cout << "  Load and play-back event log." << std::endl;
	std::cout << "-HOST2VM hostFileName vmFileName" << std::endl;
	std::cout << "  Schedule Host to VM file transfer." << std::endl;
	std::cout << "  File will be transferred when FTCLIENT.EXP is running." << std::endl;
	std::cout << "-PRETEND386DX" << std::endl;
	std::cout << "  With this option, VM will report CPU as 386DX from I/O port 0030H." << std::endl;
	std::cout << "  DOS-Extender that comes with Towns OS V2.1L10 checks the CPU and" << std::endl;
	std::cout << "  blocks if the CPU is not 386DX nor 386SX.  That is probably why" << std::endl;
	std::cout << "  we see many software titles running on Towns OS V2.1L10B, but" << std::endl;
	std::cout << "  not with V2.1L10.  This option is to start Towns OS V2.1L10-based" << std::endl;
	std::cout << "  titles such as Free Software Collection 4 Disc B" << std::endl;
	std::cout << "-MOUSEINTEGSPD speed" << std::endl;
	std::cout << "  Set mouse-integration speed.  If mouse jumps around, you can try setting lower value." << std::endl;
	std::cout << "  Minimum is 32, and maximum is 256.  Default is 256 (fastest)." << std::endl;
	std::cout << "-APP title" << std::endl;
	std::cout << "  Use application-specific customization." << std::endl;
	std::cout << "  For the list of applications, start this program with" << std::endl;
	std::cout << "   only -APP option." << std::endl;
	std::cout << "-GENFD filename.bin size_in_KB" << std::endl;
	std::cout << "  Create a new floppy image.  size_in_KB must be 1232, 1440, 640, or 720." << std::endl;
	std::cout << "-GENHD filename.bin size_in_MB" << std::endl;
	std::cout << "  Create a new harddisk image." << std::endl;
	std::cout << "-FMVOL volume" << std::endl;
	std::cout << "-PCMVOL volume" << std::endl;
	std::cout << "  Specify FM/PCM volume.  Volume will be rounded to 0 to 8192." << std::endl;
}

void TownsARGV::PrintApplicationList(void) const
{
	std::cout << "Customization Available for:" << std::endl;
	std::cout << "WC1 or WINGCOMMANDER1" << std::endl;
	std::cout << "  Mouse integration.  Wing Commander 1 uses its own mouse-reading" << std::endl;
	std::cout << "  function instead of standard Towns MOUSE BIOS.  With this option" << std::endl;
	std::cout << "  you can control mouse for selecting menus." << std::endl;
	std::cout << "SUPERDAISEN" << std::endl;
	std::cout << "  Super Daisenryaku for FM TOWNS is, I believe, the best port among" << std::endl;
	std::cout << "  the port for all other platforms.  However, especially at higher" << std::endl;
	std::cout << "  frequency, it has an usability issue, which is the map scrolls" << std::endl;
	std::cout << "  too fast while holding the mouse button down." << std::endl;
	std::cout << "  To counter this issue, the VM slows down to 4MHz while the mouse" << std::endl;
	std::cout << "  left button is down." << std::endl;
	std::cout << "LEMMINGS" << std::endl;
	std::cout << "  Mouse integration." << std::endl;
}

bool TownsARGV::AnalyzeCommandParameter(int argc,char *argv[])
{
	for(int i=1; i<argc; ++i)
	{
		std::string ARG=argv[i];
		cpputil::Capitalize(ARG);
		if("-HELP"==ARG || "-H"==ARG || "-?"==ARG)
		{
			PrintHelp();
		}
		else if("-APP"==ARG)
		{
			if(i+1<argc)
			{
				std::string APP=argv[i+1];
				cpputil::Capitalize(APP);
				appSpecificSetting=TownsStrToApp(APP);
				if(TOWNS_APPSPECIFIC_STRIKECOMMANDER==appSpecificSetting && memSizeInMB<8)
				{
					memSizeInMB=8;
				}
				++i;
			}
			else
			{
				PrintApplicationList();
			}
		}
		else if("-SCALE"==ARG && i+1<argc)
		{
			scaling=atoi(argv[i+1]);
			if(scaling<SCALING_MIN)
			{
				scaling=SCALING_MIN;
			}
			else if(SCALING_MAX<scaling)
			{
				scaling=SCALING_MAX;
			}
			++i;
		}
		else if("-PAUSE"==ARG)
		{
			autoStart=false;
		}
		else if("-SHAREDDIR"==ARG && i+1<argc)
		{
			sharedDir.push_back(argv[i+1]);
			++i;
		}
		else if("-MOUSEINTEGSPD"==ARG && i+1<argc)
		{
			mouseIntegrationSpeed=cpputil::Atoi(argv[i+1]);
			if(mouseIntegrationSpeed<32)
			{
				mouseIntegrationSpeed=32;
			}
			else if(256<mouseIntegrationSpeed)
			{
				mouseIntegrationSpeed=256;
			}
			++i;
		}
		else if("-DEBUG"==ARG || "-DEBUGGER"==ARG)
		{
			debugger=true;
		}
		else if("-UNITTEST"==ARG)
		{
			interactive=false;
		}
		else if("-POWEROFFAT"==ARG && i+1<argc)
		{
			powerOffAtBreakPoint=true;
			powerOffAt=argv[i+1];
			++i;
		}
		else if("-FREQ"==ARG && i+1<argc)
		{
			freq=cpputil::Atoi(argv[i+1]);
			++i;
		}
		else if("-MEMSIZE"==ARG && i+1<argc)
		{
			memSizeInMB=cpputil::Atoi(argv[i+1]);
			++i;
		}
		else if("-NOWAIT"==ARG)
		{
			noWait=true;
			noWaitStandby=true;
		}
		else if("-YESWAIT"==ARG)
		{
			noWait=false;
			noWaitStandby=false;
		}
		else if("-NOWAITBOOT"==ARG)
		{
			noWait=true;
			noWaitStandby=false;
		}
		else if("-NOCATCHUPREALTIME"==ARG)
		{
			catchUpRealTime=false;
		}
		else if(("-GAMEPORT0"==ARG || "-GAMEPORT1"==ARG) && i+1<argc)
		{
			int portId=(ARG[9]-'0')&1;
			std::string DEV=argv[i+1];
			cpputil::Capitalize(DEV);
			gamePort[portId]=TownsStrToGamePortEmu(DEV);
			++i;
		}
		else if("-FD0"==ARG && i+1<argc)
		{
			fdImgFName[0]=argv[i+1];
			++i;
		}
		else if("-FD1"==ARG && i+1<argc)
		{
			fdImgFName[1]=argv[i+1];
			++i;
		}
		else if("-FD0WP"==ARG)
		{
			fdImgWriteProtect[0]=true;
		}
		else if("-FD1WP"==ARG)
		{
			fdImgWriteProtect[1]=true;
		}
		else if("-FD0UP"==ARG)
		{
			fdImgWriteProtect[0]=false;
		}
		else if("-FD1UP"==ARG)
		{
			fdImgWriteProtect[1]=false;
		}
		else if("-CD"==ARG && i+1<argc)
		{
			cdImgFName=argv[i+1];
			++i;
		}
		else if("-CMOS"==ARG && i+1<argc)
		{
			CMOSFName=argv[i+1];
			++i;
		}
		else if("-KEYMAP"==ARG && i+1<argc)
		{
			keyMapFName=argv[i+1];
			++i;
		}
		else if("-DONTAUTOSAVECMOS"==ARG)
		{
			autoSaveCMOS=false;
		}
		else if(("-HD0"==ARG ||
		         "-HD1"==ARG ||
		         "-HD2"==ARG ||
		         "-HD3"==ARG ||
		         "-HD4"==ARG ||
		         "-HD5"==ARG ||
		         "-HD6"==ARG) &&
		         i+1<argc)
		{
			SCSIImage scsi;
			scsi.scsiID=ARG[3]-'0';
			scsi.imageType=SCSIIMAGE_HARDDISK;
			scsi.imgFName=argv[i+1];
			scsiImg.push_back(scsi);
			++i;
		}
		else if(("-SCSICD0"==ARG ||
		         "-SCSICD1"==ARG ||
		         "-SCSICD2"==ARG ||
		         "-SCSICD3"==ARG ||
		         "-SCSICD4"==ARG ||
		         "-SCSICD5"==ARG ||
		         "-SCSICD6"==ARG) &&
		         i+1<argc)
		{
			SCSIImage scsi;
			scsi.scsiID=ARG[7]-'0';
			scsi.imageType=SCSIIMAGE_CDROM;
			scsi.imgFName=argv[i+1];
			scsiImg.push_back(scsi);
			++i;
		}
		else if("-BOOTKEY"==ARG && i+1<argc)
		{
			std::string COMB=argv[i+1];
			cpputil::Capitalize(COMB);

			if("NONE"==COMB)
			{
				bootKeyComb=BOOT_KEYCOMB_NONE;
			}
			else if("CD"==COMB)
			{
				bootKeyComb=BOOT_KEYCOMB_CD;
			}
			else if("F0"==COMB)
			{
				bootKeyComb=BOOT_KEYCOMB_F0;
			}
			else if("F1"==COMB)
			{
				bootKeyComb=BOOT_KEYCOMB_F1;
			}
			else if("F2"==COMB)
			{
				bootKeyComb=BOOT_KEYCOMB_F2;
			}
			else if("F3"==COMB)
			{
				bootKeyComb=BOOT_KEYCOMB_F3;
			}
			else if("H0"==COMB)
			{
				bootKeyComb=BOOT_KEYCOMB_H0;
			}
			else if("H1"==COMB)
			{
				bootKeyComb=BOOT_KEYCOMB_H1;
			}
			else if("H2"==COMB)
			{
				bootKeyComb=BOOT_KEYCOMB_H2;
			}
			else if("H3"==COMB)
			{
				bootKeyComb=BOOT_KEYCOMB_H3;
			}
			else if("H4"==COMB)
			{
				bootKeyComb=BOOT_KEYCOMB_H4;
			}
			else if("ICM"==COMB)
			{
				bootKeyComb=BOOT_KEYCOMB_ICM;
			}
			else if("DEBUG"==COMB)
			{
				bootKeyComb=BOOT_KEYCOMB_DEBUG;
			}
			else if("PADA"==COMB)
			{
				bootKeyComb=BOOT_KEYCOMB_PAD_A;
			}
			else if("PADB"==COMB)
			{
				bootKeyComb=BOOT_KEYCOMB_PAD_B;
			}
			else
			{
				std::cout << "Undefined boot-key combination:" << argv[i+1] << std::endl;
				std::cout << "Must be one of: NONE,CD,F0,F1,F2,F3,H0,H1,H2,H3,H4,ICM,DEBUG,PADA,PADB." << std::endl;
				return false;
			}
			++i;
		}
		else if("-SYM"==ARG && i+1<argc)
		{
			symbolFName=argv[i+1];
			++i;
		}
		else if("-EVTLOG"==ARG && i+1<argc)
		{
			playbackEventLogFName=argv[i+1];
			++i;
		}
		else if("-HOST2VM"==ARG && i+2<argc)
		{
			if(true==cpputil::FileExists(argv[i+1]))
			{
				FileToSend f;
				f.hostFName=argv[i+1];
				f.vmFName=argv[i+2];
				toSend.push_back(f);
			}
			else
			{
				std::cout << "File Not Found:" << argv[i+1] << std::endl;
				return false;
			}
			i+=2;
		}
		else if("-PRETEND386DX"==ARG)
		{
			pretend386DX=true;
		}
		else if("-GENFD"==ARG && i+2<argc)
		{
			std::string fName=argv[i+1];
			unsigned int KB=cpputil::Atoi(argv[i+2]);
			if(1232==KB || 1440==KB || 720==KB || 640==KB)
			{
				std::vector <unsigned char> img;

				switch(KB)
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

				auto ext=cpputil::GetExtension(fName.c_str());
				auto EXT=cpputil::Capitalize(ext);
				if(".D77"==EXT)
				{
					std::cout << "Making D77 disk image." << std::endl;
					D77File d77;
					d77.SetRawBinary(img);
					img=d77.MakeD77Image();
				}

				if(true!=cpputil::WriteBinaryFile(fName,img.size(),img.data()))
				{
					std::cout << "Failed to write file: " << fName << std::endl;
					return false;
				}
				else
				{
					std::cout << "Created FD Image: " << fName << std::endl;
				}
			}
			else
			{
				std::cout << "Unsupported floppy-disk size: " << KB << std::endl;
				std::cout << "Must be 1232, 1440, 720, or 640" << std::endl;
				return false;
			}
			i+=2;
		}
		else if("-GENHD"==ARG && i+2<argc)
		{
			std::string fName=argv[i+1];
			unsigned int MB=cpputil::Atoi(argv[i+2]);
			std::vector <unsigned char> zero;
			zero.resize(1024*1024);
			for(auto &z : zero)
			{
				z=0;
			}
			std::ofstream fp(fName,std::ofstream::binary);
			if(true==fp.is_open())
			{
				while(0!=MB)
				{
					fp.write((char *)zero.data(),zero.size());
					--MB;
				}
				fp.close();
				std::cout << "Created HD Image: " << fName << std::endl;
		}
			else
			{
				std::cout << "Failed to write file: " << fName << std::endl;
				return false;
			}
			i+=2;
		}
		else if("-WINDOWSHIFT"==ARG)
		{
			windowShift=true;
		}
		else if("-FMVOL"==ARG && 1+i<argc)
		{
			int vol=cpputil::Atoi(argv[i+1]);
			if(vol<0)
			{
				vol=0;
			}
			if(8192<vol)
			{
				vol=8192;
			}
			fmVol=vol;
			++i;
		}
		else if("-PCMVOL"==ARG && 1+i<argc)
		{
			int vol=cpputil::Atoi(argv[i+1]);
			if(vol<0)
			{
				vol=0;
			}
			if(8192<vol)
			{
				vol=8192;
			}
			pcmVol=vol;
			++i;
		}
		else
		{
			if(1==i)
			{
				ROMPath=argv[i];
			}
			else
			{
				std::cout << "Undefined Option or Insufficient Parameters:" << argv[i] << std::endl;
				return false;
			}
		}
	}
	return true;
}
