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
#include <vector>
#include <unordered_map>

#include "discimg.h"
#include "cpputil.h"



// Uncomment for verbose output.
// #define DEBUG_DISCIMG

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
/* static */ const char *DiscImage::ErrorCodeToText(unsigned int errCode)
{
	switch(errCode)
	{
	case ERROR_NOERROR:
		return "No error";
	case ERROR_UNSUPPORTED:
		return "Unsupported file format.";
	case ERROR_CANNOT_OPEN:
		return "Cannot open image file.";
	case ERROR_NOT_YET_SUPPORTED:
		return "File format not yet supported. (I'm working on it!)";
	case ERROR_SECTOR_SIZE:
		return "Binary size is not integer multiple of the sector length.";
	case ERROR_TOO_FEW_ARGS:
		return "Too few arguments.";
	case ERROR_UNSUPPORTED_TRACK_TYPE:
		return "Unsupported track type.";
	case ERROR_SECTOR_LENGTH_NOT_GIVEN:
		return "Sector length of the data track is not given.";
	case ERROR_TRACK_INFO_WITHOTU_TRACK:
		return "Track information is given without a track.";
	case ERROR_INCOMPLETE_MSF:
		return "Incomplete MM:SS:FF.";
	case ERROR_BINARY_FILE_NOT_FOUND:
		return "Image binary file not found.";
	case ERROR_FIRST_TRACK_NOT_STARTING_AT_00_00_00:
		return "First track not starting at 00:00:00";
	case ERROR_BINARY_SIZE_NOT_SECTOR_TIMES_INTEGER:
		return "Binary size is not integer multiple of sector lengths.";
	}
	return "Undefined error.";
}
void DiscImage::CleanUp(void)
{
	fileType=FILETYPE_NONE;
	binFileSize=0;
	fName="";
	binFName="";
	num_sectors=0;
	tracks.clear();
	layout.clear();
}
unsigned int DiscImage::Open(const std::string &fName)
{
	auto ext=GetExtension(fName);
	Capitalize(ext);
	if(".CUE"==ext)
	{
		return OpenCUE(fName);
	}
	if(".BIN"==ext)
	{
		auto withoutExt=cpputil::RemoveExtension(fName.c_str());
		auto cue=withoutExt+".cue";
		auto CUE=withoutExt+".CUE";
		auto Cue=withoutExt+".Cue";
		if(cpputil::FileExists(cue))
		{
			return OpenCUE(cue);
		}
		if(cpputil::FileExists(CUE))
		{
			return OpenCUE(CUE);
		}
		if(cpputil::FileExists(Cue))
		{
			return OpenCUE(Cue);
		}
	}
	if(".ISO"==ext)
	{
		return OpenISO(fName);
	}
	return ERROR_UNSUPPORTED;
}
unsigned int DiscImage::OpenCUE(const std::string &fName)
{
	std::ifstream ifp;
	ifp.open(fName);
	if(true!=ifp.is_open())
	{
		return ERROR_CANNOT_OPEN;
	}

	CleanUp();
	this->fName=fName;

	// https://en.wikipedia.org/wiki/Cue_sheet_(computing)
	enum
	{
		CMD_FILE,
		CMD_TRACK,
		CMD_INDEX,
		CMD_PREGAP,
		CMD_POSTGAP,
	};
	std::unordered_map <std::string,int> cmdMap;
	cmdMap["FILE"]=CMD_FILE;
	cmdMap["TRACK"]=CMD_TRACK;
	cmdMap["INDEX"]=CMD_INDEX;
	cmdMap["PREGAP"]=CMD_PREGAP;
	cmdMap["POSTGAP"]=CMD_POSTGAP;

	while(true!=ifp.eof())
	{
		std::string line;
		std::getline(ifp,line);

		std::vector <std::string> argv=cpputil::Parser(line.c_str());
		if(0<argv.size())
		{
			cpputil::Capitalize(argv[0]);
			auto found=cmdMap.find(argv[0]);
			if(cmdMap.end()==found)
			{
				continue;
			}
			switch(found->second)
			{
			case CMD_FILE:
				if(2<=argv.size())
				{
					binFName=argv[1];
				}
				break;
			case CMD_TRACK:
				if(3<=argv.size())
				{
					Track t;
					cpputil::Capitalize(argv[2]);
					if("AUDIO"==argv[2])
					{
						t.trackType=TRACK_AUDIO;
						t.sectorLength=2352;
					}
					else if(true==cpputil::StrStartsWith(argv[2],"MODE1"))
					{
						t.trackType=TRACK_MODE1_DATA;
						auto sectLenStr=cpputil::StrSkip(argv[2].c_str(),"/");
						if(nullptr!=sectLenStr)
						{
							t.sectorLength=cpputil::Atoi(sectLenStr);
						}
						else
						{
							return ERROR_SECTOR_LENGTH_NOT_GIVEN;
						}
					}
					tracks.push_back(t);
				}
				else
				{
					return ERROR_TOO_FEW_ARGS;
				}
				break;
			case CMD_INDEX:
				if(0==tracks.size())
				{
					return ERROR_TRACK_INFO_WITHOTU_TRACK;
				}
				if(3<=argv.size())
				{
					auto indexType=cpputil::Atoi(argv[1].c_str());
					MinSecFrm msf;
					if(true!=StrToMSF(msf,argv[2].c_str()))
					{
						return ERROR_INCOMPLETE_MSF;
					}
					if(0==indexType)
					{
						tracks.back().index00=msf;
					}
					else if(1==indexType)
					{
						tracks.back().start=msf;
					}
				}
				else
				{
					return ERROR_TOO_FEW_ARGS;
				}
				break;
			case CMD_PREGAP:
				if(0==tracks.size())
				{
					return ERROR_TRACK_INFO_WITHOTU_TRACK;
				}
				if(2<=argv.size())
				{
					MinSecFrm msf;
					if(true!=StrToMSF(msf,argv[1].c_str()))
					{
						return ERROR_INCOMPLETE_MSF;
					}
					tracks.back().preGap=msf;
				}
				else
				{
					return ERROR_TOO_FEW_ARGS;
				}
				break;
			case CMD_POSTGAP:
				if(0==tracks.size())
				{
					return ERROR_TRACK_INFO_WITHOTU_TRACK;
				}
				if(2<=argv.size())
				{
					MinSecFrm msf;
					if(true!=StrToMSF(msf,argv[1].c_str()))
					{
						return ERROR_INCOMPLETE_MSF;
					}
					tracks.back().postGap=msf;
				}
				else
				{
					return ERROR_TOO_FEW_ARGS;
				}
				break;
			default:
				std::cout << "Unrecognized: " << line << std::endl;
				break;
			}
		}
	}
	return OpenCUEPostProcess();
}
unsigned int DiscImage::OpenCUEPostProcess(void)
{
	if(0==tracks.size())
	{
		return ERROR_NOERROR;
	}
	if(MinSecFrm::Zero()!=tracks.front().start ||
	   MinSecFrm::Zero()!=tracks.front().preGap ||
	   MinSecFrm::Zero()!=tracks.front().index00)
	{
		return ERROR_FIRST_TRACK_NOT_STARTING_AT_00_00_00;
	}


	std::vector <std::string> binFileCandidate;
	{
		std::string path,file;
		cpputil::SeparatePathFile(path,file,fName);
		binFileCandidate.push_back(path+binFName);
	}
	{
		std::string base=cpputil::RemoveExtension(fName.c_str());
		binFileCandidate.push_back(base+".BIN");
		binFileCandidate.push_back(base+".IMG");
		binFileCandidate.push_back(base+".bin");
		binFileCandidate.push_back(base+".img");
		binFileCandidate.push_back(base+".Bin");
		binFileCandidate.push_back(base+".Img");
	}
	binFName="";
	unsigned int binLength=0;
	for(auto fn : binFileCandidate)
	{
		binLength=cpputil::FileSize(fn);
		if(0<binLength)
		{
			binFName=fn;
			break;
		}
	}
	if(0==binLength)
	{
		return ERROR_BINARY_FILE_NOT_FOUND;
	}

	if(1==tracks.size())
	{
		unsigned int numSec=(unsigned int)binLength/tracks[0].sectorLength;
		if(0!=(binLength%tracks[0].sectorLength))
		{
			return ERROR_BINARY_SIZE_NOT_SECTOR_TIMES_INTEGER;
		}
		--numSec;
		tracks[0].end=HSGtoMSF(numSec);
		tracks[0].locationInFile=0;
	}
	else
	{
		for(long long int i=1; i<(int)tracks.size(); ++i)
		{
			auto prevEndMSF=tracks[i].start-tracks[i].preGap;
			auto prevEndHSG=MSFtoHSG(prevEndMSF);
			tracks[i-1].end=HSGtoMSF(prevEndHSG-1);

			auto prevNumSec=prevEndHSG-MSFtoHSG(tracks[i-1].start);
			tracks[i].locationInFile=tracks[i-1].locationInFile+prevNumSec*tracks[i].sectorLength;
		}
		auto lastTrackBytes=binLength-tracks.back().locationInFile;
		if(0!=(lastTrackBytes%tracks.back().sectorLength))
		{
			return ERROR_BINARY_SIZE_NOT_SECTOR_TIMES_INTEGER;
		}
		auto lastTrackNumSec=(binLength-tracks.back().locationInFile)/tracks.back().sectorLength;
		auto lastSectorHSG=MSFtoHSG(tracks.back().start)+lastTrackNumSec-1;
		tracks.back().end=HSGtoMSF((unsigned int)lastSectorHSG);
	}
	if(0<tracks.size())
	{
		num_sectors=tracks.back().end.ToHSG();
	}

	for(long long int i=0; i<tracks.size(); ++i)
	{
		DiscLayout L;
		switch(tracks[i].trackType)
		{
		case TRACK_MODE1_DATA:
		case TRACK_MODE2_DATA:
			L.layoutType=LAYOUT_DATA;
			break;
		case TRACK_AUDIO:
			L.layoutType=LAYOUT_AUDIO;
			break;
		}
		L.numSectors=tracks[i].end.ToHSG()-tracks[i].start.ToHSG()+1;
		L.sectorLength=tracks[i].sectorLength;
		L.startHSG=tracks[i].start.ToHSG();
		if(0==i)
		{
			L.locationInFile=0;
			layout.push_back(L);
		}
		else
		{
			auto preGapInHSG=tracks[i].preGap.ToHSG();
			if(0<preGapInHSG)
			{
				DiscLayout preGap;
				preGap.layoutType=LAYOUT_GAP;
				preGap.sectorLength=layout.back().sectorLength;
				preGap.startHSG=tracks[i].start.ToHSG()-preGapInHSG;
				preGap.numSectors=preGapInHSG;
				preGap.locationInFile=layout.back().locationInFile+layout.back().sectorLength*layout.back().numSectors;
				layout.push_back(preGap);
			}
			L.locationInFile=layout.back().locationInFile+layout.back().sectorLength*layout.back().numSectors;
			layout.push_back(L);
		}
	}
	{
		DiscLayout L;
		L.layoutType=LAYOUT_END;
		L.sectorLength=0;
		L.numSectors=0;
		if(0<layout.size())
		{
			L.startHSG=layout.back().startHSG+layout.back().numSectors;
			L.locationInFile=layout.back().locationInFile+layout.back().sectorLength*layout.back().numSectors;
		}
		else
		{
			L.startHSG=0;
			L.locationInFile=0;
		}
		layout.push_back(L);
	}

#ifdef DEBUG_DISCIMG
	for(auto L : layout)
	{
		std::cout << "LAYOUT " << L.startHSG << " " << L.locationInFile << std::endl;
	}
#endif

	return ERROR_NOERROR;
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
	this->fName=fName;
	this->binFName=fName;
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
/* static */ unsigned int DiscImage::BCDToBin(unsigned int bin)
{
	unsigned int high=(bin>>4);
	unsigned int low=(bin&15);
	return high*10+low;
}

std::vector <unsigned char> DiscImage::ReadSectorMODE1(unsigned int HSG,unsigned int numSec) const
{
	std::ifstream ifp;
	ifp.open(binFName,std::ios::binary);

	std::vector <unsigned char> data;
	if(true==ifp.is_open() && 0<tracks.size() && (tracks[0].trackType==TRACK_MODE1_DATA || tracks[0].trackType==TRACK_MODE2_DATA))
	{
		if(HSG+numSec<=tracks[0].end.ToHSG())
		{
			auto sectorIntoTrack=HSG-tracks[0].start.ToHSG();
			auto locationInTrack=sectorIntoTrack*tracks[0].sectorLength;

			ifp.seekg(tracks[0].locationInFile+locationInTrack,std::ios::beg);
			data.resize(numSec*MODE1_BYTES_PER_SECTOR);
			if(MODE1_BYTES_PER_SECTOR==tracks[0].sectorLength)
			{
				ifp.read((char *)data.data(),MODE1_BYTES_PER_SECTOR*numSec);
			}
			else if(RAW_BYTES_PER_SECTOR==tracks[0].sectorLength)
			{
				unsigned int dataPointer=0;
				char skipper[288];
				for(int i=0; i<(int)numSec; ++i)
				{
					ifp.read(skipper,16);
					ifp.read((char *)data.data()+dataPointer,MODE1_BYTES_PER_SECTOR);
					ifp.read(skipper,288);
					dataPointer+=MODE1_BYTES_PER_SECTOR;
				}
			}
		}
	}
	return data;
}

int DiscImage::GetTrackFromMSF(MinSecFrm MSF) const
{
	if(0<tracks.size())
	{
		int tLow=0,tHigh=(int)tracks.size()-1;
		while(tLow!=tHigh)
		{
			auto tMid=(tLow+tHigh)/2;
			if(MSF<tracks[tMid].start)
			{
				tHigh=tMid;
			}
			else if(tracks[tMid].end<MSF)
			{
				tLow=tMid;
			}
			else
			{
				return tMid+1;
			}
		}
		return tLow+1;
	}
	return -1;
}

std::vector <unsigned char> DiscImage::GetWave(MinSecFrm startMSF,MinSecFrm endMSF) const
{
	std::vector <unsigned char> wave;

	std::ifstream ifp;
	ifp.open(binFName,std::ios::binary);

	if(ifp.is_open() && 0<tracks.size() && startMSF<endMSF)
	{
		auto startHSG=startMSF.ToHSG();
		auto endHSG=endMSF.ToHSG();

	#ifdef DEBUG_DISCIMG
		std::cout << "From " << startHSG << " To " << endHSG << " (" << endHSG-startHSG << ")" << std::endl;
	#endif

		for(int i=0; i+1<layout.size(); ++i)  // Condition i<layout.size()-1 will crash when layout.size()==0 because it is unsigned.
		{
			unsigned long long int readFrom=0,readTo=0;

			if(startHSG<=layout[i].startHSG)
			{
				if(LAYOUT_DATA==layout[i].layoutType)
				{
					wave.clear();
					return wave;
				}
				readFrom=layout[i].locationInFile;
			}
			else if(startHSG<layout[i+1].startHSG)
			{
				readFrom=layout[i].locationInFile+layout[i].sectorLength*(startHSG-layout[i].startHSG);
			}
			else
			{
				continue;
			}

			if(layout[i+1].startHSG<=endHSG)
			{
				readTo=layout[i+1].locationInFile;
			}
			else
			{
				readTo=layout[i].locationInFile+layout[i].sectorLength*(endHSG-layout[i].startHSG);
				i=layout.size(); // Let it loop-out.
			}

			if(readFrom<readTo)
			{
				auto readSize=(readTo-readFrom)&(~3);

			#ifdef DEBUG_DISCIMG
				std::cout << readFrom << " " << readTo << " " << readSize << " " << std::endl;
			#endif

				ifp.seekg(readFrom,std::ios::beg);

				auto curSize=wave.size();
				wave.resize(wave.size()+readSize);
				ifp.read((char *)(wave.data()+curSize),readSize);
			}
		}
	}

	return wave;
}

DiscImage::TrackTime DiscImage::DiscTimeToTrackTime(MinSecFrm discMSF) const
{
	TrackTime trackTime;
	for(int i=0; i<tracks.size(); ++i)
	{
		if(tracks[i].start<=discMSF && discMSF<=tracks[i].end)
		{
			trackTime.track=i+1;
			trackTime.MSF=discMSF-tracks[i].start;
			break;
		}
	}
	return trackTime;
}

/* static */ bool DiscImage::StrToMSF(MinSecFrm &msf,const char str[])
{
	msf.min=cpputil::Atoi(str);
	str=cpputil::StrSkip(str,":");
	if(nullptr==str)
	{
		return false;
	}
	msf.sec=cpputil::Atoi(str);
	str=cpputil::StrSkip(str,":");
	if(nullptr==str)
	{
		return false;
	}
	msf.frm=cpputil::Atoi(str);
	return true;
}
