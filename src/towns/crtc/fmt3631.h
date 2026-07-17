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
	static const uint32_t defPalette[];

	// Based on Linux p9000regs.h and Weitek Power 9100 Graphics Controller Preliminary Nov93
	enum
	{
		//
		VRAM_SIZE_3631  =0x200000,
		VRAM_SIZE_3632  =0x400000,
		COORD_MAX       =4, // Quadrilateral max
		COMMAND_MASK    =0x0FFFFF,

		SYSREG_BEGIN      =0x000004,
		CONTROL_BEGIN_3631=0x100000,
		CONTROL_BEGIN_3632=0x000000,
		VRAM_BEGIN_3631=0x200000,
		VRAM_END_3631=0x400000,
		VRAM_BEGIN_3632=0x800000,
		VRAM_END_3632=0xC00000,

		FMT3632REG_LEN=256, // Unknown purposes.

		// RAMDAC Control
		BT_WRITE_ADDR=   0x000080,  // Probably
			BT_CURS_OR_PTN=  0,
			BT_CURS_AND_PTN= 0x80,
		BT_RAMDAC_DATA=  0x000084,

		// Linux JE4 startxP9 writes:
		//  46000090 01
		//  46000094 FF
		//  46000094 FF
		//  46000094 FF
		//  46000094 00
		//  46000094 00
		//  46000094 00
		// Perfectly matches with the p9000BtRecolorCursor function in p9000BtCurs.c
		// If so, 000090 is BT_CURS_WR_ADDR, and 000094 is BT_CURS_DATA.
		BT_CURS_WR_ADDR= 0x000090,
		BT_CURS_DATA=    0x000094,

		BT_CURS_RAM_DATA=0x0000AC,  // Probably
		BT_CURS_X_LOW=   0x0000B0,
		BT_CURS_X_HIGH=  0x0000B4,  // Both Linux and Windows writes Low then High.
		BT_CURS_Y_LOW=   0x0000B8,
		BT_CURS_Y_HIGH=  0x0000BC,

		BT_COMMAND_REG_1=0x0000A0,
			BT_CR1_BP16=0x20,
			BT_CR1_BP8= 0x40,
			BT_CR1_565RGB=0x08,
		BT_COMMAND_REG_2=0x0000A4,
			BT_CR2_CURSOR_ENABLE=0x02, // Apparently b1 is for enabing, b1 is for two-color cursor, which I don't know what it is.
			BT_CR2_2COLOR_CURSOR=0x01,
		BT_COMMAND_REG_3=0x0000A8,
			BT_CR3_64SQ_CURSOR=0x04,

		// System Control Registers
		MASTERSWITCH    =0x00000, // Apparently bit4 is enable/disable flag.  Probably FMT3631-specific.
			MS_ENABLE=0x10,

		SYSCONFIG       =0x00004,
		INTERRUPT       =0x00008,
		INTERRUPT_EN    =0x0000C,
		ALT_READ_BANK   =0x00010,
		ALT_WRITE_BANK  =0x00014,

		P9100_PALETTE_COLOR_ADDR= 0x00200,
		P9100_PALETTE_COLOR_DATA= 0x00204,
		P9100_CURSOR_REGSEL=      0x00210,
			P9100_CURSOR_REG_ON_OFF=0x30,   // 0xFC:Off  0xFE:On  Prob b1 is the switch.
			P9100_CURSOR_REG_PATTERN=0x00,
			P9100_CURSOR_REG_POSITION=0x31,
		P9100_CURSOR_UNKNOWN=     0x00214,
		P9100_CURSOR_DATA=        0x00218,
		P9100_CURSOR_IS_ARRAYDATA=0x0021C, // 1->Sequence of data  0->1-byte of data

		// Parameter Engine Registers 4.4
		// Device Coordinate
		DEVICE_COORD    =0x81000, // See 4.4.1 for low 8 bits.

		// Status
		STATUS          =0x80000,
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
		CTL_COND_BEGIN  =0x80184,

		OOR             =0x80184,
		//               0x80188, // Not used
		CINDEX          =0x8018C,
		WINDOW_OFFSET_XY=0x80190,
		P_W_MIN         =0x80194, // Read Only
		P_W_MAX         =0x80198,
		//               0x8019C, // Not used
		YCLIP           =0x801A0,
		XCLIP           =0x801A4,
		XEDGE_LT        =0x801A8,
		XEDGE_GT        =0x801AC,
		YEDGE_LT        =0x801B0,
		YEDGE_GT        =0x801B4,

		CTL_COND_END    =0x801B8,


		// Drawing Engine Registers
		// Pixel Processing 4.5
		FGCOLOR         =0x80200,
		BGCOLOR         =0x80204,
		PLANE_MASK      =0x80208,
		DRAWING_MODE    =0x8020C,
		PATTERN_X0      =0x80210,
		PATTERN_Y0      =0x80214,
		RASTER          =0x80218,
			RASTER_OVERSIZED=0x10000,
			RASTER_USEPATTERN=0x20000,

			RASTER_P9100_TRANSPARENT=0x20000,
			RASTER_P9100_PIXEL1_TRANSPARENT=0x8000,
			RASTER_P9100_PATTERN_DEPTH=0x4000,
			RASTER_P9100_PATTERN_ENABLE=0x2000, // Solid_color_disable.
		PIXEL8          =0x8021C,
		WINDOW_MIN      =0x80220,  // Write only use P_W_MIN to read.
		WINDOW_MAX      =0x80224,  // Write only use P_W_MIN to read.

		DRAWING_ATTRIB_END=0x80228,

		// Pattern
		PATTERN0=        0x80280,
		PATTERN1=        0x80284,
		PATTERN2=        0x80288,
		PATTERN3=        0x8028C,
		PATTERN4=        0x80290,
		PATTERN5=        0x80294,
		PATTERN6=        0x80298,
		PATTERN7=        0x8029C,

		PATTERN_END=     0x802A0,
		PATTERN_LEN=     8,


		// Video Control Registers 4.6
		// Horizontal
		HRZC            =0x00104,
		HRZT            =0x00108,
		HRZSR           =0x0010C,
		HRZBR           =0x00110,
		HRZBF           =0x00114,
		PREHRZC         =0x00118,
		// Vertical
		VRTC            =0x0011C,
		VRTT            =0x00120,
		VRTSR           =0x00124,
		VRTBR           =0x00128,
		VRTBF           =0x0012C,
		PREVRTC         =0x00130,
		// Repaint
		SRADDR          =0x00134,
		SRTCTL          =0x00138,
		SRTCTL2         =0x0013C,
		QSFCOUNTER      =0x00140,
		VIDCTRL_LAST    =0x00144,

		// VRAM Control Registers 4.7
		MEM_CONFIG      =0x00184,
		RFPERIOD        =0x00188,
		RFCOUNT         =0x0018C,
		RLMAX           =0x00190,
		RLCUR           =0x00194,
		VRAMCTRL_LAST   =0x00198,

		POWER_UP_CONFIG =0x00198, // Read only.  Accessed only from IsReadableParameter

		// Drawing Coordinate?  Linux driver defines it as META_COORD.
		LOAD_COORD              =0x81200,
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
			LOAD_COORD_PRIMTYPE_NOT_LOADED=0xFF,
			LOAD_COORD_PRIMTYPE_NOT_TYPED=0xFE,
		LOAD_COORD_X_Y_XY_MASK  =0x000018,
			LOAD_COORD_X=0x08,
			LOAD_COORD_Y=0x10,
			LOAD_COORD_XY=0x18,


		// Commands
		NEXT_PIXELS_CMD =0x80014,
		QUAD_CMD        =0x80008,
		BLIT_CMD        =0x80004,
		PIXEL1_CMD      =0x80080,
		PIXEL1_WORD_SWAP_CMD= 0xC0080,
		PIXEL1_BYTE_SWAP_CMD =0xE0080,
		PIXEL1_BIT_REVERSE_CMD=0xF0080,
		PIXEL8_CMD      =0x8000C,
		PIXEL8_WORD_SWAP_CMD=0xC000C,
		PIXEL8_BYTE_SWAP_CMD =0xE000C,
		PIXEL8_BIT_REVERSE_CMD =0xF000C,

		// Power 9100 only
		// Power 9100 registers are 0x2xxx, but to make it common with Power 9000,
		// 0x2000 (bit 13) will be moved to 0x80000 (bit 19).
		// Therefore, keep it 0x80xxx even for Power 9100 registers.
		COLOR2=          0x80238, // 010 0010 0011 1000 P9100 only
		COLOR3=          0x8023C, // 010 0010 0011 1100 P9100 only
		BYTE_WIN_MIN=    0x802A0, // 010 0010 1010 0000 P9100 only
		BYTE_WIN_MAX=    0x802A4, // 010 0010 1010 0100 P9100 only

		FGCOLOR_BYTESWAP=0xE0200,
		BGCOLOR_BYTESWAP=0xE0204,
		COLOR2_BYTESWAP=0xE0238,
		COLOR3_BYTESWAP=0xE023C,
	};

	class State
	{
	friend class FMT3631;

	private:
		bool enabled=false;
		bool isFMT3632=false;
	public:
		std::vector <uint8_t> vram;

		uint32_t BaseAddr=0;
		uint32_t RAMDACBaseAddr=0;
		uint32_t ControlBaseAddr=0;
		uint32_t VRAMBaseAddr=0,VRAMEndAddr=0;

		mutable int nLoadedCoord=0;
		int nextLoadIndex=0,lastLoadedCoord=0;
		Vec2i coord[COORD_MAX];
		uint32_t metaCoordType[COORD_MAX];

		uint32_t color2_3[2];  // P9100 only.
		uint32_t byteWinMinMax[2]; // P9100 only.

		uint32_t btCommandReg[4];
		uint8_t btWriteAddr_Low=0;

		uint32_t p9100CursorRegSel=0;
		uint32_t p9100CursorDataCount=0;

		AnalogPalette plt;
		HardwareMouseCursor hwCursor;
		unsigned int hwCursorTwoColorReadPos=0;
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

		uint16_t fmt3632RegSel=0;  // What is this register for?? -> Looks like Configuration Registers.
		uint8_t fmt3632Regs[FMT3632REG_LEN];  // See p.17 of Power 9100 Graphics Controller Datasheet.

		uint32_t bitsPerPixel=8;
		bool highColor565=false;
		uint32_t masterSwitch=0,sysconfig=0,interrupt=0,interrupt_en=0;
		uint32_t alt_read_bank=0,alt_write_bank=0;
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
	int sneak=0;

	const char *DeviceName(void) const override {return "FMT3631";}

	FMT3631(class FMTownsCommon *ptr);

	void Disable(void);
	void EnableAsFMT3631(void);
	void EnableAsFMT3632(void);

	void PowerOn(void) override;
	void Reset(void) override;

	static int U16toS16(uint32_t in);
	static int U32toS32(uint32_t in);
	static uint32_t S32toU32(int in);
	static uint32_t ByteSwap32(uint32_t in);

	uint16_t GetBtWriteAddr(void) const
	{
		uint16_t addr=0;
		addr=state.btCommandReg[3]&3;
		addr<<=8;
		addr|=uint8_t(state.btWriteAddr_Low);
		return addr;
	}

	inline uint32_t Translate3631to3632(uint32_t addr) const
	{
		if(true==state.isFMT3632)
		{
			if(addr&0x2000)
			{
				addr|=0x80000;
				addr&=~0x2000;
			}
		}
		return addr;
	}
	inline void ProcessDwordReverse(uint32_t &addr,uint32_t &data) const
	{
		if(addr&0x40000)
		{
			auto up=(data<<16)&0xFFFF0000; // AABBCCDD -> CCDDAABB
			auto down=(data>>16)&0xFFFF;
			data=up|down;
		}
		if(addr&0x20000)
		{
			auto up=(data<<8)&0xFF00FF00;  // CCDDAABB -> DDCCBBAA
			auto down=(data>>8)&0x00FF00FF;
			data=up|down;
		}
		if(addr&0x10000)
		{
			auto D=0;
			uint32_t mask=0x80808080;
			for(int shift=7; 1<=shift; shift-=2)
			{
				D|=(data&mask)>>shift;
				mask>>=1;
			}
			for(int shift=1; shift<=7; shift+=2)
			{
				D|=(data&mask)<<shift;
				mask>>=1;
			}
			data=D;
		}
		addr&=~0x70000;
	}

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
	void SysConfigToBpp3632(void);

	const uint32_t *GetControlWordPtr(unsigned int physAddr) const;
	uint32_t *GetControlWordPtr(unsigned int physAddr);

	Vec2i GetWindowOffset(void) const;
	Vec2i GetWindowMin(void) const;
	Vec2i GetWindowMax(void) const;

	// Device Coordinate command
	void DeviceCoord(uint32_t physAddr,uint32_t data);

	// Load Coordinate Command
	void LoadCoord(uint32_t physAddr,uint32_t data);
	int DeviceCoordOrLoadCoord(Vec2i &coord,Vec2i absRef,Vec2i relRef,uint32_t physAddr,uint32_t data);
	void ClearLoadedFlags(void);
	void DrawPoint(void);
	void DrawLine(Vec2i p0,Vec2i p1);
	void DrawTri(void);
	void DrawQuad(void);
	void DrawRect(Vec2i p0,Vec2i p1);

	// Pixels Command
	void CmdNextPixels(uint32_t physAddr,uint32_t data);
	void CmdPixel1(uint32_t physAddr,uint32_t data,bool byteSwap,bool bitSwap,bool wordSwap);
	template <class Pixel1LogicOp>
	void CmdPixel1LoopP9000(uint32_t physAddr,uint32_t data,bool byteSwap,bool bitSwap);
	template <class Pixel1LogicOp>
	void CmdPixel1LoopP9100(uint32_t physAddr,uint32_t data,bool byteSwap,bool bitSwap);

	void CmdPixel8(uint32_t physAddr,uint32_t data,bool byteSwap,bool bitSwap,bool wordSwap);

	uint32_t CmdQuad(uint32_t physAddr);
	uint32_t CmdBlit(uint32_t physAddr);

	bool IsCommand(uint32_t physAddr,uint32_t data);

	void SetControlByte(uint32_t physAddr,uint8_t data);
	void SetControlWord(uint32_t physAddr,uint16_t data);
	void SetControlDword(uint32_t physAddr,uint32_t data);

	std::vector <std::string> GetStatusText(void) const;
	std::vector <std::string> GetAdditionalStatusText(void) const;

	unsigned int FetchByte(unsigned int physAddr) const override;
	unsigned int FetchWord(unsigned int physAddr) const override;
	unsigned int FetchDword(unsigned int physAddr) const override;
	void StoreByte(unsigned int physAddr,unsigned char data) override;
	void StoreWord(unsigned int physAddr,unsigned int data) override;
	void StoreDword(unsigned int physAddr,unsigned int data) override;

	/*! Version used for serialization.
	*/
	uint32_t SerializeVersion(void) const override;
	/*! Device-specific Serialization.
	*/
	void SpecificSerialize(std::vector <unsigned char> &data,std::string stateFName) const override;
	/*! Device-specific De-serialization.
	*/
	bool SpecificDeserialize(const unsigned char *&data,std::string stateFName,uint32_t version) override;
};

/* } */
#endif
