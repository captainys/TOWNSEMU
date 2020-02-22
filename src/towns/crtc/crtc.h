#ifndef CRTC_IS_INCLUDED
#define CRTC_IS_INCLUDED
/* { */

#include <vector>

#include "device.h"
#include "cheapmath.h"



class TownsCRTC : public Device
{
public:
	enum
	{
		REG_HSW1=   0x00,
		REG_HSW2=   0x01,
		REG_UNUSED1=0x02,
		REG_UNUSED2=0x03,
		REG_HST=    0x04,
		REG_VST1=   0x05,
		REG_VST2=   0x06,
		REG_EET=    0x07,
		REG_VST=    0x08,
		REG_HDS0=   0x09,
		REG_HDE0=   0x0A,
		REG_HDS1=   0x0B,
		REG_HDE1=   0x0C,
		REG_VDS0=   0x0D,
		REG_VDE0=   0x0E,
		REG_VDS1=   0x0F,
		REG_VDE1=   0x10,
		REG_FA0=    0x11,
		REG_HAJ0=   0x12,
		REG_FO0=    0x13,
		REG_LO0=    0x14,
		REG_FA1=    0x15,
		REG_HAJ1=   0x16,
		REG_FO1=    0x17,
		REG_LO1=    0x18,
		REG_EHAJ=   0x19,
		REG_EVAJ=   0x1A,
		REG_ZOOM=   0x1B,
		REG_CR0=    0x1C,
		REG_CR1=    0x1D,
		REG_FR=     0x1E,
		REG_CR2=    0x1F,
	};

	class Layer
	{
	public:
		int mode; // -1 if it is using non-standard screen mode.
		unsigned bitsPerPixel;
		unsigned int virtualWid,virtualHei;
		unsigned int visibleWid,visibleHei;
		unsigned int bytesPerLine;
	};
	class ScreenModeCache
	{
	public:
		unsigned int numLayers;
		Layer layer[2];
		ScreenModeCache();
		void MakeFMRCompatible(void);
	};

	class State
	{
	public:
		unsigned short crtcReg[32];
		unsigned int crtcAddrLatch;

		unsigned char sifter[4];   // Is it really Sifter?  Isn't it Shifter? [2] pp.140
		unsigned int sifterAddrLatch;

		std::vector <unsigned int> mxVideoOutCtrl;
		unsigned int mxVideoOutCtrlAddrLatch;

		void Reset(void);
	};

	class FMTowns *townsPtr;
	State state;

	bool cached;
	ScreenModeCache cache;

	virtual const char *DeviceName(void) const{return "CRTC";}

	TownsCRTC(class FMTowns *ptr);

	bool InVSYNC(const unsigned long long int townsTime) const;
	bool InHSYNC(const unsigned long long int townsTime) const;

	/*! [2] pp.152
	*/
	bool InSinglePageMode(void) const;

	unsigned int GetBaseClockFreq(void) const;
	unsigned int GetBaseClockScaler(void) const;

	/*! Returns vertical scaling.  Between 1 to 4. */
	unsigned int GetPageZoomV(unsigned char page) const;
	/*! Returns horizontal scaling.  Between 1 to 4. */
	unsigned int GetPageZoomH(unsigned char page) const;
	/*! Tentatively returning (0,0)
	*/
	Vec2i GetTopLeftCorner(unsigned char page) const;
	/*! Returns width and height of the page display size in VGA (640x480) coordinate.
	*/
	Vec2i GetDisplaySize(unsigned char page) const;
	/*! Returns number of bytes in VRAM per line.
	*/
	unsigned int GetBytesPerLine(unsigned char page) const;
	/*! Returns bits per pixel.  4, 8, or 16
	    [2] pp.147
	*/
	unsigned int GetPageBitsPerPixel(unsigned char page) const;


	virtual void IOWriteByte(unsigned int ioport,unsigned int data);
	virtual void IOWriteWord(unsigned int ioport,unsigned int data); // Default behavior calls IOWriteByte twice
	virtual void IOWriteDword(unsigned int ioport,unsigned int data); // Default behavior calls IOWriteByte 4 times
	virtual unsigned int IOReadByte(unsigned int ioport);

	virtual void Reset(void);

	void GetRenderSize(unsigned int &wid,unsigned int &hei) const;

	std::vector <std::string> GetStatusText(void) const;
	std::vector <std::string> GetPageStatusText(int page) const;
};


/* } */
#endif
