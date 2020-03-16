/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
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
