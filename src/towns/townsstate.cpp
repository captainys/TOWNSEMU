#include <fstream>

#include "towns.h"

bool FMTowns::SaveState(std::string fName) const
{
	return false;
}
bool FMTowns::LoadState(std::string fName)
{
	return false;
}

/* virtual */ std::vector <unsigned char> FMTowns::Serialize(std::string) const
{
	std::vector <unsigned char> data;
	BeginSerialization(data,serialDataVersion);


	PushInt64(data,state.townsTime);
	PushInt64(data,state.nextDevicePollingTime);
	PushInt64(data,state.cpuTime);
	PushInt64(data,state.timeDeficit);
	PushBool(data,state.noWait);
	PushBool(data,state.pretend386DX);
	PushInt64(data,state.nextFastDevicePollingTime);
	PushInt64(data,state.nextSecondInTownsTime);
	PushInt64(data,state.clockBalance);
	PushInt64(data,state.freq);
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

	return data;
}
/* virtual */ bool FMTowns::Deserialize(const std::vector <unsigned char> &dat,std::string)
{
	const unsigned char *data=dat.data()+deviceIdLength;
	auto version=ReadUint32(data);

	state.townsTime=ReadInt64(data);
	state.nextDevicePollingTime=ReadInt64(data);
	state.cpuTime=ReadInt64(data);
	state.timeDeficit=ReadInt64(data);
	state.noWait=ReadBool(data);
	state.pretend386DX=ReadBool(data);
	state.nextFastDevicePollingTime=ReadInt64(data);
	state.nextSecondInTownsTime=ReadInt64(data);
	state.clockBalance=ReadInt64(data);
	state.freq=ReadInt64(data);
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

	state.appSpecificSetting=ReadUint32(data);
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

	return data;
}
