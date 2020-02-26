#include <stdio.h>

#include "discimg.h"



bool TestSingleSector(const DiscImage &disc,unsigned int HSG)
{
	auto sec0=disc.ReadSectorMODE1(HSG,1);
	if(2048!=sec0.size())
	{
		fprintf(stderr,"Incorrect sector length.\n");
		return false;
	}
	for(auto c : sec0)
	{
		if(c!=HSG)
		{
			fprintf(stderr,"Incorrect sector data Expected %d Reading %d.\n",HSG,c);
			return false;
		}
	}
	return true;
}

int main(int ac,char *av[])
{
	DiscImage disc;
	if(ac<2)
	{
		return 1;
	}

	auto err=disc.Open(av[1]);
	if(DiscImage::ERROR_NOERROR!=err)
	{
		fprintf(stderr,"ERROR: %s\n",DiscImage::ErrorCodeToText(err));
		return 1;
	}


	if(1!=disc.GetNumTracks() || 4!=disc.GetNumSectors())
	{
		fprintf(stderr,"Incorrect number of tracks or number of sectors\n");
		return 1;
	}


	if(true!=TestSingleSector(disc,0) ||
	   true!=TestSingleSector(disc,1) ||
	   true!=TestSingleSector(disc,2) ||
	   true!=TestSingleSector(disc,3))
	{
		return 1;
	}
	if(true==TestSingleSector(disc,4))
	{
		fprintf(stderr,"Overrun.\n");
		return 1;
	}


	return 0;
}
