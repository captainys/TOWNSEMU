#ifndef CRTCBASE_H_IS_INCLUDED
#define CRTCBASE_H_IS_INCLUDED

#include "cheapmath.h"
#include <vector>

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
	class AnalogPalette
	{
	public:
		unsigned int codeLatch;
		Vec4ub plt16[2][16];
		Vec4ub plt256[256];
		void Reset(void);

		void Set16(unsigned int page,unsigned int component,unsigned char v);
		void Set256(unsigned int component,unsigned char v);
		void SetRed(unsigned char v,unsigned int PLT); // PLT is (sifter[1]>>2)&3
		void SetGreen(unsigned char v,unsigned int PLT);
		void SetBlue(unsigned char v,unsigned int PLT);

		unsigned char Get16(unsigned int page,unsigned int component) const;
		unsigned char Get256(unsigned int component) const;
		unsigned char GetRed(unsigned int PLT) const;
		unsigned char GetGreen(unsigned int PLT) const;
		unsigned char GetBlue(unsigned int PLT) const;

		void Serialize(std::vector <unsigned char> &data) const;
		void Deserialize(const unsigned char *&data);
	};
};

#endif