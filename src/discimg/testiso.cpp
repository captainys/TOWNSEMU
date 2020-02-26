#include <stdio.h>

#include "discimg.h"



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

	return 0;
}
