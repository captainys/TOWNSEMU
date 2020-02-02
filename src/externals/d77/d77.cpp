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
	for(auto &c : reservedByte)
	{
		c=0;
	}
	sectorDataSize=0; // Including the header.
	sectorData.clear();
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
		UnsignedIntToDWord(d77Img.data()+trackPtr[trackIdx],(unsigned int)d77Img.size());
		if(0<t.sector.size())
		{
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
				d77Img.push_back(s.crcStatus);
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
			// Minimum 128-byte sector data + 0x10 header.  Am I rite?
			for(int i=0; i<144; ++i)
			{
				d77Img.push_back(0);
			}
		}
		++trackIdx;
	}
	UnsignedIntToDWord(d77Img.data()+diskSizePtr,(unsigned int)d77Img.size());

	return d77Img;
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
	for(auto offset : trackOffset)
	{
		if(0!=offset)
		{
			if(true==verboseMode)
			{
				printf("Reading Track... DiskOffset=%08x Count=%d ",(int)offset,trackCount);
				if(0<trackCount)
				{
					printf("Step=%04x",(int)(offset-prevOffset));
				}
				printf("\n");
			}
			track.push_back(MakeTrackData(d77Img+offset,d77Img+header.diskSize));
			prevOffset=offset;
			++trackCount;
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
				return 0!=s.crcStatus;
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
			return secPtr->sectorData;
		}
	}

	std::vector <unsigned char> empty;
	return empty;
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
		D77Disk disk;
		disk.SetD77Image(diskPtr,verboseMode);
		auto nextDiskOffset=diskOffset+disk.header.diskSize;
		this->disk.push_back((D77Disk &&)disk);
		diskOffset=nextDiskOffset;
	}
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
		break;
	case 1261568:
		bytesPerSector= 1024;
		sectorsPerTrack=8;
		numTracks=      77;
		break;
	case  737280:
		bytesPerSector= 512;
		sectorsPerTrack=9;
		numTracks=      80;
		break;
	case  655360:
		bytesPerSector= 512;
		sectorsPerTrack=8;
		numTracks=      80;
		break;
	case  327680:
		bytesPerSector= 256;
		sectorsPerTrack=16;
		numTracks=      40;
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

