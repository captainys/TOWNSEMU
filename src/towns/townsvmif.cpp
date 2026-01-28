/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include <iostream>

#include "cpputil.h"
#include "towns.h"
#include "townsdef.h"
#include "townsmap.h"
#include "render.h"
#include "outside_world.h"



void FMTownsCommon::ProcessVMToHostCommand(unsigned int vmCmd,unsigned int paramLen,const unsigned char param[])
{
	auto &cpu=CPU();
	switch(vmCmd)
	{
	case TOWNS_VMIF_CMD_CAPTURE_CRTC:
		std::cout << "\n";
		std::cout << "0x" << cpputil::Ubtox(param[0]) << ",0x" << cpputil::Ubtox(param[1]) << "," << std::endl;
		{
			const int nReg=sizeof(crtc.state.crtcReg)/sizeof(crtc.state.crtcReg[0]);
			for(int i=0; i<nReg; ++i)
			{
				std::cout << "0x" << cpputil::Ustox(crtc.state.crtcReg[i]) << ",";
				if(i%16==15 || nReg-1==i)
				{
					std::cout << std::endl;
				}
			}
			for(int i=0; i<4; ++i)
			{
				std::cout << "0x" << cpputil::Ubtox(crtc.state.sifter[i]) << ",";
			}
			std::cout << "\n";
		}
		break;
	case TOWNS_VMIF_CMD_PAUSE:
		debugger.ExternalBreak("Break Request from VM");
		break;
	case TOWNS_VMIF_CMD_EXIT_VM:
		var.powerOff=true;
		if(0<paramLen)
		{
			// In Windows 95, apparently the testing thread is called while TGMOUSE was
			// trying to notify the mouse coordinate.
			// To avoid fake failure from the test, it should take the last parameter pushed, instead of param[0].
			// However, there is a possibility that TGMOUSE is invoked after the testing
			// pushed the parameter, before EXIT_VM is written to the I/O.
			// The problem is only for pre-emptive multi-tasking operating systems, and
			// this change will make it exremely unlikely.  So, I leave it this way for the time being.
			var.returnCode=param[paramLen-1];
		}
		else
		{
			var.returnCode=-1;
		}
		break;
	case TOWNS_VMIF_CMD_FILE_RXRDY:
	case TOWNS_VMIF_CMD_FILE_TXRDY:
		VMHostFileTransfer();
		break;
	case TOWNS_VMIF_CMD_NOTIFY_DOSSEG:
		std::cout << "DOSSEG=" << cpputil::Ustox(cpu.state.CS().value) << std::endl;
		state.DOSSEG=cpu.state.CS().value;
		break;
	case TOWNS_VMIF_CMD_NOTIFY_DOSVER: // Capture DOS Version.  Use it immediately after MOV AH,30H  INT 21H  MOV BX,AX
		std::cout << "DOSVER=" << cpu.GetBL() << "." << cpu.GetBH() << std::endl;
		state.DOSVER=cpu.GetBX();
		break;
	case TOWNS_VMIF_CMD_NOTIFY_DOSLOL: // Capture DOS List of Lists, not DOS Laugh Out Loud.
		std::cout << "DOS List Of Lists=" << cpputil::Ustox(cpu.state.ES().value) << ":" << cpputil::Ustox(cpu.GetBX()) << std::endl;
		state.DOSSEG=cpu.state.ES().value;
		state.DOSLOLSEG=cpu.state.ES().value;
		state.DOSLOLOFF=cpu.GetBX();
		break;
	case TOWNS_VMIF_CMD_NOTIFY_MOUSE:
		if(4<=paramLen)
		{
			var.mousePositionReported=true;
			var.mouseXReported=cpputil::GetWord(param);
			var.mouseYReported=cpputil::GetWord(param+2);
		}
		break;
	case TOWNS_VMIF_CMD_BYTE_VM_TO_HOST:
		for(int i=0; i<paramLen; ++i)
		{
			var.vmToHost.push_back(param[i]);
		}
		break;

	case TOWNS_VMIF_CMD_CAPTURE_FM_FNUM:
		std::cout << "\n";
		std::cout << "\t0x" << cpputil::Ustox(sound.state.ym2612.state.channels[0].F_NUM) << ",";
		std::cout << "0x" << cpputil::Ustox(sound.state.ym2612.state.channels[0].BLOCK) << ",\n";
		break;
	case TOWNS_VMIF_CMD_CAPTURE_FM_TL:
		std::cout << "\n";
		std::cout << "\t0x" << cpputil::Ubtox(sound.state.ym2612.state.channels[0].slots[3].TL) << ",\n";
		break;

	case TOWNS_VMIF_CMD_GET_VMFLAGS:
		cpu.state.EAX()=var.VMFlags;
		break;
	case TOWNS_VMIF_CMD_RELEASE_CONSOLE_CMD:
		if(0<var.consoleCmd.size())
		{
			keyboard.SetAutoType(var.consoleCmd[0]);
			var.consoleCmd.erase(var.consoleCmd.begin());
		}
		break;
	}
}
void FMTownsCommon::VMHostFileTransfer(void)
{
	if(0<var.ftfr.toSend.size())
	{
		auto &file=var.ftfr.toSend[0];
		if(true!=file.started)
		{
			file.started=true;
			file.offset=0;

			// 1st Batch
			//   [0]     TOWNS_VMIF_TFR_HOST_TO_VM
			//   [1..3]  Unused(Zero)
			//   [4..7]  File Size
			//   [8..63] Unused(Zero)
			//   [64..]  File name in VM

			file.bin=cpputil::ReadBinaryFile(file.hostFName);
			auto fSize=file.bin.size();

			for(auto &d : physMem.state.spriteRAM)
			{
				d=0;
			}
			physMem.state.spriteRAM[0]=TOWNS_VMIF_TFR_HOST_TO_VM;
			physMem.state.spriteRAM[4]=( fSize     &255);
			physMem.state.spriteRAM[5]=((fSize>> 8)&255);
			physMem.state.spriteRAM[6]=((fSize>>16)&255);
			physMem.state.spriteRAM[7]=((fSize>>24)&255);

			for(int i=0; i<260 && i<file.vmFName.size(); ++i)
			{
				physMem.state.spriteRAM[64+i]=file.vmFName[i];
			}
		}
		else
		{
			// 2nd Batch and on
			//   File Contents

			unsigned int sizeLeft=(unsigned int)file.bin.size()-file.offset;
			unsigned int batchSize=(TOWNS_SPRITERAM_SIZE<sizeLeft ? TOWNS_SPRITERAM_SIZE : sizeLeft);

			for(unsigned int i=0; i<batchSize; ++i)
			{
				physMem.state.spriteRAM[i]=file.bin[file.offset+i];
			}

			file.offset+=batchSize;
			if(file.bin.size()<=file.offset)
			{
				var.ftfr.toSend.erase(var.ftfr.toSend.begin());
			}
		}
	}
	else if(0<var.ftfr.toRecv.size())
	{
		auto &file=var.ftfr.toRecv[0];
		if(true!=file.started)
		{
			file.started=true;
			file.offset=0;

			// 1st Batch
			//   [0]     TOWNS_VMIF_TFR_VM_TO_HOST
			//   [1..3]  Unused(Zero)
			//   [4..7]  File Size
			//   [8..63] Unused(Zero)
			//   [64..]  File name in VM

			for(auto &d : physMem.state.spriteRAM)
			{
				d=0;
			}
			physMem.state.spriteRAM[0]=TOWNS_VMIF_TFR_VM_TO_HOST;
			for(int i=0; i<260 && i<file.vmFName.size(); ++i)
			{
				physMem.state.spriteRAM[64+i]=file.vmFName[i];
			}
		}
		else
		{
			// 2nd Batch and on
			//   File Contents
			if(2==physMem.state.spriteRAM[0])
			{
				std::cout << "Read error in VM." << std::endl;
				var.ftfr.toRecv.erase(var.ftfr.toRecv.begin());
				return;
			}

			unsigned int batchSize=cpputil::GetDword(physMem.state.spriteRAM+4);
			for(unsigned int i=0; i<batchSize; ++i)
			{
				file.bin.push_back(physMem.state.spriteRAM[8+i]);
			}

			if(1==physMem.state.spriteRAM[0])
			{
				std::cout << "File finished.  " << file.bin.size() << " bytes." << std::endl;
				if(0==cpputil::WriteBinaryFile(file.hostFName,file.bin.size(),file.bin.data()))
				{
					std::cout << "File Write Error: " << file.hostFName << std::endl;
				}
				var.ftfr.toRecv.erase(var.ftfr.toRecv.begin());
			}
		}
	}
	else
	{
		physMem.state.spriteRAM[0]=TOWNS_VMIF_TFR_END;
	}
}
void FMTownsCommon::VMHostFileTransfer::AddHostToVM(std::string hostFName,std::string vmFName)
{
	File f;
	toSend.push_back(f);
	toSend.back().hostFName=hostFName;
	toSend.back().vmFName=vmFName;
}
void FMTownsCommon::VMHostFileTransfer::AddVMToHost(std::string vmFName,std::string hostFName)
{
	File f;
	toRecv.push_back(f);
	toRecv.back().hostFName=hostFName;
	toRecv.back().vmFName=vmFName;
}
