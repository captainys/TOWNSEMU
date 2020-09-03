/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#ifndef TOWNSARGV_IS_INCLUDED
#define TOWNSARGV_IS_INCLUDED
/* { */

#include <string>
#include <vector>
#include "townsdef.h"


class TownsStartParameters
{
public:
	unsigned int gamePort[2];
	unsigned int bootKeyComb=BOOT_KEYCOMB_NONE;

	bool windowShift=false;

	bool noWait=true;
	bool noWaitStandby=false;

	bool catchUpRealTime=true;

	std::string ROMPath;
	std::string CMOSFName;
	bool autoSaveCMOS=true; // If this flag is false, CMOSFName will not be copied to FMTowns::Variable.
	std::string fdImgFName[2];
	bool fdImgWriteProtect[2]={false,false};
	std::string cdImgFName;
	std::string startUpScriptFName;
	std::string symbolFName;
	std::string playbackEventLogFName;
	std::string keyMapFName;

	std::vector <std::string> sharedDir;

	int fmVol=-1,pcmVol=-1;

	bool mouseByFlightstickAvailable=false;
	int mouseByFlightstickPhysicalId=-1;  // Physical joystick ID.
	int mouseByFlightstickRecalibrateButton=-1;
	int mouseByFlightstickCenterX=320,mouseByFlightstickCenterY=200;
	float mouseByFlightstickScaleX=320,mouseByFlightstickScaleY=200;

	enum
	{
		SCSIIMAGE_HARDDISK,
		SCSIIMAGE_CDROM
	};
	class SCSIImage
	{
	public:
		unsigned int scsiID;
		unsigned imageType;
		std::string imgFName;
	};
	std::vector <SCSIImage> scsiImg;

	class FileToSend
	{
	public:
		std::string hostFName,vmFName;
	};
	std::vector <FileToSend> toSend;

	bool autoStart=true;
	bool debugger=false;
	bool interactive=true;

	bool powerOffAtBreakPoint=false;
	std::string powerOffAt;

	bool pretend386DX=false;

	unsigned int memSizeInMB=0;

	unsigned int freq=0;

	unsigned int mouseIntegrationSpeed=256;

	unsigned int appSpecificSetting=0;

	enum
	{
		SCALING_DEFAULT=100,
		SCALING_MIN=50,
		SCALING_MAX=1000,
	};
	unsigned int scaling=100;

	TownsStartParameters();
};

class TownsARGV : public TownsStartParameters
{
public:
	TownsARGV();
	void PrintHelp(void) const;
	void PrintApplicationList(void) const;
	bool AnalyzeCommandParameter(int argc,char *argv[]);
};


/* } */
#endif
