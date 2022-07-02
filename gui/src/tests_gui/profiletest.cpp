#include <iostream>
#include <string>
#include <math.h>

#include "cpputil.h"
#include "townsprofile.h"


bool Equal(const TownsProfile &a,const TownsProfile &b)
{
	if(a.townsType!=b.townsType)
	{
		return false;
	}

	for(int i=0; i<TownsProfile::NUM_GAMEPORTS; ++i)
	{
		if(a.gamePort[i]!=b.gamePort[i])
		{
			return false;
		}
	}

	if(
	   a.bootKeyComb!=b.bootKeyComb ||
	   a.windowShift!=b.windowShift ||
	   a.noWait!=b.noWait ||
	   a.noWaitStandby!=b.noWaitStandby ||
	   a.highResAvailable!=b.highResAvailable ||
	   a.catchUpRealTime!=b.catchUpRealTime ||
	   a.damperWireLine!=b.damperWireLine ||
	   a.scanLineEffectIn15KHz!=b.scanLineEffectIn15KHz ||
	   a.startUpStateFName!=b.startUpStateFName ||
	   a.ROMPath!=b.ROMPath ||
	   a.CMOSFName!=b.CMOSFName ||
	   a.autoSaveCMOS!=b.autoSaveCMOS)
	{
		return false;
	}

	for(int i=0; i<TownsProfile::NUM_FDDRIVES; ++i)
	{
		if(a.fdImgFName[i]!=b.fdImgFName[i] ||
		   a.fdImgWriteProtect[i]!=b.fdImgWriteProtect[i])
		{
			return false;
		}
	}

	if(
	   a.cdImgFName!=b.cdImgFName ||
	   a.cdSpeed!=b.cdSpeed ||
	   a.memCardType!=b.memCardType ||
	   a.memCardImgFName!=b.memCardImgFName ||
	   a.startUpScriptFName!=b.startUpScriptFName ||
	   a.symbolFName!=b.symbolFName ||
	   a.playbackEventLogFName!=b.playbackEventLogFName ||
	   a.keyMapFName!=b.keyMapFName ||
	   a.sharedDir!=b.sharedDir ||
	   a.fdSearchPaths!=b.fdSearchPaths ||
	   a.cdSearchPaths!=b.cdSearchPaths ||
	   a.keyboardMode!=b.keyboardMode)
	{
		return false;
	}

	if(a.virtualKeys.size()!=b.virtualKeys.size())
	{
		return false;
	}
	for(int i=0; i<a.virtualKeys.size(); ++i)
	{
		if(a.virtualKeys[i].townsKey!=b.virtualKeys[i].townsKey ||
		   a.virtualKeys[i].physicalId!=b.virtualKeys[i].physicalId ||
		   a.virtualKeys[i].button!=b.virtualKeys[i].button)
		{
			return false;
		}
	}
	if(a.hostShortCutKeys.size()!=b.hostShortCutKeys.size())
	{
		return false;
	}
	for(int i=0; i<a.hostShortCutKeys.size(); ++i)
	{
		if(a.hostShortCutKeys[i].hostKey!=b.hostShortCutKeys[i].hostKey ||
		   a.hostShortCutKeys[i].ctrl!=b.hostShortCutKeys[i].ctrl ||
		   a.hostShortCutKeys[i].shift!=b.hostShortCutKeys[i].shift ||
		   a.hostShortCutKeys[i].cmdStr!=b.hostShortCutKeys[i].cmdStr)
		{
			return false;
		}
	}

	if(
	   a.fmVol!=b.fmVol ||
	   a.pcmVol!=b.pcmVol ||
	   a.mouseByFlightstickAvailable!=b.mouseByFlightstickAvailable ||
	   a.cyberStickAssignment!=b.cyberStickAssignment ||
	   a.mouseByFlightstickPhysicalId!=b.mouseByFlightstickPhysicalId ||
	   a.mouseByFlightstickCenterX!=b.mouseByFlightstickCenterX ||
	   a.mouseByFlightstickCenterY!=b.mouseByFlightstickCenterY)
	{
		return false;
	}

	if(
	   fabs(a.mouseByFlightstickZeroZoneX-b.mouseByFlightstickZeroZoneX)>0.000001 ||
	   fabs(a.mouseByFlightstickZeroZoneY-b.mouseByFlightstickZeroZoneY)>0.000001 ||
	   fabs(a.mouseByFlightstickScaleX-b.mouseByFlightstickScaleX)>0.000001 ||
	   fabs(a.mouseByFlightstickScaleY-b.mouseByFlightstickScaleY)>0.000001)
	{
		return false;
	}

	if(
	   a.throttlePhysicalId!=b.throttlePhysicalId ||
	   (0<=a.throttlePhysicalId && a.throttleAxis!=b.throttleAxis))
	{
		return false;
	}

	for(int i=0; i<TownsProfile::MAX_NUM_SCSI_DEVICES; ++i)
	{
		if(
		   a.scsiImg[i].imageType!=b.scsiImg[i].imageType ||
		   a.scsiImg[i].imgFName!=b.scsiImg[i].imgFName)
		{
			return false;
		}
	}

	if(a.toSend.size()!=b.toSend.size())
	{
		return false;
	}
	for(int i=0; i<a.toSend.size(); ++i)
	{
		if(a.toSend[i].hostFName!=b.toSend[i].hostFName ||
		   a.toSend[i].vmFName!=b.toSend[i].vmFName)
		{
			return false;
		}
	}


	if(
	   a.autoStart!=b.autoStart ||
	   a.debugger!=b.debugger ||
	   a.interactive!=b.interactive ||
	   a.powerOffAtBreakPoint!=b.powerOffAtBreakPoint ||
	   a.forceQuitOnPowerOff!=b.forceQuitOnPowerOff ||
	   a.powerOffAt!=b.powerOffAt ||
	   a.pretend386DX!=b.pretend386DX ||
	   a.memSizeInMB!=b.memSizeInMB ||
	   a.freq!=b.freq ||
	   a.mouseIntegrationSpeed!=b.mouseIntegrationSpeed ||
	   a.appSpecificSetting!=b.appSpecificSetting ||
	   a.scaling!=b.scaling ||
	   a.autoScaling!=b.autoScaling ||
	   a.windowModeOnStartUp!=b.windowModeOnStartUp ||
	   a.quickScrnShotDir!=b.quickScrnShotDir ||
	   a.quickStateSaveFName!=b.quickStateSaveFName ||
	   a.pauseResumeKeyLabel!=b.pauseResumeKeyLabel)
	{
		return false;
	}

	return true;
}

bool TestLoadSave(void)
{
	TownsProfile profile;
	auto save=profile.Serialize();
	return profile.Deserialize(save);
}
bool TestRecovery(void)
{
	TownsProfile a,b;

	// a.townsType=77;
	a.gamePort[0]=10;
	a.gamePort[1]=11;
	a.bootKeyComb=3;

	// a.noWait=false;
	// a.noWaitStandby=true;

	a.ROMPath="ROMPath";
	// a.CMOSFName="CMOSFile";  Not currently saved in profile
	a.fdImgFName[0]="drive 0";
	a.fdImgFName[1]="drive 1";

	a.fmVol=5000;
	a.pcmVol=6000;

	a.quickScrnShotDir="ScrnShot";
	a.quickStateSaveFName="state.TState";
	a.pauseResumeKeyLabel="F12";

	if(true==Equal(a,b))
	{
		std::cout << "Wrong comparison." << std::endl;
		return false;
	}

	auto save=a.Serialize();
	if(true!=b.Deserialize(save))
	{
		std::cout << "Deserialization failure." << std::endl;
		return false;
	}
	if(true!=Equal(a,b))
	{
		std::cout << "Recovery Failure." << std::endl;
		return false;
	}


	return true;
}
int main(int ac,char *av[])
{
	if(true!=TestLoadSave())
	{
		return 1;
	}
	if(true!=TestRecovery())
	{
		return 1;
	}

	std::cout << "Tests pass." << std::endl;

	return 0;
}
