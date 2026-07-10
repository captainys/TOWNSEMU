/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */

#ifndef FMT3631_IS_INCLUDED
#define FMT3631_IS_INCLUDED
/* { */

#include "device.h"
#include "ramrom.h"
#include "townsdef.h"
#include "crtcbase.h"


// Device Coord Registers are supposed to be:
// 31        15 14            8 7                      0
//  | prefix   | 0 1 1 0 0 0 0 | R R | Rel | Y X | 0 0 0|
//           |        |        |              |         |
// Translated into:
//  0x83xxx
// However, p9000regs.h translates to 0x81xxx.

// The Status Register is supposed to be:
// 31       15 14                       2   1  0
//  | prefix  | 0 1 0 0 0 0 0 0 0 0 0 0 0 | 0 0|
//  |       |        |       |       |         |
// Translated into:
//   0x82000
// However, p9000regs.h translates to 0x80000.

// Should ignore bit 13?

// FMT-3631 Weitek Power 9000 Windows Accelerator Card.
class FMT3631 : public Device, public MemoryAccess, public TownsCrtcBase
{
public:
	static const uint32_t FMT3631::defPalette[];

	// Based on Linux p9000regs.h and Weitek Power 9100 Graphics Controller Preliminary Nov93
	enum
	{
		//
		VRAM_SIZE       =0x200000,
		COORD_MAX       =4, // Quadrilateral max
		COMMAND_MASK    =0x1FFFFF,

		// RAMDAC Control
		BT_WRITE_ADDR=   0x000080,  // Probably
			BT_CURS_OR_PTN=  0,
			BT_CURS_AND_PTN= 0x80,
		BT_RAMDAC_DATA=  0x000084,
		BT_CURS_RAM_DATA=0x0000AC,  // Probably
		BT_CURS_X_LOW=   0x0000B0,
		BT_CURS_X_HIGH=  0x0000B4,  // Both Linux and Windows writes Low then High.
		BT_CURS_Y_LOW=   0x0000B8,
		BT_CURS_Y_HIGH=  0x0000BC,

		BT_COMMAND_REG_1=0x0000A0,
			BT_CR1_BP16=0x20,
			BT_CR1_BP8= 0x40,
			BT_CR1_565RGB=0x08,
		BT_COMMAND_REG_3=0x0000A8,
			BT_CR3_64SQ_CURSOR=0x04,

		// System Control Registers
		MASTERSWITCH    =0x00000, // Apparently bit4 is enable/disable flag.  Probably FMT3631-specific.
			MS_ENABLE=0x10,
		SYSCONFIG       =0x00004,
		INTERRUPT       =0x00008,
		INTERRUPT_EN    =0x0000C,

		// Parameter Engine Registers 4.4
		// Device Coordinate
		DEVICE_COORD    =0x181000, // See 4.4.1 for low 8 bits.

		// Status
		STATUS          =0x180000,
			STATUS_FLAG_ISSUE_QBN=     0x80000000,
			STATUS_FLAG_BUSY=          0x40000000,
			STATUS_FLAG_PICKED=        0x00000080,
			STATUS_FLAG_PIXEL_SW=      0x00000040,
			STATUS_FLAG_BLIT_SW=       0x00000020,
			STATUS_FLAG_QUAD_SW=       0x00000010,
			STATUS_FLAG_QUAD_CONCAVE=  0x00000008,
			STATUS_FLAG_QUAD_HIDDEN=   0x00000004,
			STATUS_FLAG_QUAD_VISIBLE=  0x00000002,
			STATUS_FLAG_QUAD_INTERSECT=0x00000001,

		// Control and condition
		CTL_COND_BEGIN  =0x180184,

		OOR             =0x180184,
		//               0x180188, // Not used
		CINDEX          =0x18018C,
		WINDOW_OFFSET_XY=0x180190,
		P_W_MIN         =0x180194, // Read Only
		P_W_MAX         =0x180198,
		//               0x18019C, // Not used
		YCLIP           =0x1801A0,
		XCLIP           =0x1801A4,
		XEDGE_LT        =0x1801A8,
		XEDGE_GT        =0x1801AC,
		YEDGE_LT        =0x1801B0,
		YEDGE_GT        =0x1801B4,

		CTL_COND_END    =0x1801B8,


		// Drawing Engine Registers
		// Pixel Processing 4.5
		FGCOLOR         =0x180200,
		BGCOLOR         =0x180204,
		PLANE_MASK      =0x180208,
		DRAWING_MODE    =0x18020C,
		PATTERN_X0      =0x180210,
		PATTERN_Y0      =0x180214,
		RASTER          =0x180218,
			RASTER_OVERSIZED=0x10000,
			RASTER_USEPATTERN=0x20000,
		PIXEL8          =0x18021C,
		WINDOW_MIN      =0x180220,
		WINDOW_MAX      =0x180224,

		DRAWING_ATTRIB_END=0x180228,

		// Pattern
		PATTERN0=        0x180280,
		PATTERN1=        0x180284,
		PATTERN2=        0x180288,
		PATTERN3=        0x18028C,
		PATTERN4=        0x180290,
		PATTERN5=        0x180294,
		PATTERN6=        0x180298,
		PATTERN7=        0x18029C,

		PATTERN_END=     0x1802A0,
		PATTERN_LEN=     8,


		// Video Control Registers 4.6
		// Horizontal
		HRZC            =0x100104,
		HRZT            =0x100108,
		HRZSR           =0x10010C,
		HRZBR           =0x100110,
		HRZBF           =0x100114,
		PREHRZC         =0x100118,
		// Vertical
		VRTC            =0x10011C,
		VRTT            =0x100120,
		VRTSR           =0x100124,
		VRTBR           =0x100128,
		VRTBF           =0x10012C,
		PREVRTC         =0x100130,
		// Repaint
		SRADDR          =0x100134,
		SRTCTL          =0x100138,
		//SRTCTL2         = P9100?
		//QSFCOUNTER      = P9100?
		VIDCTRL_LAST    =0x100138,

		// VRAM Control Registers 4.7
		MEM_CONFIG      =0x100184,
		RFPERIOD        =0x100188,
		RFCOUNT         =0x10018C,
		RLMAX           =0x100190,
		RLCUR           =0x100194,
		VRAMCTRL_LAST   =0x100194,

		// Drawing Coordinate?  Linux driver defines it as META_COORD.
		LOAD_COORD              =0x181200,
		LOAD_COORD_ABS_REL_MASK =0x000020,
		LOAD_COORD_PRIMTYPE_MASK=0x0001C0,
			LOAD_COORD_PRIMTYPE_POINT=0,
			LOAD_COORD_PRIMTYPE_LINE=0x40,
			LOAD_COORD_PRIMTYPE_TRI=0x80,
			LOAD_COORD_PRIMTYPE_QUAD=0xC0,
			LOAD_COORD_PRIMTYPE_RECT=0x100,  // RECT, ABS, XY ->  46181318
			// Linux p9000regs.h implies that RECT command is 0x800,
			// which contradicts with P9100 data sheet.
			// Also contradicts with what comes in from Windows P9000 driver.
		LOAD_COORD_X_Y_XY_MASK  =0x000018,
			LOAD_COORD_X=0x08,
			LOAD_COORD_Y=0x10,
			LOAD_COORD_XY=0x18,


		// Commands
		NEXT_PIXELS_CMD =0x180014,
		QUAD_CMD        =0x180008,
		BLIT_CMD        =0x180004,
		PIXEL1_CMD      =0x180080,
		PIXEL1_SWAP_CMD =0x1E0080,
		PIXEL8_CMD      =0x18000C,
		PIXEL8_SWAP_CMD =0x1E000C,
	};

	class State
	{
	public:
		bool enabled=false;
		std::vector <uint8_t> vram;

		mutable int nLoadedCoord=0;
		int lastLoadedCoord=0;
		Vec2i coord[COORD_MAX];

		uint32_t btCommandReg[4];

		AnalogPalette plt;
		HardwareMouseCursor hwCursor;
		unsigned int hwCursorXY_LowByte[2]={0,0};
		unsigned int writingPalette=0;
		unsigned int writingPaletteRGBCount=0;
		unsigned int writingPaletteRGB[3]={0,0,0};

		// For PIXEL1 and PIXEL8 commands.
		// The doc says position and width are taken from x0, (x2,y2), and y-increment from x3.
		// No mention about NEXT_PIXEL.
		// Also it says NEXT_PIXEL sets up the next bitmap.
		// Linux P9000 driver uses NEXT_PIXEL, but does not load x0,x2,y2, and x3.
		// Windows driver apparently does the same.
		Vec2i pixelLeftUp,pixelCurrent;
		int pixelWid=0,pixelYIncrement=1; // Keep these signed.  Coordinates may go negative.  Don't mix signed and unsigned.

		uint32_t bitsPerPixel=8;
		bool highColor565=false;
		uint32_t masterSwitch=0,sysconfig=0,interrupt=0,interrupt_en=0;
		uint32_t status=0;
		uint32_t drawingAttrib[(DRAWING_ATTRIB_END-FGCOLOR)/4];
		uint32_t videoCtrl[(VIDCTRL_LAST-HRZC)/4];
		uint32_t vramCtrl[(VRAMCTRL_LAST-MEM_CONFIG)/4];
		uint32_t ctlCond[(CTL_COND_END-CTL_COND_BEGIN)/4];
		uint32_t pattern[PATTERN_LEN];
	};
	State state;
	FMT3631 *mutableThis;
	bool monitorCtrl=false,monitorVRAM=false;
	bool breakOnUnsupported=false;

	const char *DeviceName(void) const override {return "FMT3631";}

	const uint32_t baseAddr=TOWNSADDR_FMT3631_BASE;
	const uint32_t vramBaseAddr=TOWNSADDR_FMT3631_VRAM;

	FMT3631(class FMTownsCommon *ptr);

	void PowerOn(void) override;
	void Reset(void) override;

	static int U16toS16(uint32_t in);
	static int U32toS32(uint32_t in);
	static uint32_t S32toU32(int in);
	static uint32_t ByteSwap32(uint32_t in);

	bool IsEnabled(void) const;
	unsigned int Height(void) const;
	unsigned int Width(void) const;
	unsigned int BytesPerLine(void) const;
	unsigned int BitsPerPixel(void) const;
	unsigned int BytesPerPixel(void) const;

	void MakePageLayerInfo(Layer &layer) const;
	const AnalogPalette &GetPalette(void) const;

	unsigned int IOReadByte(unsigned int ioport) override;
	void IOWriteByte(unsigned int ioport,unsigned int data) override;


	template <class returnType,class stateType>
	inline static returnType GetControlWordPtrTemplate(uint32_t physAddr,stateType &state);
	bool IsReadableParameter(uint32_t &data,uint32_t physAddr) const;

	const uint32_t *GetControlWordPtr(unsigned int physAddr) const;
	uint32_t *GetControlWordPtr(unsigned int physAddr);

	Vec2i GetWindowOffset(void) const;
	Vec2i GetWindowMin(void) const;
	Vec2i GetWindowMax(void) const;

	// Device Coordinate command
	void DeviceCoord(uint32_t physAddr,uint32_t data);

	// Load Coordinate Command
	void LoadCoord(uint32_t physAddr,uint32_t data);
	void DeviceCoordOrLoadCoord(Vec2i &coord,Vec2i absRef,Vec2i relRef,uint32_t physAddr,uint32_t data);
	void DrawPoint(void);
	void DrawLine(void);
	void DrawTri(void);
	void DrawQuad(void);
	void DrawRect(Vec2i p0,Vec2i p1);

	// Pixels Command
	void CmdNextPixels(uint32_t physAddr,uint32_t data);
	void CmdPixel1(uint32_t physAddr,uint32_t data,bool doSwap);
	template <class Pixel1LogicOp>
	void CmdPixel1Loop(uint32_t physAddr,uint32_t data,bool doSwap);

	void CmdPixel8(uint32_t physAddr,uint32_t data,bool doSwap);

	uint32_t CmdQuad(uint32_t physAddr);
	uint32_t CmdBlit(uint32_t physAddr);

	bool IsCommand(uint32_t physAddr,uint32_t data);

	void SetControlByte(uint32_t physAddr,uint8_t data);
	void SetControlWord(uint32_t physAddr,uint16_t data);
	void SetControlDword(uint32_t physAddr,uint32_t data);

	unsigned int FetchByte(unsigned int physAddr) const override;
	unsigned int FetchWord(unsigned int physAddr) const override;
	unsigned int FetchDword(unsigned int physAddr) const override;
	void StoreByte(unsigned int physAddr,unsigned char data) override;
	void StoreWord(unsigned int physAddr,unsigned int data) override;
	void StoreDword(unsigned int physAddr,unsigned int data) override;
};

/* } */
#endif
