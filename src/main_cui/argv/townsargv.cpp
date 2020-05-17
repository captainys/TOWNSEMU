/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include <iostream>

#include "townsargv.h"
#include "cpputil.h"



TownsARGV::TownsARGV()
{
	autoStart=true;
	debugger=false;
	interactive=true;
}
void TownsARGV::PrintHelp(void) const
{
	std::cout << "Usage:" << std::endl;
	std::cout << "-HELP,-H,-?" << std::endl;
	std::cout << "  Print Help." << std::endl;
	std::cout << "-PAUSE" << std::endl;
	std::cout << "  Machine state is PAUSE on start up." << std::endl;
	std::cout << "-DEBUG,-DEBUGGER" << std::endl;
	std::cout << "  Start the machine with debugger enabled." << std::endl;
	std::cout << "-UNITTEST" << std::endl;
	std::cout << "  Let it run automatically to the end without taking control commands." << std::endl;
	std::cout << "-FREQ frequency_in_MHz" << std::endl;
	std::cout << "  Specify CPU frequency in Megahertz." << std::endl;
	std::cout << "-FD0 filename" << std::endl;
	std::cout << "  Floppy disk image file name for Drive A." << std::endl;
	std::cout << "-FD1 filename" << std::endl;
	std::cout << "  Floppy disk image file name for Drive B." << std::endl;
	std::cout << "-CD image-file-name" << std::endl;
	std::cout << "  CD-ROM image file name for the internal drive. ISO or CUE." << std::endl;
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
		else if("-PAUSE"==ARG)
		{
			autoStart=false;
		}
		else if("-DEBUG"==ARG || "-DEBUGGER"==ARG)
		{
			debugger=true;
		}
		else if("-UNITTEST"==ARG)
		{
			interactive=false;
		}
		else if("-FREQ"==ARG && i+1<argc)
		{
			freq=cpputil::Atoi(argv[i+1]);
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
