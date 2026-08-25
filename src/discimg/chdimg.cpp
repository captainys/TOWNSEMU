/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include <string.h>
#include <stdio.h>
#include <algorithm>

#include <libchdr/chd.h>

#include "chdimg.h"


// The way the sectors of a .CHD map to the sectors of the disc is taken from MAME's
// src/lib/util/cdrom.cpp, which is where the format comes from.  See MakeTrackTable.


////////////////////////////////////////////////////////////


CHDImage::CHDImage()
{
}
CHDImage::~CHDImage()
{
	Close();
}

void CHDImage::Close(void)
{
	if(nullptr!=chd)
	{
		chd_close((chd_file *)chd);
		chd=nullptr;
	}
	tracks.clear();
	regions.clear();
	numSectors=0;
	imageSize=0;
	framesPerHunk=0;
	hunkBuf.clear();
	hunkInBuf=~0;
}
bool CHDImage::IsOpen(void) const
{
	return nullptr!=chd;
}

unsigned int CHDImage::Open(const std::string &fName)
{
	Close();

	chd_file *chdFile=nullptr;
	if(CHDERR_NONE!=chd_open(fName.c_str(),CHD_OPEN_READ,nullptr,&chdFile))
	{
		return CHDERROR_CANNOT_OPEN;
	}
	chd=(struct _chd_file *)chdFile;

	auto header=chd_get_header(chdFile);
	if(nullptr==header || FRAME_SIZE!=header->unitbytes || 0!=header->hunkbytes%FRAME_SIZE)
	{
		// Only a CD image stores 2352+96 bytes per unit.  A hard-disk image is not one.
		Close();
		return CHDERROR_NOT_A_CD;
	}
	framesPerHunk=header->hunkbytes/FRAME_SIZE;
	hunkBuf.resize(header->hunkbytes);

	auto errCode=MakeTrackTable();
	if(CHDERROR_NOERROR!=errCode)
	{
		Close();
		return errCode;
	}

	return CHDERROR_NOERROR;
}

/*! Fills the track table and the regions of the flattened image from the CHTR/CHT2 metadata.
*/
unsigned int CHDImage::MakeTrackTable(void)
{
	unsigned int LBA=0,frame=0;

	for(unsigned int trackIdx=0; trackIdx<99; ++trackIdx)
	{
		char metaData[512];
		uint32_t metaLen=0;
		unsigned int trackNum=0,numFrames=0,preGap=0,postGap=0;
		// The %s of the metadata formats are unbounded.  Give them a buffer that
		// the longest possible metadata string cannot overrun.
		char type[sizeof(metaData)]={0},subType[sizeof(metaData)]={0};
		char preGapType[sizeof(metaData)]={0},preGapSub[sizeof(metaData)]={0};

		// CHT2 carries the gaps.  CHTR is the older tag, which does not.
		if(CHDERR_NONE==chd_get_metadata((chd_file *)chd,CDROM_TRACK_METADATA2_TAG,trackIdx,metaData,sizeof(metaData)-1,&metaLen,nullptr,nullptr))
		{
			metaData[std::min<uint32_t>(metaLen,sizeof(metaData)-1)]=0;
			if(8!=sscanf(metaData,CDROM_TRACK_METADATA2_FORMAT,&trackNum,type,subType,&numFrames,&preGap,preGapType,preGapSub,&postGap))
			{
				return CHDERROR_INVALID_METADATA;
			}
		}
		else if(CHDERR_NONE==chd_get_metadata((chd_file *)chd,CDROM_TRACK_METADATA_TAG,trackIdx,metaData,sizeof(metaData)-1,&metaLen,nullptr,nullptr))
		{
			metaData[std::min<uint32_t>(metaLen,sizeof(metaData)-1)]=0;
			if(4!=sscanf(metaData,CDROM_TRACK_METADATA_FORMAT,&trackNum,type,subType,&numFrames))
			{
				return CHDERROR_INVALID_METADATA;
			}
		}
		else
		{
			break; // No more tracks.  GD-ROM and the pre-CHTR binary metadata are not supported.
		}

		Track trk;
		trk.trackNum=trackNum;
		if(0==strcmp(type,"AUDIO"))
		{
			trk.trackType=TRACK_AUDIO;
			trk.sectorLength=2352;
		}
		else if(0==strcmp(type,"MODE1"))
		{
			trk.trackType=TRACK_MODE1_DATA;
			trk.sectorLength=2048;
		}
		else if(0==strcmp(type,"MODE1_RAW"))
		{
			trk.trackType=TRACK_MODE1_DATA;
			trk.sectorLength=2352;
		}
		else if(0==strcmp(type,"MODE2") || 0==strcmp(type,"MODE2_FORM_MIX"))
		{
			trk.trackType=TRACK_MODE2_DATA;
			trk.sectorLength=2336;
		}
		else if(0==strcmp(type,"MODE2_FORM1"))
		{
			trk.trackType=TRACK_MODE2_DATA;
			trk.sectorLength=2048;
		}
		else if(0==strcmp(type,"MODE2_FORM2"))
		{
			trk.trackType=TRACK_MODE2_DATA;
			trk.sectorLength=2324;
		}
		else if(0==strcmp(type,"MODE2_RAW"))
		{
			trk.trackType=TRACK_MODE2_DATA;
			trk.sectorLength=2352;
		}
		else
		{
			return CHDERROR_UNSUPPORTED_TRACK_TYPE;
		}

		// A pre-gap of a type starting with 'V' is stored in the .CHD, as part of numFrames,
		// and then the track starts one pre-gap later than the first stored sector.
		// Any other pre-gap, and every post-gap, was never written to the .CHD and must be
		// made up out of silence.
		const bool preGapInCHD=(0<preGap && 'V'==preGapType[0]);
		const unsigned int gapBefore=(true==preGapInCHD ? 0 : preGap);

		trk.firstLBA=LBA;
		trk.dataStartLBA=LBA+gapBefore;
		trk.index01LBA=trk.dataStartLBA+(true==preGapInCHD ? preGap : 0);
		trk.numFramesInCHD=numFrames;
		trk.postGapFrames=postGap;
		trk.chdFrame=frame;
		trk.locationInImage=imageSize+(uint64_t)gapBefore*trk.sectorLength;

		{
			Region reg;
			reg.sectorLength=trk.sectorLength;
			reg.audio=(TRACK_AUDIO==trk.trackType);

			if(0<gapBefore)
			{
				reg.locationInImage=imageSize;
				reg.numBytes=(uint64_t)gapBefore*trk.sectorLength;
				reg.inCHD=false;
				regions.push_back(reg);
				imageSize+=reg.numBytes;
			}

			reg.locationInImage=imageSize;
			reg.numBytes=(uint64_t)numFrames*trk.sectorLength;
			reg.inCHD=true;
			reg.chdFrame=frame;
			regions.push_back(reg);
			imageSize+=reg.numBytes;

			if(0<postGap)
			{
				reg.locationInImage=imageSize;
				reg.numBytes=(uint64_t)postGap*trk.sectorLength;
				reg.inCHD=false;
				regions.push_back(reg);
				imageSize+=reg.numBytes;
			}
		}

		LBA=trk.dataStartLBA+numFrames+postGap;
		frame+=numFrames+(TRACK_PADDING-numFrames%TRACK_PADDING)%TRACK_PADDING;

		tracks.push_back(trk);
	}

	if(0==tracks.size())
	{
		return CHDERROR_INVALID_METADATA;
	}

	numSectors=LBA;

	return CHDERROR_NOERROR;
}

/*! Returns the 2448 bytes of a frame, or nullptr if it cannot be read.
    readLock must be held.
*/
const unsigned char *CHDImage::GetFrame(unsigned int frame) const
{
	const unsigned int hunk=frame/framesPerHunk;
	if(hunk!=hunkInBuf)
	{
		if(CHDERR_NONE!=chd_read((chd_file *)chd,hunk,hunkBuf.data()))
		{
			hunkInBuf=~0;
			return nullptr;
		}
		hunkInBuf=hunk;
	}
	return hunkBuf.data()+(frame%framesPerHunk)*(unsigned int)FRAME_SIZE;
}

bool CHDImage::Read(unsigned char *dst,uint64_t offset,uint64_t len) const
{
	memset(dst,0,len); // Whatever the .CHD does not store is silence.

	if(nullptr==chd || 0==len || imageSize<=offset)
	{
		return false;
	}
	len=std::min<uint64_t>(len,imageSize-offset);

	std::lock_guard <std::mutex> lock(readLock);

	bool succeeded=true;
	for(auto &reg : regions)
	{
		const uint64_t regEnd=reg.locationInImage+reg.numBytes;
		if(regEnd<=offset || offset+len<=reg.locationInImage)
		{
			continue;
		}
		if(true!=reg.inCHD)
		{
			continue; // Already zero-filled.
		}

		const uint64_t readFrom=std::max<uint64_t>(offset,reg.locationInImage);
		const uint64_t readTo=std::min<uint64_t>(offset+len,regEnd);

		uint64_t inRegion=readFrom-reg.locationInImage;
		for(uint64_t ptr=readFrom; ptr<readTo; )
		{
			const unsigned int sector=(unsigned int)(inRegion/reg.sectorLength);
			const unsigned int inSector=(unsigned int)(inRegion%reg.sectorLength);
			const uint64_t nBytes=std::min<uint64_t>(readTo-ptr,reg.sectorLength-inSector);

			auto frame=GetFrame(reg.chdFrame+sector);
			if(nullptr==frame)
			{
				succeeded=false;
			}
			else if(true!=reg.audio)
			{
				memcpy(dst+(ptr-offset),frame+inSector,nBytes);
			}
			else
			{
				// CD-DA is big-endian in a .CHD, and little-endian everywhere in Tsugaru.
				// Swap the sample within the sector, so that an odd offset or length cannot
				// throw the sample boundary off.
				auto out=dst+(ptr-offset);
				for(uint64_t i=0; i<nBytes; ++i)
				{
					out[i]=frame[(inSector+i)^1];
				}
			}

			ptr+=nBytes;
			inRegion+=nBytes;
		}
	}

	return succeeded;
}
