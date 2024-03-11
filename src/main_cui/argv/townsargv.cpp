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
#include <stdlib.h>

#include "townsargv.h"
#include "cpputil.h"
#include "diskimg.h"
#include "d77.h"



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
	std::cout << "-AUTOSCALE" << std::endl;
	std::cout << "  Auto scaleing screen to match the window size." << std::endl;
	std::cout << "-MAXIMIZE" << std::endl;
	std::cout << "  Maximize the window on start up" << std::endl;
	std::cout << "-FULLSCREEN" << std::endl;
	std::cout << "  Fullscreen on start up" << std::endl;
	std::cout << "-HIGHRES" << std::endl;
	std::cout << "  Enable High-Resolution CRTC (default)" << std::endl;
	std::cout << "-NOHIGHRES" << std::endl;
	std::cout << "  Disable High-Resolution CRTC" << std::endl;
	std::cout << "-HIGHRESPCM" << std::endl;
	std::cout << "  Enable High-Resolution PCM" << std::endl;
	std::cout << "-NOHIGHRESPCM" << std::endl;
	std::cout << "  Disable High-Resolution PCM" << std::endl;
	std::cout << "-HIGHFIDELITY" << std::endl;
	std::cout << "-HIGHFIDELITYCPU" << std::endl;
	std::cout << "  Use high-fidelity CPU core.  Needed to run Windows 3.1." << std::endl;
	std::cout << "  High-fidelity CPU core does more exception handling than" << std::endl;
	std::cout << "  the default CPU core for running Windows 3.1, however," << std::endl;
	std::cout << "  it runs slower." << std::endl;
	std::cout << "  The machine-state file saved from the default-fidelity CPU core" << std::endl;
	std::cout << "  may crash if loaded to the high-fidelity CPU core, or vise-versa." << std::endl;
	std::cout << "-LOWRES" << std::endl;
	std::cout << "  Disable High Resolution CRTC" << std::endl;
	std::cout << "-PAUSE" << std::endl;
	std::cout << "  Machine state is PAUSE on start up." << std::endl;
	std::cout << "-SHAREDDIR path" << std::endl;
	std::cout << "-SHAREDIR path" << std::endl;
	std::cout << "  Directory visible in the VM via VNDRV." << std::endl;
	std::cout << "-DEBUG,-DEBUGGER" << std::endl;
	std::cout << "  Start the machine with debugger enabled." << std::endl;
	std::cout << "-COPYFILE src dst" << std::endl;
	std::cout << "  Copy a file before starting the VM." << std::endl;
	std::cout << "  Used for auto testing." << std::endl;
	std::cout << "-UNITTEST" << std::endl;
	std::cout << "  Let it run automatically to the end without taking control commands." << std::endl;
	std::cout << "-FREQ frequency_in_MHz" << std::endl;
	std::cout << "  Specify CPU frequency in Megahertz." << std::endl;
	std::cout << "-SLOWMODEFREQ frequency_in_MHz" << std::endl;
	std::cout << "  Specify CPU frequency when set to SLOW mode in Megahertz." << std::endl;
	std::cout << "  Some FM TOWNS models (confirmed 2F) sets machine in SLOW mode" << std::endl;
	std::cout << "  in the boot ROM." << std::endl;
	std::cout << "-USEFPU / -DONTUSEFPU" << std::endl;
	std::cout << "  Use or do not use floating-point unit." << std::endl;
	std::cout << "-MEMSIZE memory_size_in_MB" << std::endl;
	std::cout << "  Specify main RAM size in mega bytes.  Max 64." << std::endl;
	std::cout << "-NOWAIT" << std::endl;
	std::cout << "  VM never waits for real time when VM time runs ahead of the real time." << std::endl;
	std::cout << "-YESWAIT" << std::endl;
	std::cout << "  VM always waits for real time when VM time runs ahead of the real time." << std::endl;
	std::cout << "-NOWAITBOOT" << std::endl;
	std::cout << "  No wait during the memory test, and then VM waits for real time when" << std::endl;
	std::cout << "  VM time runs ahead of the real time." << std::endl;
	std::cout << "-FASTSCSI" << std::endl;
	std::cout << "-NORMALSCSI" << std::endl;
	std::cout << "-FASTFD" << std::endl;
	std::cout << "-NORMALFD" << std::endl;
	std::cout << "  Control speed of SCSI and FDC.  Some program may not run with FASTSCSI or FASTFD" << std::endl;
	std::cout << "-NOCATCHUPREALTIME" << std::endl;
	std::cout << "  By default, when VM lags behind the real time, the VM timer is fast-forwarded to catch" << std::endl;
	std::cout << "  up with the real time.  This will play YM2612 BGMs roughly correct timing." << std::endl;
	std::cout << "  However, it may break time-sensitive applications.  Like ChaseHQ flickers." << std::endl;
	std::cout << "  To prevent such break down, this option will prevent VM timer from fast-forwarded" << std::endl;
	std::cout << "  when VM lags.  In return, the execution may become slower." << std::endl;
	std::cout << "-ALIAS aliasLabel filename" << std::endl;
	std::cout << "  Define file-name alias.  Alias can later be used as a parameter to FDxLOAD, TAPELOAD commands." << std::endl;
	std::cout << "  eg. You can use -ALIAS DISKA \"full-path-to-game-diskA\" to ease disk swap from command." << std::endl;
	std::cout << "-CMOS filename" << std::endl;
	std::cout << "  Specify CMOS (BIOS settings) file.  If you specify this option," << std::endl;
	std::cout << "  CMOS settings will be saved automatically when closing the program." << std::endl;
	std::cout << "-KEYBOARD mode" << std::endl;
	std::cout << "  Select keyboard mode.  mode can be one of:" << std::endl;
	std::cout << "    DIRECT  <- Direct mode." << std::endl;
	std::cout << "    TRANS1  <- Translation mode 1.  Physical ESC will make virtual ESC and BREAK." << std::endl;
	std::cout << "    TRANS2  <- Translation mode 2.  Physical ESC will make virtual ESC." << std::endl;
	std::cout << "    TRANS3  <- Translation mode 3.  Physical ESC will make virtual BREAK." << std::endl;
	std::cout << "-KEYMAP filename" << std::endl;
	std::cout << "  Load Key-Mapping file." << std::endl;
	std::cout << "-DONTAUTOSAVECMOS" << std::endl;
	std::cout << "  This option will prevent VM from saving CMOS file on exit." << std::endl;
	std::cout << "-ZEROCMOS" << std::endl;
	std::cout << "-RESETCMOS" << std::endl;
	std::cout << "  Zero-clear CMOS on boot." << std::endl;
	std::cout << "-FDPATH path" << std::endl;
	std::cout << "  Floppy-Disk Image Search Path." << std::endl;
	std::cout << "  Repeat -FDPATH path1 -FDPATH path2 ... to specify multipl paths." << std::endl;
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
	std::cout << "-CDPATH path" << std::endl;
	std::cout << "  CD-ROM Image Search Path." << std::endl;
	std::cout << "  Repeat -CDPATH path1 -CDPATH path2 ... to specify multipl paths." << std::endl;
	std::cout << "-CDSPEED N" << std::endl;
	std::cout << "  Set CD speed.  CD speed will be set to Nx." << std::endl;
	std::cout << "-ICM image-file-name" << std::endl;
	std::cout << "  IC Memory Card image file name." << std::endl;
	std::cout << "-JEIDA4 image-file-name" << std::endl;
	std::cout << "  IC Memory Card image file name.  Opened as JEIDA4 (PCMCIA)." << std::endl;
	std::cout << "-ICMWP" << std::endl;
	std::cout << "-JEIDA4WP" << std::endl;
	std::cout << "  Write protect memory card." << std::endl;
	std::cout << "-ICMUP" << std::endl;
	std::cout << "-JEIDA4UP" << std::endl;
	std::cout << "  Write un-protect memory card." << std::endl;
	std::cout << "-GAMEPORT0 KEY|PHYSx|ANAx|NONE" << std::endl;
	std::cout << "-GAMEPORT1 KEY|PHYSx|ANAx|NONE" << std::endl;
	std::cout << "  Specify game-port emulation.  By keyboard (Arrow,Z,X,A,S), or physical gamepad." << std::endl;
	std::cout << "  PHYS0,PHYS1,PHYS2,PHYS3 use physical game pad direction button (or hat switch) as up/down/left/right." << std::endl;
	std::cout << "  PHYS0CAPCOM,PHYS1CAPCOM,PHYS2CAPCOM,PHYS3CAPCOM use physcal game pad as CAPCOM CPSF Joystick." << std::endl;
	std::cout << "  ANA0,ANA1,ANA2,ANA3 use physical game pad analog stick as up/down/left/right." << std::endl;
	std::cout << "  KEYMOUSE use arrow keys and ZX keys for mouse cursor and buttons." << std::endl;
	std::cout << "  NUMPADMOUSE use NUMPAD number keys and /* keys for mouse cursor and buttons." << std::endl;
	std::cout << "  PHYS0MOUSE,PHYS1MOUSE,PHYS2MOUSE,PHYS3MOUSE use physical game pad digital axis for mouse." << std::endl;
	std::cout << "  ANA0MOUSE,ANA1MOUSE,ANA2MOUSE,ANA3MOUSE use physical game pad analog axis for mouse." << std::endl;
	std::cout << "-BUTTONHOLDTIME0 0|1 time_in_millisec" << std::endl;
	std::cout << "-BUTTONHOLDTIME1 0|1 time_in_millisec" << std::endl;
	std::cout << "  In some games, when you click on a menu or a button, you end up selecting the next menu" << std::endl;
	std::cout << "  or the button that happens to appear in the next options.  Daikoukai Jidai (KOEI)" << std::endl;
	std::cout << "  is virtually unplayable.  Super Daisenryaku keeps scrolling to the end when you want" << std::endl;
	std::cout << "  to scroll only by one screen width.  You really had to press the button and release" << std::endl;
	std::cout << "  immediately.  How patient we were!" << std::endl;
	std::cout << "  It is due to the bad programming.  When the program reads button state as DOWN, it must" << std::endl;
	std::cout << "  wait until the state changes to UP before making a next selection.  But, we cannot go" << std::endl;
	std::cout << "  in to all of the programs and write patches.  Instead, this option let you limit" << std::endl;
	std::cout << "  maximum time that the button is sent as DOWN to the virtual machine even when" << std::endl;
	std::cout << "  you keep it down for 100 seconds." << std::endl;
	std::cout << "-FLIGHTMOUSE physIdx cx cy wid hei zeroZoneInPercent" << std::endl;
	std::cout << "  Wing Commander and Strike Commander series let you control your fighter by mouse." << std::endl;
	std::cout << "  This option virtually move mouse coordinate by physical joystick so that you can" << std::endl;
	std::cout << "  control your fighter easier." << std::endl;
	std::cout << "  Recommended (assuming your joystick is joystick #0):" << std::endl;
	std::cout << "    Wing Commander 2   -FLIGHTMOUSE 0 320 135 400 300 3" << std::endl;
	std::cout << "    Strike Commander   -FLIGHTMOUSE 0 320 200 400 300 3" << std::endl;
	std::cout << "  (Joystick ID, button ID starts with 0)" << std::endl;
	std::cout << "-VIRTKEY townsKey gamePadPhysicalId button" << std::endl;
	std::cout << "  Assign a virtual key to a gamepad button." << std::endl;
	std::cout << "-FLIGHTSTICK physIdx zeroZoneInPercent" << std::endl;
	std::cout << "  Joystick assignment to be used with CYBERSTICK." << std::endl;
	std::cout << "  Difference from -FLIGHTMOUSE is the setting is applied to the CYBERSTICK." << std::endl;
	std::cout << "  Has no effect if CYBERSTICK is not connected to the game port." << std::endl;
	std::cout << "-FLIGHTTHR physicalId axis" << std::endl;
	std::cout << "  Assign a joystick analog axis for throttle integration." << std::endl;
	std::cout << "  Throttle integration is available for:" << std::endl;
	std::cout << "    Strike Commander, Strike Commander Plus, Wing Commander, Wing Commander 2," << std::endl;
	std::cout << "    Fujitsu Air Warrior V2.1 L10" << std::endl;
	std::cout << "  Must be used together with -APP options." << std::endl;
	std::cout << "  Cycle throttle all the way forward and backward when start a mission to enable this feature." << std::endl;
	std::cout << "-STCMTHR physicalId axis" << std::endl;
	std::cout << "  Same as -FLIGHTTHR" << std::endl;
	std::cout << "-WCTHR  physicalId axis" << std::endl;
	std::cout << "  Same as -FLIGHTTHR" << std::endl;
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
	std::cout << "-MOUSEINTEGVRAMOFFSET 1/0" << std::endl;
	std::cout << "  1 for consider VRAM offset, or 0 for do not consider VRAM offset in mouse integration." << std::endl;
	std::cout << "-MOUSERANGE minX minY maxX maxY" << std::endl;
	std::cout << "  Specify the range of the mouse cursor location in 1X scale." << std::endl;
	std::cout <<"   Default is 0 0 1023 767." << std::endl;
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
	std::cout << "-MAXSNDDBLBUF" << std::endl;
	std::cout << "  Try this option if the sound is choppy or hear static noise." << std::endl;
	std::cout << "-DAMPERWIRELINE" << std::endl;
	std::cout << "  Render damper-wire line to make you feel nostalgic." << std::endl;
	std::cout << "-TOWNSTYPE" << std::endl;
	std::cout << "  Specify FM TOWNS type.  Can be one of:" << std::endl;
	std::cout << "    MODEL2 2F 20F UX CX UG HG HR UR MA MX ME MF HC" << std::endl;
	std::cout << "  It affects the return value from I/O 0030H." << std::endl;
	std::cout << "  Also selection of UX and UG (386SX models) will affect memory mappings." << std::endl;
	std::cout << "-FORCEQUITONPOFF" << std::endl;
	std::cout << "  Use exit(0) to exit the program on power off." << std::endl;
	std::cout << "-LOADSTATE stateFName.TState" << std::endl;
	std::cout << "  Load specified state file on start-up." << std::endl;

	std::cout << "-QUICKSSDIR dir" << std::endl;
	std::cout << "  Specify quick screen shot directory." << std::endl;
	std::cout << "-SSCROP x0 y0 wid hei" << std::endl;
	std::cout << "  Crop screen shot." << std::endl;
	std::cout << "-HOSTSHORTCUT hostKey ctrl shift \"command\"" << std::endl;
	std::cout << "  Assign host short cut key (hot key)." << std::endl;

	std::cout << "-QUICKSTATESAVE filename" << std::endl;
	std::cout << "  Specify quick state save/load file name." << std::endl;

	std::cout << "-INITCMD cmd" << std::endl;
	std::cout << "  Add commands that are executed at the start of the VM." << std::endl;

	std::cout << "-PAUSEKEY keyLabel" << std::endl;
	std::cout << "  Specify a hot key for pause/resume.  Default is SCROLLLOCK" << std::endl;
	std::cout << "-RSTCP IPAddress:port" << std::endl;
	std::cout << "  Forward RS232C (COM0) to TCP/IP." << std::endl;
	std::cout << "  !!CAUTION!! TRANSMISSION IS NOT ENCRYPTED!!!!\n" << std::endl;
	std::cout << "  !!CAUTION!! DO NOT SEND SENSITIVE AND/OR PERSONAL INFORMATION OVER TCP/IP FORWARDING!!!!\n" << std::endl;
	std::cout << "  !!CAUTION!! I WILL NOT TAKE RESPONSIBILITY FOR ANY DAMAGE CAUSED BY COMPROMISED INFORMATION!!!!" << std::endl;
}

void TownsARGV::PrintApplicationList(void) const
{
	std::cout << "Customization Available for:" << std::endl;
	std::cout << "WC1 or WINGCOMMANDER1" << std::endl;
	std::cout << "  Mouse integration.  Wing Commander 1 uses its own mouse-reading" << std::endl;
	std::cout << "  function instead of standard Towns MOUSE BIOS.  With this option" << std::endl;
	std::cout << "  you can control mouse for selecting menus.." << std::endl;
	std::cout << "WC2 or WINGCOMMANDER2" << std::endl;
	std::cout << "  Mouse integration." << std::endl;
	std::cout << "STRIKECOMMANDER" << std::endl;
	std::cout << "  Mouse integration." << std::endl;
	std::cout << "SUPERDAISEN" << std::endl;
	std::cout << "  Super Daisenryaku for FM TOWNS is, I believe, the best port among" << std::endl;
	std::cout << "  the port for all other platforms.  However, especially at higher" << std::endl;
	std::cout << "  frequency, it has an usability issue, which is the map scrolls" << std::endl;
	std::cout << "  too fast while holding the mouse button down." << std::endl;
	std::cout << "  To counter this issue, the VM slows down to 4MHz while the mouse" << std::endl;
	std::cout << "  left button is down." << std::endl;
	std::cout << "LEMMINGS" << std::endl;
	std::cout << "  Mouse integration." << std::endl;
	std::cout << "LEMMINGS2" << std::endl;
	std::cout << "  Mouse integration." << std::endl;
	std::cout << "AMARANTH3" << std::endl;
	std::cout << "  Mouse integration." << std::endl;
	std::cout << "ULTIMAUNDERWORLD" << std::endl;
	std::cout << "  Mouse integration." << std::endl;
}

void TownsARGV::CopyFile(std::string src,std::string dst)
{
	std::ifstream ifp(src,std::ios::binary);
	if(ifp.is_open())
	{
		ifp.seekg(0,ifp.end);
		auto fSize=ifp.tellg();
		ifp.seekg(0,ifp.beg);

		std::vector <char> buf;
		buf.resize(fSize);
		ifp.read(buf.data(),buf.size());
		ifp.close();

		std::ofstream ofp(dst,std::ios::binary);
		if(ofp.is_open())
		{
			ofp.write(buf.data(),buf.size());
			ofp.close();
			return;
		}
	}
	std::cout << "Failed to copy a file." << std::endl;
	exit(1);
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
			exit(1);
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
		else if("-AUTOSCALE"==ARG)
		{
			autoScaling=true;
		}
		else if("-MAXIMIZE"==ARG)
		{
			windowModeOnStartUp=WINDOW_MAXIMIZE;
		}
		else if("-FULLSCREEN"==ARG)
		{
			windowModeOnStartUp=WINDOW_FULLSCREEN;
		}
		else if("-PAUSE"==ARG)
		{
			autoStart=false;
		}
		else if(("-SHAREDDIR"==ARG || "-SHAREDIR"==ARG) && i+1<argc)
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
		else if("-MOUSEINTEGVRAMOFFSET"==ARG && i+1<argc)
		{
			considerVRAMOffsetInMouseIntegration=(0!=cpputil::Atoi(argv[i+1]));
			++i;
		}
		else if("-MOUSERANGE"==ARG && i+4<argc)
		{
			mouseMinX=cpputil::Atoi(argv[i+1]);
			mouseMinY=cpputil::Atoi(argv[i+2]);
			mouseMaxX=cpputil::Atoi(argv[i+3]);
			mouseMaxY=cpputil::Atoi(argv[i+4]);
			i+=4;
		}
		else if("-DEBUG"==ARG || "-DEBUGGER"==ARG)
		{
			debugger=true;
		}
		else if("-COPYFILE"==ARG && i+2<argc)
		{
			CopyFile(argv[i+1],argv[i+2]);
			i+=2;
		}
		else if("-UNITTEST"==ARG)
		{
			interactive=false;
		}
		else if("-POWEROFFAT"==ARG && i+1<argc)
		{
			powerOffAtBreakPoint=true;
			powerOffAt=argv[i+1];
			powerOffAtPassCount=0; // Tentative
			for(size_t j=0; j<powerOffAt.size(); ++j)
			{
				if('@'==powerOffAt[j])
				{
					powerOffAtPassCount=cpputil::Atoi(powerOffAt.c_str()+j+1);
					powerOffAt.resize(j);
					break;
				}
			}
			++i;
		}
		else if("-FREQ"==ARG && i+1<argc)
		{
			freq=cpputil::Atoi(argv[i+1]);
			++i;
		}
		else if("-SLOWMODEFREQ"==ARG && i+1<argc)
		{
			slowModeFreq=cpputil::Atoi(argv[i+1]);
			++i;
		}
		else if("-USEFPU"==ARG)
		{
			useFPU=true;
		}
		else if("-DONTUSEFPU"==ARG)
		{
			useFPU=false;
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
		else if("-FASTSCSI"==ARG)
		{
			fastSCSI=true;
		}
		else if("-NORMALSCSI"==ARG || "-SLOWSCSI"==ARG)
		{
			fastSCSI=false;
		}
		else if("-FASTFD"==ARG)
		{
			fastFD=true;
		}
		else if("-NORMALFD"==ARG || "-SLOWFD"==ARG)
		{
			fastFD=false;
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
			int portId=(ARG.back()-'0')&1;
			std::string DEV=argv[i+1];
			cpputil::Capitalize(DEV);
			gamePort[portId]=TownsStrToGamePortEmu(DEV);
			if(TOWNS_GAMEPORTEMU_ERROR==gamePort[portId])
			{
				std::cout << "Undefined Game Port Emulation Type." << argv[i+1] << std::endl;
				return false;
			}
			++i;
		}
		else if(("-BUTTONHOLDTIME0"==ARG || "-BUTTONHOLDTIME1"==ARG) && i+2<argc)
		{
			int portId=(ARG.back()-'0')&1;
			int button=cpputil::Atoi(argv[i+1])&1;
			long long int holdTime=cpputil::Atoi(argv[i+2]);
			maxButtonHoldTime[portId][button]=holdTime*1000000; // Make it nano sec.
			i+=2;
		}
		else if("-FLIGHTMOUSE"==ARG && i+6<argc)
		{
			mouseByFlightstickAvailable=true;
			cyberStickAssignment=false;
			mouseByFlightstickPhysicalId=cpputil::Atoi(argv[i+1]);
			mouseByFlightstickCenterX=cpputil::Atoi(argv[i+2]);
			mouseByFlightstickCenterY=cpputil::Atoi(argv[i+3]);
			mouseByFlightstickScaleX=atof(argv[i+4]);
			mouseByFlightstickScaleY=atof(argv[i+5]);
			mouseByFlightstickZeroZoneX=atof(argv[i+6])/100.0F;
			mouseByFlightstickZeroZoneY=mouseByFlightstickZeroZoneX;
			i+=6;
		}
		else if("-FLIGHTSTICK"==ARG && i+2<argc)
		{
			mouseByFlightstickAvailable=false;
			cyberStickAssignment=true;
			mouseByFlightstickPhysicalId=cpputil::Atoi(argv[i+1]);
			mouseByFlightstickZeroZoneX=atof(argv[i+2])/100.0F;
			mouseByFlightstickZeroZoneY=mouseByFlightstickZeroZoneX;
			i+=2;
		}
		else if("-VIRTKEY"==ARG && i+3<argc)
		{
			VirtualKey vk;
			vk.townsKey=argv[i+1];
			vk.physicalId=cpputil::Atoi(argv[i+2]);
			vk.button=cpputil::Atoi(argv[i+3]);
			virtualKeys.push_back(vk);
			i+=3;
		}
		else if("-FLIGHTTHR"==ARG && i+2<argc)
		{
			throttlePhysicalId=cpputil::Atoi(argv[i+1]);
			throttleAxis=cpputil::Atoi(argv[i+2]);
			i+=2;
		}
		else if("-STCMTHR"==ARG && i+2<argc)
		{
			throttlePhysicalId=cpputil::Atoi(argv[i+1]);
			throttleAxis=cpputil::Atoi(argv[i+2]);
			i+=2;
		}
		else if("-WCTHR"==ARG && i+2<argc)
		{
			throttlePhysicalId=cpputil::Atoi(argv[i+1]);
			throttleAxis=cpputil::Atoi(argv[i+2]);
			i+=2;
		}
		else if("-FDPATH"==ARG && i+1<argc)
		{
			fdSearchPaths.push_back(argv[i+1]);
			++i;
		}
		else if(("-FD0"==ARG || "-FD1"==ARG || "-FD2"==ARG || "-FD3"==ARG) && i+1<argc)
		{
			int drv=ARG[3]-'0';
			fdImgFName[drv]=argv[i+1];
			++i;
		}
		else if("-FD0WP"==ARG || "-FD1WP"==ARG || "-FD2WP"==ARG || "-FD3WP"==ARG)
		{
			int drv=ARG[3]-'0';
			fdImgWriteProtect[drv]=true;
		}
		else if("-FD0UP"==ARG || "-FD1UP"==ARG || "-FD2UP"==ARG || "-FD3UP"==ARG)
		{
			int drv=ARG[3]-'0';
			fdImgWriteProtect[drv]=false;
		}
		else if("-ALIAS"==ARG && i+2<argc)
		{
			auto LABEL=argv[i+1];
			cpputil::Capitalize(LABEL);
			fileNameAlias[LABEL]=argv[i+2];
			i+=2;
		}
		else if("-CDPATH"==ARG && i+1<argc)
		{
			cdSearchPaths.push_back(argv[i+1]);
			++i;
		}
		else if("-CDSPEED"==ARG && i+1<argc)
		{
			cdSpeed=cpputil::Atoi(argv[i+1]);
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
		else if("-KEYBOARD"==ARG && i+1<argc)
		{
			std::string MODE=argv[i+1];
			cpputil::Capitalize(MODE);
			if("DIRECT"==MODE)
			{
				keyboardMode=TOWNS_KEYBOARD_MODE_DIRECT;
			}
			else if("TRANS"==MODE || "TRANSLATION"==MODE || "TRANS1"==MODE || "TRANSLATION1"==MODE)
			{
				keyboardMode=TOWNS_KEYBOARD_MODE_TRANSLATION1;
			}
			else if("TRANS2"==MODE || "TRANSLATION2"==MODE)
			{
				keyboardMode=TOWNS_KEYBOARD_MODE_TRANSLATION2;
			}
			else if("TRANS3"==MODE || "TRANSLATION3"==MODE)
			{
				keyboardMode=TOWNS_KEYBOARD_MODE_TRANSLATION3;
			}
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
		else if("-ZEROCMOS"==ARG || "-RESETCMOS"==ARG)
		{
			zeroCMOS=true;
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
			auto scsiID=ARG[3]-'0';
			scsiImg[scsiID].imageType=SCSIIMAGE_HARDDISK;
			scsiImg[scsiID].imgFName=argv[i+1];
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
			auto scsiID=ARG[7]-'0';
			scsiImg[scsiID].imageType=SCSIIMAGE_CDROM;
			scsiImg[scsiID].imgFName=argv[i+1];
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
			else if("FAST"==COMB || "T"==COMB)
			{
				bootKeyComb=BOOT_KEYCOMB_FASTMODE;
			}
			else if("SLOW"==COMB || "N"==COMB)
			{
				bootKeyComb=BOOT_KEYCOMB_SLOWMODE;
			}
			else
			{
				std::cout << "Undefined boot-key combination:" << argv[i+1] << std::endl;
				std::cout << "Must be one of: NONE,CD,F0,F1,F2,F3,H0,H1,H2,H3,H4,ICM,DEBUG,PADA,PADB,T,N." << std::endl;
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
		else if("-MAXSNDDBLBUF"==ARG)
		{
			maximumSoundDoubleBuffering=true;
		}
		else if("-ICM"==ARG && i+1<argc)
		{
			memCardType=TOWNS_MEMCARD_TYPE_OLD;
			memCardImgFName=argv[i+1];
			++i;
		}
		else if("-JEIDA4"==ARG && i+1<argc)
		{
			memCardType=TOWNS_MEMCARD_TYPE_JEIDA4;
			memCardImgFName=argv[i+1];
			++i;
		}
		else if("-ICMWP"==ARG || "-JEIDA4WP"==ARG)
		{
			memCardWriteProtected=true;
		}
		else if("-ICMUP"==ARG || "-JEIDA4UP"==ARG)
		{
			memCardWriteProtected=false;
		}
		else if("-HIGHRESPCM"==ARG)
		{
			highResPCM=true;
		}
		else if("-NOHIGHRESPCM"==ARG)
		{
			highResPCM=false;
		}
		else if("-HIGHRES"==ARG)
		{
			highResAvailable=true;
		}
		else if("-NOHIGHRES"==ARG)
		{
			highResAvailable=false;
		}
		else if("-HIGHFIDELITY"==ARG || "-HIGHFIDELITYCPU"==ARG)
		{
			CPUFidelityLevel=i486DXCommon::HIGH_FIDELITY;
		}
		else if("-LOWRES"==ARG)
		{
			highResAvailable=false;
		}
		else if("-DAMPERWIRELINE"==ARG)
		{
			damperWireLine=true;
		}
		else if("-SCANLINE15K"==ARG)
		{
			scanLineEffectIn15KHz=true;
		}
		else if("-TOWNSTYPE"==ARG && i+1<argc)
		{
			townsType=StrToTownsType(argv[i+1]);
			++i;
		}
		else if("-FORCEQUITONPOFF"==ARG)
		{
			forceQuitOnPowerOff=true;
		}
		else if("-LOADSTATE"==ARG && i+1<argc)
		{
			startUpStateFName=argv[i+1];
			++i;
		}
		else if("-QUICKSSDIR"==ARG && i+1<argc)
		{
			quickScrnShotDir=argv[i+1];
			++i;
		}
		else if("-SSCROP"==ARG && i+4<argc)
		{
			scrnShotX0=cpputil::Atoi(argv[i+1]);
			scrnShotY0=cpputil::Atoi(argv[i+2]);
			scrnShotWid=cpputil::Atoi(argv[i+3]);
			scrnShotHei=cpputil::Atoi(argv[i+4]);
			i+=4;
		}
		else if("-MAPX"==ARG && i+1<argc)
		{
			mapXYExpression[0]=argv[i+1];
			++i;
		}
		else if("-MAPY"==ARG && i+1<argc)
		{
			mapXYExpression[1]=argv[i+1];
			++i;
		}
		else if("-HOSTSHORTCUT"==ARG && i+4<argc)
		{
			HostShortCut hsc;
			hsc.hostKey=argv[i+1];
			hsc.ctrl=(0!=cpputil::Atoi(argv[i+2]));
			hsc.shift=(0!=cpputil::Atoi(argv[i+3]));
			hsc.cmdStr=argv[i+4];
			hostShortCutKeys.push_back(hsc);
			i+=4;
		}
		else if("-QUICKSTATESAVE"==ARG && i+1<argc)
		{
			quickStateSaveFName=argv[i+1];
			++i;
		}
		else if("-PAUSEKEY"==ARG && i+1<argc)
		{
			pauseResumeKeyLabel=argv[i+1];
			++i;
		}
		else if("-RSTCP"==ARG && i+1<argc)
		{
			RS232CtoTCPAddr=argv[i+1];
			++i;
		}
		else if("-INITCMD"==ARG && i+1<argc)
		{
			initCmd.push_back(argv[i+1]);
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
