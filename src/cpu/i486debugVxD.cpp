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
#include <stdint.h>

#include "i486.h"
#include "i486inst.h"
#include "i486debug.h"
#include "i486symtable.h"

#include "cpputil.h"



void i486Debugger::MakeVxDLabelTable(void)
{
	// Reference : Ralf Brown's Interrupt List  http://www.ctyme.com/intr/int-20.htm
	VxDLabel[0x0001]="VMM";
	VxDLabel[0x0002]="DEBUG";
	VxDLabel[0x0003]="VPICD";
	VxDLabel[0x0004]="VDMAD";
	VxDLabel[0x0005]="VTD";
	VxDLabel[0x0006]="V86MMGR";
	VxDLabel[0x0007]="PAGESWAP";
	VxDLabel[0x0009]="REBOOT";
	VxDLabel[0x000A]="VDD";
	VxDLabel[0x000B]="VSD";
	VxDLabel[0x000C]="VMD";
	VxDLabel[0x000D]="VKD";
	VxDLabel[0x000E]="VCD";
	VxDLabel[0x0010]="BLOCKDEV";
	VxDLabel[0x0011]="VMCPD";
	VxDLabel[0x0012]="EBIOS";
	VxDLabel[0x0014]="VNETBIOS";
	VxDLabel[0x0015]="DOSMGR";
	VxDLabel[0x0017]="SHELL";
	VxDLabel[0x0018]="VMPoll";
	VxDLabel[0x001A]="DOSNET";
	VxDLabel[0x001B]="VFD";
	VxDLabel[0x001C]="LOADHI";
	VxDLabel[0x0020]="INT13";
	VxDLabel[0x0021]="PAGEFILE";
	VxDLabel[0x0026]="VPOWERD";
	VxDLabel[0x0027]="VXDLDR";
	VxDLabel[0x0028]="NDIS";
	VxDLabel[0x002A]="VWIN32";
	VxDLabel[0x002B]="VCOMM";
	VxDLabel[0x002C]="SPOOLER";
	VxDLabel[0x0032]="VSERVER";
	VxDLabel[0x0033]="CONFIGMG";
	VxDLabel[0x0034]="DWCFGMG.SYS";
	VxDLabel[0x0036]="VFBACKUP";
	VxDLabel[0x0037]="VMINI";
	VxDLabel[0x0038]="VCOND";
	VxDLabel[0x003D]="BIOS";
	VxDLabel[0x003E]="WSOCK";
	VxDLabel[0x0040]="IFSMGR";
	VxDLabel[0x0041]="VCDFSD";
	VxDLabel[0x0048]="PERF";
	VxDLabel[0x004A]="MTRR";
	VxDLabel[0x004B]="NTKERN";
	VxDLabel[0x011F]="VFLATD";
	VxDLabel[0x0449]="VJOYD";
	VxDLabel[0x044A]="MMDEVLDR";
	VxDLabel[0x0480]="VNETSUP";
	VxDLabel[0x0481]="VREDIR";
	VxDLabel[0x0483]="VSHARE";
	VxDLabel[0x0487]="NWLINK";
	VxDLabel[0x0488]="VTDI";
	VxDLabel[0x0489]="VIP";
	VxDLabel[0x048A]="MSTCP";
	VxDLabel[0x048B]="VCACHE";
	VxDLabel[0x048E]="NWREDIR";
	VxDLabel[0x0491]="FILESEC";
	VxDLabel[0x0492]="NWSERVER";
	VxDLabel[0x0493]="MSSP";
	VxDLabel[0x0494]="NSCL";
	VxDLabel[0x0495]="AFVXD";
	VxDLabel[0x0496]="NDIS2SUP";
	VxDLabel[0x0498]="SPLITTER";
	VxDLabel[0x0499]="PPPMAC";
	VxDLabel[0x049A]="VDHCP";
	VxDLabel[0x049B]="VNBT";
	VxDLabel[0x049D]="LOGGER";
	VxDLabel[0x097C]="PCCARD";
	VxDLabel[0x3098]="QEMM";
	VxDLabel[0x30F6]="WSVV";
	VxDLabel[0x33FC]="ASPIENUM";
	VxDLabel[0x357E]="DSOUND";
	VxDLabel[0x39E6]="A3D";
	VxDLabel[0x3BFD]="CWCPROXY";
	VxDLabel[0x3C78]="VGARTD";

	// VxD
	VxDFuncLabel[0x0001][0x0000]="GET VERSION";
	VxDFuncLabel[0x0001][0x0001]="GET CURRENT VM HANDLE";
	VxDFuncLabel[0x0001][0x0002]="TEST CURRENT VM HANDLE";
	VxDFuncLabel[0x0001][0x0003]="GET SYSTEM VM HANDLE";
	VxDFuncLabel[0x0001][0x0004]="TEST SYSTEM VM HANDLE";
	VxDFuncLabel[0x0001][0x0005]="VALIDATE VM HANDLE";
	VxDFuncLabel[0x0001][0x0006]="GET VM REENTER COUNT";
	VxDFuncLabel[0x0001][0x0007]="BEGIN REENTRANT EXECUTION";
	VxDFuncLabel[0x0001][0x0008]="END REENTRANT EXECUTION";
	VxDFuncLabel[0x0001][0x0009]="INSTALL VM86 BREAKPOINT";
	VxDFuncLabel[0x0001][0x000A]="REMOVE VM86 BREAKPOINT";
	VxDFuncLabel[0x0001][0x000B]="ALLOC VM86 CALLBACK";
	VxDFuncLabel[0x0001][0x000C]="ALLOC PROTECTED-MODE CALLBACK";
	VxDFuncLabel[0x0001][0x000D]="CALL WHEN VM RETURNS";
	VxDFuncLabel[0x0001][0x000E]="SCHEDULE GLOBAL EVENT";
	VxDFuncLabel[0x0001][0x000F]="SCHEDULE VM EVENT";
	VxDFuncLabel[0x0001][0x0010]="CALL GLOBAL EVENT";
	VxDFuncLabel[0x0001][0x0011]="CALL VM EVENT";
	VxDFuncLabel[0x0001][0x0012]="CANCEL GLOBAL EVENT";
	VxDFuncLabel[0x0001][0x0013]="CANCEL VM EVENT";
	VxDFuncLabel[0x0001][0x0014]="CALL PRIORITY VM EVENT";
	VxDFuncLabel[0x0001][0x0015]="CANCEL PRIORITY VM EVENT";
	VxDFuncLabel[0x0001][0x0016]="GET NMI HANDLER ADDR";
	VxDFuncLabel[0x0001][0x0017]="SET NMI HANDLER ADDR";
	VxDFuncLabel[0x0001][0x0018]="HOOK NMI EVENT";
	VxDFuncLabel[0x0001][0x0019]="CALL WHEN VM INT ENABLED";
	VxDFuncLabel[0x0001][0x001A]="ENABLE VM INT";
	VxDFuncLabel[0x0001][0x001B]="DISABLE VM INT";
	VxDFuncLabel[0x0001][0x001C]="MAP FLAT";
	VxDFuncLabel[0x0001][0x001D]="MAP LINEAR TO VM ADDR";
	VxDFuncLabel[0x0001][0x001E]="ADJUST EXEC PRIORITY"; 
	VxDFuncLabel[0x0001][0x001F]="BEGIN CRITICAL SECTION"; 
	VxDFuncLabel[0x0001][0x0020]="END CRITICAL SECTION"; 
	VxDFuncLabel[0x0001][0x0021]="END CRITICAL SECTION AND SUSPEND"; 
	VxDFuncLabel[0x0001][0x0022]="CLAIM CRITICAL SECTION"; 
	VxDFuncLabel[0x0001][0x0023]="RELEASE CRITICAL SECTION"; 
	VxDFuncLabel[0x0001][0x0024]="CALL WHEN NOT CRITICAL"; 
	VxDFuncLabel[0x0001][0x0025]="CREATE SEMAPHORE"; 
	VxDFuncLabel[0x0001][0x0026]="DESTROY SEMAPHORE"; 
	VxDFuncLabel[0x0001][0x0027]="WAIT ON SEMAPHORE"; 
	VxDFuncLabel[0x0001][0x0028]="SIGNAL SEMAPHORE"; 
	VxDFuncLabel[0x0001][0x0029]="GET CRITICAL SECTION STATUS"; 
	VxDFuncLabel[0x0001][0x002A]="CALL WHEN TASK SWITCHED"; 
	VxDFuncLabel[0x0001][0x002B]="SUSPEND VM"; 
	VxDFuncLabel[0x0001][0x002C]="RESUME VM"; 
	VxDFuncLabel[0x0001][0x002D]="NO-RAIL RESUME VM"; 
	VxDFuncLabel[0x0001][0x002E]="NUKE VM"; 
	VxDFuncLabel[0x0001][0x002F]="CRASH CURRENT VM"; 
	VxDFuncLabel[0x0001][0x0030]="GET EXEC FOCUS"; 
	VxDFuncLabel[0x0001][0x0031]="SET EXEC FOCUS"; 
	VxDFuncLabel[0x0001][0x0032]="GET TIME SLICE PRIORITY"; 
	VxDFuncLabel[0x0001][0x0033]="SET TIME SLICE PRIORITY"; 
	VxDFuncLabel[0x0001][0x0034]="GET TIME SLICE GRANULARITY"; 
	VxDFuncLabel[0x0001][0x0035]="SET TIME SLICE GRANULARITY"; 
	VxDFuncLabel[0x0001][0x0036]="GET TIME SLICE INFO"; 
	VxDFuncLabel[0x0001][0x0037]="ADJUST EXEC TIME"; 
	VxDFuncLabel[0x0001][0x0038]="RELEASE TIME SLICE"; 
	VxDFuncLabel[0x0001][0x0039]="WAKE UP VM"; 
	VxDFuncLabel[0x0001][0x003A]="CALL WHEN IDLE"; 
	VxDFuncLabel[0x0001][0x003B]="GET NEXT VM HANDLE"; 
	VxDFuncLabel[0x0001][0x003C]="SET GLOBAL TIMEOUT"; 
	VxDFuncLabel[0x0001][0x003D]="SET VM TIMEOUT"; 
	VxDFuncLabel[0x0001][0x003E]="CANCEL TIMEOUT"; 
	VxDFuncLabel[0x0001][0x003F]="GET SYSTEM TIME"; 
	VxDFuncLabel[0x0001][0x0040]="GET VM EXEC TIME"; 
	VxDFuncLabel[0x0001][0x0041]="HOOK VM86 INT CHAIN"; 
	VxDFuncLabel[0x0001][0x0042]="GET VM86 INT VECTOR"; 
	VxDFuncLabel[0x0001][0x0043]="SET VM86 INT VECTOR"; 
	VxDFuncLabel[0x0001][0x0044]="GET PROTECTED-MODE INT VECTOR"; 
	VxDFuncLabel[0x0001][0x0045]="SET PROTECTED-MODE INT VECTOR"; 
	VxDFuncLabel[0x0001][0x0046]="SIMULATE INT"; 
	VxDFuncLabel[0x0001][0x0047]="SIMULATE IRET";
	VxDFuncLabel[0x0001][0x0048]="SIMULATE CALLF"; 
	VxDFuncLabel[0x0001][0x0049]="SIMULATE JMPF"; 
	VxDFuncLabel[0x0001][0x004A]="SIMULATE RETF"; 
	VxDFuncLabel[0x0001][0x004B]="SIMULATE RETF N";  
	VxDFuncLabel[0x0001][0x004C]="BUILD INT STACK FRAME";
	VxDFuncLabel[0x0001][0x004D]="SIMULATE PUSH"; 
	VxDFuncLabel[0x0001][0x004E]="SIMULATE POP";  
	VxDFuncLabel[0x0001][0x0089]="SET VM86 EXEC MODE"; 
	VxDFuncLabel[0x0001][0x008A]="SET PROTECTED EXEC MODE"; 
	VxDFuncLabel[0x0001][0x009B]="CREATE LIST"; 
	VxDFuncLabel[0x0001][0x009C]="DESTROY LIST"; 
	VxDFuncLabel[0x0001][0x009D]="ALLOC LIST"; 
	VxDFuncLabel[0x0001][0x009E]="ATTACH LIST"; 
	VxDFuncLabel[0x0001][0x009F]="ATTACH LIST TAIL"; 
	VxDFuncLabel[0x0001][0x00A0]="INSERT INTO LIST"; 
	VxDFuncLabel[0x0001][0x00A1]="REMOVE FROM LIST"; 
	VxDFuncLabel[0x0001][0x00A2]="DEALLOC LIST"; 
	VxDFuncLabel[0x0001][0x00A3]="GET FIRST IN LIST"; 
	VxDFuncLabel[0x0001][0x00A4]="GET NEXT IN LIST"; 
	VxDFuncLabel[0x0001][0x00A5]="REMOVE FIRST IN LIST"; 

	VxDFuncLabel[0x0003][0x0000]="GET VERSION"; 
	VxDFuncLabel[0x0003][0x0001]="VIRTUALIZE IRQ"; 
	VxDFuncLabel[0x0003][0x0002]="SET INT REQUEST"; 
	VxDFuncLabel[0x0003][0x0003]="CLEAR INT REQUEST"; 
	VxDFuncLabel[0x0003][0x0004]="PHYSICAL EOI"; 
	VxDFuncLabel[0x0003][0x0005]="GET COMPLETE STATUS"; 
	VxDFuncLabel[0x0003][0x0006]="GET STATUS"; 
	VxDFuncLabel[0x0003][0x0007]="TEST PHYSICAL REQUEST"; 
	VxDFuncLabel[0x0003][0x0008]="PHYSICALLY MASK"; 
	VxDFuncLabel[0x0003][0x0009]="PHYSICALLY UNMASK"; 
	VxDFuncLabel[0x0003][0x000A]="SET AUTOMATIC MASKING"; 
	VxDFuncLabel[0x0003][0x000B]="GET IRQ COMPLETE STATUS"; 
	VxDFuncLabel[0x0003][0x000C]="CONVERT HANDLE TO IRQ"; 
	VxDFuncLabel[0x0003][0x000D]="CONVERT IRQ TO INTERRUPT"; 
	VxDFuncLabel[0x0003][0x000E]="CONVERT INTERRUPT TO IRQ"; 
	VxDFuncLabel[0x0003][0x000F]="CALL ON HARDWARE INT"; 
	VxDFuncLabel[0x0003][0x0010]="FORCE DEFAULT OWNER"; 
	VxDFuncLabel[0x0003][0x0011]="FORCE DEFAULT BEHAVIOR"; 
	VxDFuncLabel[0x0003][0x0012]="AUTO MASK AT INT SWAP"; 
	VxDFuncLabel[0x0003][0x0013]="BEGIN INST PAGE SWAP"; 
	VxDFuncLabel[0x0003][0x0014]="END INST PAGE SWAP"; 
	VxDFuncLabel[0x0003][0x0015]="VIRTUAL EOI"; 
	VxDFuncLabel[0x0003][0x0016]="GET VIRTUALIZATION COUNT"; 
	VxDFuncLabel[0x0003][0x0017]="POST SYS CRITICAL INIT"; 
	VxDFuncLabel[0x0003][0x0018]="SECONDARY PIC MASK CHANGE"; 
}

void i486Debugger::AutoAnnotateVxD(const i486DXCommon &cpu,const Memory &mem,const i486DXCommon::SegmentRegister &reg,uint32_t EIP)
{
	if(true!=cpu.IsInRealMode() && true!=cpu.GetVM())
	{
		auto &symTable=GetSymTable();
		auto command=cpu.DebugFetchWord(32,reg,EIP+2,mem);
		auto vxd=cpu.DebugFetchWord(32,reg,EIP+4,mem);

		std::string label;
		{
			auto found=VxDLabel.find(vxd);
			if(found!=VxDLabel.end())
			{
				label=found->second;
				label+="(";
				label+=cpputil::Ustox(vxd);
				label+="H)";
			}
			else
			{
				label+="V?D(";
				label+=cpputil::Ustox(vxd);
				label+="H)";
			}
		}

		{
			std::string cmd;
			auto found1=VxDFuncLabel.find(vxd);
			if(VxDFuncLabel.end()!=found1)
			{
				auto found2=found1->second.find(command&0x7FFF);
				if(found1->second.end()!=found2)
				{
					cmd=found2->second;
				}
			}
			if(""==cmd)
			{
				cmd="?CMD";
			}
			label+=" "+cmd+"(";
			label+=cpputil::Ustox(command);
			label+="H)";
		}

		i486DXCommon::FarPointer ptr;
		ptr.SEG=reg.value;
		ptr.OFFSET=EIP;
		symTable.SetComment(ptr,label);

		ptr.OFFSET+=2;
		auto *newSym=symTable.Update(ptr,"VxD");
		newSym->symType=i486Symbol::SYM_RAW_DATA;
		newSym->rawDataBytes=4;
	}
}
