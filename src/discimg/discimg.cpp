#include "iostream"
#include "fstream"
#include "discimg.h"



/* static */ DiscImage::MinSecFrm DiscImage::MinSecFrm::Zero(void)
{
	MinSecFrm MSF;
	MSF.min=0;
	MSF.sec=0;
	MSF.frm=0;
	return MSF;
}
/* static */ DiscImage::MinSecFrm DiscImage::MinSecFrm::TwoSeconds(void)
{
	MinSecFrm MSF;
	MSF.min=0;
	MSF.sec=2;
	MSF.frm=0;
	return MSF;
}


////////////////////////////////////////////////////////////


static std::string GetExtension(const std::string &fName)
{
	for(auto i=fName.size()-1; 0<=i; --i)
	{
		if(fName[i]=='.')
		{
			std::string ext;
			ext.insert(ext.end(),fName.begin()+i,fName.end());
			return ext;
		}
	}
	return "";
}
static void Capitalize(std::string &str)
{
	for(auto &c : str)
	{
		if('a'<=c && c<='z')
		{
			c+=('A'-'a');
		}
	}
}


////////////////////////////////////////////////////////////


DiscImage::DiscImage()
{
	CleanUp();
}
void DiscImage::CleanUp(void)
{
	fileType=FILETYPE_NONE;
	binFileSize=0;
	fName="";
	binFName="";
	num_sectors=0;
	tracks.clear();
}
unsigned int DiscImage::Open(const std::string &fName)
{
	auto ext=GetExtension(fName);
	Capitalize(ext);
	if(".CUE"==ext)
	{
		return OpenCUE(fName);
	}
	if(".ISO"==ext)
	{
		return OpenISO(fName);
	}
	return ERROR_UNSUPPORTED;
}
unsigned int DiscImage::OpenCUE(const std::string &fName)
{
	return ERROR_NOT_YET_SUPPORTED;
}
unsigned int DiscImage::OpenISO(const std::string &fName)
{
	std::ifstream ifp;
	ifp.open(fName,std::ios::binary);
	if(true!=ifp.is_open())
	{
		return ERROR_CANNOT_OPEN;
	}

	auto begin=ifp.tellg();
	ifp.seekg(0,std::ios::end);
	auto end=ifp.tellg();

	auto fSize=end-begin;

	if(0!=fSize%2048)
	{
		return ERROR_SECTOR_SIZE;
	}

	CleanUp();


	fileType=FILETYPE_ISO;
	num_sectors=(unsigned int)(fSize/2048);

	tracks.resize(1);
	tracks[0].trackType=TRACK_MODE1_DATA;
	tracks[0].sectorLength=2048;
	tracks[0].start=MinSecFrm::Zero();
	tracks[0].end=HSGtoMSF(num_sectors);
	tracks[0].postGap=MinSecFrm::Zero();

	return ERROR_NOERROR;
}
unsigned int DiscImage::GetNumTracks(void) const
{
	return (unsigned int)tracks.size();
}
unsigned int DiscImage::GetNumSectors(void) const
{
	return num_sectors;
}
const std::vector <DiscImage::Track> &DiscImage::GetTracks(void) const
{
	return tracks;
}
/* static */ DiscImage::MinSecFrm DiscImage::HSGtoMSF(unsigned int HSG)
{
	MinSecFrm MSF;
	MSF.FromHSG(HSG);
	return MSF;
}
/* static */ unsigned int DiscImage::MSFtoHSG(MinSecFrm MSF)
{
	return MSF.ToHSG();
}
/* static */ unsigned int DiscImage::BinToBCD(unsigned int bin)
{
	unsigned int high=bin/10;
	unsigned int low=bin%10;
	return (high<<4)+low;
}

std::vector <unsigned char> DiscImage::ReadSectorMODE1(unsigned int HSG,unsigned int numSec) const
{
	std::vector <unsigned char> data;
	if(0<tracks.size() && (tracks[0].trackType==TRACK_MODE1_DATA || tracks[0].trackType==TRACK_MODE2_DATA))
	{
		if(HSG+numSec<=tracks[0].end.ToHSG())
		{
			data.resize(numSec*MODE1_BYTES_PER_SECTOR);
			if(MODE1_BYTES_PER_SECTOR==tracks[0].sectorLength)
			{
			}
			else
			{
			}
		}
	}
	return data;
}
