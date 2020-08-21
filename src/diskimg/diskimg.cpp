#include "diskimg.h"



extern const unsigned long long sizeof_FDIMG640KB;
extern const unsigned char FDIMG640KB[];

extern const unsigned long long sizeof_FDIMG720KB;
extern const unsigned char FDIMG720KB[];

extern const unsigned long long sizeof_FDIMG1232KB;
extern const unsigned char FDIMG1232KB[];

extern const unsigned long long sizeof_FDIMG1440KB;
extern const unsigned char FDIMG1440KB[];

static std::vector <unsigned char> MakeDiskImg(long long int diskSize,long long int srcLength,const unsigned char src[])
{
	std::vector <unsigned char> dat;
	dat.resize(diskSize);
	for(long long int i=0; i<srcLength; ++i)
	{
		dat[i]=src[i];
	}
	for(long long int i=srcLength; i<diskSize; ++i)
	{
		dat[i]=src[srcLength-1];
	}
	return dat;
}

std::vector <unsigned char> Get1440KBFloppyDiskImage(void)
{
	return MakeDiskImg(1440*1024,sizeof_FDIMG1440KB,FDIMG1440KB);
}

std::vector <unsigned char> Get1232KBFloppyDiskImage(void)
{
	return MakeDiskImg(1232*1024,sizeof_FDIMG1232KB,FDIMG1232KB);
}

std::vector <unsigned char> Get720KBFloppyDiskImage(void)
{
	return MakeDiskImg(720*1024,sizeof_FDIMG720KB,FDIMG720KB);
}

std::vector <unsigned char> Get640KBFloppyDiskImage(void)
{
	return MakeDiskImg(640*1024,sizeof_FDIMG640KB,FDIMG640KB);
}
