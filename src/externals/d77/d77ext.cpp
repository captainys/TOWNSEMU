#include <stdlib.h>
#include <fstream>
#include <iostream>
#include "d77ext.h"


unsigned int D77ExtraInfo::ReadD77Ext(std::string fName)
{
	// Comment line starts with //
	// Anything behind // will be ignored.

	// Specifying Cylinder, Head, Sector:
	//   Three numbers.  Track nine, side zero, sector 1 should be written as  9 0 1
	//   For specifying physical location rather than logical CHR, add CAPITAL 'P' before the track.
	//   No space between 'P' and the number, like P9 0 1

	// Per-sector info starts with 'S' followed by C,H,R.

	// Nanosec Per Byte
	//   Example: Cylinder 6, Side 1, Sector 4 should be read at 32000 nanosec per byte.
	//   S 6 1 4 NSBYTE 32000

	// Unstable bytes
	//   Example: Cylidner 0, Side 0, Sector 247 ($F7) should have unstable bytes from offset 288 ($120) for 4 bytes.
	//   S 0 0 247 USBYTE 288 4

	std::ifstream ifp(fName);
	if(true==ifp.is_open())
	{
		while(true!=ifp.eof())
		{
			std::string str;
			std::getline(ifp,str);

			for(int i=0; i+1<str.size(); ++i)
			{
				if('/'==str[i] && '/'==str[i+1])
				{
					str.resize(i);
					break;
				}
			}

			auto STR=str;
			for(auto &c : STR)
			{
				if('a'<=c && c<='z')
				{
					c+='A'-'a';
				}
			}

			std::vector <std::string> ARGV;
			{
				int state=0;
				for(auto c : STR)
				{
					if(0==state)
					{
						if(' '<c)
						{
							ARGV.push_back("");
							ARGV.back().push_back(c);
							state=1;
						}
					}
					else
					{
						if(c<=' ')
						{
							state=0;
						}
						else
						{
							ARGV.back().push_back(c);
						}
					}
				}
			}
			if(0==ARGV.size())
			{
				continue;
			}

			if('S'==ARGV[0][0])
			{
				SectorExtInfo sec;
				auto err=DecodeSectorCHR(sec,ARGV);
				if(ERR_NOERROR!=err)
				{
					return err;
				}

				for(int i=4; i<ARGV.size(); ++i)
				{
					if("NSBYTE"==ARGV[i])
					{
						if(ARGV.size()<=i+1)
						{
							return ERR_INSUFFICIENT_ARGS;
						}
						sec.nanosecPerByte=atoi(ARGV[i+1].c_str());
						++i;
					}
					if("USBYTE"==ARGV[i])
					{
						if(ARGV.size()<=i+2)
						{
							return ERR_INSUFFICIENT_ARGS;
						}
						UnstableBytes usBytes;
						usBytes.offset=atoi(ARGV[i+1].c_str());
						usBytes.length=atoi(ARGV[i+2].c_str());
						sec.unstableBytes.push_back(usBytes);
						i+=2;
					}
				}
				perSector.push_back(sec);
			}
		}
		return ERR_NOERROR;
	}
	return ERR_CANNOT_OPEN;
}
void D77ExtraInfo::Apply(D77File::D77Disk &disk) const
{
	for(auto &s : perSector)
	{
		D77File::D77Disk::D77Sector *secPtr=nullptr;
		if(true==s.isLogicalLoc)
		{
			secPtr=disk.GetSector(s.C,s.H,s.R);
		}
		else
		{
			secPtr=disk.GetSectorByIndex(s.C,s.H,s.R);
		}
		if(0!=s.nanosecPerByte && nullptr!=secPtr)
		{
			secPtr->nanosecPerByte=s.nanosecPerByte;
			std::cout << "NSBYTE applied to " << s.C << " " << s.H << " " << s.R << std::endl;
		}
		for(auto &usBytes : s.unstableBytes)
		{
			while(secPtr->unstableBytes.size()<secPtr->sectorData.size())
			{
				secPtr->unstableBytes.push_back(false);
			}
			for(unsigned i=0; i<usBytes.length && usBytes.offset+i<secPtr->unstableBytes.size(); ++i)
			{
				secPtr->unstableBytes[usBytes.offset+i]=true;
			}
			std::cout << "USBYTE applied to " << s.C << " " << s.H << " " << s.R << std::endl;
		}
	}
}
/* static */ unsigned int D77ExtraInfo::DecodeSectorCHR(SectorExtInfo &sec,const std::vector <std::string> &argv)
{
	if(argv.size()<4)  // S C H R.  Minimum 4 args.
	{
		return ERR_INSUFFICIENT_ARGS;
	}

	if('P'==argv[1][0])
	{
		sec.isLogicalLoc=false;
		sec.C=atoi(argv[1].c_str()+1);
	}
	else
	{
		sec.isLogicalLoc=true;
		sec.C=atoi(argv[1].c_str());
	}

	sec.H=atoi(argv[2].c_str());
	sec.R=atoi(argv[3].c_str());

	return ERR_NOERROR;
}
