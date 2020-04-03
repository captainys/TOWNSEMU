#include <algorithm>

#include <string.h>
#include <stdio.h>
#include <math.h>
#include "yssimplesound.h"

YsSoundPlayer *YsSoundPlayer::currentPlayer=nullptr;

YsSoundPlayer::YsSoundPlayer()
{
	api=CreateAPISpecificData();
	playerStatePtr.reset(new STATE);
	*playerStatePtr=STATE_UNINITIALIZED;
}
YsSoundPlayer::~YsSoundPlayer()
{
	End();
	if(GetCurrentPlayer()==this)
	{
		NullifyCurrentPlayer();
	}
	DeleteAPISpecificData(api);
}

void YsSoundPlayer::MakeCurrent(void)
{
	currentPlayer=this;
}

/* static */ void YsSoundPlayer::NullifyCurrentPlayer(void)
{
	currentPlayer=nullptr;
}
/* static */ YsSoundPlayer *YsSoundPlayer::GetCurrentPlayer(void)
{
	return currentPlayer;
}



////////////////////////////////////////////////////////////



YsSoundPlayer::SoundData::MemInStream::MemInStream(long long int len,const unsigned char dat[])
{
	this->length=len;
	this->pointer=0;
	this->dat=dat;
}
long long int YsSoundPlayer::SoundData::MemInStream::Fetch(unsigned char buf[],long long int len)
{
	long long int nCopy=0;
	if(pointer+len<length)
	{
		nCopy=len;
	}
	else
	{
		nCopy=length-pointer;
	}
	for(long long int i=0; i<nCopy; ++i)
	{
		buf[i]=dat[pointer];
		++pointer;
	}
	return nCopy;
}

long long int YsSoundPlayer::SoundData::MemInStream::Skip(long long int len)
{
	long long int nSkip=0;
	if(pointer+len<length)
	{
		nSkip=len;
	}
	else
	{
		nSkip=length-pointer;
	}
	pointer+=nSkip;
	return nSkip;
}

YsSoundPlayer::SoundData::FileInStream::FileInStream(FILE *fp)
{
	this->fp=fp;
}
long long int YsSoundPlayer::SoundData::FileInStream::Skip(long long int len)
{
	size_t totalSkipped=0;

	const long long skipBufSize=1024*1024;
	char *skipbuf=new char [skipBufSize];
	while(0<len)
	{
		auto toSkip=len;
		if(skipBufSize<toSkip)
		{
			toSkip=skipBufSize;
		}
		auto skipped=fread(skipbuf,1,toSkip,fp);
		totalSkipped+=skipped;
		len-=skipped;
		if(0==skipped)
		{
			break;
		}
	}
	delete [] skipbuf;
	return totalSkipped;
}
long long int YsSoundPlayer::SoundData::FileInStream::Fetch(unsigned char buf[],long long int len)
{
	return fread(buf,1,len,fp);
}

////////////////////////////////////////////////////////////

void YsSoundPlayer::Start(void)
{
	if(YSOK==StartAPISpecific())
	{
		*playerStatePtr=STATE_STARTED;
	}
}
void YsSoundPlayer::End(void)
{
	EndAPISpecific();
	*playerStatePtr=STATE_ENDED;
}

void YsSoundPlayer::PlayOneShot(SoundData &dat)
{
	if(nullptr==dat.playerStatePtr)
	{
		dat.playerStatePtr=this->playerStatePtr;
	}
	else if(dat.playerStatePtr!=this->playerStatePtr)
	{
		printf("%s %d\n",__FUNCTION__,__LINE__);
		printf("  YsSoundPlayer::SoundData can be associated with only one player.\n");
		return;
	}

	dat.PreparePlay(*this);
	if(YSOK==PlayOneShotAPISpecific(dat))
	{
	}
}
void YsSoundPlayer::PlayBackground(SoundData &dat)
{
	if(nullptr==dat.playerStatePtr)
	{
		dat.playerStatePtr=this->playerStatePtr;
	}
	else if(dat.playerStatePtr!=this->playerStatePtr)
	{
		printf("%s %d\n",__FUNCTION__,__LINE__);
		printf("  YsSoundPlayer::SoundData can be associated with only one player.\n");
		return;
	}

	dat.PreparePlay(*this);
	if(YSOK==PlayBackgroundAPISpecific(dat))
	{
	}
}
void YsSoundPlayer::Stop(SoundData &dat)
{
	StopAPISpecific(dat);
}
void YsSoundPlayer::Pause(SoundData &dat)
{
	PauseAPISpecific(dat);
}
void YsSoundPlayer::Resume(SoundData &dat)
{
	ResumeAPISpecific(dat);
}

void YsSoundPlayer::KeepPlaying(void)
{
	KeepPlayingAPISpecific();
}

YSBOOL YsSoundPlayer::IsPlaying(const SoundData &dat) const
{
	return IsPlayingAPISpecific(dat);
}

double YsSoundPlayer::GetCurrentPosition(const SoundData &dat) const
{
	return GetCurrentPositionAPISpecific(dat);
}

void YsSoundPlayer::SetVolume(SoundData &dat,float vol)
{
	dat.playBackVolume=vol;
	SetVolumeAPISpecific(dat,vol);
}

////////////////////////////////////////////////////////////



YsSoundPlayer::SoundData::SoundData()
{
	api=CreateAPISpecificData();
	Initialize();
}

YsSoundPlayer::SoundData::~SoundData()
{
	CleanUp();
	DeleteAPISpecificData(api);
}

void YsSoundPlayer::SoundData::Initialize(void)
{
	CleanUp();
}

void YsSoundPlayer::SoundData::CleanUp(void)
{
	CleanUpAPISpecific();

	dat.clear();

	lastModifiedChannel=0;
	stereo=YSFALSE;
	bit=16;
	rate=44100;
	sizeInBytes=0;
	isSigned=YSTRUE;
	playBackVolume=1.0;
}

unsigned int YsSoundPlayer::SoundData::NTimeStep(void) const
{
	return SizeInByte()/BytePerTimeStep();
}

YSBOOL YsSoundPlayer::SoundData::Stereo(void) const
{
	return stereo;
}

unsigned int YsSoundPlayer::SoundData::BytePerTimeStep(void) const
{
	unsigned int nChannel=(YSTRUE==stereo ? 2 : 1);
	return nChannel*BytePerSample();
}

unsigned int YsSoundPlayer::SoundData::BitPerSample(void) const
{
	return bit;
}

unsigned int YsSoundPlayer::SoundData::BytePerSample(void) const
{
	return bit/8;
}

long long YsSoundPlayer::SoundData::SecToNumSample(double sec) const
{
	return SecToNumSample(sec,PlayBackRate());
}
/* static */ long long YsSoundPlayer::SoundData::SecToNumSample(double sec,unsigned int playBackRate)
{
	return (long long)(sec*(double)playBackRate);
}
double YsSoundPlayer::SoundData::NumSampleToSec(long long numSample) const
{
	return NumSampleToSec(numSample,PlayBackRate());
}
/* static */ double YsSoundPlayer::SoundData::NumSampleToSec(long long numSample,unsigned int playBackRate)
{
	return (double)numSample/(double)playBackRate;
}

unsigned int YsSoundPlayer::SoundData::PlayBackRate(void) const
{
	return rate;
}

unsigned int YsSoundPlayer::SoundData::SizeInByte(void) const
{
	return sizeInBytes;
}

YSBOOL YsSoundPlayer::SoundData::IsSigned(void) const
{
	return isSigned;
}

const unsigned char *YsSoundPlayer::SoundData::DataPointer(void) const
{
	return dat.data();
}

const unsigned char *YsSoundPlayer::SoundData::DataPointerAtTimeStep(unsigned int ts) const
{
	return dat.data()+ts*BytePerTimeStep();
}

static unsigned GetUnsigned(const unsigned char buf[])
{
	return buf[0]+buf[1]*0x100+buf[2]*0x10000+buf[3]*0x1000000;
}

static unsigned GetUnsignedShort(const unsigned char buf[])
{
	return buf[0]+buf[1]*0x100;
}



YSRESULT YsSoundPlayer::SoundData::CreateFrom44100HzStereo(std::vector <unsigned char> &wave)
{
	CleanUp();

	stereo=YSTRUE;
	bit=16;
	rate=44100;
	sizeInBytes=wave.size();
	isSigned=YSTRUE;
	playBackVolume=1.0;

	dat.swap(wave);

	return YSOK;
}



YSRESULT YsSoundPlayer::SoundData::LoadWav(const char fn[])
{
	FILE *fp;
	printf("Loading %s\n",fn);
	fp=fopen(fn,"rb");
	if(fp!=NULL)
	{
		auto res=LoadWav(fp);
		fclose(fp);
		return res;
	}
	return YSERR;
}

YSRESULT YsSoundPlayer::SoundData::LoadWav(FILE *fp)
{
	if(nullptr!=fp)
	{
		FileInStream inStream(fp);
		return LoadWav(inStream);
	}
	return YSERR;
}

YSRESULT YsSoundPlayer::SoundData::LoadWavFromMemory(long long int length,const unsigned char incoming[])
{
	MemInStream inStream(length,incoming);
	return LoadWav(inStream);
}

YSRESULT YsSoundPlayer::SoundData::LoadWav(BinaryInStream &inStream)
{
	CleanUp();

	unsigned char buf[256];
	unsigned int l;
	unsigned int fSize,hdrSize,dataSize;

	// Wave Header
	unsigned short wFormatTag,nChannels;
	unsigned nSamplesPerSec,nAvgBytesPerSec;
	unsigned short nBlockAlign,wBitsPerSample,cbSize;


	if(inStream.Fetch(buf,4)!=4)
	{
		printf("Error in reading RIFF.\n");
		return YSERR;
	}
	if(strncmp((char *)buf,"RIFF",4)!=0)
	{
		printf("Warning: RIFF not found.\n");
	}


	if(inStream.Fetch(buf,4)!=4)
	{
		printf("Error in reading file size.\n");
		return YSERR;
	}
	fSize=GetUnsigned(buf);
	printf("File Size=%d\n",fSize+8);
	// Wait, is it fSize+12?  A new theory tells that "fmt " immediately following "WAVE"
	// is a chunk???

	if(inStream.Fetch(buf,8)!=8)
	{
		printf("Error in reading WAVEfmt.\n");
		return YSERR;
	}
	if(strncmp((char *)buf,"WAVEfmt",7)!=0)
	{
		printf("Warning: WAVEfmt not found\n");
	}


	if(inStream.Fetch(buf,4)!=4)
	{
		printf("Error in reading header size.\n");
		return YSERR;
	}
	hdrSize=GetUnsigned(buf);
	printf("Header Size=%d\n",hdrSize);


	//    WORD  wFormatTag; 
	//    WORD  nChannels; 
	//    DWORD nSamplesPerSec; 
	//    DWORD nAvgBytesPerSec; 
	//    WORD  nBlockAlign; 
	//    WORD  wBitsPerSample; 
	//    WORD  cbSize; 
	if(inStream.Fetch(buf,hdrSize)!=hdrSize)
	{
		printf("Error in reading header.\n");
		return YSERR;
	}
	wFormatTag=GetUnsignedShort(buf);
	nChannels=GetUnsignedShort(buf+2);
	nSamplesPerSec=GetUnsigned(buf+4);
	nAvgBytesPerSec=GetUnsigned(buf+8);
	nBlockAlign=GetUnsignedShort(buf+12);
	wBitsPerSample=(hdrSize>=16 ? GetUnsignedShort(buf+14) : 0);
	cbSize=(hdrSize>=18 ? GetUnsignedShort(buf+16) : 0);

	printf("wFormatTag=%d\n",wFormatTag);
	printf("nChannels=%d\n",nChannels);
	printf("nSamplesPerSec=%d\n",nSamplesPerSec);
	printf("nAvgBytesPerSec=%d\n",nAvgBytesPerSec);
	printf("nBlockAlign=%d\n",nBlockAlign);
	printf("wBitsPerSample=%d\n",wBitsPerSample);
	printf("cbSize=%d\n",cbSize);



	for(;;)
	{
		if(inStream.Fetch(buf,4)!=4)
		{
			printf("Error while waiting for data.\n");
			return YSERR;
		}

		if((buf[0]=='d' || buf[0]=='D') && (buf[1]=='a' || buf[1]=='A') &&
		   (buf[2]=='t' || buf[2]=='T') && (buf[3]=='a' || buf[3]=='A'))
		{
			break;
		}
		else
		{
			printf("Skipping %c%c%c%c (Unknown Block)\n",buf[0],buf[1],buf[2],buf[3]);
			if(inStream.Fetch(buf,4)!=4)
			{
				printf("Error while skipping unknown block.\n");
				return YSERR;
			}



			l=GetUnsigned(buf);
			if(inStream.Skip(l)!=l)
			{
				printf("Error while skipping unknown block.\n");
				return YSERR;
			}
		}
	}


	if(inStream.Fetch(buf,4)!=4)
	{
		printf("Error in reading data size.\n");
		return YSERR;
	}
	dataSize=GetUnsigned(buf);
	printf("Data Size=%d (0x%x)\n",dataSize,dataSize);

	dat.resize(dataSize);
	if((l=inStream.Fetch(dat.data(),dataSize))!=dataSize)
	{
		printf("Warning: File ended before reading all data.\n");
		printf("  %d (0x%x) bytes have been read\n",l,l);
	}

	this->stereo=(nChannels==2 ? YSTRUE : YSFALSE);
	this->bit=wBitsPerSample;
	this->sizeInBytes=dataSize;
	this->rate=nSamplesPerSec;

	if(wBitsPerSample==8)
	{
		isSigned=YSFALSE;
	}
	else
	{
		isSigned=YSTRUE;
	}

	return YSOK;
}


YSRESULT YsSoundPlayer::SoundData::ConvertTo16Bit(void)
{
	if(bit==16)
	{
		return YSOK;
	}
	else if(bit==8)
	{
		if(sizeInBytes>0 && 0<dat.size()) // ? Why did I write 0<dat.size()?  2020/04/02
		{
			std::vector <unsigned char> newDat;
			newDat.resize(sizeInBytes*2);
			for(int i=0; i<sizeInBytes; i++)
			{
				newDat[i*2]  =dat[i];
				newDat[i*2+1]=dat[i];
			}
			std::swap(dat,newDat);

			sizeInBytes*=2;
			bit=16;
		}
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsSoundPlayer::SoundData::ConvertTo8Bit(void)
{
	if(bit==8)
	{
		return YSOK;
	}
	else if(bit==16)
	{
		std::vector <unsigned char> newDat;
		newDat.resize(sizeInBytes/2);
		for(int i=0; i<sizeInBytes; i+=2)
		{
			newDat[i/2]=dat[i];
		}
		std::swap(dat,newDat);
		bit=8;
		sizeInBytes/=2;
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsSoundPlayer::SoundData::ConvertToStereo(void)
{
	if(stereo==YSTRUE)
	{
		return YSOK;
	}
	else
	{
		if(bit==8)
		{
			std::vector <unsigned char> newDat;
			newDat.resize(sizeInBytes*2);
			for(int i=0; i<sizeInBytes; i++)
			{
				newDat[i*2  ]=dat[i];
				newDat[i*2+1]=dat[i];
			}
			std::swap(dat,newDat);
			stereo=YSTRUE;
			sizeInBytes*=2;
			return YSOK;
		}
		else if(bit==16)
		{
			std::vector <unsigned char> newDat;
			newDat.resize(sizeInBytes*2);
			for(int i=0; i<sizeInBytes; i+=2)
			{
				newDat[i*2  ]=dat[i];
				newDat[i*2+1]=dat[i+1];
				newDat[i*2+2]=dat[i];
				newDat[i*2+3]=dat[i+1];
			}
			std::swap(dat,newDat);
			stereo=YSTRUE;
			sizeInBytes*=2;
			return YSOK;
		}
	}
	return YSERR;
}

YSRESULT YsSoundPlayer::SoundData::Resample(int newRate)
{
	if(rate!=newRate)
	{
		const size_t nChannel=(YSTRUE==stereo ? 2 : 1);
		const size_t bytePerSample=bit/8;
		const size_t bytePerTimeStep=nChannel*bytePerSample;
		const size_t curNTimeStep=sizeInBytes/bytePerTimeStep;

		const size_t newNTimeStep=curNTimeStep*newRate/rate;
		const size_t newSize=newNTimeStep*bytePerTimeStep;

		std::vector <unsigned char> newDat;
		newDat.resize(newSize);
		{
			for(size_t ts=0; ts<newNTimeStep; ts++)
			{
				double oldTimeStepD=(double)curNTimeStep*(double)ts/(double)newNTimeStep;
				size_t oldTimeStep=(size_t)oldTimeStepD;
				double param=fmod(oldTimeStepD,1.0);
				unsigned char *newTimeStepPtr=newDat.data()+ts*bytePerTimeStep;

				for(size_t ch=0; ch<nChannel; ++ch)
				{
					if(curNTimeStep-1<=oldTimeStep)
					{
						const int value=GetSignedValueRaw(ch,curNTimeStep-1);
						SetSignedValueRaw(newTimeStepPtr+bytePerSample*ch,value);
					}
					else if(0==oldTimeStep || curNTimeStep-2<=oldTimeStep)
					{
						const double value[2]=
							{
								(double)GetSignedValueRaw(ch,oldTimeStep),
								(double)GetSignedValueRaw(ch,oldTimeStep+1)
							};
						const int newValue=(int)(value[0]*(1.0-param)+value[1]*param);
						SetSignedValueRaw(newTimeStepPtr+bytePerSample*ch,newValue);
					}
					else
					{
						const double v[4]=
							{
								(double)GetSignedValueRaw(ch,oldTimeStep-1),  // At x=-1.0
								(double)GetSignedValueRaw(ch,oldTimeStep),    // At x= 0.0
								(double)GetSignedValueRaw(ch,oldTimeStep+1),  // At x= 1.0
								(double)GetSignedValueRaw(ch,oldTimeStep+2)   // At x= 2.0
							};

						// Cubic interpolation.  Linear didn't work well.
						// axxx+bxx+cx+d=e
						// x=-1  -> -a+b-c+d=v0   (A)
						// x= 0  ->        d=v1   (B)
						// x= 1  ->  a+b+c+d=v2   (C)
						// x= 2  -> 8a+4b+2c+d=v3 (D)
						//
						// (B) =>  d=v1;
						// (A)+(C) => 2b+2d=v0+v2  => b=(v0+v2-2d)/2
						//
						// (D)-2*(B) =>  6a+2b-d=v3-2*v2
						//           =>  a=(v3-2*v2-2b+d)/6

						const double d=v[1];
						const double b=(v[0]+v[2]-2.0*d)/2.0;
						const double a=(v[3]-2.0*v[2]-2.0*b+d)/6.0;
						const double c=v[2]-a-b-d;

						double newValue=a*param*param*param
							+b*param*param
							+c*param
							+d;
						SetSignedValueRaw(newTimeStepPtr+bytePerSample*ch,(int)newValue);
					}
				}
			}
		}

		rate=newRate;
		std::swap(dat,newDat);
		sizeInBytes=newSize;
	}
	return YSOK;
}

YSRESULT YsSoundPlayer::SoundData::ConvertToMono(void)
{
	if(YSTRUE==stereo)
	{
		const size_t bytePerSample=bit/8;
		const size_t bytePerTimeStep=2*bytePerSample;
		const size_t nTimeStep=sizeInBytes/bytePerTimeStep;

		const size_t newSize=nTimeStep*bytePerSample;

		std::vector <unsigned char> newDat;
		newDat.resize(newSize);
		{
			for(size_t ts=0; ts<nTimeStep; ts++)
			{
				const int newValue=(GetSignedValueRaw(0,ts)+GetSignedValueRaw(1,ts))/2;
				unsigned char *const newTimeStepPtr=newDat.data()+ts*bytePerSample;
				SetSignedValueRaw(newTimeStepPtr,newValue);
			}

			std::swap(dat,newDat);
			sizeInBytes=newSize;
			stereo=YSFALSE;

			return YSOK;
		}
	}
	return YSERR;
}

YSRESULT YsSoundPlayer::SoundData::ConvertToSigned(void)
{
	if(isSigned==YSTRUE)
	{
		return YSOK;
	}
	else
	{
		if(bit==8)
		{
			for(int i=0; i<sizeInBytes; i++)
			{
				dat[i]-=128;
			}
		}
		else if(bit==16)
		{
			for(int i=0; i<sizeInBytes-1; i+=2)
			{
				int d;
				d=dat[i]+dat[i+1]*256;
				d-=32768;
				dat[i]=d&255;
				dat[i+1]=(d>>8)&255;
			}
		}
		isSigned=YSTRUE;
	}
	return YSOK;
}

YSRESULT YsSoundPlayer::SoundData::ConvertToUnsigned(void)
{
	if(isSigned!=YSTRUE)
	{
		return YSOK;
	}
	else
	{
		if(bit==8)
		{
			for(int i=0; i<sizeInBytes; i++)
			{
				dat[i]+=128;
			}
		}
		else if(bit==16)
		{
			for(int i=0; i<sizeInBytes-1; i+=2)
			{
				int d=dat[i]+dat[i+1]*256;
				if(d>=32768)
				{
					d-=65536;
				}
				d+=32768;
				dat[i]=d&255;
				dat[i+1]=(d>>8)&255;
			}
		}
		isSigned=YSFALSE;
	}
	return YSOK;
}

YSRESULT YsSoundPlayer::SoundData::DeleteChannel(int channel)
{
	if(YSTRUE==stereo)
	{
		const size_t bytePerSample=bit/8;
		const size_t bytePerTimeStep=2*bytePerSample;
		const size_t nTimeStep=sizeInBytes/bytePerTimeStep;

		const size_t newSize=nTimeStep*bytePerSample;

		std::vector <unsigned char> newDat;
		newDat.resize(newSize);
		{
			for(size_t ts=0; ts<nTimeStep; ts++)
			{
				const int newValue=GetSignedValueRaw(1-channel,ts);
				unsigned char *const newTimeStepPtr=newDat.data()+ts*bytePerSample;
				SetSignedValueRaw(newTimeStepPtr,newValue);
			}

			std::swap(dat,newDat);
			sizeInBytes=newSize;
			stereo=YSFALSE;

			if(channel<=lastModifiedChannel)
			{
				lastModifiedChannel--;
				if(0>lastModifiedChannel)
				{
					lastModifiedChannel=0;
				}
			}

			return YSOK;
		}
	}
	return YSERR;
}

//int main(int ac,char *av[])
//{
//	YsWavFile test;
//	test.LoadWav(av[1]);
//	return 0;
//}

int YsSoundPlayer::SoundData::GetNumChannel(void) const
{
	return (YSTRUE==stereo ? 2 : 1);
}

int YsSoundPlayer::SoundData::GetLastModifiedChannel(void) const
{
	return lastModifiedChannel;
}

int YsSoundPlayer::SoundData::GetNumSample(void) const
{
	return (sizeInBytes*8/bit);
}

int YsSoundPlayer::SoundData::GetNumSamplePerChannel(void) const
{
	return GetNumSample()/GetNumChannel();
}

size_t YsSoundPlayer::SoundData::GetUnitSize(void) const
{
	return BytePerSample()*GetNumChannel();
}

size_t YsSoundPlayer::SoundData::GetSamplePosition(int atIndex) const
{
	return atIndex*GetNumChannel()*(bit/8);
}

int YsSoundPlayer::SoundData::GetSignedValueRaw(int channel,int atTimeStep) const
{
	const size_t sampleIdx=GetSamplePosition(atTimeStep);
	const size_t unitSize=GetUnitSize();

	if(sampleIdx+unitSize<=sizeInBytes && 0<=channel && channel<GetNumChannel())
	{
		int rawSignedValue=0;
		size_t offset=sampleIdx+channel*BytePerSample();
		switch(BitPerSample())
		{
		case 8:
			if(YSTRUE==isSigned)
			{
				rawSignedValue=dat[offset];
				if(128<=rawSignedValue)
				{
					rawSignedValue-=256;
				}
			}
			else
			{
				rawSignedValue=dat[offset]-128;
			}
			break;
		case 16:
			// Assume little endian
			rawSignedValue=dat[offset]+256*dat[offset+1];
			if(YSTRUE==isSigned)
			{
				if(32768<=rawSignedValue)
				{
					rawSignedValue-=65536;
				}
			}
			else
			{
				rawSignedValue-=32768;
		    }
			break;
		}
		return rawSignedValue;
	}
	return 0;
}

void YsSoundPlayer::SoundData::SetSignedValue16(int channel,int atTimeStep,int rawSignedValue)
{
	const size_t sampleIdx=GetSamplePosition(atTimeStep);
	const size_t unitSize=GetUnitSize();

	if(sampleIdx+unitSize<=sizeInBytes && 0<=channel && channel<GetNumChannel())
	{
		lastModifiedChannel=channel;
		size_t offset=sampleIdx+channel*BytePerSample();
		switch(BitPerSample())
		{
		case 8:
			if(YSTRUE==isSigned)
			{
				rawSignedValue>>=8;
				rawSignedValue&=255;
				dat[offset]=rawSignedValue;
			}
			else
			{
				rawSignedValue>>=8;
				rawSignedValue+=128;
				rawSignedValue&=255;
				dat[offset]=rawSignedValue;
			}
			break;
		case 16:
			// Assume little endian
			if(YSTRUE==isSigned)
			{
				dat[offset  ]=(rawSignedValue&255);
				dat[offset+1]=((rawSignedValue>>8)&255);
			}
			else
			{
				rawSignedValue+=32768;
				dat[offset  ]=(rawSignedValue&255);
				dat[offset+1]=((rawSignedValue>>8)&255);
		    }
			break;
		}
	}
}

int YsSoundPlayer::SoundData::GetSignedValue16(int channel,int atTimeStep) const
{
	if(16==bit)
	{
		return GetSignedValueRaw(channel,atTimeStep);
	}
	else if(8==bit)
	{
		auto value=GetSignedValueRaw(channel,atTimeStep);
		value=value*32767/127;
		return value;
	}
	return 0; // ?
}

void YsSoundPlayer::SoundData::ResizeByNumSample(long long int nSample)
{
	long long int newDataSize=nSample*GetNumChannel()*BytePerSample();
	std::vector <unsigned char> newDat;
	newDat.resize(newDataSize);

	for(long long int i=0; i<newDataSize && i<sizeInBytes; ++i)
	{
		newDat[i]=dat[i];
	}
	for(long long int i=sizeInBytes; i<newDataSize; ++i)
	{
		newDat[i]=0;
	}

	std::swap(dat,newDat);
	sizeInBytes=newDataSize;
}

void YsSoundPlayer::SoundData::SetSignedValueRaw(unsigned char *savePtr,int rawSignedValue)
{
	switch(bit)
	{
	case 8:
		if(rawSignedValue<-128)
		{
			rawSignedValue=-128;
		}
		else if(127<rawSignedValue)
		{
			rawSignedValue=127;
		}
		if(YSTRUE==isSigned)
		{
			if(0>rawSignedValue)
			{
				rawSignedValue+=256;
			}
			*savePtr=(unsigned char)rawSignedValue;
		}
		else
		{
			rawSignedValue+=128;
			*savePtr=(unsigned char)rawSignedValue;
		}
		break;
	case 16:
		if(-32768>rawSignedValue)
		{
			rawSignedValue=-32768;
		}
		else if(32767<rawSignedValue)
		{
			rawSignedValue=32767;
		}

		if(YSTRUE==isSigned)
		{
			if(0>rawSignedValue)
			{
				rawSignedValue+=65536;
			}
		}
		else
		{
			rawSignedValue+=32768;
		}

		// Assume little endian (.WAV is supposed to use little endian).
		savePtr[0]=(rawSignedValue&255);
		savePtr[1]=((rawSignedValue>>8)&255);
		break;
	}
}



std::vector <unsigned char> YsSoundPlayer::SoundData::MakeWavByteData(void) const
{
	std::vector <unsigned char> byteData;
	byteData.push_back('R');
	byteData.push_back('I');
	byteData.push_back('F');
	byteData.push_back('F');

	AddUnsignedInt(byteData,0);

	byteData.push_back('W');
	byteData.push_back('A');
	byteData.push_back('V');
	byteData.push_back('E');
	byteData.push_back('f');
	byteData.push_back('m');
	byteData.push_back('t');
	byteData.push_back(' ');

	const int nChannels=GetNumChannel();
	const int nBlockAlign=nChannels*BitPerSample()/8;
	const int nAvgBytesPerSec=PlayBackRate()*nBlockAlign;

	AddUnsignedInt(byteData,16);
	AddUnsignedShort(byteData,1); // wFormatTag=1
	AddUnsignedShort(byteData,(unsigned short)GetNumChannel());
	AddUnsignedInt(byteData,PlayBackRate());  // nSamplesPerSec
	AddUnsignedInt(byteData,nAvgBytesPerSec);
	AddUnsignedShort(byteData,(unsigned short)nBlockAlign);
	AddUnsignedShort(byteData,(unsigned short)BitPerSample()); // wBitsPerSample

	byteData.push_back('d');
	byteData.push_back('a');
	byteData.push_back('t');
	byteData.push_back('a');

	AddUnsignedInt(byteData,SizeInByte());

	for(int i=0; i<SizeInByte(); ++i)
	{
		byteData.push_back(dat[i]);
	}

	auto totalSize=byteData.size()-8;
	byteData[4]=totalSize&255;
	byteData[5]=(totalSize>>8)&255;
	byteData[6]=(totalSize>>16)&255;
	byteData[7]=(totalSize>>24)&255;

	return byteData;
}

/* static */ void YsSoundPlayer::SoundData::AddUnsignedInt(std::vector <unsigned char> &byteData,unsigned int dat)
{
	unsigned char buf[4];
	buf[0]=dat&255;
	buf[1]=(dat>>8)&255;
	buf[2]=(dat>>16)&255;
	buf[3]=(dat>>24)&255;
	byteData.push_back(buf[0]);
	byteData.push_back(buf[1]);
	byteData.push_back(buf[2]);
	byteData.push_back(buf[3]);
}

/* static */ void YsSoundPlayer::SoundData::AddUnsignedShort(std::vector <unsigned char> &byteData,unsigned short dat)
{
	unsigned char buf[2];
	buf[0]=dat&255;
	buf[1]=(dat>>8)&255;
	byteData.push_back(buf[0]);
	byteData.push_back(buf[1]);
}
