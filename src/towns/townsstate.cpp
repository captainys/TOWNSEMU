#include <fstream>

#include "towns.h"

// Disk-image (Disc-image) search rule:
// (1) Hard-disk image is not auto-mounted.
// (2) Try using the filename stored in the state file as is.
// (3) Try state path+relative path
// (4) Try image search path+file name
// (5) Try state path+file name
// (6) If floppy-disk image, use image stored in the state file.


bool FMTownsCommon::SaveState(std::string fName) const
{
	auto &cpu=CPU();
	std::ofstream ofp(fName,std::ios::binary);
	if(true==ofp.is_open())
	{
		std::vector <const Device *> allDevices;
		allDevices.push_back(this);
		allDevices.push_back(&cpu);
		allDevices.push_back(&pic);
		allDevices.push_back(&dmac);
		allDevices.push_back(&physMem);
		allDevices.push_back(&crtc);
		allDevices.push_back(&sprite);
		allDevices.push_back(&fdc);
		allDevices.push_back(&scsi);
		allDevices.push_back(&cdrom);
		allDevices.push_back(&rtc);
		allDevices.push_back(&sound);
		allDevices.push_back(&gameport);
		allDevices.push_back(&timer);
		allDevices.push_back(&keyboard);
		allDevices.push_back(&serialport);
		// allDevices.push_back(&vndrv);

		for(auto devPtr : allDevices)
		{
			auto dat=devPtr->Serialize(fName);
			uint32_t len=(uint32_t)dat.size();

			ofp.write((char *)&len,4);
			ofp.write((char *)dat.data(),len);
		}
		return true;
	}
	return false;
}
bool FMTownsCommon::LoadState(std::string fName,class Outside_World::Sound &soundDevice)
{
	auto &cpu=CPU();
	std::ifstream ifp(fName,std::ios::binary);
	if(true==ifp.is_open())
	{
		std::vector <Device *> allDevices;
		allDevices.push_back(this);
		allDevices.push_back(&cpu);
		allDevices.push_back(&pic);
		allDevices.push_back(&dmac);
		allDevices.push_back(&physMem);
		allDevices.push_back(&crtc);
		allDevices.push_back(&sprite);
		allDevices.push_back(&fdc);
		allDevices.push_back(&scsi);
		allDevices.push_back(&cdrom);
		allDevices.push_back(&rtc);
		allDevices.push_back(&sound);
		allDevices.push_back(&gameport);
		allDevices.push_back(&timer);
		allDevices.push_back(&keyboard);
		allDevices.push_back(&serialport);
		// allDevices.push_back(&vndrv);

		while(true!=ifp.eof())
		{
			uint32_t len=0;
			ifp.read((char *)&len,4);
			if(0==len)
			{
				break;
			}

			std::vector <unsigned char> data;
			data.resize(len);
			ifp.read((char *)data.data(),len);

			bool successful=false;
			for(auto devPtr : allDevices)
			{
				if(true==devPtr->Deserialize(data,fName))
				{
					successful=true;
					break;
				}
			}

			if(true!=successful)
			{
				return false;
			}
		}

		// I was first running a loop for unscheduling all devices,
		// and then a loop for re-scheduling devices that has non-null scheduleTime
		// only to realize that UnscheduleDeviceCallBack was nullifying the scheduleTime.

		for(auto devPtr : allDevices)
		{
			if(TIME_NO_SCHEDULE!=devPtr->commonState.scheduleTime)
			{
				ScheduleDeviceCallBack(*devPtr,devPtr->commonState.scheduleTime);
			}
			else
			{
				UnscheduleDeviceCallBack(*devPtr);
			}
		}

		cdrom.ResumeCDDAAfterRestore(&soundDevice);

		var.justLoadedState=true;

		return true;
	}
	return false;
}

/* virtual */ uint32_t FMTownsCommon::SerializeVersion(void) const
{
	// Version 1 added app-specific settings for Daikoukaijidai
	// Version 2 added DOSLOLSEG, DOSLOLOFF, DOSVER
	// Version 3 added fastModeFreq, mainRAMWait,VRAMWait
	return 3;
}

/* virtual */ void FMTownsCommon::SpecificSerialize(std::vector <unsigned char> &data,std::string stateFName) const
{
	PushInt64(data,state.townsTime);
	PushInt64(data,state.nextRenderingTime);
	PushInt64(data,state.nextDevicePollingTime);
	PushInt64(data,state.townsTime); // Now dummy.  Used to be cpuTime, but no longer used.
	PushInt64(data,state.timeDeficit);
	PushBool(data,state.noWait);
	PushBool(data,state.pretend386DX);
	PushInt64(data,state.nextFastDevicePollingTime);
	PushInt64(data,state.nextSecondInTownsTime);
	PushInt64(data,state.clockBalance);
	PushInt64(data,state.currentFreq);
	PushUint32(data,state.resetReason);
	PushUint32(data,state.serialROMBitCount);
	PushUint32(data,state.lastSerialROMCommand);

	for(int i=0; i<2; ++i)
	{
		for(int j=0; j<4; ++j)
		{
			PushBool(data,state.eleVol[i][j].EN);
			PushBool(data,state.eleVol[i][j].C32);
			PushBool(data,state.eleVol[i][j].C0);
			PushUint32(data,state.eleVol[i][j].vol);
		}
	}

	PushUint32(data,state.eleVolChLatch[0]);
	PushUint32(data,state.eleVolChLatch[1]);

	PushUint32(data,state.tbiosVersion);
	PushBool(data,state.mouseBIOSActive);
	PushInt32(data,state.mouseDisplayPage);
	PushUint32(data,state.TBIOS_physicalAddr);
	PushUint32(data,state.TBIOS_mouseInfoOffset);
	PushUint32(data,state.MOS_work_linearAddr);
	PushUint32(data,state.MOS_work_physicalAddr);
	PushUint32(data,state.MOS_pulsePerPixelH);
	PushUint32(data,state.MOS_pulsePerPixelV);
	PushInt32(data,state.mouseIntegrationSpeed);

	PushUint16(data,state.DOSSEG);
	// Version 2 and later >>
	PushUint16(data,state.DOSVER);
	PushUint16(data,state.DOSLOLOFF);
	PushUint16(data,state.DOSLOLSEG);
	// Version 2 and later <<

	PushUint32(data,state.appSpecificSetting);
	PushUint32(data,state.appSpecific_MousePtrX);
	PushUint32(data,state.appSpecific_MousePtrY);
	PushUint32(data,state.appSpecific_StickPosXPtr);
	PushUint32(data,state.appSpecific_StickPosYPtr);
	PushUint32(data,state.appSpecific_ThrottlePtr);
	PushUint32(data,state.appSpecific_RudderPtr);
	PushUint32(data,state.appSpecific_WC2_EventQueueBaseAddr);  // DS:03CCH
	PushUint32(data,state.appSpecific_WC_setSpeedPtr);
	PushUint32(data,state.appSpecific_WC_maxSpeedPtr);
	PushBool(data,state.appSpecific_HoldMouseIntegration);

	// Version 1 and later
	PushUint32(data,state.appSpecific_Daikoukai_YNDialogXAddr);
	PushUint32(data,state.appSpecific_Daikoukai_YNDialogYAddr);
	PushUint32(data,state.appSpecific_Daikoukai_DentakuDialogXAddr);
	PushUint32(data,state.appSpecific_Daikoukai_DentakuDialogYAddr);

	// Version 3 and later
	PushUint32(data,state.fastModeFreq);
	PushUint32(data,state.mainRAMWait);
	PushUint32(data,state.VRAMWait);
}
/* virtual */ bool FMTownsCommon::SpecificDeserialize(const unsigned char *&data,std::string stateFName,uint32_t version)
{
	state.townsTime=ReadInt64(data);
	state.nextRenderingTime=ReadInt64(data);
	state.nextDevicePollingTime=ReadInt64(data);
	ReadInt64(data);
	state.timeDeficit=ReadInt64(data);
	state.noWait=ReadBool(data);
	state.pretend386DX=ReadBool(data);
	state.nextFastDevicePollingTime=ReadInt64(data);
	state.nextSecondInTownsTime=ReadInt64(data);
	state.clockBalance=ReadInt64(data);
	state.currentFreq=ReadInt64(data);
	state.resetReason=ReadUint32(data);
	state.serialROMBitCount=ReadUint32(data);
	state.lastSerialROMCommand=ReadUint32(data);

	for(int i=0; i<2; ++i)
	{
		for(int j=0; j<4; ++j)
		{
			state.eleVol[i][j].EN=ReadBool(data);
			state.eleVol[i][j].C32=ReadBool(data);
			state.eleVol[i][j].C0=ReadBool(data);
			state.eleVol[i][j].vol=ReadUint32(data);
		}
	}

	state.eleVolChLatch[0]=ReadUint32(data);
	state.eleVolChLatch[1]=ReadUint32(data);

	state.tbiosVersion=ReadUint32(data);
	state.mouseBIOSActive=ReadBool(data);
	state.mouseDisplayPage=ReadInt32(data);
	state.TBIOS_physicalAddr=ReadUint32(data);
	state.TBIOS_mouseInfoOffset=ReadUint32(data);
	state.MOS_work_linearAddr=ReadUint32(data);
	state.MOS_work_physicalAddr=ReadUint32(data);
	state.MOS_pulsePerPixelH=ReadUint32(data);
	state.MOS_pulsePerPixelV=ReadUint32(data);
	state.mouseIntegrationSpeed=ReadInt32(data);

	state.DOSSEG=ReadUint16(data);
	if(2<=version)
	{
		state.DOSVER=ReadUint16(data);
		state.DOSLOLOFF=ReadUint16(data);
		state.DOSLOLSEG=ReadUint16(data);
	}

	// If the user chose an app-specific setting on start, it shouldn't override it.
	// For example, if start Dungeon Master without app-specific setting, and then later want to
	// turn it on, the start-up setting should have priority.
	if(TOWNS_APPSPECIFIC_NONE==state.appSpecificSetting)
	{
		state.appSpecificSetting=ReadUint32(data);
	}
	else
	{
		ReadUint32(data); // Dummy read
	}
	state.appSpecific_MousePtrX=ReadUint32(data);
	state.appSpecific_MousePtrY=ReadUint32(data);
	state.appSpecific_StickPosXPtr=ReadUint32(data);
	state.appSpecific_StickPosYPtr=ReadUint32(data);
	state.appSpecific_ThrottlePtr=ReadUint32(data);
	state.appSpecific_RudderPtr=ReadUint32(data);
	state.appSpecific_WC2_EventQueueBaseAddr=ReadUint32(data);  // DS:03CCH
	state.appSpecific_WC_setSpeedPtr=ReadUint32(data);
	state.appSpecific_WC_maxSpeedPtr=ReadUint32(data);
	state.appSpecific_HoldMouseIntegration=ReadBool(data);

	if(1<=version)
	{
		state.appSpecific_Daikoukai_YNDialogXAddr=ReadUint32(data);
		state.appSpecific_Daikoukai_YNDialogYAddr=ReadUint32(data);
		state.appSpecific_Daikoukai_DentakuDialogXAddr=ReadUint32(data);
		state.appSpecific_Daikoukai_DentakuDialogYAddr=ReadUint32(data);
	}
	else
	{
		state.appSpecific_Daikoukai_YNDialogXAddr=0;
		state.appSpecific_Daikoukai_YNDialogYAddr=0;
		state.appSpecific_Daikoukai_DentakuDialogXAddr=0;
		state.appSpecific_Daikoukai_DentakuDialogYAddr=0;
	}

	if(3<=version)
	{
	// Version 3 and later
		state.fastModeFreq=ReadUint32(data);
		state.mainRAMWait =ReadUint32(data);
		state.VRAMWait    =ReadUint32(data);
	}
	else
	{
		state.fastModeFreq=state.currentFreq;
		state.mainRAMWait=0;
		state.VRAMWait=0;
	}

	VMBase::ClearAbortFlag();

	lastAutoQSSCheckTime=0;

	return true;
}
