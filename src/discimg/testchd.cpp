/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */

// Tests the .CHD images of testdata/CHDIMG, which testdata/CHDIMG/makefakechd.py makes.
// Every sector of those discs is a function of its LBA, which is what the expected-value
// functions below repeat.
//
//   testchd fake2048.chd fakemixed.chd
//
// fake2048.chd  One 300-sector TYPE:MODE1 (2048 bytes/sector) track.  Zstandard (cdzs).
// fakemixed.chd TYPE:MODE1_RAW track 1 (LBA 0 to 299), then audio track 2 (LBA 300 to 474,
//               INDEX 00 at 300 and INDEX 01 at 375, its pre-gap stored in the image), then
//               audio track 3 (INDEX 01 at LBA 625, behind a 150-sector pre-gap that the image
//               does not store).  LZMA/Deflate/FLAC (cdlz,cdzl,cdfl).

#include <stdio.h>
#include <vector>

#include "discimg.h"


const unsigned int DATA_SECTORS=300;
const unsigned int TRACK2_INDEX00=300;
const unsigned int TRACK2_INDEX01=375;
const unsigned int TRACK3_GAP_START=475;
const unsigned int TRACK3_INDEX01=625;
const unsigned int DISC_SECTORS=725;


std::vector <unsigned char> ExpectedUserData(unsigned int LBA)
{
	std::vector <unsigned char> data;
	for(unsigned int i=0; i<2048; ++i)
	{
		data.push_back((LBA+i)&0xFF);
	}
	return data;
}
std::vector <unsigned char> ExpectedAudioSector(unsigned int LBA)
{
	std::vector <unsigned char> data;
	for(unsigned int i=0; i<588*2; ++i)
	{
		auto sample=(LBA*1000+i)&0x7FFF;
		data.push_back(sample&0xFF);
		data.push_back((sample>>8)&0xFF);
	}
	return data;
}

bool Same(const std::vector <unsigned char> &expected,const std::vector <unsigned char> &actual,const char label[])
{
	if(expected.size()!=actual.size())
	{
		fprintf(stderr,"%s: Expected %d bytes.  Read %d bytes.\n",label,(int)expected.size(),(int)actual.size());
		return false;
	}
	for(size_t i=0; i<expected.size(); ++i)
	{
		if(expected[i]!=actual[i])
		{
			fprintf(stderr,"%s: Byte %d is %02x.  Expected %02x.\n",label,(int)i,actual[i],expected[i]);
			return false;
		}
	}
	return true;
}

bool TestDataSectors(const DiscImage &disc,const char label[])
{
	for(unsigned int LBA : {0u,1u,137u,DATA_SECTORS-2,DATA_SECTORS-1})
	{
		auto sector=disc.ReadSectorMODE1(LBA,1);
		char msg[256];
		snprintf(msg,sizeof(msg),"%s MODE1 sector %d",label,LBA);
		if(true!=Same(ExpectedUserData(LBA),sector,msg))
		{
			return false;
		}
	}

	// Several sectors at once, which crosses the 8-frame hunks of the .CHD.
	auto sectors=disc.ReadSectorMODE1(10,20);
	if(20*2048!=sectors.size())
	{
		fprintf(stderr,"%s: Cannot read 20 sectors at once.\n",label);
		return false;
	}
	for(unsigned int i=0; i<20; ++i)
	{
		auto expected=ExpectedUserData(10+i);
		std::vector <unsigned char> actual(sectors.begin()+i*2048,sectors.begin()+(i+1)*2048);
		char msg[256];
		snprintf(msg,sizeof(msg),"%s multi-sector read, sector %d",label,10+i);
		if(true!=Same(expected,actual,msg))
		{
			return false;
		}
	}

	// Reading beyond the data track must fail rather than return garbage.
	auto pastEnd=disc.GetTracks()[0].end.ToHSG()+1;
	if(0!=disc.ReadSectorMODE1(pastEnd,1).size())
	{
		fprintf(stderr,"%s: Read past the end of the data track.\n",label);
		return false;
	}

	return true;
}

bool TestWave(const DiscImage &disc,unsigned int LBA,unsigned int numSec,const std::vector <unsigned char> &expected,const char label[])
{
	auto wave=disc.GetWave(DiscImage::HSGtoMSF(LBA),DiscImage::HSGtoMSF(LBA+numSec));
	return Same(expected,wave,label);
}

bool Test2048(const char fName[])
{
	DiscImage disc;
	auto err=disc.Open(fName);
	if(DiscImage::ERROR_NOERROR!=err)
	{
		fprintf(stderr,"%s: %s\n",fName,DiscImage::ErrorCodeToText(err));
		return false;
	}

	if(DiscImage::FILETYPE_CHD!=disc.fileType)
	{
		fprintf(stderr,"fake2048: Not recognized as a .CHD image.\n");
		return false;
	}
	if(1!=disc.GetNumTracks() || DATA_SECTORS!=disc.GetNumSectors())
	{
		fprintf(stderr,"fake2048: %d tracks and %d sectors.  Expected 1 track and %d sectors.\n",
		    disc.GetNumTracks(),disc.GetNumSectors(),DATA_SECTORS);
		return false;
	}
	if(2048!=disc.GetTracks()[0].sectorLength || DiscImage::TRACK_MODE1_DATA!=disc.GetTracks()[0].trackType)
	{
		fprintf(stderr,"fake2048: Track 1 is not a 2048-bytes/sector MODE1 track.\n");
		return false;
	}

	return TestDataSectors(disc,"fake2048");
}

bool TestMixed(const char fName[])
{
	DiscImage disc;
	auto err=disc.Open(fName);
	if(DiscImage::ERROR_NOERROR!=err)
	{
		fprintf(stderr,"%s: %s\n",fName,DiscImage::ErrorCodeToText(err));
		return false;
	}

	if(3!=disc.GetNumTracks() || DISC_SECTORS!=disc.GetNumSectors())
	{
		fprintf(stderr,"fakemixed: %d tracks and %d sectors.  Expected 3 tracks and %d sectors.\n",
		    disc.GetNumTracks(),disc.GetNumSectors(),DISC_SECTORS);
		return false;
	}

	auto &tracks=disc.GetTracks();
	if(DiscImage::TRACK_MODE1_DATA!=tracks[0].trackType || 2352!=tracks[0].sectorLength ||
	   DiscImage::TRACK_AUDIO!=tracks[1].trackType || DiscImage::TRACK_AUDIO!=tracks[2].trackType)
	{
		fprintf(stderr,"fakemixed: Wrong track types.\n");
		return false;
	}

	// A track starts at its INDEX 01, and ends one sector before the next track starts playing,
	// so the pre-gap of a track counts to the track before it.  That is where a .CUE image puts
	// the end too, and it leaves no sector of the disc outside of a track.
	const unsigned int expectedStart[3]={0,TRACK2_INDEX01,TRACK3_INDEX01};
	const unsigned int expectedEnd[3]={TRACK2_INDEX01-1,TRACK3_INDEX01-1,DISC_SECTORS-1};
	for(int i=0; i<3; ++i)
	{
		if(expectedStart[i]!=tracks[i].start.ToHSG() || expectedEnd[i]!=tracks[i].end.ToHSG())
		{
			fprintf(stderr,"fakemixed: Track %d is LBA %d to %d.  Expected %d to %d.\n",
			    i+1,tracks[i].start.ToHSG(),tracks[i].end.ToHSG(),expectedStart[i],expectedEnd[i]);
			return false;
		}
	}

	if(true!=TestDataSectors(disc,"fakemixed"))
	{
		return false;
	}

	// The 2352-byte sectors give a RAW read as well.  It drops the 12 sync bytes, and starts
	// with the four header bytes.
	{
		const unsigned int LBA=100;
		auto raw=disc.ReadSectorRAW(LBA,1);
		if(2340!=raw.size())
		{
			fprintf(stderr,"fakemixed: RAW read returned %d bytes.\n",(int)raw.size());
			return false;
		}
		const unsigned int MSF=LBA+150;
		const unsigned char expectedHeader[4]=
		{
			(unsigned char)((((MSF/(60*75))/10)<<4)|((MSF/(60*75))%10)),
			(unsigned char)(((((MSF/75)%60)/10)<<4)|(((MSF/75)%60)%10)),
			(unsigned char)((((MSF%75)/10)<<4)|((MSF%75)%10)),
			0x01,
		};
		for(int i=0; i<4; ++i)
		{
			if(expectedHeader[i]!=raw[i])
			{
				fprintf(stderr,"fakemixed: RAW header byte %d is %02x.  Expected %02x.\n",i,raw[i],expectedHeader[i]);
				return false;
			}
		}
		std::vector <unsigned char> userData(raw.begin()+4,raw.begin()+4+2048);
		if(true!=Same(ExpectedUserData(LBA),userData,"fakemixed RAW user data"))
		{
			return false;
		}
	}

	// Audio.  A .CHD keeps CD-DA big-endian, and it must come back little-endian like a BIN file.
	// The pre-gap of track 2 is in the image, so it plays back as it was recorded.
	if(true!=TestWave(disc,TRACK2_INDEX00,1,ExpectedAudioSector(TRACK2_INDEX00),"fakemixed INDEX 00 of track 2"))
	{
		return false;
	}
	{
		std::vector <unsigned char> expected;
		for(unsigned int LBA=TRACK2_INDEX01; LBA<TRACK2_INDEX01+3; ++LBA)
		{
			auto sector=ExpectedAudioSector(LBA);
			expected.insert(expected.end(),sector.begin(),sector.end());
		}
		if(true!=TestWave(disc,TRACK2_INDEX01,3,expected,"fakemixed track 2"))
		{
			return false;
		}
	}

	// The pre-gap of track 3 is in neither the .CUE nor the .CHD, and has to play as silence.
	{
		std::vector <unsigned char> expected(2352*2,0);
		if(true!=TestWave(disc,TRACK3_GAP_START,2,expected,"fakemixed pre-gap of track 3"))
		{
			return false;
		}
	}

	// Reading across the end of that pre-gap must line up with the first sector of track 3.
	{
		std::vector <unsigned char> expected(2352,0);
		auto sector=ExpectedAudioSector(TRACK3_INDEX01);
		expected.insert(expected.end(),sector.begin(),sector.end());
		if(true!=TestWave(disc,TRACK3_INDEX01-1,2,expected,"fakemixed start of track 3"))
		{
			return false;
		}
	}

	return true;
}

int main(int ac,char *av[])
{
	if(ac<3)
	{
		fprintf(stderr,"Usage: testchd fake2048.chd fakemixed.chd\n");
		return 1;
	}

	if(true!=Test2048(av[1]) || true!=TestMixed(av[2]))
	{
		return 1;
	}

	printf("CHD test passed.\n");
	return 0;
}
