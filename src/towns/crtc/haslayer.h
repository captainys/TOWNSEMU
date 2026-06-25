#ifndef HASLAYER_H_IS_INCLUDED
#define HASLAYER_H_IS_INCLUDED

#include "cheapmath.h"

class TownsDeviceHasLayer
{
public:
	class Layer
	{
	public:
		unsigned int bitsPerPixel;
		unsigned int highResRGBSwap; // Hopefully meaningful only in the 24-bit color mode.  Figured 2025/07/02
		unsigned int VRAMAddr;
		unsigned int VRAMOffset;
		unsigned int FlipVRAMOffset;
		unsigned int FMRGVRAMMask;
		Vec2i originOnMonitor;
		unsigned int VRAMHSkipBytes;
		Vec2i sizeOnMonitor;
		Vec2i VRAMCoverage1X;
		Vec2i zoom2x;
		unsigned int bytesPerLine;

		unsigned int HScrollMask,VScrollMask;
	};
};

#endif