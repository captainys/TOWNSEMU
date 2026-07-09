#ifndef CRTCBASE_H_IS_INCLUDED
#define CRTCBASE_H_IS_INCLUDED

#include "cheapmath.h"
#include <vector>

class TownsCrtcBase
{
public:
	class Layer
	{
	public:
		unsigned int bitsPerPixel;
		bool highColor565=false; // FMT-3631 can use RRRRRGGGGGGBBBBB
		bool highColorGRB=true;
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
	class HardwareMouseCursor
	{
	public:
		bool defining=false,defined=false;
		uint32_t ptnCount=0;
		uint32_t unknownValueReg8=0;
		uint32_t X=0,Y=0;
		uint32_t originX=0,originY=0;
		uint32_t wid=64; // Can be 64 or 32.
		uint8_t ANDPtn[512]={0},ORPtn[512]={0};

		void Reset(void);
	};
};

#endif