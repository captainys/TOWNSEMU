/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include <algorithm>
#include <set>
#include <string.h>
#include "d77.h"



std::vector <std::string> D77File::QuickParser(const char str[])
{
	int state=0;
	std::string curStr;
	std::vector <std::string> argv;
	for(int i=0; 0!=str[i]; ++i)
	{
		if(0==state)
		{
			if(' '!=str[i] && '\t'!=str[i])
			{
				curStr.push_back(str[i]);
				state=1;
			}
		}
		else if(1==state)
		{
			if(' '==str[i] || '\t'==str[i] || 0==str[i+1])
			{
				argv.push_back((std::string &&)curStr);
				state=0;
			}
			else
			{
				curStr.push_back(str[i]);
			}
		}
	}

	return argv;
}

void D77File::Capitalize(std::string &s)
{
	for(auto &c : s)
	{
		if('a'<=c && c<='z')
		{
			c=c+'A'-'a';
		}
	}
}

/* static */ unsigned int D77File::DWordToUnsignedInt(const unsigned char ptr[])
{
	unsigned int p0=ptr[0];
	unsigned int p1=ptr[1];
	unsigned int p2=ptr[2];
	unsigned int p3=ptr[3];
	return p0+(p1<<8)+(p2<<16)+(p3<<24);
}
/* static */ unsigned short D77File::WordToUnsignedShort(const unsigned char ptr[])
{
	unsigned short p0=ptr[0];
	unsigned short p1=ptr[1];
	return p0+(p1<<8);
}
/* static */ void D77File::UnsignedIntToDWord(unsigned char ptr[],const unsigned int dat)
{
	ptr[0]=  dat    &0xff;
	ptr[1]= (dat>>8)&0xff;
	ptr[2]=(dat>>16)&0xff;
	ptr[3]=(dat>>24)&0xff;
}
/* static */ void D77File::UnsignedShortToWord(unsigned char ptr[],const unsigned int dat)
{
	ptr[0]=  dat    &0xff;
	ptr[1]= (dat>>8)&0xff;
}

////////////////////////////////////////////////////////////

D77File::D77Disk::D77Sector::D77Sector()
{
	CleanUp();
}
D77File::D77Disk::D77Sector::~D77Sector()
{
	CleanUp();
}
void D77File::D77Disk::D77Sector::CleanUp(void)
{
	cylinder=0;
	head=0;
	sector=0;
	sizeShift=0;  // 128<<sizeShift=size
	nSectorTrack=0;
	density=0;
	deletedData=0;
	crcStatus=0;

	nanosecPerByte=0;
	resampled=false;
	probLeafInTheForest=false;

	for(auto &c : reservedByte)
	{
		c=0;
	}
	sectorDataSize=0; // Including the header.
	sectorData.clear();
	unstableBytes.clear();
}
bool D77File::D77Disk::D77Sector::SameCHR(const D77Sector &s) const
{
	return (cylinder==s.cylinder &&
	        head==s.head &&
	        sector==s.sector);
}
bool D77File::D77Disk::D77Sector::SameCHRN(const D77Sector &s) const
{
	return (cylinder==s.cylinder &&
	        head==s.head &&
	        sector==s.sector &&
	        sizeShift==s.sizeShift);
}
bool D77File::D77Disk::D77Sector::SameCHRNandActualSize(const D77Sector &s) const
{
	return SameCHRN(s) && sectorData.size()==s.sectorData.size();
}
bool D77File::D77Disk::D77Sector::Make(int trk,int sid,int secId,int secSize)
{
	int sizeShift=0;
	switch(secSize)
	{
	default:
		fprintf(stderr,"Size needs to be 128,256,512, or 1024.\n");
		return false;
	case 128:
		sizeShift=0;
		break;
	case 256:
		sizeShift=1;
		break;
	case 512:
		sizeShift=2;
		break;
	case 1024:
		sizeShift=3;
		break;
	}
	CleanUp();
	cylinder=trk;
	head=sid;
	sector=secId;
	this->sizeShift=sizeShift;
	sectorDataSize=secSize;
	sectorData.resize(secSize);
	resampled=false;
	for(auto &b : sectorData)
	{
		b=0;
	}
	return true;
}

////////////////////////////////////////////////////////////

D77File::D77Disk::D77Track::D77Track()
{
}
D77File::D77Disk::D77Track::~D77Track()
{
	CleanUp();
}
void D77File::D77Disk::D77Track::CleanUp(void)
{
	for(auto &s : sector)
	{
		s.CleanUp();
	}
	sector.clear();
	trackImage.clear();
	IDMark.clear();
	FDCStatusAfterTrackRead=0;
}

void D77File::D77Disk::D77Track::PrintInfo(void) const
{
	if(0<sector.size())
	{
		printf("Cyl:%d Head:%d\n",(int)sector[0].cylinder,(int)sector[0].head);
		for(int i=0; i<sector.size(); ++i)
		{
			if(0<i && 0==i%8)
			{
				printf("\n");
			}
			auto &s=sector[i];
			printf("%02x[%3d]",s.sector,(int)s.sectorData.size());
			if(0!=s.crcStatus)
			{
				printf("C");
			}
			if(0!=s.deletedData)
			{
				printf("D");
			}
			printf(" ");
		}
		printf("\n");
	}
	else
	{
		printf("No sector (Unformatted?)\n");
	}
}

void D77File::D77Disk::D77Track::PrintDetailedInfo(void) const
{
	if(0<sector.size())
	{
		printf("Cyl:%d Head:%d\n",(int)sector[0].cylinder,(int)sector[0].head);
		for(int i=0; i<sector.size(); ++i)
		{
			auto &s=sector[i];
			printf("%3d:%02x[%4d] ",i+1,s.sector,(int)s.sectorData.size());

			printf("CHRN:%02x%02x%02x%02x",s.cylinder,s.head,s.sector,s.sizeShift);

			if(0!=s.crcStatus)
			{
				printf(" CRC");
			}
			else
			{
				printf("    ");
			}
			if(0!=s.deletedData)
			{
				printf(" DDM");
			}
			else
			{
				printf("   ");
			}
			printf("\n");
		}
	}
	else
	{
		printf("No sector (Unformatted?)\n");
	}
}

std::vector <D77File::D77Disk::D77Track::SectorLocation> D77File::D77Disk::D77Track::Find(const std::vector <unsigned char> &pattern) const
{
	std::vector <SectorLocation> found;
	
	for(int sectorPos=0; sectorPos<sector.size(); ++sectorPos)
	{
		auto &s=sector[sectorPos];
		for(int i=0; i<=s.sectorData.size()-pattern.size(); ++i)
		{
			bool diff=false;
			for(int j=0; j<pattern.size(); ++j)
			{
				if(s.sectorData[i+j]!=pattern[j])
				{
					diff=true;
					break;
				}
			}
			if(true!=diff)
			{
				SectorLocation f;
				f.sectorPos=sectorPos;
				f.track=s.cylinder;
				f.side=s.head;
				f.sector=s.sector;
				f.addr=i;
				found.push_back(f);
			}
		}
	}
	return found;
}

void D77File::D77Disk::D77Track::Replace(SectorLocation pos,const std::vector <unsigned char> &pattern)
{
	if(0<=pos.sectorPos && pos.sectorPos<sector.size())
	{
		auto &s=sector[pos.sectorPos];
		for(int i=0; i<pattern.size() && pos.addr+i<s.sectorData.size(); ++i)
		{
			s.sectorData[pos.addr+i]=pattern[i];
		}
	}
}

D77File::D77Disk::D77Sector *D77File::D77Disk::D77Track::FindSector(int sectorId)
{
	for(auto &s : sector)
	{
		if(s.sector==sectorId)
		{
			return &s;
		}
	}
	return nullptr;
}
const D77File::D77Disk::D77Sector *D77File::D77Disk::D77Track::FindSector(int sectorId) const
{
	for(auto &s : sector)
	{
		if(s.sector==sectorId)
		{
			return &s;
		}
	}
	return nullptr;
}

int D77File::D77Disk::D77Track::GetTrackNumber(void) const
{
	if(0<sector.size())
	{
		return sector[0].cylinder;
	}
	return -1;
}
void D77File::D77Disk::D77Track::WriteTrack(int nSec,const D77Disk::D77Sector sec[])
{
	this->sector.resize(nSec);
	for(int s=0; s<nSec; ++s)
	{
		this->sector[s]=sec[s];
	}

	// Once formatted, RDD track image and RDD IDMarks are invalid.
	trackImage.clear();
	IDMark.clear();
}
bool D77File::D77Disk::D77Track::AddSector(int trk,int sid,int secId,int secSize)
{
	D77Sector newSec;
	if(true==newSec.Make(trk,sid,secId,secSize))
	{
		this->sector.push_back(newSec);
		for(auto &s : this->sector)
		{
			s.nSectorTrack=(unsigned short)this->sector.size();
		}
		return true;
	}
	return false;
}
bool D77File::D77Disk::D77Track::SetSectorCHRN(int secId,int C,int H,int R,int N)
{
	if(1<=secId && secId<=this->sector.size())
	{
		sector[secId-1].cylinder=C;
		sector[secId-1].head=H;
		sector[secId-1].sector=R;
		sector[secId-1].sizeShift=N;
		sector[secId-1].sectorDataSize=(128<<(N&3));
		sector[secId-1].sectorData.resize(sector[secId-1].sectorDataSize);
		return true;
	}
	return false;
}
int D77File::D77Disk::D77Track::GetSide(void) const
{
	if(0<sector.size())
	{
		return sector[0].head;
	}
	return -1;
}

bool D77File::D77Disk::D77Track::SuspectedLeafInTheForest(void) const
{
	for(auto &s : sector)
	{
		if(true!=sector[0].SameCHR(s))
		{
			return false;
		}
	}
	return true;
}

void D77File::D77Disk::D77Track::IdentifyUnstableByte(void)
{
	if(true==SuspectedLeafInTheForest())
	{
		return;
	}


	for(auto &s : sector)
	{
		s.unstableBytes.clear();
	}

	// If the value changes, can be an unstable byte.
	for(int i=0; i<sector.size(); ++i)
	{
		auto &s0=sector[i];
		if(0<s0.unstableBytes.size()) // Already taken care?
		{
			continue;
		}

		s0.unstableBytes.resize(s0.sectorData.size());
		for(int i=0; i<s0.unstableBytes.size(); ++i)
		{
			s0.unstableBytes[i]=false;
		}

		for(int j=i+1; j<sector.size(); ++j)
		{
			auto &s1=sector[j];
			if(true==s0.SameCHRNandActualSize(s1))
			{
				for(int k=0; k<s0.sectorData.size(); ++k)
				{
					if(s0.sectorData[k]!=s1.sectorData[k])
					{
						s0.unstableBytes[k]=true;
					}
				}
			}
		}

		// Once identified, copy to all multi-sample sectors.
		for(int j=i+1; j<sector.size(); ++j)
		{
			auto &s1=sector[j];
			if(true==s0.SameCHRNandActualSize(s1))
			{
				s1.unstableBytes=s0.unstableBytes;
			}
		}
	}
	UnidentifyUnstableByteForContinuousData();
}

void D77File::D77Disk::D77Track::IdentifyUnstableByteRDD(void)
{
	for(auto &s : sector)
	{
		s.unstableBytes.clear();
	}
	// If the value changes, can be an unstable byte.
	for(int i=0; i<sector.size(); ++i)
	{
		auto &s0=sector[i];
		if(0<s0.unstableBytes.size()) // Already taken care?
		{
			continue;
		}
		if(true!=s0.resampled)
		{
			continue;
		}

		s0.unstableBytes.resize(s0.sectorData.size());
		for(int i=0; i<s0.unstableBytes.size(); ++i)
		{
			s0.unstableBytes[i]=false;
		}

		for(int j=i+1; j<sector.size(); ++j)
		{
			auto &s1=sector[j];
			if(true==s1.resampled &&
			   true==s0.SameCHRNandActualSize(s1))
			{
				for(int k=0; k<s0.sectorData.size(); ++k)
				{
					if(s0.sectorData[k]!=s1.sectorData[k])
					{
						s0.unstableBytes[k]=true;
					}
				}
			}
		}

		// Once identified, copy to all multi-sample sectors.
		for(int j=i+1; j<sector.size(); ++j)
		{
			auto &s1=sector[j];
			if(true==s1.resampled &&
			   true==s0.SameCHRNandActualSize(s1))
			{
				s1.unstableBytes=s0.unstableBytes;
			}
		}
	}
	UnidentifyUnstableByteForContinuousData();
}

void D77File::D77Disk::D77Track::UnidentifyUnstableByteForContinuousData(void)
{
	// Trailing stable bytes can flip between 0xF6 and 0x00, therefore can be identified as unstable bytes, but must not be randomized.
	// If the same byte repeats more than 4 bytes, it must be a stable byte,
	for(auto &s : sector)
	{
		if(s.sectorData.size()==s.unstableBytes.size())
		{
			for(int i=0; i+4<=s.sectorData.size(); ++i)
			{
				int j=i+1;
				for(; j<s.sectorData.size() && s.sectorData[i]==s.sectorData[j]; ++j)
				{
				}
				if(i+4<=j)
				{
					for(; i<j; ++i)
					{
						s.unstableBytes[i]=false;
					}
					--i;
				}
			}
		}
	}
}

std::vector <D77File::D77Disk::D77Track::SectorLocation> D77File::D77Disk::D77Track::AllSector(void) const
{
	std::vector <SectorLocation> secList;
	int pos=0;
	for(auto &s : sector)
	{
		SectorLocation loc;
		loc.sectorPos=pos;
		loc.track=s.cylinder;
		loc.side=s.head;
		loc.sector=s.sector;
		loc.addr=0;
		secList.push_back(loc);
		++pos;
	}
	return secList;
}

////////////////////////////////////////////////////////////

void D77File::D77Disk::D77Header::CleanUp(void)
{
	writeProtected=0;
	mediaType=0;
	diskSize=0;

	for(auto &c : diskName)
	{
		c=0;
	}
	for(auto &c : reservedByte)
	{
		c=0;
	}
}

////////////////////////////////////////////////////////////

D77File::D77Disk::D77Disk(void)
{
	CleanUp();
}
D77File::D77Disk::~D77Disk(void)
{
	CleanUp();
}
void D77File::D77Disk::CleanUp(void)
{
	header.CleanUp();
	for(auto &t : track)
	{
		t.CleanUp();
	}
	rddDiskName="";
	modified=false;
}

void D77File::D77Disk::PrintInfo(void) const
{
	printf("Disk Name:[%s]\n",header.diskName);
	printf("Reserved Bytes:");
	for(int i=0; i<9; ++i)
	{
		printf(" %02x",header.reservedByte[i]);
	}
	printf("\n");
	printf("Write Protect Flag:%02x\n",header.writeProtected);
	printf("Media Type:%02x\n",header.mediaType);
	for(auto &t : track)
	{
		t.PrintInfo();
	}
}

D77File::D77Disk::D77Track *D77File::D77Disk::FindEditableTrack(int cyl,int side)
{
	for(auto &t : track)
	{
		if(true!=t.sector.empty() &&
		   t.sector[0].cylinder==cyl &&
		   t.sector[0].head==side)
		{
			return &t;
		}
	}
	return nullptr;
}

D77File::D77Disk::D77Track *D77File::D77Disk::GetEditableTrack(int trk,int side)
{
	int t=trk*2+side;
	if(0<=t && t<track.size())
	{
		return &track[t];
	}
	return nullptr;
}

const D77File::D77Disk::D77Track *D77File::D77Disk::FindTrack(int cyl,int side) const
{
	for(auto &t : track)
	{
		if(true!=t.sector.empty() &&
		   t.sector[0].cylinder==cyl &&
		   t.sector[0].head==side)
		{
			return &t;
		}
	}
	return nullptr;
}

const D77File::D77Disk::D77Track *D77File::D77Disk::GetTrack(int trk,int side) const
{
	int t=trk*2+side;
	if(0<=t && t<track.size())
	{
		return &track[t];
	}
	return nullptr;
}

D77File::D77Disk::D77Track *D77File::D77Disk::FindTrack(int cyl,int side)
{
	for(auto &t : track)
	{
		if(true!=t.sector.empty() &&
		   t.sector[0].cylinder==cyl &&
		   t.sector[0].head==side)
		{
			return &t;
		}
	}
	return nullptr;
}

D77File::D77Disk::D77Track *D77File::D77Disk::GetTrack(int trk,int side)
{
	int t=trk*2+side;
	if(0<=t && t<track.size())
	{
		return &track[t];
	}
	return nullptr;
}

std::vector <unsigned char> D77File::D77Disk::MakeD77Image(void) const
{
	std::vector <unsigned char> d77Img;

	for(int i=0; i<16; ++i)
	{
		d77Img.push_back(header.diskName[i]);
	}
	d77Img.push_back(0);
	for(int i=0; i<9; ++i)
	{
		d77Img.push_back(header.reservedByte[i]);
	}
	d77Img.push_back(header.writeProtected);
	d77Img.push_back(header.mediaType);

	auto diskSizePtr=d77Img.size();
	d77Img.push_back(0);
	d77Img.push_back(0);
	d77Img.push_back(0);
	d77Img.push_back(0);

	long long int trackPtr[164];
	for(int i=0; i<164; ++i)
	{
		trackPtr[i]=d77Img.size();
		d77Img.push_back(0);
		d77Img.push_back(0);
		d77Img.push_back(0);
		d77Img.push_back(0);
	}
	int trackIdx=0;
	for(auto &t : track)
	{
		if(0<t.sector.size())
		{
			UnsignedIntToDWord(d77Img.data()+trackPtr[trackIdx],(unsigned int)d77Img.size());
			for(auto &s : t.sector)
			{
				unsigned char buf[2];
				d77Img.push_back(s.cylinder);
				d77Img.push_back(s.head);
				d77Img.push_back(s.sector);
				d77Img.push_back(s.sizeShift);
				UnsignedShortToWord(buf,s.nSectorTrack);
				d77Img.push_back(buf[0]);
				d77Img.push_back(buf[1]);
				d77Img.push_back(s.density);
				d77Img.push_back(s.deletedData);
				d77Img.push_back(s.crcStatus!=0 ? 0xb0 : 0);
				for(int i=0; i<5; ++i)
				{
					d77Img.push_back(s.reservedByte[i]);
				}
				UnsignedShortToWord(buf,s.sectorDataSize);
				d77Img.push_back(buf[0]);
				d77Img.push_back(buf[1]);
				for(auto d : s.sectorData)
				{
					d77Img.push_back(d);
				}
			}
		}
		else
		{
			// Inserting an unformatted track.
			// Just make a NULL pointer.
			UnsignedIntToDWord(d77Img.data()+trackPtr[trackIdx],0);
		}
		++trackIdx;
	}
	UnsignedIntToDWord(d77Img.data()+diskSizePtr,(unsigned int)d77Img.size());

	return d77Img;
}

std::vector <unsigned char> D77File::D77Disk::MakeRDDImage(void) const
{
	std::vector <unsigned char> bin;

	// Signature
	{
		for(auto c : "REALDISKDUMP")
		{
			bin.push_back(c);
		}
		bin.push_back(0);
		bin.push_back(0);
		bin.push_back(0);
		bin.push_back(0);
	}

	// Begin Disk
	bin.push_back(0x00);  // Begin disk
	bin.push_back(0x00);  // Version
	bin.push_back(header.mediaType);
	unsigned char flags=0;
	if(0!=header.writeProtected)
	{
		flags|=1;
	}
	bin.push_back(flags);
	while(0!=(bin.size()&0x0F))
	{
		bin.push_back(0);
	}

	// Disk Name
	if(0<rddDiskName.size())
	{
		for(int i=0; i<32; ++i)
		{
			if(i<rddDiskName.size())
			{
				bin.push_back(rddDiskName[i]);
			}
			else
			{
				bin.push_back(0);
			}
		}
	}
	else
	{
		for(int i=0; i<17; ++i)
		{
			bin.push_back(header.diskName[i]);
		}
		for(int i=17; i<32; ++i)
		{
			bin.push_back(0);
		}
	}

	for(unsigned int trk=0; trk<track.size(); ++trk)
	{
		auto C=trk/2;
		auto H=trk%2;

		// Begin Track
		bin.push_back(0x01);
		bin.push_back(C);
		bin.push_back(H);
		while(0!=(bin.size()&0x0F))
		{
			bin.push_back(0);
		}

		for(auto IDMark : track[trk].IDMark)
		{
			bin.insert(bin.end(),IDMark.data.begin(),IDMark.data.end());
			IDMark.EncodeRDDUnstableBytes(bin);
		}

		if(0<track[trk].trackImage.size())
		{
			// Track Read
			bin.push_back(0x04);
			bin.push_back(C);
			bin.push_back(H);
			bin.push_back(track[trk].FDCStatusAfterTrackRead);
			while(14!=(bin.size()&0x0F))
			{
				bin.push_back(0);
			}
			bin.push_back(track[trk].trackImage.size()&0xFF);
			bin.push_back((track[trk].trackImage.size()>>8)&0xFF);

			for(auto d : track[trk].trackImage)
			{
				bin.push_back(d);
			}
			while(0!=(bin.size()&0x0F))
			{
				bin.push_back(0);
			}
		}
		track[trk].EncodeRDDUnstableBytes(bin);

		for(auto &s : track[trk].sector)
		{
			// Data
			bin.push_back(0x03);
			bin.push_back(s.cylinder);
			bin.push_back(s.head);
			bin.push_back(s.sector);
			bin.push_back(s.sizeShift);

			// Make up MB8877 Status Byte
			unsigned char st=0;
			if(0!=s.deletedData)
			{
				st|=MB8877_STATUS_DELETED_DATA; // Deleted Data or Record Type
			}
			if(0xF0==s.crcStatus)
			{
				st|=MB8877_STATUS_RECORD_NOT_FOUND; // Record Not Found
			}
			else if(0!=s.crcStatus)
			{
				st|=MB8877_STATUS_CRC; // CRC Error
			}
			bin.push_back(st);

			unsigned char flags=0;
			if(0!=s.density)
			{
				flags|=1; // Single density
			}
			if(true==s.resampled)
			{
				flags|=2;
			}
			if(true==s.probLeafInTheForest)
			{
				flags|=4;
			}
			bin.push_back(flags);

			while(0x0B!=(bin.size()&0x0F))
			{
				bin.push_back(0);
			}

			unsigned int millisec=s.nanosecPerByte;
			millisec*=s.sectorData.size();
			millisec/=1000;
			bin.push_back(millisec&0xFF);
			bin.push_back((millisec>>8)&0xFF);
			bin.push_back((millisec>>24)&0xFF);

			bin.push_back(s.sectorData.size()&0xFF);
			bin.push_back((s.sectorData.size()>>8)&0xFF);

			for(auto c : s.sectorData)
			{
				bin.push_back(c);
			}
			while(0!=(bin.size()&0x0F))
			{
				bin.push_back(0);
			}

			s.EncodeRDDUnstableBytes(bin);
		}

		// End Track
		bin.push_back(0x05);
		while(0!=(bin.size()&0x0F))
		{
			bin.push_back(0);
		}
	}

	// End Disk
	bin.push_back(0x06);
	while(0!=(bin.size()&0x0F))
	{
		bin.push_back(0);
	}

	return bin;
}

std::vector <unsigned char> D77File::D77Disk::MakeRawImage(void) const
{
	std::vector <unsigned char> rawImg;

	for(auto &t : track)
	{
		auto allSector=t.AllSector();
		// Bubble Sort: I hope number of sectors is not thousands.
		for(int i=0; i<allSector.size(); i++)
		{
			for(int j=i+1; j<allSector.size(); ++j)
			{
				if(allSector[i].sector>allSector[j].sector)
				{
					std::swap(allSector[i],allSector[j]);
				}
			}
		}

		for(auto s : allSector)
		{
			auto sectorDump=ReadSector(s.track,s.side,s.sector);
			rawImg.insert(rawImg.end(),sectorDump.begin(),sectorDump.end());
		}
	}

	return rawImg;
}

bool D77File::D77Disk::IsModified(void) const
{
	return modified;
}
void D77File::D77Disk::ClearModified(void)
{
	modified=false;
}
void D77File::D77Disk::SetModified(void)
{
	modified=true;
}

bool D77File::D77Disk::SetD77Image(const unsigned char d77Img[],bool verboseMode)
{
	CleanUp();

	for(int i=0; i<17; ++i)
	{
		header.diskName[i]=d77Img[i];
	}
	for(int i=0; i<9; ++i)
	{
		header.reservedByte[i]=d77Img[0x11+i];
	}
	header.writeProtected=d77Img[0x1a];
	header.mediaType=d77Img[0x1b];
	header.diskSize=DWordToUnsignedInt(d77Img+0x1c);

	auto trackTablePtr=d77Img+0x20;
	// According to the D77 format, I need to find the first non-zero offset
	// to know how many tracks in this 
	std::vector <long long int> trackOffset;
	long long int headerEndPtr=DAMN_BIG_NUMBER;
	for(long long int i=0; 0x20+i<header.diskSize && 0x20+i<headerEndPtr; i+=4)
	{
		auto offset=DWordToUnsignedInt(d77Img+0x20+i);
		if(0!=offset && DAMN_BIG_NUMBER==headerEndPtr)
		{
			headerEndPtr=offset;
		}
		trackOffset.push_back(offset);
	}

	if(true==verboseMode)
	{
		printf("Header says %d tracks.\n",(int)trackOffset.size());
	}
	int trackCount=0;
	long long int prevOffset=0;
	int nUnformat=0;
	for(auto offset : trackOffset)
	{
		if(0!=offset)
		{
			if(true==verboseMode)
			{
				for(int i=0; i<nUnformat; ++i)
				{
					printf("Unformatted Track\n");
				}
				printf("Reading Track... DiskOffset=%08x Count=%d ",(int)offset,trackCount);
				if(0<trackCount)
				{
					printf("Step=%04x",(int)(offset-prevOffset));
				}
				printf("\n");
			}
			for(int i=0; i<nUnformat; ++i)
			{
				D77Track trk;
				track.push_back(trk);
			}
			nUnformat=0;
			track.push_back(MakeTrackData(d77Img+offset,d77Img+header.diskSize));
			track.back().IdentifyUnstableByte();
			prevOffset=offset;
			++trackCount;
		}
		else
		{
			++nUnformat;
		}
	}
	return true;
}

bool D77File::D77Disk::SetRDDImage(size_t &bytesUsed,size_t len,const unsigned char rdd[],bool verboseMode)
{
	CleanUp();

	size_t ptr=0;
	bytesUsed=0;

	if(0!=strncmp((const char *)rdd,"REALDISKDUMP",12))
	{
		if(true==verboseMode)
		{
			fprintf(stderr,"Wrong RDD Signature.\n");
		}
		return false;
	}

	// Begin Disk
	ptr+=16;
	if(0!=rdd[ptr])
	{
		if(true==verboseMode)
		{
			fprintf(stderr,"Begin Disk (00h) does not follow the signature.\n");
		}
		return false;
	}

	unsigned int version=rdd[ptr+1];
	header.mediaType=rdd[ptr+2];
	header.writeProtected=(rdd[ptr+3]&1);

	// Disk Name
	ptr+=16;
	strncpy(header.diskName,(const char *)rdd+ptr,16);
	for(int i=0; i<32; ++i)
	{
		rddDiskName.push_back(rdd[ptr+i]);
	}


	// Begin Track or can be Track Read
	ptr+=32;
	unsigned int C=0,H=0;
	D77Track *trkPtr=nullptr;
	HasUnstableByteFlags *lastData=nullptr;
	while(ptr+16<=len)
	{
		switch(rdd[ptr])
		{
		case 1: // Begin Track
			C=rdd[ptr+1];
			H=rdd[ptr+2];
			ptr+=16;
			if(track.size()<=C*2+H)
			{
				track.resize(C*2+H+1);
				trkPtr=&track.back();
			}
			lastData=nullptr;
			break;
		case 2: // ID Mark
			if(nullptr!=trkPtr)
			{
				D77IDMark IDMark;
				for(int i=0; i<16; ++i)
				{
					IDMark.data[i]=rdd[ptr+i];
				}
				trkPtr->IDMark.push_back(IDMark);
				lastData=&trkPtr->IDMark.back();
			}
			else
			{
				if(true==verboseMode)
				{
					fprintf(stderr,"Sector data without a track.\n");
				}
				return false;
			}
			ptr+=16;
			break;
		case 3: // Sector Data
			if(nullptr!=trkPtr)
			{
				auto cc=rdd[ptr+1];
				auto hh=rdd[ptr+2];
				auto rr=rdd[ptr+3];
				auto nn=rdd[ptr+4];
				auto FDCStatus=rdd[ptr+5];
				auto flags=rdd[ptr+6];
				unsigned int millisec=rdd[ptr+0x0B]|(rdd[ptr+0x0C]<<8)|(rdd[ptr+0x0D]<<16);
				unsigned int realLen=rdd[ptr+0x0E]|(rdd[ptr+0x0F]<<8);

				D77Sector sector;
				sector.Make(cc,hh,rr,128<<(nn&3));
				sector.resampled=(0!=(flags&2));
				sector.probLeafInTheForest=(0!=(flags&4));
				sector.density=(0!=(flags&0) ? D77_DENSITY_FM : 0x00);
				sector.deletedData=((FDCStatus & MB8877_STATUS_DELETED_DATA) ? D77_DATAMARK_DELETED : 0);

				if(0!=(FDCStatus&MB8877_STATUS_CRC))
				{
					sector.crcStatus=D77_SECTOR_STATUS_CRC; // CRC Error
				}
				else if(0!=(FDCStatus&MB8877_STATUS_RECORD_NOT_FOUND))
				{
					sector.crcStatus=D77_SECTOR_STATUS_RECORD_NOT_FOUND; // Record Not Found;
				}
				sector.nanosecPerByte=millisec*1000/std::max<int>(realLen,1);
				sector.sectorData.resize(realLen);
				for(size_t i=0; i<realLen; ++i)
				{
					sector.sectorData[i]=rdd[ptr+16+i];
				}
				trkPtr->sector.push_back(sector);
				lastData=&trkPtr->sector.back();

				for(auto &s : trkPtr->sector) // **** D77!
				{
					s.nSectorTrack=trkPtr->sector.size();
				}

				ptr+=16+(realLen+15)&0xFFF0;
			}
			else
			{
				if(true==verboseMode)
				{
					fprintf(stderr,"Sector data without a track.\n");
				}
				return false;
			}
			break;
		case 4: // Track Read
			if(C!=rdd[ptr+1] || H!=rdd[ptr+2])
			{
				if(true==verboseMode)
				{
					fprintf(stderr,"Track dump for wrong track.\n");
				}
				return false;
			}
			else if(nullptr!=trkPtr)
			{
				unsigned int realLen=rdd[ptr+0x0E]|(rdd[ptr+0x0F]<<8);
				for(size_t i=0; i<realLen; ++i)
				{
					trkPtr->trackImage.push_back(rdd[ptr+16+i]);
				}
				lastData=trkPtr;
				ptr+=16+(realLen+15)&0xFFF0;
			}
			else
			{
				if(true==verboseMode)
				{
					fprintf(stderr,"Track dump without a track.\n");
				}
				return false;
			}
			break;
		case 5: // End of Track
			trkPtr->IdentifyUnstableByteRDD();
			trkPtr=nullptr;
			ptr+=16;
			break;
		case 6: // End of Disk.  Force it to be done
			bytesUsed=ptr+16;
			ptr=len;
			break;
		case 0x10: // Unstable byte flags
			if(nullptr==lastData)
			{
				if(true==verboseMode)
				{
					fprintf(stderr,"Unstable byte flags without a data.\n");
				}
				return false;
			}
			else
			{
				unsigned int sz=rdd[ptr+2];
				sz<<=8;
				sz|=rdd[ptr+1];

				lastData->unstableBytes.resize(sz);
				for(unsigned int i=0; i<sz; ++i)
				{
					auto flag=rdd[ptr+16+(i/8)]&(1<<(i%8));
					lastData->unstableBytes[i]=(0!=flag);
				}

				unsigned int skipSize=((sz+15)&~15);
				ptr+=16+skipSize;
			}
			break;
		default:
			if(true==verboseMode)
			{
				fprintf(stderr,"Undefined RDD tag %02xh at %xh\n",rdd[ptr],ptr);
			}
			return false;
		}
	}

	return true;
}

D77File::D77Disk::D77Track D77File::D77Disk::MakeTrackData(const unsigned char trackPtr[],const unsigned char *lastPtr) const
{
	D77Track trk;

	long long int sectorCount=0;
	long long int nSectorTrack=DAMN_BIG_NUMBER;
	for(long long int sectorOffset=0; trackPtr+sectorOffset<lastPtr && sectorCount<nSectorTrack; )
	{
		auto sectorPtr=trackPtr+sectorOffset;
		long long int sectorNByte=WordToUnsignedShort(sectorPtr+0x0e);
		if(lastPtr<=sectorPtr+0x10)
		{
			printf("Broken Data. Overflow.\n");
			break;
		}

		D77Disk::D77Sector sec;
		sec.cylinder=sectorPtr[0];
		sec.head=sectorPtr[1];
		sec.sector=sectorPtr[2];
		if(lastPtr<=sectorPtr+sectorNByte)
		{
			printf("Broken Data. Overflow.\n");
			printf("  Cyl:%d Head:%d Sec:%d\n",sec.cylinder,sec.head,sec.sector);
			break;
		}

		sec.sizeShift=sectorPtr[3];
		sec.nSectorTrack=WordToUnsignedShort(sectorPtr+4);
		if(DAMN_BIG_NUMBER==nSectorTrack)
		{
			nSectorTrack=sec.nSectorTrack;
		}
		else if(nSectorTrack!=sec.nSectorTrack)
		{
			printf("Broken Data.  Number of sectors inconsistent within a track.\n");
			printf("  Cyl:%d Head:%d Sec:%d\n",sec.cylinder,sec.head,sec.sector);
			printf("  Previous number of sectors for the track:%d\n",(int)nSectorTrack);
			printf("  Number of sectors for the track:%d\n",sec.nSectorTrack);
			break;
		}


		if(0==nSectorTrack)
		{
			// printf("Unformatted Track.\n");
			break;
		}


		sec.density=sectorPtr[6];
		sec.deletedData=sectorPtr[7];
		sec.crcStatus=sectorPtr[8];
		for(int i=0; i<5; ++i)
		{
			sec.reservedByte[i]=sectorPtr[9+i];
		}
		sec.sectorDataSize=WordToUnsignedShort(sectorPtr+0x0e);

		auto nextSectorOffset=sectorOffset+0x10+sec.sectorDataSize;

		long long int sizeFromShift=(128<<(sec.sizeShift&3));
		long long int sizeFromDataSize=sec.sectorDataSize;
		if(sizeFromShift!=sizeFromDataSize)
		{
			printf("Broken Data.  Sector size doesn't match number of bytes for the sector.\n");
			printf("  Cyl:%d Head:%d Sec:%d\n",sec.cylinder,sec.head,sec.sector);
			printf("  From shift:%d\n",(int)sizeFromShift);
			printf("  From data size:%d\n",(int)sizeFromDataSize);
			break;
		}

		sec.sectorData.resize(sizeFromShift);
		for(int i=0; i<sizeFromShift; ++i)
		{
			sec.sectorData[i]=sectorPtr[0x10+i];
		}
		trk.sector.push_back((D77Disk::D77Sector &&)sec);

		++sectorCount;
		sectorOffset=nextSectorOffset;
	}

	return trk;
}

void D77File::D77Disk::CreateStandardFormatted(void)
{
	CleanUp();

	header.CleanUp();
	header.diskName[0]='F';
	header.diskName[1]='M';
	header.diskName[2]='7';
	header.diskName[3]='D';
	header.diskName[4]='I';
	header.diskName[5]='S';
	header.diskName[6]='K';
	header.diskName[7]=0;
	for(int i=0; i<80; ++i)
	{
		const int cyl=i/2;
		const int head=i%2;

		D77Disk::D77Track trk;
		for(int j=1; j<=16; ++j)
		{
			D77Disk::D77Sector sec;
			sec.cylinder=cyl;
			sec.head=head;
			sec.sector=j;
			sec.sizeShift=1;
			sec.nSectorTrack=16;
			sec.sectorDataSize=256;
			sec.sectorData.resize(256);
			for(auto &b : sec.sectorData)
			{
				b=0xff;
			}
			trk.sector.push_back((D77Disk::D77Sector &&)sec);
		}

		track.push_back((D77Disk::D77Track &&)trk);
	}

	auto FAT=ReadSector(1,0,1);
	FAT[0]=0;
	WriteSector(1,0,1,FAT.size(),FAT.data());

	auto ID=ReadSector(0,0,3);
	for(auto &b : ID)
	{
		b=0;
	}
	ID[0]=0x53;
	ID[1]=0x20;
	ID[2]=0x20;
	WriteSector(0,0,3,ID.size(),ID.data());
}

void D77File::D77Disk::CreateUnformatted(int nTrack,const char diskName[])
{
	CleanUp();

	header.CleanUp();
	if(nullptr==diskName)
	{
		header.diskName[0]='F';
		header.diskName[1]='M';
		header.diskName[2]='7';
		header.diskName[3]='D';
		header.diskName[4]='I';
		header.diskName[5]='S';
		header.diskName[6]='K';
		header.diskName[7]=0;
	}
	else
	{
		for(auto &c : header.diskName)
		{
			c=0;
		}
		for(int i=0; i<16 && 0!=diskName[i]; ++i)
		{
			header.diskName[i]=diskName[i];
		}
	}

	for(int i=0; i<nTrack; ++i)
	{
		D77Disk::D77Track trk;
		track.push_back((D77Disk::D77Track &&)trk);
	}
}

std::vector <unsigned char> D77File::D77Disk::ReadTrack(int trk,int sid) const
{
	std::vector <unsigned char> data;

	auto trkPtr=GetTrack(trk,sid);
	if(nullptr!=trkPtr)
	{
		// Use track image if available.
		if(0<trkPtr->trackImage.size())
		{
			return trkPtr->trackImage;
		}


		// GAP 0
		for(int i=0; i<80; ++i)
		{
			data.push_back(0x4E);
		}
		// SYNC
		for(int i=0; i<12; ++i)
		{
			data.push_back(0);
		}
		// Index Mark
		data.push_back(0xC2);
		data.push_back(0xC2);
		data.push_back(0xC2);
		data.push_back(0xFC);

		// GAP 1
		for(int i=0; i<50; ++i)
		{
			data.push_back(0x4E);
		}

		std::set <uint32_t> visited; // To prevent multi-sample for corocoro protect to be written multiple times.
		for(auto &s : trkPtr->sector)
		{
			uint32_t CHRN;
			CHRN=s.cylinder;
			CHRN<<=8;
			CHRN|=s.head;
			CHRN<<=8;
			CHRN|=s.sector;
			CHRN<<=8;
			CHRN|=s.sizeShift;
			if(visited.find(CHRN)==visited.end())
			{
				visited.insert(CHRN);
				for(int i=0; i<12; ++i)
				{
					data.push_back(0);
				}
				data.push_back(0xA1);
				data.push_back(0xA1);
				data.push_back(0xA1);
				data.push_back(0xFE);
				data.push_back(s.cylinder);
				data.push_back(s.head);
				data.push_back(s.sector);
				data.push_back(s.sizeShift);
				data.push_back(0xCC); // Should be CRC.
				data.push_back(0xCC);
				// GAP 2
				for(int i=0; i<22; ++i)
				{
					data.push_back(0x4E);
				}
				for(int i=0; i<12; ++i)
				{
					data.push_back(0);
				}
				data.push_back(0xA1);
				data.push_back(0xA1);
				data.push_back(0xA1);
				data.push_back(0xFB);
				data.insert(data.end(),s.sectorData.begin(),s.sectorData.end());
				data.push_back(0xCC); // Should be CRC.
				data.push_back(0xCC);
				// GAP 3
				for(int i=0; i<84; ++i)
				{
					data.push_back(0);
				}
			}
		}

		// GAP 4
		for(int i=0; i<400; ++i)
		{
			data.push_back(0x4E);
		}
	}
	else
	{
		for(int i=0; i<1024*6; ++i) // Fill random number for 6KB
		{
			data.push_back(rand()%256);
		}
	}


	return data;
}

bool D77File::D77Disk::WriteTrack(int trk,int sid,int nSec,const D77Disk::D77Sector sec[])
{
	if(true==IsWriteProtected())
	{
		return false;
	}

	auto trkId=trk*2+sid;
	if(0<=trkId && trkId<track.size())
	{
		auto &t=track[trkId];
		t.WriteTrack(nSec,sec);
		SetModified();
		return true;
	}
	return false;
}

bool D77File::D77Disk::ForceWriteTrack(int trk,int sid,int nSec,const D77Disk::D77Sector sec[])
{
	if(true==IsWriteProtected())
	{
		return false;
	}

	auto trkId=trk*2+sid;
	if(track.size()<=trkId)
	{
		track.resize(trkId+1);
		SetModified();
	}
	return WriteTrack(trk,sid,nSec,sec);
}

void D77File::D77Disk::SetNumTrack(unsigned int nTrack)
{
	track.resize(nTrack*2);
}

bool D77File::D77Disk::AddSector(int trk,int sid,int secId,int secSize)
{
	if(true==IsWriteProtected())
	{
		return false;
	}

	auto trkId=trk*2+sid;
	if(0<=trkId && trkId<track.size())
	{
		auto &t=track[trkId];
		SetModified();
		return t.AddSector(trk,sid,secId,secSize);
	}
	return false;
}
bool D77File::D77Disk::SetSectorCHRN(int trk,int sid,int secId,int C,int H,int R,int N)
{
	if(true==IsWriteProtected())
	{
		return false;
	}

	auto trkId=trk*2+sid;
	if(0<=trkId && trkId<track.size())
	{
		auto &t=track[trkId];
		SetModified();
		return t.SetSectorCHRN(secId,C,H,R,N);
	}
	return false;
}

bool D77File::D77Disk::ReplaceSectorCHRN(int C0,int H0,int R0,int N0,int C,int H,int R,int N)
{
	if(true==IsWriteProtected())
	{
		return false;
	}

	for(int trkIdx=0; trkIdx<track.size(); ++trkIdx)
	{
		auto &t=track[trkIdx];
		for(int secIdx=0; secIdx<t.sector.size(); ++secIdx)
		{
			auto &s=t.sector[secIdx];
			if(s.cylinder==C0 &&
			   s.head==H0 &&
			   s.sector==R0 &&
			   s.sizeShift==N0)
			{
				printf("Replaced in track %d side %d\n",trkIdx/2,trkIdx%2);
				t.SetSectorCHRN(secIdx+1,C,H,R,N);
				SetModified();
			}
		}
	}
	return true;
}

std::vector <D77File::D77Disk::TrackLocation> D77File::D77Disk::AllTrack(void) const
{
	std::vector <TrackLocation> trkList;
	for(auto &t : track)
	{
		TrackLocation loc;
		loc.track=t.GetTrackNumber();
		loc.side=t.GetSide();
		trkList.push_back(loc);
	}
	return trkList;
}

long long int D77File::D77Disk::WriteSector(int trk,int sid,int sec,long long int nByte,const unsigned char dat[])
{
	if(true==IsWriteProtected())
	{
		return 0;
	}

	auto trkPtr=FindEditableTrack(trk,sid);
	if(nullptr!=trkPtr)
	{
		auto secPtr=trkPtr->FindSector(sec);
		if(nullptr!=secPtr)
		{
			long long int nWritten=0;
			for(long long int i=0; i<nByte && i<(long long int)(secPtr->sectorData.size()); ++i)
			{
				secPtr->sectorData[i]=dat[i];
				++nWritten;
			}
			if(0<nWritten)
			{
				SetModified();
			}
			return nWritten;
		}
	}
	return 0;
}

bool D77File::D77Disk::RenumberSector(int trk,int sid,int secFrom,int secTo)
{
	if(true==IsWriteProtected())
	{
		return false;
	}

	bool renumbered=false;

	auto trackPtr=FindEditableTrack(trk,sid);
	if(nullptr!=trackPtr)
	{
		for(auto &s : trackPtr->sector)
		{
			if(s.sector==secFrom)
			{
				printf("Renumber Track:%d Side:%d Sector:%d to Sector:%d\n",trk,sid,secFrom,secTo);
				s.sector=secTo;
				renumbered=true;
				SetModified();
			}
		}
	}

	return renumbered;
}

bool D77File::D77Disk::ResizeSector(int trk,int sid,int sec,int newSize)
{
	if(true==IsWriteProtected())
	{
		return false;
	}

	int newSizeShift=0;
	if(newSize==128)
	{
		newSizeShift=0;
	}
	else if(256==newSize)
	{
		newSizeShift=1;
	}
	else if(512==newSize)
	{
		newSizeShift=2;
	}
	else if(1024==newSize)
	{
		newSizeShift=3;
	}
	else
	{
		return false;
	}

	bool resized=false;

	auto trackPtr=FindEditableTrack(trk,sid);
	if(nullptr!=trackPtr)
	{
		for(auto &s : trackPtr->sector)
		{
			if(s.sector==sec)
			{
				printf("Resize Track:%d Side:%d Sector:%d to %d bytes\n",trk,sid,sec,newSize);

				auto curSize=(128<<s.sizeShift);
				s.sizeShift=newSizeShift;
				s.sectorDataSize=newSize;
				s.sectorData.resize(newSize);

				for(auto i=curSize; i<newSize; ++i)
				{
					s.sectorData[i]=0;
				}

				resized=true;
				SetModified();
			}
		}
	}

	return resized;
}

bool D77File::D77Disk::SetCRCError(int trk,int sid,int sec,bool crcError)
{
	if(true==IsWriteProtected())
	{
		return false;
	}
	bool modified=false;

	auto trackPtr=FindEditableTrack(trk,sid);
	if(nullptr!=trackPtr)
	{
		for(auto &s : trackPtr->sector)
		{
			if(-1==sec || s.sector==sec)
			{
				printf("Modify CRC Error Track:%d Side:%d Sector:%d\n",trk,sid,s.sector);

				if(true==crcError)
				{
					s.crcStatus=1;
				}
				else
				{
					s.crcStatus=0;
				}

				modified=true;
				SetModified();
			}
		}
	}

	return modified;
}

bool D77File::D77Disk::SetDDM(int trk,int sid,int sec,bool ddm)
{
	if(true==IsWriteProtected())
	{
		return false;
	}
	bool modified=false;

	auto trackPtr=FindEditableTrack(trk,sid);
	if(nullptr!=trackPtr)
	{
		for(auto &s : trackPtr->sector)
		{
			if(-1==sec || s.sector==sec)
			{
				printf("Modify DDM:%d Side:%d Sector:%d\n",trk,sid,s.sector);

				if(true==ddm)
				{
					s.deletedData=1;
				}
				else
				{
					s.deletedData=0;
				}

				modified=true;
				SetModified();
			}
		}
	}

	return modified;
}

bool D77File::D77Disk::GetCRCError(int trk,int sid,int sec) const
{
	auto trackPtr=FindTrack(trk,sid);
	if(nullptr!=trackPtr)
	{
		for(auto &s : trackPtr->sector)
		{
			if(s.sector==sec)
			{
				return 0!=s.crcStatus && 0xF0!=s.crcStatus;
			}
		}
	}
	return false;
}

bool D77File::D77Disk::GetRecordNotFound(int trk,int sid,int sec) const
{
	auto trackPtr=FindTrack(trk,sid);
	if(nullptr!=trackPtr)
	{
		for(auto &s : trackPtr->sector)
		{
			if(s.sector==sec)
			{
				return 0xF0==s.crcStatus;
			}
		}
	}
	return false;
}

bool D77File::D77Disk::GetDDM(int trk,int sid,int sec) const
{
	auto trackPtr=FindTrack(trk,sid);
	if(nullptr!=trackPtr)
	{
		for(auto &s : trackPtr->sector)
		{
			if(s.sector==sec)
			{
				return 0!=s.deletedData;
			}
		}
	}
	return false;
}

bool D77File::D77Disk::DeleteDuplicateSector(int trk,int sid)
{
	if(true==IsWriteProtected())
	{
		return false;
	}

	bool modified=false;

	auto trkPtr=FindEditableTrack(trk,sid);
	if(nullptr!=trkPtr)
	{
		auto &t=*trkPtr;
		for(int i=0; i<t.sector.size(); ++i)
		{
			auto &s0=t.sector[i];
			for(long long int j=t.sector.size()-1; i<j; --j)
			{
				auto &s1=t.sector[j];
				if(s0.sector==s1.sector)
				{
					printf("Deleted Duplicate Sector Track:%d Side:%d Sector:%d\n",s0.cylinder,s0.head,s0.sector);
					t.sector.erase(t.sector.begin()+j);
					modified=true;
					SetModified();
				}
			}
		}
		for(auto &s : t.sector)
		{
			s.nSectorTrack=t.sector.size();
		}
	}

	return modified;
}

bool D77File::D77Disk::CheckDuplicateSector(int trk,int sid) const
{
	auto trkPtr=FindTrack(trk,sid);
	if(nullptr!=trkPtr)
	{
		auto &t=*trkPtr;
		for(int i=0; i<t.sector.size(); ++i)
		{
			auto &s0=t.sector[i];
			for(long long int j=t.sector.size()-1; i<j; --j)
			{
				auto &s1=t.sector[j];
				if(s0.sector==s1.sector)
				{
					printf("Duplicate Sector found Track:%d Side:%d Sector:%d\n",s0.cylinder,s0.head,s0.sector);
					return true;
				}
			}
		}
	}
	return false;
}

bool D77File::D77Disk::DeleteSectorWithId(int trk,int sid,int sectorId)
{
	if(true==IsWriteProtected())
	{
		return false;
	}

	bool modified=false;

	auto trkPtr=FindEditableTrack(trk,sid);
	if(nullptr!=trkPtr)
	{
		auto &t=*trkPtr;
		for(long long int j=t.sector.size()-1; 0<=j; --j)
		{
			auto &s0=t.sector[j];
			if(sectorId==s0.sector)
			{
				printf("Deleted Sector Track:%d Side:%d Sector:%d\n",s0.cylinder,s0.head,s0.sector);
				t.sector.erase(t.sector.begin()+j);
				modified=true;
				SetModified();
			}
		}
		for(auto &s : t.sector)
		{
			s.nSectorTrack=t.sector.size();
		}
	}

	return modified;
}

bool D77File::D77Disk::DeleteSectorByIndex(int trk,int sid,int sectorIdx)
{
	if(true==IsWriteProtected())
	{
		return false;
	}

	bool modified=false;

	auto trkPtr=FindEditableTrack(trk,sid);
	if(nullptr!=trkPtr)
	{
		auto &t=*trkPtr;
		if(0<=sectorIdx && sectorIdx<t.sector.size())
		{
			auto &s0=t.sector[sectorIdx];
			printf("Deleted %dth sector in Track %d Side %d\n",sectorIdx,trk,sid);
			printf("(Sector Track:%d Side:%d Sector:%d)\n",s0.cylinder,s0.head,s0.sector);

			t.sector.erase(t.sector.begin()+sectorIdx);
			modified=true;
			SetModified();
		}
		for(auto &s : t.sector)
		{
			s.nSectorTrack=t.sector.size();
		}
	}

	return modified;
}

bool D77File::D77Disk::ReplaceData(const std::vector <unsigned char> &from,const std::vector <unsigned char> &to)
{
	if(true==IsWriteProtected())
	{
		return false;
	}
	if(from.size()!=to.size())
	{
		return false;
	}

	bool modified=false;

	for(auto &t : track)
	{
		for(auto found : t.Find(from))
		{
			t.Replace(found,to);
			printf("Replaced Track:%d Side:%d Sector:%d(0x%x) Addr:%04x\n",
			    found.track,
			    found.side,
			    found.sector,found.sector,
			    found.addr);
			modified=true;
			SetModified();
		}
	}

	return modified;
}

std::vector <unsigned char> D77File::D77Disk::ReadSector(int trk,int sid,int sec) const
{
	auto trkPtr=FindTrack(trk,sid);
	if(nullptr!=trkPtr)
	{
		auto secPtr=trkPtr->FindSector(sec);
		if(nullptr!=secPtr)
		{
			return secPtr->GetData();
		}
	}

	std::vector <unsigned char> empty;
	return empty;
}

const D77File::D77Disk::D77Sector *D77File::D77Disk::GetSectorFrom(int trk,int sid,int sec,unsigned int &posInTrack,unsigned int &nStep) const
{
	auto trkPtr=FindTrack(trk,sid);
	nStep=0;
	if(nullptr!=trkPtr)
	{
		for(nStep=0; nStep<trkPtr->sector.size(); ++nStep)
		{
			auto idx=(posInTrack+nStep)%trkPtr->sector.size();
			if(sec==trkPtr->sector[idx].sector)
			{
				posInTrack=(idx+1)%trkPtr->sector.size();
				return &trkPtr->sector[idx];
			}
		}
	}
	return nullptr;
}
D77File::D77Disk::D77Sector *D77File::D77Disk::GetSectorFrom(int trk,int sid,int sec,unsigned int &posInTrack,unsigned int &nStep)
{
	auto trkPtr=FindTrack(trk,sid);
	nStep=0;
	if(nullptr!=trkPtr)
	{
		for(nStep=0; nStep<trkPtr->sector.size(); ++nStep)
		{
			auto idx=(posInTrack+nStep)%trkPtr->sector.size();
			if(sec==trkPtr->sector[idx].sector)
			{
				posInTrack=(idx+1)%trkPtr->sector.size();
				return &trkPtr->sector[idx];
			}
		}
	}
	return nullptr;
}

const D77File::D77Disk::D77Sector *D77File::D77Disk::GetSector(int trk,int sid,int sec) const
{
	auto trkPtr=FindTrack(trk,sid);
	if(nullptr!=trkPtr)
	{
		return trkPtr->FindSector(sec);
	}
	return nullptr;
}

const D77File::D77Disk::D77Sector *D77File::D77Disk::GetSectorByIndex(int trk,int sid,int sec) const
{
	auto trkPtr=FindTrack(trk,sid);
	if(nullptr!=trkPtr && 0<=sec && sec<trkPtr->sector.size())
	{
		return &trkPtr->sector[sec];
	}
	return nullptr;
}

D77File::D77Disk::D77Sector *D77File::D77Disk::GetSector(int trk,int sid,int sec)
{
	auto trkPtr=FindTrack(trk,sid);
	if(nullptr!=trkPtr)
	{
		return trkPtr->FindSector(sec);
	}
	return nullptr;
}

D77File::D77Disk::D77Sector *D77File::D77Disk::GetSectorByIndex(int trk,int sid,int sec)
{
	auto trkPtr=FindTrack(trk,sid);
	if(nullptr!=trkPtr && 0<=sec && sec<trkPtr->sector.size())
	{
		return &trkPtr->sector[sec];
	}
	return nullptr;
}

bool D77File::D77Disk::CopyTrack(int dstTrk,int dstSide,int srcTrk,int srcSide)
{
	auto fromTrk=GetTrack(srcTrk,srcSide);
	auto toTrk=GetEditableTrack(dstTrk,dstSide);
	if(nullptr!=fromTrk && nullptr!=toTrk && fromTrk!=toTrk)
	{
		toTrk->sector=fromTrk->sector;
		for(auto &s : toTrk->sector)
		{
			s.cylinder=dstTrk;
			s.head=dstSide;
		}
		return true;
	}
	return false;
}

bool D77File::D77Disk::IsWriteProtected(void) const
{
	return (0!=header.writeProtected);
}

void D77File::D77Disk::SetWriteProtected(void)
{
	if(0==header.writeProtected)
	{
		header.writeProtected=0x10;
		SetModified();
	}
}
void D77File::D77Disk::ClearWriteProtected(void)
{
	if(0!=header.writeProtected)
	{
		header.writeProtected=0;
		SetModified();
	}
}

////////////////////////////////////////////////////////////

D77File::D77File()
{
}
D77File::~D77File()
{
	CleanUp();
}
void D77File::CleanUp(void)
{
	for(auto &d : disk)
	{
		d.CleanUp();
	}
	disk.clear();
}

void D77File::SetData(const std::vector <unsigned char> &byteData,bool verboseMode)
{
	SetData(byteData.size(),byteData.data(),verboseMode);
}
void D77File::SetData(long long int nByte,const unsigned char byteData[],bool verboseMode)
{
	long long int diskOffset=0;
	while(diskOffset<nByte)
	{
		if(true==verboseMode)
		{
			printf("File Offset: %08x\n",(int)diskOffset);
		}

		const unsigned char *diskPtr=byteData+diskOffset;

		size_t sz=diskPtr[0x1C]+(diskPtr[0x1D]<<8)+(diskPtr[0x1E]<<16)+(diskPtr[0x1F]<<24);
		if(0==sz || nByte<diskOffset+sz || nByte<672+diskOffset)
		{
			break;
		}

		D77Disk disk;
		disk.SetD77Image(diskPtr,verboseMode);
		auto nextDiskOffset=diskOffset+disk.header.diskSize;
		this->disk.push_back((D77Disk &&)disk);
		diskOffset=nextDiskOffset;
	}
}

bool D77File::SetRDDData(const std::vector <unsigned char> &byteData,bool verboseMode)
{
	size_t ptr=0;
	while(ptr+16<=byteData.size() &&
		   0==strncmp((const char *)byteData.data()+ptr,"REALDISKDUMP",12))
	{
		D77Disk disk;
		size_t bytesUsed=0;
		if(true==disk.SetRDDImage(bytesUsed,byteData.size()-ptr,byteData.data()+ptr,verboseMode))
		{
			this->disk.push_back((D77Disk &&)disk);
			ptr+=bytesUsed;
		}
		else
		{
			return false;
		}
	}
	return true;
}

bool D77File::SetRawBinary(const std::vector <unsigned char> &byteData,bool verboseMode)
{
	return SetRawBinary(byteData.size(),byteData.data(),verboseMode);
}
bool D77File::SetRawBinary(long long int nByte,const unsigned char byteData[],bool verboseMode)
{
	int bytesPerSector;
	int sectorsPerTrack;
	int numTracks;
	unsigned char mediaType=0;
	/*    1474560 bytes -> 1440KB   512bytes/sector, 18sectors/track, 80tracks, 2sides
	      1261568 bytes -> 1232KB  1024bytes/sector,  8sectors/track, 77tracks, 2sides
		   737280 bytes ->  720KB   512bytes/sector,  9sectors/track, 80tracks, 2sides
		   655360 bytes ->  640KB   512bytes/sector,  8sectors/track, 80tracks, 2sides
		   327680 bytes ->  320KB   256bytes/sector, 16sectors/track, 40tracks, 2sides */
	switch(nByte)
	{
	case 1474560:
		bytesPerSector= 512;
		sectorsPerTrack=18;
		numTracks=      80;
		mediaType=      D77_MEDIATYPE_2HD;
		break;
	case 1261568:
		bytesPerSector= 1024;
		sectorsPerTrack=8;
		numTracks=      77;
		mediaType=      D77_MEDIATYPE_2HD;
		break;
	case  737280:
		bytesPerSector= 512;
		sectorsPerTrack=9;
		numTracks=      80;
		mediaType=      D77_MEDIATYPE_2DD;
		break;
	case  655360:
		bytesPerSector= 512;
		sectorsPerTrack=8;
		numTracks=      80;
		mediaType=      D77_MEDIATYPE_2DD;
		break;
	case  327680:
		bytesPerSector= 256;
		sectorsPerTrack=16;
		numTracks=      40;
		mediaType=      D77_MEDIATYPE_2D;
		break;
	default:
		return false;
	}

	auto verifySize=bytesPerSector*sectorsPerTrack*numTracks*2;
	if(verifySize!=nByte)
	{
		printf("%s %d\n",__FUNCTION__,__LINE__);
		printf("Error.  Did not identify disk geometry correctly.\n");
		return false;
	}

	if(true==verboseMode)
	{
		printf("Identified as %lldKB disk.\n",nByte/1024);
		printf("%d bytes/sector\n",bytesPerSector);
		printf("%d sectors/track\n",sectorsPerTrack);
		printf("%d tracks\n",numTracks);
		printf("2 sides\n");
	}

	CleanUp();
	auto diskId=CreateUnformatted(numTracks*2,"D77_DISK");
	auto diskPtr=GetDisk(diskId);
	diskPtr->header.mediaType=mediaType;
	unsigned long long int imgPtr=0;
	for(int track=0; track<numTracks; ++track)
	{
		for(int side=0; side<2; ++side)
		{
			for(int sector=1; sector<=sectorsPerTrack; ++sector)
			{
				diskPtr->AddSector(track,side,sector,bytesPerSector);
				diskPtr->WriteSector(track,side,sector,bytesPerSector,byteData+imgPtr);
				imgPtr+=bytesPerSector;
			}
		}
	}

	diskPtr->ClearModified();

	if(imgPtr!=nByte)
	{
		printf("%s %d\n",__FUNCTION__,__LINE__);
		printf("Error.  Total number of bytes used is incorrect.\n");
		return false;
	}

	return true;
}

int D77File::CreateStandardFormatted(void)
{
	D77Disk disk;
	disk.CreateStandardFormatted();
	this->disk.push_back((D77Disk &&)disk);
	return (int)this->disk.size()-1;
}

int D77File::CreateUnformatted(int nTrack,const char diskName[])
{
	D77Disk disk;
	disk.CreateUnformatted(nTrack,diskName);
	this->disk.push_back((D77Disk &&)disk);
	return (int)this->disk.size()-1;
}

std::vector <unsigned char> D77File::MakeD77Image(void) const
{
	std::vector <unsigned char> bin;
	for(auto &d : disk)
	{
		auto diskBin=d.MakeD77Image();
		bin.insert(bin.end(),diskBin.begin(),diskBin.end());
	}
	return bin;
}

std::vector <unsigned char> D77File::MakeRDDImage(void) const
{
	std::vector <unsigned char> bin;
	for(auto &d : disk)
	{
		auto diskBin=d.MakeRDDImage();
		bin.insert(bin.end(),diskBin.begin(),diskBin.end());
	}
	return bin;
}

std::vector <unsigned char> D77File::MakeRawImage(void) const
{
	std::vector <unsigned char> bin;
	for(auto &d : disk)
	{
		auto diskBin=d.MakeRawImage();
		bin.insert(bin.end(),diskBin.begin(),diskBin.end());
	}
	return bin;
}

bool D77File::IsModified(void) const
{
	for(auto &d : disk)
	{
		if(true==d.IsModified())
		{
			return true;
		}
	}
	return false;
}

void D77File::PrintInfo(void) const
{
	int diskCount=0;
	for(auto &d : disk)
	{
		printf("Disk[%d]\n",diskCount);
		d.PrintInfo();
		++diskCount;
	}
}

long long int D77File::GetNumDisk(void) const
{
	return disk.size();
}

D77File::D77Disk *D77File::GetDisk(int diskId)
{
	if(0<=diskId && diskId<disk.size())
	{
		return &disk[diskId];
	}
	return nullptr;
}

const D77File::D77Disk *D77File::GetDisk(int diskId) const
{
	if(0<=diskId && diskId<disk.size())
	{
		return &disk[diskId];
	}
	return nullptr;
}

////////////////////////////////////////////////////////////

