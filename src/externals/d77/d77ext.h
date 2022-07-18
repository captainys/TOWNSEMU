#ifndef D77EXT_IS_INCLUDED
#define D77EXT_IS_INCLUDED
/* { */



#include <string>
#include <vector>
#include "d77.h"

class D77ExtraInfo
{
public:
	enum
	{
		ERR_NOERROR,
		ERR_CANNOT_OPEN,
		ERR_UNDEFINED_TAG,
		ERR_INSUFFICIENT_ARGS
	};

	class SectorExtInfo
	{
	public:
		// If true, take C, H, R as logical C, H, R information stored in the
		// address mark, which may be different from the actual location.
		// If false, take C, H, R as physical location in the disk image.
		// R will be index to the sector in the track of the disk image.
		bool isLogicalLoc=true;
		unsigned int C,H,R;

		// If non zero, the sector must be read at a different rate from the
		// rate calculated from the RPM and the track length.
		unsigned int nanosecPerByte=0;

		// The bytes indexed by unstableBytes[i] in this sector must change
		// value every time the sector is read.
		std::vector <unsigned int> unstableBytes;
	};

	std::vector <SectorExtInfo> perSector;

	unsigned int ReadD77Ext(std::string fName);
	void Apply(D77File::D77Disk &disk) const;

	static unsigned int DecodeSectorCHR(SectorExtInfo &sec,const std::vector <std::string> &argv);
};


/* } */
#endif
