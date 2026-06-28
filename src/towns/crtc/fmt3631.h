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
#include "haslayer.h"


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
class FMT3631 : public Device, public MemoryAccess, public TownsDeviceHasLayer
{
public:
	// Based on Linux p9000regs.h and Weitek Power 9100 Graphics Controller Preliminary Nov93
	enum
	{
		//
		VRAM_SIZE       =0x200000,
		COORD_MAX       =4, // Quadrilateral max

		// System Control Registers
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

		// Drawing Engine Registers
		// Pixel Processing 4.5
		FGCOLOR         =0x180200,
		BGCOLOR         =0x180204,
		PLANE_MASK      =0x180208,
		DRAWING_MODE    =0x18020C,
		PATTERN_X0      =0x180210,
		PATTERN_Y0      =0x180214,
		RASTER          =0x180218,
		PIXEL8          =0x18021C,
		WINDOW_MIN      =0x180220,
		WINDOW_MAX      =0x180224,

		DRAWING_ATTRIB_END=0x180228,


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

		// Drawing Coordinate?
		LOAD_COORD              =0x181200,
		LOAD_COORD_ABS_REL_MASK =0x000020,
		LOAD_COORD_PRIMTYPE_MASK=0x0001C0,
			LOAD_COORD_PRIMTYPE_POINT=0,
			LOAD_COORD_PRIMTYPE_LINE=0x40,
			LOAD_COORD_PRIMTYPE_TRI=0x80,
			LOAD_COORD_PRIMTYPE_QUAD=0xC0,
			LOAD_COORD_PRIMTYPE_RECT=0x100,
			// Linux p9000regs.h implies that RECT command is 0x800,
			// which contradicts with P9100 data sheet.
			// Also contradicts with what comes in from Windows P9000 driver.
		LOAD_COORD_X_Y_XY_MASK  =0x000018,
			LOAD_COORD_X=0x08,
			LOAD_COORD_Y=0x10,
			LOAD_COORD_XY=0x18,
	};

	class State
	{
	public:
		bool enabled=false;
		std::vector <uint8_t> vram;

		int nLoadedCoord=0;
		int lastLoadedCoord=0;
		Vec2i coord[COORD_MAX];

		uint32_t bitsPerPixel=8;
		uint32_t sysconfig=0,interrupt=0,interrupt_en=0;
		uint32_t status=0;
		uint32_t drawingAttrib[(DRAWING_ATTRIB_END-FGCOLOR)/4];
		uint32_t videoCtrl[(VIDCTRL_LAST-HRZC)/4];
		uint32_t vramCtrl[(VRAMCTRL_LAST-MEM_CONFIG)/4];
	};
	State state;
	bool monitor=true;

	const char *DeviceName(void) const override {return "FMT3631";}

	const uint32_t baseAddr=TOWNSADDR_FMT3631_BASE;
	const uint32_t vramBaseAddr=TOWNSADDR_FMT3631_VRAM;

	FMT3631(class FMTownsCommon *ptr);

	void PowerOn(void) override;
	void Reset(void) override;

	bool IsEnabled(void) const;
	unsigned int Height(void) const;
	unsigned int Width(void) const;
	unsigned int BytesPerLine(void) const;
	unsigned int BitsPerPixel(void) const;

	void MakePageLayerInfo(Layer &layer) const;
	const AnalogPalette &GetPalette(void) const;

	unsigned int IOReadByte(unsigned int ioport) override;
	void IOWriteByte(unsigned int ioport,unsigned int data) override;


	template <class returnType,class stateType>
	inline static returnType GetControlWordPtrTemplate(uint32_t physAddr,stateType &state);

	const uint32_t *GetControlWordPtr(unsigned int physAddr) const;
	uint32_t *GetControlWordPtr(unsigned int physAddr);

	// Load Coordinate Command
	void LoadCoord(uint32_t physAddr,uint32_t data);
	void DrawPoint(void);
	void DrawLine(void);
	void DrawTri(void);
	void DrawQuad(void);
	void DrawRect(void);

	void SetControlByte(uint32_t physAddr,uint8_t data);
	void SetControlWord(uint32_t physAddr,uint16_t data);
	void SetControlDword(uint32_t physAddr,uint32_t data);

	unsigned int FetchByte(unsigned int physAddr) const override;
	unsigned int FetchDword(unsigned int physAddr) const override;
	void StoreByte(unsigned int physAddr,unsigned char data) override;
	void StoreDword(unsigned int physAddr,unsigned int data) override;
};

/* } */
#endif
