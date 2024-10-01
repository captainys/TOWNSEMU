/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#ifndef TOWNSDEF_IS_INCLUDED
#define TOWNSDEF_IS_INCLUDED
/* { */



#include <stdint.h>

#ifdef __cplusplus
	#include <string>

	const uint64_t PER_SECOND=1000000000;  // Unit: Nano Seconds
	const uint64_t TOWNS_RENDERING_FREQUENCY=PER_SECOND/60;

	const uint32_t TOWNS_VRAM_SIZE     =1024*1024;
	const uint32_t TOWNS_MIDRES_VRAM_SIZE=512*1024;
	const uint32_t TOWNS_CMOS_SIZE     =8192;
	const uint32_t TOWNS_WAVERAM_SIZE  =65536;
	const uint32_t TOWNS_SPRITERAM_SIZE=128*1024;
	const uint32_t TOWNS_CVRAM_SIZE    =32*1024;

	const uint32_t TBIOS_ID_FREQUENCY=100;

	const uint32_t TOWNS_NUM_GAMEPORTS=2;

	const uint32_t TOWNS_CD_READ_SECTOR_TIME_1X=13300000; // nanoseconds
	const uint32_t TOWNS_CD_SEEK_TIME_1X     =2000000000; // nanoseconds.    // From BCC: Official specification of 1X CD-ROM Seek Time is 3 seconds.  2secs should be close enough with no harm.
#endif



enum
{
	TOWNSTYPE_UNKNOWN,
	FMR_50_60,
	FMR_50S,
	FMR_70,
	TOWNSTYPE_MODEL1_2,  // 1st Gen: model1, model2
	TOWNSTYPE_1F_2F,     // 2nd Gen: 1F,2F
	TOWNSTYPE_10F_20F,   // 3rd Gen: 10F,20F
	TOWNSTYPE_2_UX,
	TOWNSTYPE_2_CX,
	TOWNSTYPE_2_UG,
	TOWNSTYPE_2_HG,
	TOWNSTYPE_2_HR,
	TOWNSTYPE_2_UR,
	TOWNSTYPE_2_MA,
	TOWNSTYPE_2_MX,
	TOWNSTYPE_2_ME,
	TOWNSTYPE_2_MF_FRESH,
	TOWNSTYPE_2_HC,
	TOWNSTYPE_MARTY,
};

#ifdef __cplusplus
std::string TownsTypeToStr(unsigned int townsType);
unsigned int StrToTownsType(std::string str);
#endif

enum
{
	TOWNSCPU_UNKNOWN,
	TOWNSCPU_80386DX,
	TOWNSCPU_80386SX,
	TOWNSCPU_80486DX,
	TOWNSCPU_80486SX,
	TOWNSCPU_PENTIUM,
};

enum
{
	TOWNSIRQ_TIMER=0,
	TOWNSIRQ_KEYBOARD=1,
	TOWNSIRQ_RS232C=2,
	TOWNSIRQ_EXT_RS232C=3,
	TOWNSIRQ_MIDI_SERIAL=4,
	TOWNSIRQ_MIDI_TIMER=5,
	TOWNSIRQ_FDC=6,
	TOWNSIRQ_PIC_BRIDGE=7,
	TOWNSIRQ_SCSI=8,
	TOWNSIRQ_CDROM=9,
	TOWNSIRQ_EXPANSION3=10,
	TOWNSIRQ_VSYNC=11,
	TOWNSIRQ_PRINTER=12,
	TOWNSIRQ_SOUND=13,
	TOWNSIRQ_EXPANSION4=14,
	TOWNSIRQ_UNUSED=15,
};

enum
{
	// [2] pp. 56
	TOWNSDMA_FPD=0,
	TOWNSDMA_SCSI=1,
	TOWNSDMA_PRINTER=2,
	TOWNSDMA_CDROM=3
};

/*! Test of I/O FF82H behavior on FM Towns 2F indicated that the FM-R mode VRAM offset for Page 1
    is not 0x20000.  It seems to be off by 13 and half lines.
    However, so far no FM Towns applications actually using this offset has been confirmed.
*/
#define TOWNS_FMRMODE_VRAM_OFFSET         0x20000

#define TOWNSADDR_FMR_VRAM_BASE           0xC0000
#define TOWNSADDR_FMR_VRAM_END            0xC8000

#define TOWNSADDR_FMR_CVRAM_BASE          0xC8000
#define TOWNSADDR_FMR_CVRAM_END           0xC9000
#define TOWNSADDR_FMR_KVRAM_END           0xCB000

#define TOWNSADDR_FMR_ANK8_BASE           0xCA000 // [2] pp.12 Figure I-1-6
#define TOWNSADDR_FMR_ANK8_END            0xCB000
#define TOWNSADDR_FMR_ANK8_MAP            0x3D000 // Error in [2] pp.12 Figue I-1-6?  Offset 1E800 is �� not A

#define TOWNSADDR_FMR_ANK16_BASE          0xCB000 // [2] pp.12 Figure I-1-6
#define TOWNSADDR_FMR_ANK16_END           0xCC000
#define TOWNSADDR_FMR_ANK16_MAP           0x3D800 // Error in [2] pp.12 Figure I-1-6?  Offset 1F000 is �� not wide-A

#define TOWNSADDR_FMR_VRAM_CVRAM_FONT_END 0xD0000


#define TOWNSADDR_FMR_DICROM_BASE         0xD0000
#define TOWNSADDR_FMR_DICROM_END          0xD8000

#define TOWNSADDR_BACKUP_RAM_BASE         0xD8000
#define TOWNSADDR_BACKUP_RAM_END          0xDA000

#define TOWNSADDR_FMR_RESERVED_BASE       0xDA000
#define TOWNSADDR_FMR_RESERVED_END        0xF0000

#define TOWNSADDR_SYSROM_MAP_BASE         0xF8000
#define TOWNSADDR_SYSROM_MAP_END         0x100000
#define TOWNSADDR_SYSROM_MAP_SIZE          0x8000
#define TOWNSADDR_SYSROM_MAP_OFFSET_DIFFERENCE ((256-32)*1024)   // SYSROM 256KB.  Last 32KB is mapped to 0xF8000 to 0xFFFFF.

#define TOWNSADDR_VRAM0_BASE           0x80000000
#define TOWNSADDR_VRAM0_END            0x80080000
#define TOWNSADDR_VRAM_AND             0x000FFFFF

#define TOWNSADDR_VRAM1_BASE           0x80100000
#define TOWNSADDR_VRAM1_END            0x80180000

#define TOWNSADDR_SPRITERAM_BASE       0x81000000
#define TOWNSADDR_SPRITERAM_END        0x81020000
#define TOWNSADDR_SPRITERAM_AND        (TOWNSADDR_SPRITERAM_END-TOWNSADDR_SPRITERAM_BASE-1)


#define TOWNSADDR_VRAM_HIGHRES0_BASE   0x82000000
#define TOWNSADDR_VRAM_HIGHRES0_END    (TOWNSADDR_VRAM_HIGHRES0_BASE+512*1024)

#define TOWNSADDR_VRAM_HIGHRES1_BASE   0x82800000
#define TOWNSADDR_VRAM_HIGHRES1_END    (TOWNSADDR_VRAM_HIGHRES1_BASE+512*1024)

#define TOWNSADDR_VRAM_HIGHRES2_BASE   0x83000000
#define TOWNSADDR_VRAM_HIGHRES2_END    (TOWNSADDR_VRAM_HIGHRES2_BASE+1024*1024)

#define TOWNSADDR_MEMCARD_OLD_BASE     0xC0000000
#define TOWNSADDR_MEMCARD_OLD_END      0xC1000000
#define TOWNSADDR_MEMCARD_AND          (TOWNSADDR_MEMCARD_OLD_END-TOWNSADDR_MEMCARD_OLD_BASE-1)
#define TOWNSADDR_386SX_MEMCARD_AND    (TOWNSADDR_386SX_MEMCARD_END-TOWNSADDR_386SX_MEMCARD_BASE-1)

#define TOWNSADDR_MEMCARD_JEIDA4_BASE  0xC1000000
#define TOWNSADDR_MEMCARD_JEIDA4_END   0xC2000000

#define TOWNSADDR_OSROM_BASE           0xC2000000
#define TOWNSADDR_OSROM_END            0xC2080000
#define TOWNSADDR_OSROM_AND            (TOWNSADDR_OSROM_END-TOWNSADDR_OSROM_BASE-1)

#define TOWNSADDR_NATIVE_DICROM_BASE   0xC2080000
#define TOWNSADDR_NATIVE_DICROM_END    0xC2100000
#define TOWNSADDR_NATIVE_DICROM_AND    (TOWNSADDR_NATIVE_DICROM_END-TOWNSADDR_NATIVE_DICROM_BASE-1)

#define TOWNSADDR_FONT_BASE            0xC2100000
#define TOWNSADDR_FONT_END             0xC2140000
#define TOWNSADDR_FONT_AND             (TOWNSADDR_FONT_END-TOWNSADDR_FONT_BASE-1)

#define TOWNSADDR_NATIVE_CMOSRAM_BASE  0xC2140000
#define TOWNSADDR_NATIVE_CMOSRAM_END   0xC2142000
#define TOWNSADDR_NATIVE_CMOSRAM_AND   (TOWNSADDR_NATIVE_CMOSRAM_END-TOWNSADDR_NATIVE_CMOSRAM_BASE-1)

#define TOWNSADDR_FONT20_BASE          0xC2180000
#define TOWNSADDR_FONT20_END           0xC2200000
#define TOWNSADDR_FONT20_AND           (TOWNSADDR_FONT20_END-TOWNSADDR_FONT20_BASE-1)

#define TOWNSADDR_WAVERAM_WINDOW_BASE  0xC2200000
#define TOWNSADDR_WAVERAM_WINDOW_END   0xC2201000
#define TOWNSADDR_WAVERAM_WINDOW_AND   (TOWNSADDR_WAVERAM_WINDOW_END-TOWNSADDR_WAVERAM_WINDOW_BASE-1)

#define TOWNSADDR_SYSROM_BASE          0xFFFC0000
#define TOWNSADDR_SYSROM_AND           0x0003FFFF



#define TOWNSADDR_MARTY_ROM0_BASE            0x680000
#define TOWNSADDR_MARTY_ROM0_END             0x700000

#define TOWNSADDR_MARTY_ROM1_BASE            0x700000
#define TOWNSADDR_MARTY_ROM1_END             0x780000

#define TOWNSADDR_MARTY_ROM2_BASE            0x780000
#define TOWNSADDR_MARTY_ROM2_END             0x800000

#define TOWNSADDR_MARTY_ROM3_BASE            0x800000
#define TOWNSADDR_MARTY_ROM3_END             0x880000



#define TOWNSADDR_386SX_VRAM0_BASE           0xA00000
#define TOWNSADDR_386SX_VRAM0_END            0xA80000

#define TOWNSADDR_386SX_VRAM1_BASE           0xB00000
#define TOWNSADDR_386SX_VRAM1_END            0xB80000

#define TOWNSADDR_386SX_SPRITERAM_BASE       0xC00000
#define TOWNSADDR_386SX_SPRITERAM_END        0xC20000


#define TOWNSADDR_386SX_MEMCARD_BASE         0xD00000
#define TOWNSADDR_386SX_MEMCARD_END          0xE00000

#define TOWNSADDR_386SX_OSROM_BASE           0xE00000
#define TOWNSADDR_386SX_OSROM_END            0xE80000

#define TOWNSADDR_386SX_NATIVE_DICROM_BASE   0xE80000
#define TOWNSADDR_386SX_NATIVE_DICROM_END    0xF00000

#define TOWNSADDR_386SX_FONT_BASE            0xF00000
#define TOWNSADDR_386SX_FONT_END             0xF40000

#define TOWNSADDR_386SX_NATIVE_CMOSRAM_BASE  0xF40000
#define TOWNSADDR_386SX_NATIVE_CMOSRAM_END   0xF42000

#define TOWNSADDR_386SX_WAVERAM_WINDOW_BASE  0xF80000
#define TOWNSADDR_386SX_WAVERAM_WINDOW_END   0xF81000

#define TOWNSADDR_386SX_SYSROM_BASE          0xFC0000
#define TOWNSADDR_386SX_SYSROM_END          0x1000000



// When adding an IO port, make sure to register in FMTownsCommon::FMTownsCommon()
enum
{
	TOWNSIO_PIC_PRIMARY_ICW1=          0x00,
	TOWNSIO_PIC_PRIMARY_ICW2_3_4_OCW=  0x02,
	TOWNSIO_PIC_SECONDARY_ICW1=        0x10,
	TOWNSIO_PIC_SECONDARY_ICW2_3_4_OCW=0x12,

	TOWNSIO_RESET_REASON=              0x20,
	TOWNSIO_POWER_CONTROL=             0x22,

	TOWNSIO_CPU_MISC3=                 0x24,

	TOWNSIO_CPU_MISC4=                 0x25, // FM TOWNS Techincal Databook p.778

	TOWNSIO_FREERUN_TIMER     =0x26,
	TOWNSIO_FREERUN_TIMER_LOW =0x26,
	TOWNSIO_FREERUN_TIMER_HIGH=0x27,

	TOWNSIO_NMI_MASK=      0x28,

	TOWNSIO_MACHINE_ID_LOW=         0x30,
	TOWNSIO_MACHINE_ID_HIGH=        0x31,

	TOWNSIO_SERIAL_ROM_CTRL=        0x32,

	TOWNSIO_TIMER0_COUNT=             0x40,
	TOWNSIO_TIMER1_COUNT=             0x42,
	TOWNSIO_TIMER2_COUNT=             0x44,
	TOWNSIO_TIMER_0_1_2_CTRL=         0x46,
	TOWNSIO_TIMER3_COUNT=             0x50,
	TOWNSIO_TIMER4_COUNT=             0x52,
	TOWNSIO_TIMER5_COUNT=             0x54,
	TOWNSIO_TIMER_3_4_5_CTRL=         0x56,
	TOWNSIO_TIMER_INT_CTRL_INT_REASON=0x60,

	TOWNSIO_INTERVAL_TIMER2_CONTROL=  0x68, // FM TOWNS Technical Databook p.779
	TOWNSIO_INTERVAL_TIMER2_DATA_LOW= 0x6A, // FM TOWNS Technical Databook p.779
	TOWNSIO_INTERVAL_TIMER2_DATA_HIGH=0x6B, // FM TOWNS Technical Databook p.779

	TOWNSIO_TIMER_1US_WAIT=           0x6C, // Since Towns 20F

	TOWNSIO_RTC_DATA=                 0x70,
	TOWNSIO_RTC_COMMAND=              0x80,

	TOWNSIO_DMAC_INITIALIZE=          0xA0,
	TOWNSIO_DMAC_CHANNEL=             0xA1,
	TOWNSIO_DMAC_COUNT_LOW=           0xA2,
	TOWNSIO_DMAC_COUNT_HIGH=          0xA3,
	TOWNSIO_DMAC_ADDRESS_LOWEST=      0xA4,
	TOWNSIO_DMAC_ADDRESS_MIDLOW=      0xA5,
	TOWNSIO_DMAC_ADDRESS_MIDHIGH=     0xA6,
	TOWNSIO_DMAC_ADDRESS_HIGHEST=     0xA7,
	TOWNSIO_DMAC_DEVICE_CONTROL_LOW=  0xA8,
	TOWNSIO_DMAC_DEVICE_CONTROL_HIGH= 0xA9,
	TOWNSIO_DMAC_MODE_CONTROL=        0xAA,
	TOWNSIO_DMAC_STATUS=              0xAB,
	TOWNSIO_DMAC_TEMPORARY_REG_LOW=   0xAC,
	TOWNSIO_DMAC_TEMPORARY_REG_HIGH=  0xAD,
	TOWNSIO_DMAC_REQUEST=             0xAE,
	TOWNSIO_DMAC_MASK=                0xAF,

	TOWNSIO_EXDMAC_INITIALIZE=          0xB0,
	TOWNSIO_EXDMAC_CHANNEL=             0xB1,
	TOWNSIO_EXDMAC_COUNT_LOW=           0xB2,
	TOWNSIO_EXDMAC_COUNT_HIGH=          0xB3,
	TOWNSIO_EXDMAC_ADDRESS_LOWEST=      0xB4,
	TOWNSIO_EXDMAC_ADDRESS_MIDLOW=      0xB5,
	TOWNSIO_EXDMAC_ADDRESS_MIDHIGH=     0xB6,
	TOWNSIO_EXDMAC_ADDRESS_HIGHEST=     0xB7,
	TOWNSIO_EXDMAC_DEVICE_CONTROL_LOW=  0xB8,
	TOWNSIO_EXDMAC_DEVICE_CONTROL_HIGH= 0xB9,
	TOWNSIO_EXDMAC_MODE_CONTROL=        0xBA,
	TOWNSIO_EXDMAC_STATUS=              0xBB,
	TOWNSIO_EXDMAC_TEMPORARY_REG_LOW=   0xBC,
	TOWNSIO_EXDMAC_TEMPORARY_REG_HIGH=  0xBD,
	TOWNSIO_EXDMAC_REQUEST=             0xBE,
	TOWNSIO_EXDMAC_MASK=                0xBF,

	TOWNSIO_80486_CACHE_CONTROL=      0xC0,
	TOWNSIO_80486_CACHE_DIAGNOSTIC=   0xC2,

	TOWNSIO_FDC_STATUS_COMMAND=       0x200, // [2] pp.253
	TOWNSIO_FDC_TRACK=                0x202, // [2] pp.253
	TOWNSIO_FDC_SECTOR=               0x204, // [2] pp.253
	TOWNSIO_FDC_DATA=                 0x206, // [2] pp.253
	TOWNSIO_FDC_DRIVE_STATUS_CONTROL= 0x208, // [2] pp.253
	TOWNSIO_FDC_DRIVE_SELECT=         0x20C, // [2] pp.253
	TOWNSIO_FDC_FDDV_EXT=             0x20D, // [2] pp.809
	TOWNSIO_FDC_DRIVE_SWITCH=         0x20E, // [2] pp.253

	TOWNSIO_FMR_RESOLUTION=           0x400, // [2] pp.91

	TOWNSIO_FMR_VRAM_OR_MAINRAM=      0x404, // [2] pp.91

	TOWNSIO_SPRITE_ADDRESS=           0x450, // [2] pp.128
	TOWNSIO_SPRITE_DATA=              0x452, // [2] pp.128

	TOWNSIO_VRAMACCESSCTRL_ADDR=      0x458, // [2] pp.17,pp.112
	TOWNSIO_VRAMACCESSCTRL_DATA_LOW=  0x45A, // [2] pp.17,pp.112
	TOWNSIO_VRAMACCESSCTRL_DATA_HIGH= 0x45B, // [2] pp.17,pp.112

	TOWNSIO_CRTC_ADDRESS=             0x440,
	TOWNSIO_CRTC_DATA_LOW=            0x442,
	TOWNSIO_CRTC_DATA_HIGH=           0x443,

	TOWNSIO_VIDEO_OUT_CTRL_ADDRESS=   0x448,
	TOWNSIO_VIDEO_OUT_CTRL_DATA=      0x44A,

	TOWNSIO_DPMD_SPRITEBUSY_SPRITEPAGE=0x44C,

	TOWNSIO_MX_HIRES=                 0x470,
	TOWNSIO_MX_VRAMSIZE=              0x471,
	TOWNSIO_MX_IMGOUT_ADDR_LOW=       0x472,
	TOWNSIO_MX_IMGOUT_ADDR_HIGH=      0x473,
	TOWNSIO_MX_IMGOUT_D0=             0x474,
	TOWNSIO_MX_IMGOUT_D1=             0x475,
	TOWNSIO_MX_IMGOUT_D2=             0x476,
	TOWNSIO_MX_IMGOUT_D3=             0x477,

	TOWNSIO_SYSROM_DICROM=            0x480, // [2] pp.91
	TOWNSIO_DICROM_BANK=              0x484, // [2] pp.92

	TOWNSIO_MEMCARD_STATUS=           0x48A, // [2] pp.93
	TOWNSIO_MEMCARD_BANK=             0x490, // [2] pp.794
	TOWNSIO_MEMCARD_ATTRIB=           0x491, // [2] pp.795

	TOWNSIO_CDROM_CAPS=               0x4B0, // FM TOWNS Techincal Databook p.847

	TOWNSIO_CDROM_MASTER_CTRL_STATUS=0x4C0, // [2] pp.224
	TOWNSIO_CDROM_COMMAND_STATUS=    0x4C2, // [2] pp.224
	TOWNSIO_CDROM_PARAMETER_DATA=    0x4C4, // [2] pp.224
	TOWNSIO_CDROM_TRANSFER_CTRL=     0x4C6, // [2] pp.227
	TOWNSIO_CDROM_CACHE_2XSPEED=     0x4C8, // [2] pp.847
	TOWNSIO_CDROM_SUBCODE_STATUS=    0x4CC,
	TOWNSIO_CDROM_SUBCODE_DATA=      0x4CD,

	TOWNSIO_GAMEPORT_A_INPUT=        0x4D0,
	TOWNSIO_GAMEPORT_B_INPUT=        0x4D2,
	TOWNSIO_GAMEPORT_OUTPUT=         0x4D6,

	TOWNSIO_SOUND_MUTE=              0x4D5, // [2] pp.18,
	TOWNSIO_SOUND_STATUS_ADDRESS0=   0x4D8, // [2] pp.18,
	TOWNSIO_SOUND_DATA0=             0x4DA, // [2] pp.18,
	TOWNSIO_SOUND_ADDRESS1=          0x4DC, // [2] pp.18,
	TOWNSIO_SOUND_DATA1=             0x4DE, // [2] pp.18,

	TOWNSIO_SOUND_SAMPLING_DATA=     0x4E7, // [2] pp.179,
	TOWNSIO_SOUND_SAMPLING_FLAGS=    0x4E8, // [2] pp.179,

	TOWNSIO_SOUND_INT_REASON=        0x4E9, // [2] pp.19,
	TOWNSIO_SOUND_PCM_INT_MASK=      0x4EA, // [2] pp.19,
	TOWNSIO_SOUND_PCM_INT=           0x4EB, // [2] pp.19,
	TOWNSIO_SOUND_PCM_ENV=           0x4F0, // [2] pp.19,
	TOWNSIO_SOUND_PCM_PAN=           0x4F1, // [2] pp.19,
	TOWNSIO_SOUND_PCM_FDL=           0x4F2, // [2] pp.19,
	TOWNSIO_SOUND_PCM_FDH=           0x4F3, // [2] pp.19,
	TOWNSIO_SOUND_PCM_LSL=           0x4F4, // [2] pp.19,
	TOWNSIO_SOUND_PCM_LSH=           0x4F5, // [2] pp.19,
	TOWNSIO_SOUND_PCM_ST=            0x4F6, // [2] pp.19,
	TOWNSIO_SOUND_PCM_CTRL=          0x4F7, // [2] pp.19,
	TOWNSIO_SOUND_PCM_CH_ON_OFF=     0x4F8, // [2] pp.19,

	// Electric Volume affects multiple devices.
	// FMTownsCommon class instead of Sound or CDRom classes intercepts these.
	TOWNSIO_ELEVOL_1_DATA=           0x4E0, // [2] pp.18, pp.174
	TOWNSIO_ELEVOL_1_COM=            0x4E1, // [2] pp.18, pp.174
	TOWNSIO_ELEVOL_2_DATA=           0x4E2, // [2] pp.18, pp.174
	TOWNSIO_ELEVOL_2_COM=            0x4E3, // [2] pp.18, pp.174

	TOWNSIO_HIGHRESPCM_BANK=         0x510, // [2] pp.832
	TOWNSIO_HIGHRESPCM_DMASTATUS=    0x511, // [2] pp.832
	TOWNSIO_HIGHRESPCM_DMACOUNT_LOW= 0x512, // [2] pp.833
	TOWNSIO_HIGHRESPCM_DMACOUNT_HIGH=0x513, // [2] pp.833
	TOWNSIO_HIGHRESPCM_DMAADDR_LOW=  0x514, // [2] pp.834
	TOWNSIO_HIGHRESPCM_DMAADDR_MIDLOW=0x515, // [2] pp.834
	TOWNSIO_HIGHRESPCM_DMAADDR_MIDHIGH=0x516, // [2] pp.834
	TOWNSIO_HIGHRESPCM_DMAADDR_HIGH= 0x517, // [2] pp.834
	TOWNSIO_HIGHRESPCM_CLOCK=0x518,         // [2] pp.834
	TOWNSIO_HIGHRESPCM_MODE=         0x519, // [2] pp.835
	TOWNSIO_HIGHRESPCM_SYSCONTROL=   0x51A, // [2] pp.836
	TOWNSIO_HIGHRESPCM_BUFFCONTROL=  0x51B, // [2] pp.837
	TOWNSIO_HIGHRESPCM_REC_PLAYBACK= 0x51C, // [2] pp.838
	TOWNSIO_HIGHRESPCM_REC_PEAK_MON= 0x51D, // [2] pp.839
	TOWNSIO_HIGHRESPCM_DATA_LOW=     0x51E, // [2] pp.840
	TOWNSIO_HIGHRESPCM_DATA_HIGH=    0x51F, // [2] pp.840

	TOWNSIO_NMI_MASK_STATUS=         0x5C0, // FM TOWNS Technical Databook p.19

	TOWNSIO_TVRAM_WRITE=             0x5C8, // [2] pp.19, pp.94
	TOWNSIO_WRITE_TO_CLEAR_VSYNCIRQ= 0x5CA, // [2] pp.19, pp.95

	TOWNSIO_MAINRAM_WAIT_1STGEN=     0x5E0,
	TOWNSIO_MAINRAM_WAIT=            0x5E2,
	TOWNSIO_VRAMWAIT=                0x5E6,
	TOWNSIO_MEMSIZE=                  0x5E8, // [2] pp.827
	TOWNSIO_FASTMODE=                0x5EC, // [2] pp.794
	TOWNSIO_MAXIMUM_CLOCK=           0x5ED, // FM TOWNS Techincal Databook p.800

	TOWNSIO_VRAMCACHE_CONTROL=       0x5EE, // FM TOWNS Techincal Databook p.816

	TOWNSIO_KEYBOARD_DATA=       0x600, // [2] pp.234
	TOWNSIO_KEYBOARD_STATUS_CMD= 0x602, // [2] pp.231
	TOWNSIO_KEYBOARD_IRQ=        0x604, // [2] pp.236

	TOWNSIO_BUFFUL=               0x606, // FM TOWNS Techincal Databook p.789

	TOWNSIO_PRINTER_STATUS1_DATA=     0x800, // FM TOWNS Techincal Databook p.20
	TOWNSIO_PRINTER_STATUS2_CONTROL=  0x802, // FM TOWNS Techincal Databook p.20
	TOWNSIO_PRINTER_INT_CONTROL=      0x804, // FM TOWNS Techincal Databook p.20

	TOWNSIO_RS232C_STATUS_COMMAND=0xA02, // [2] pp.269
	TOWNSIO_RS232C_DATA=          0xA00, // [2] pp.274
	TOWNSIO_RS232C_STATUS2=       0xA04, // FM TOWNS Techincal Databook p.20
	TOWNSIO_RS232C_INT_REASON=    0xA06, // [2] pp.275
	TOWNSIO_RS232C_INT_CONTROL=   0xA08, // [2] pp.276
	TOWNSIO_RS232C_MODEM_CONTROL= 0xA0A, // FM TOWNS Techincal Databook p.21

	TOWNSIO_FUSART_FIFO_MODE=     0xA0C, // FM TOWNS Technical Databook p.829
	TOWNSIO_FUSART_FIFO_STATUS=   0xA0D, // FM TOWNS Technical Databook p.829
	TOWNSIO_FUSART_FIFO_CONTROL=  0xA0E, // FM TOWNS Technical Databook p.830

	TOWNSIO_SCSI_DATA=           0xC30, // [2] pp.263
	TOWNSIO_SCSI_STATUS_CONTROL= 0xC32, // [2] pp.262
	TOWNSIO_SCSI_WORD_TFR_AVAIL= 0xC34, // [2] pp.801


	TOWNSIO_MIDI_CARD1_DATREG1=0x0E50,         // MIDI card(MT-402 or 403) No.1 Out port 1 datReg1 (from Linux source)
	TOWNSIO_MIDI_CARD1_CMDREG1=0x0E51,         // MIDI card(MT-402 or 403) No.1 cmdReg1 (Linux source)
	TOWNSIO_MIDI_CARD1_DATREG2=0x0E54,         // MIDI card(MT-402 or 403) No.1
	TOWNSIO_MIDI_CARD1_CMDREG2=0x0E55,         // MIDI card(MT-402 or 403) No.1
	TOWNSIO_MIDI_CARD1_FIFODAT=0x0E52,         // MIDI card(MT-402 or 403) No.1
	TOWNSIO_MIDI_CARD1_FIFOREG=0x0E53,         // MIDI card(MT-402 or 403) No.1

	TOWNSIO_MIDI_CARD2_DATREG1=0x0E58,         // MIDI card 2
	TOWNSIO_MIDI_CARD2_CMDREG1=0x0E59,         // MIDI card 2
	TOWNSIO_MIDI_CARD2_DATREG2=0x0E5C,         // MIDI card 2
	TOWNSIO_MIDI_CARD2_CMDREG2=0x0E5D,         // MIDI card 2
	TOWNSIO_MIDI_CARD2_FIFODAT=0x0E5A,         // MIDI card 2
	TOWNSIO_MIDI_CARD2_FIFOREG=0x0E5B,         // MIDI card 2

	TOWNSIO_MIDI_CARD3EF_DATREG1=0x0E60,       // MIDI card 3 [E][F] according to Linux midi.c
	TOWNSIO_MIDI_CARD3EF_CMDREG1=0x0E61,       // MIDI card 3 [E][F]
	TOWNSIO_MIDI_CARD3EF_DATREG2=0x0E64,       // MIDI card 3 [E][F] according to Linux midi.c
	TOWNSIO_MIDI_CARD3EF_CMDREG2=0x0E65,       // MIDI card 3 [E][F]
	TOWNSIO_MIDI_CARD3EF_FIFODAT=0x0E62,       // MIDI card 3 [E][F] according to Linux midi.c
	TOWNSIO_MIDI_CARD3EF_FIFOREG=0x0E63,       // MIDI card 3 [E][F]

	TOWNSIO_MIDI_CARD3GH_DATREG1=0x0E68,       // MIDI card 3 [G][H] according to Linux midi.c
	TOWNSIO_MIDI_CARD3GH_CMDREG1=0x0E69,       // MIDI card 3 [G][H]
	TOWNSIO_MIDI_CARD3GH_DATREG2=0x0E6C,       // MIDI card 3 [G][H] according to Linux midi.c
	TOWNSIO_MIDI_CARD3GH_CMDREG2=0x0E6D,       // MIDI card 3 [G][H]
	TOWNSIO_MIDI_CARD3GH_FIFODAT=0x0E6A,       // MIDI card 3 [G][H] according to Linux midi.c
	TOWNSIO_MIDI_CARD3GH_FIFORET=0x0E6B,       // MIDI card 3 [G][H]

	TOWNSIO_MIDI_CARD1GEN2_DATREG1=0x04A8,     // 2nd Gen MIDI-1 according to Linux midi.c
	TOWNSIO_MIDI_CARD1GEN2_CMDREG1=0x04A9,     // 2nd Gen MIDI-1
	TOWNSIO_MIDI_CARD1GEN2_DATREG2=0x04AC,     // 2nd Gen MIDI-1 according to Linux midi.c
	TOWNSIO_MIDI_CARD1GEN2_CMDREG2=0x04AD,     // 2nd Gen MIDI-1
	TOWNSIO_MIDI_CARD1GEN2_FIFODAT=0x04AA,     // 2nd Gen MIDI-1 according to Linux midi.c
	TOWNSIO_MIDI_CARD1GEN2_FIFOREG=0x04AB,     // 2nd Gen MIDI-1

	TOWNSIO_MIDI_INT_MASK_SEND=0x0E70,    // MIDI SEND interrupt MASK
	TOWNSIO_MIDI_INT_MASK_RECEIVE=0x0E71, // MIDI RECEIVE interrupt MASK

	TOWNSIO_MIDI_INT_MASK_SEND_FMT401GEN2=0x4A0, // MIDI INT Mask for FMT-401 Second Gen (according to Linux midi.c)
	TOWNSIO_MIDI_INT_MASK_RECEIVE_FMT401GEN2=0x4A1, // MIDI INT Mask for FMT-401 Second Gen (according to Linux midi.c)

	// Probably >>
	TOWNSIO_MIDI_TIMER_INT_CTRL_INT_REASON=0x0E73,
	TOWNSIO_MIDI_TIMER0_COUNT=             0x0E74,
	TOWNSIO_MIDI_TIMER1_COUNT=             0x0E75,
	TOWNSIO_MIDI_TIMER2_COUNT=             0x0E76,
	TOWNSIO_MIDI_TIMER_CTRL=               0x0E77,
	// Probably <<



	TOWNSIO_CMOS_BASE=           0x3000,
	TOWNSIO_CMOS_END=            0x4000,

	TOWNSIO_ANALOGPALETTE_CODE=  0xFD90,
	TOWNSIO_ANALOGPALETTE_BLUE=  0xFD92,
	TOWNSIO_ANALOGPALETTE_RED=   0xFD94,
	TOWNSIO_ANALOGPALETTE_GREEN= 0xFD96,

	TOWNSIO_FMR_DIGITALPALETTE0= 0xFD98,
	TOWNSIO_FMR_DIGITALPALETTE1= 0xFD99,
	TOWNSIO_FMR_DIGITALPALETTE2= 0xFD9A,
	TOWNSIO_FMR_DIGITALPALETTE3= 0xFD9B,
	TOWNSIO_FMR_DIGITALPALETTE4= 0xFD9C,
	TOWNSIO_FMR_DIGITALPALETTE5= 0xFD9D,
	TOWNSIO_FMR_DIGITALPALETTE6= 0xFD9E,
	TOWNSIO_FMR_DIGITALPALETTE7= 0xFD9F,

	TOWNSIO_HSYNC_VSYNC=         0xFDA0,

	TOWNSIO_CRT_OUTPUT_CONTROL_READ=0xFDA2, // FM TOWNS Technical Databook p.802
	TOWNSIO_CRT_READ_COMPATIBLE= 0xFDA4,    // FM TOWNS Technical Databook p.803

	TOWNSIO_FMR_VRAMMASK=        0xFF81, 
	// Not official.  But 2MX BIOS is obviously writing it.
	// It apparently works same ax memory-mapped I/O 000CFF81H.
	TOWNSIO_FMR_VRAMDISPLAYMODE= 0xFF82, 
	// Not official.  But, since FMR_VRAMMASK is also mapped to I/O FF81H, FF82H may be too.
	TOWNSIO_FMR_VRAMPAGESEL=     0xFF83, 
	// Not official.  But, since FMR_VRAMMASK is also mapped to I/O FF81H, FF83H may be too.

	// The 4th Unit Series ACT4 Zero uses FF86 for HSYNC and VSYNC.
	TOWNSIO_FMR_HSYNC_VSYNC=     0xFF86,

	TOWNSIO_FMR_VRAMDISPLAYMODE_READ=0xFF88, // FM TOWNS Technical Databook p.803  Memory-Mapped 0xCFF88

	TOWNSIO_KANJI_JISCODE_HIGH=  0xFF94,
	TOWNSIO_KANJI_JISCODE_LOW=   0xFF95,
	TOWNSIO_KANJI_PTN_HIGH=      0xFF96,
	TOWNSIO_KANJI_PTN_LOW=       0xFF97,
	TOWNSIO_KVRAM_OR_ANKFONT=    0xFF99,

	TOWNSIO_KANJI_CG_ROW=        0xFF9E, // FM TOWNS Technical Databook p.803  Memory-Mapped 0xCFF9E

	// 0x2000 to 0x2FFF must be open for testing for Fujitsu's internal development.
	// [2] pp.12
	// I take two addresses for VM-Host communication.
	// In the VM, push parameters to TOWNSIO_VM_HOST_IF_DATA, and then write a command.
	TOWNSIO_VM_HOST_IF_CMD_STATUS=0x2386,
	TOWNSIO_VM_HOST_IF_DATA=      0x2387,

	TOWNSIO_VM_TGDRV=             0x2388,

	TOWNSIO_VNDRV_APICHECK=       0x2F10,
	TOWNSIO_VNDRV_ENABLE=         0x2F12,
	TOWNSIO_VNDRV_COMMAND=        0x2F14,
	TOWNSIO_VNDRV_AUXCOMMAND=     0x2F18,

	TOWNS_QUICK_DEBUG_BREAK=      0xEA,  // Writing to this I/O port will break the VM.
	TOWNS_QUICK_DEBUG_STATE=      0xEB,  // Writing to this I/O port will show the VM state, not break.
};

enum
{
	TOWNSMEMIO_MIX=                0x000CFF80, // Called Dummy [2] pp.22, pp.158
	TOWNSMEMIO_FMR_GVRAMMASK=      0x000CFF81, // [2] pp.22,pp.159
	TOWNSMEMIO_FMR_GVRAMDISPMODE=  0x000CFF82, // [2] pp.22,pp.158
	TOWNSMEMIO_FMR_GVRAMPAGESEL=   0x000CFF83, // [2] pp.22,pp.159
	TOWNSMEMIO_FIRQ=               0x000CFF84, // [2] pp.22,pp.95 Always zero in FM TOWNS
	TOWNSMEMIO_FMR_HSYNC_VSYNC=    0x000CFF86, // [2] pp.22,pp.160

	TOWNSMEMIO_KANJI_JISCODE_HIGH= 0x000CFF94,
	TOWNSMEMIO_KANJI_JISCODE_LOW=  0x000CFF95,
	TOWNSMEMIO_KANJI_PTN_HIGH=     0x000CFF96,
	TOWNSMEMIO_KANJI_PTN_LOW=      0x000CFF97,
	TOWNSMEMIO_BUZZER_CONTROL=     0x000CFF98,
	TOWNSMEMIO_KVRAM_OR_ANKFONT=   0x000CFF99,
};

enum
{
	TOWNS_MEMIO_1_LOW=    0x0000FD90,
	TOWNS_MEMIO_1_HIGH=   0x0000FDA0,
	TOWNS_MEMIO_2_LOW=    0x000CFF80,
	TOWNS_MEMIO_2_HIGH=   0x000CFFBB,
};

enum
{
	TOWNS_ELEVOL_FOR_LINE=     0,
	TOWNS_ELEVOL_FOR_PCM=      0,
	TOWNS_ELEVOL_FOR_CD=       1,
	TOWNS_ELEVOL_FOR_MIC_IN=   1,
	TOWNS_ELEVOL_FOR_MODEM_OUT=1,

	TOWNS_ELEVOL_LINE_IN_LEFT= 0,
	TOWNS_ELEVOL_LINE_IN_RIGHT=1,
	TOWNS_ELEVOL_PCM_LEFT=     2,
	TOWNS_ELEVOL_PCM_RIGHT=    3,

	TOWNS_ELEVOL_CD_LEFT=      0,
	TOWNS_ELEVOL_CD_RIGHT=     1,
	TOWNS_ELEVOL_MIC_IN=       2,
	TOWNS_ELEVOL_MODEM_OUT=    3,

	TOWNS_ELEVOL_MAX=63,
};

enum
{
	TOWNS_KEYFLAG_THUMBSHIFT_RIGHT=0x01,
	TOWNS_KEYFLAG_THUMBSHIFT_LEFT= 0x02,
	TOWNS_KEYFLAG_SHIFT=           0x04,
	TOWNS_KEYFLAG_CTRL=            0x08,
	TOWNS_KEYFLAG_PRESS=           0x00, // It's zero, but for the sake of symmetry.
	TOWNS_KEYFLAG_RELEASE=         0x10,
	TOWNS_KEYFLAG_TYPE_THUMBSHIFT= 0x00,
	TOWNS_KEYFLAG_TYPE_JIS=        0x20,
	TOWNS_KEYFLAG_TYPE_NEWJIS=     0x40,
	TOWNS_KEYFLAG_TYPE_UNDEFINED=  0x60,
	TOWNS_KEYFLAG_TYPE_FIRSTBYTE=  0x80,
};

enum
{
	TOWNS_KEYBOARD_MODE_DEFAULT,
	TOWNS_KEYBOARD_MODE_DIRECT,
	TOWNS_KEYBOARD_MODE_TRANSLATION1, // ESC -> BREAK+ESC
	TOWNS_KEYBOARD_MODE_TRANSLATION2, // ESC -> ESC
	TOWNS_KEYBOARD_MODE_TRANSLATION3, // ESC -> BREAK

TOWNS_KEYBOARD_MODE_NUM_MODES
};

#ifdef __cplusplus
	std::string TownsKeyboardModeToStr(unsigned int keyboardMode);
	unsigned int TownsStrToKeyboardMode(std::string str);
#endif

#define TOWNS_KEYFLAG_JIS_PRESS (TOWNS_KEYFLAG_TYPE_FIRSTBYTE|TOWNS_KEYFLAG_TYPE_JIS|TOWNS_KEYFLAG_PRESS)
#define TOWNS_KEYFLAG_JIS_RELEASE (TOWNS_KEYFLAG_TYPE_FIRSTBYTE|TOWNS_KEYFLAG_TYPE_JIS|TOWNS_KEYFLAG_RELEASE)

enum
{
	TOWNS_JISKEY_NULL=   0,

	TOWNS_JISKEY_BREAK=  0x7C,
	TOWNS_JISKEY_COPY=   0x7D,
	TOWNS_JISKEY_PF01=   0x5D,
	TOWNS_JISKEY_PF02=   0x5E,
	TOWNS_JISKEY_PF03=   0x5F,
	TOWNS_JISKEY_PF04=   0x60,
	TOWNS_JISKEY_PF05=   0x61,
	TOWNS_JISKEY_PF06=   0x62,
	TOWNS_JISKEY_PF07=   0x63,
	TOWNS_JISKEY_PF08=   0x64,
	TOWNS_JISKEY_PF09=   0x65,
	TOWNS_JISKEY_PF10=   0x66,
	TOWNS_JISKEY_PF11=   0x69,
	TOWNS_JISKEY_PF12=   0x5B,

	TOWNS_JISKEY_PF13=   0x74,
	TOWNS_JISKEY_PF14=   0x75,
	TOWNS_JISKEY_PF15=   0x76,
	TOWNS_JISKEY_PF16=   0x77,
	TOWNS_JISKEY_PF17=   0x78,
	TOWNS_JISKEY_PF18=   0x79,
	TOWNS_JISKEY_PF19=   0x7A,
	TOWNS_JISKEY_PF20=   0x7B,

	TOWNS_JISKEY_KANJI_DIC= 0x6B,
	TOWNS_JISKEY_ERASE_WORD=0x6C,
	TOWNS_JISKEY_ADD_WORD=  0x6D,
	TOWNS_JISKEY_PREV=      0x6E,
	TOWNS_JISKEY_HOME=      0x4E,
	TOWNS_JISKEY_NEXT=      0x70,
	TOWNS_JISKEY_CHAR_PITCH=0x71,
	TOWNS_JISKEY_DELETE=    0x4B,
	TOWNS_JISKEY_INSERT=    0x48,

	TOWNS_JISKEY_UP=        0x4D,
	TOWNS_JISKEY_LEFT=      0x4F,
	TOWNS_JISKEY_RIGHT=     0x51,
	TOWNS_JISKEY_DOWN=      0x50,

	TOWNS_JISKEY_EXECUTE=   0x73,

	TOWNS_JISKEY_ESC=       0x01,
	TOWNS_JISKEY_1=         0x02,
	TOWNS_JISKEY_2=         0x03,
	TOWNS_JISKEY_3=         0x04,
	TOWNS_JISKEY_4=         0x05,
	TOWNS_JISKEY_5=         0x06,
	TOWNS_JISKEY_6=         0x07,
	TOWNS_JISKEY_7=         0x08,
	TOWNS_JISKEY_8=         0x09,
	TOWNS_JISKEY_9=         0x0A,
	TOWNS_JISKEY_0=         0x0B,
	TOWNS_JISKEY_MINUS=     0x0C,
	TOWNS_JISKEY_HAT=       0x0D,
	TOWNS_JISKEY_BACKSLASH= 0x0E,
	TOWNS_JISKEY_BACKSPACE= 0x0F,

	TOWNS_JISKEY_TAB=             0x10,
	TOWNS_JISKEY_Q=               0x11,
	TOWNS_JISKEY_W=               0x12,
	TOWNS_JISKEY_E=               0x13,
	TOWNS_JISKEY_R=               0x14,
	TOWNS_JISKEY_T=               0x15,
	TOWNS_JISKEY_Y=               0x16,
	TOWNS_JISKEY_U=               0x17,
	TOWNS_JISKEY_I=               0x18,
	TOWNS_JISKEY_O=               0x19,
	TOWNS_JISKEY_P=               0x1A,
	TOWNS_JISKEY_AT=              0x1B,
	TOWNS_JISKEY_LEFT_SQ_BRACKET= 0x1C,
	TOWNS_JISKEY_RETURN=          0x1D,

	TOWNS_JISKEY_CTRL=            0x52,
	TOWNS_JISKEY_A=               0x1E,
	TOWNS_JISKEY_S=               0x1F,
	TOWNS_JISKEY_D=               0x20,
	TOWNS_JISKEY_F=               0x21,
	TOWNS_JISKEY_G=               0x22,
	TOWNS_JISKEY_H=               0x23,
	TOWNS_JISKEY_J=               0x24,
	TOWNS_JISKEY_K=               0x25,
	TOWNS_JISKEY_L=               0x26,
	TOWNS_JISKEY_SEMICOLON=       0x27,
	TOWNS_JISKEY_COLON=           0x28,
	TOWNS_JISKEY_RIGHT_SQ_BRACKET=0x29,

	TOWNS_JISKEY_SHIFT=           0x53,
	TOWNS_JISKEY_Z=               0x2A,
	TOWNS_JISKEY_X=               0x2B,
	TOWNS_JISKEY_C=               0x2C,
	TOWNS_JISKEY_V=               0x2D,
	TOWNS_JISKEY_B=               0x2E,
	TOWNS_JISKEY_N=               0x2F,
	TOWNS_JISKEY_M=               0x30,
	TOWNS_JISKEY_COMMA=           0x31,
	TOWNS_JISKEY_DOT=             0x32,
	TOWNS_JISKEY_SLASH=           0x33,
	TOWNS_JISKEY_DOUBLEQUOTE=     0x34,

	TOWNS_JISKEY_HIRAGANA=        0x56,
	TOWNS_JISKEY_CAPS=            0x55,
	TOWNS_JISKEY_SPACE=           0x35,
	TOWNS_JISKEY_KANA_KANJI=      0x59,
	TOWNS_JISKEY_KATAKANA=        0x5A,
	TOWNS_JISKEY_CANCEL=          0x72,
	TOWNS_JISKEY_NO_CONVERT=      0x57,
	TOWNS_JISKEY_CONVERT=         0x58,

	TOWNS_JISKEY_NUM_STAR=        0x36,
	TOWNS_JISKEY_NUM_SLASH=       0x37,
	TOWNS_JISKEY_NUM_PLUS=        0x38,
	TOWNS_JISKEY_NUM_MINUS=       0x39,
	TOWNS_JISKEY_NUM_7=           0x3A,
	TOWNS_JISKEY_NUM_8=           0x3B,
	TOWNS_JISKEY_NUM_9=           0x3C,
	TOWNS_JISKEY_NUM_EQUAL=       0x3D,
	TOWNS_JISKEY_NUM_4=           0x3E,
	TOWNS_JISKEY_NUM_5=           0x3F,
	TOWNS_JISKEY_NUM_6=           0x40,
	TOWNS_JISKEY_NUM_DOT=         0x47,
	TOWNS_JISKEY_NUM_1=           0x42,
	TOWNS_JISKEY_NUM_2=           0x43,
	TOWNS_JISKEY_NUM_3=           0x44,
	TOWNS_JISKEY_NUM_RETURN=      0x45,
	TOWNS_JISKEY_NUM_0=           0x46,
	TOWNS_JISKEY_NUM_000=         0x4A,

	TOWNS_JISKEY_ALT=             0x5C, // Thanks, WINDY!
};

#ifdef __cplusplus
	std::string TownsKeyCodeToStr(unsigned int keycode);
	unsigned int TownsStrToKeyCode(std::string str);
#endif

enum
{
	BOOT_KEYCOMB_NONE,
	BOOT_KEYCOMB_CD,
	BOOT_KEYCOMB_F0,
	BOOT_KEYCOMB_F1,
	BOOT_KEYCOMB_F2,
	BOOT_KEYCOMB_F3,
	BOOT_KEYCOMB_H0,
	BOOT_KEYCOMB_H1,
	BOOT_KEYCOMB_H2,
	BOOT_KEYCOMB_H3,
	BOOT_KEYCOMB_H4,
	BOOT_KEYCOMB_ICM,
	BOOT_KEYCOMB_DEBUG,
	BOOT_KEYCOMB_PAD_A,
	BOOT_KEYCOMB_PAD_B,
	BOOT_KEYCOMB_FASTMODE,
	BOOT_KEYCOMB_SLOWMODE,
BOOT_KEYCOMB_ERROR,
};

#ifdef __cplusplus
	unsigned int TownsStrToKeyComb(std::string str);
	std::string TownsKeyCombToStr(unsigned int keycomb);
#endif

enum
{
	TOWNS_GAMEPORTEMU_NONE,
	TOWNS_GAMEPORTEMU_MOUSE,
	TOWNS_GAMEPORTEMU_KEYBOARD,
	TOWNS_GAMEPORTEMU_PHYSICAL0,
	TOWNS_GAMEPORTEMU_PHYSICAL1,
	TOWNS_GAMEPORTEMU_PHYSICAL2,
	TOWNS_GAMEPORTEMU_PHYSICAL3,
	TOWNS_GAMEPORTEMU_PHYSICAL4,
	TOWNS_GAMEPORTEMU_PHYSICAL5,
	TOWNS_GAMEPORTEMU_PHYSICAL6,
	TOWNS_GAMEPORTEMU_PHYSICAL7,
	TOWNS_GAMEPORTEMU_ANALOG0,
	TOWNS_GAMEPORTEMU_ANALOG1,
	TOWNS_GAMEPORTEMU_ANALOG2,
	TOWNS_GAMEPORTEMU_ANALOG3,
	TOWNS_GAMEPORTEMU_ANALOG4,
	TOWNS_GAMEPORTEMU_ANALOG5,
	TOWNS_GAMEPORTEMU_ANALOG6,
	TOWNS_GAMEPORTEMU_ANALOG7,
	TOWNS_GAMEPORTEMU_PHYSICAL0_AS_CYBERSTICK,
	TOWNS_GAMEPORTEMU_PHYSICAL1_AS_CYBERSTICK,
	TOWNS_GAMEPORTEMU_PHYSICAL2_AS_CYBERSTICK,
	TOWNS_GAMEPORTEMU_PHYSICAL3_AS_CYBERSTICK,
	TOWNS_GAMEPORTEMU_PHYSICAL4_AS_CYBERSTICK,
	TOWNS_GAMEPORTEMU_PHYSICAL5_AS_CYBERSTICK,
	TOWNS_GAMEPORTEMU_PHYSICAL6_AS_CYBERSTICK,
	TOWNS_GAMEPORTEMU_PHYSICAL7_AS_CYBERSTICK,
	TOWNS_GAMEPORTEMU_MOUSE_BY_KEY,
	TOWNS_GAMEPORTEMU_MOUSE_BY_NUMPAD,
	TOWNS_GAMEPORTEMU_MOUSE_BY_PHYSICAL0,
	TOWNS_GAMEPORTEMU_MOUSE_BY_PHYSICAL1,
	TOWNS_GAMEPORTEMU_MOUSE_BY_PHYSICAL2,
	TOWNS_GAMEPORTEMU_MOUSE_BY_PHYSICAL3,
	TOWNS_GAMEPORTEMU_MOUSE_BY_PHYSICAL4,
	TOWNS_GAMEPORTEMU_MOUSE_BY_PHYSICAL5,
	TOWNS_GAMEPORTEMU_MOUSE_BY_PHYSICAL6,
	TOWNS_GAMEPORTEMU_MOUSE_BY_PHYSICAL7,
	TOWNS_GAMEPORTEMU_MOUSE_BY_ANALOG0,
	TOWNS_GAMEPORTEMU_MOUSE_BY_ANALOG1,
	TOWNS_GAMEPORTEMU_MOUSE_BY_ANALOG2,
	TOWNS_GAMEPORTEMU_MOUSE_BY_ANALOG3,
	TOWNS_GAMEPORTEMU_MOUSE_BY_ANALOG4,
	TOWNS_GAMEPORTEMU_MOUSE_BY_ANALOG5,
	TOWNS_GAMEPORTEMU_MOUSE_BY_ANALOG6,
	TOWNS_GAMEPORTEMU_MOUSE_BY_ANALOG7,
	TOWNS_GAMEPORTEMU_CYBERSTICK,
	TOWNS_GAMEPORTEMU_CAPCOM_BY_PHYSICAL0,
	TOWNS_GAMEPORTEMU_CAPCOM_BY_PHYSICAL1,
	TOWNS_GAMEPORTEMU_CAPCOM_BY_PHYSICAL2,
	TOWNS_GAMEPORTEMU_CAPCOM_BY_PHYSICAL3,
	TOWNS_GAMEPORTEMU_CAPCOM_BY_PHYSICAL4,
	TOWNS_GAMEPORTEMU_CAPCOM_BY_PHYSICAL5,
	TOWNS_GAMEPORTEMU_CAPCOM_BY_PHYSICAL6,
	TOWNS_GAMEPORTEMU_CAPCOM_BY_PHYSICAL7,
TOWNS_GAMEPORTEMU_NUM_DEVICES,
TOWNS_GAMEPORTEMU_ERROR,
};

#ifdef __cplusplus
	unsigned int TownsStrToGamePortEmu(std::string str);
	std::string TownsGamePortEmuToStr(unsigned int emu);
#endif

enum	// Commands written to I/O TOWNSIO_VM_HOST_IF_CMD_STATUS=0x2386
{
	TOWNS_VMIF_CMD_NOP=           0x00, 
	TOWNS_VMIF_CMD_CAPTURE_CRTC=  0x01, // Capture CRTC. Followed by two bytes mode0, and mode1.
	TOWNS_VMIF_CMD_PAUSE=         0x02, // Pause VM
	TOWNS_VMIF_CMD_EXIT_VM=       0x03, // Exit the VM.  Return code taken from the data queu.

	TOWNS_VMIF_CMD_FILE_RXRDY=    0x04, // File Transfer Ready to Read (Ready to receive data in Sprite RAM)
	TOWNS_VMIF_CMD_FILE_ACK=      0x05, // File Transfer 
	TOWNS_VMIF_CMD_FILE_TXRDY=    0x06, // File Transfer Ready to Send (Data is in Sprite RAM)

	TOWNS_VMIF_CMD_NOTIFY_DOSSEG= 0x07, // Capture CS as DOSSEG
	TOWNS_VMIF_CMD_NOTIFY_DOSVER= 0x08, // Capture DOS Version.  Use it after MOV AH,30H  ->  INT 21H  ->  MOV BX,AX
	TOWNS_VMIF_CMD_NOTIFY_DOSLOL= 0x09, // Capture DOS List of Lists, not DOS Laugh Out Loud.  Also updates DOSSEG (supposed to be the same)

	TOWNS_VMIF_CMD_NOTIFY_MOUSE = 0x0A, // Notify Mouse Position

	TOWNS_VMIF_CMD_BYTE_VM_TO_HOST=0x0B,// Send byte to the host.  Write to data register then write this command.

	TOWNS_VMIF_CMD_CAPTURE_FM_FNUM=0x0C, // Capture F-Number of YM2612 Ch0.
	TOWNS_VMIF_CMD_CAPTURE_FM_TL  =0x0D, // Capture TL of YM2612 Ch0 Slot 4
};

enum
{
	TOWNS_VM_TGDRV_INSTALL=     0x01,
	TOWNS_VM_TGDRV_INT2FH=      0x02,
};

enum
{
	TOWNS_TGDRV_MAX_NUM_DRIVES=8
};

enum
{
	TOWNS_VMIF_TFR_END=           0x00,
	TOWNS_VMIF_TFR_HOST_TO_VM=    0x01,
	TOWNS_VMIF_TFR_VM_TO_HOST=    0x02,
};

enum
{
	TOWNS_VNDRV_CMD_GET_DRIVES=   0x00,
	TOWNS_VNDRV_CMD_FIND_FIRST=   0x1B,
	TOWNS_VNDRV_CMD_FIND_NEXT=    0x1C,

	TOWNS_VNDRV_AUXCMD_DEBUGBREAK=         0x00,
	TOWNS_VNDRV_AUXCMD_PRINTCSTR=          0x09,
	TOWNS_VNDRV_AUXCMD_MEMDUMP=            0x0A,
	TOWNS_VNDRV_AUXCMD_MEMDUMP_LINEAR=     0x0B,
	TOWNS_VNDRV_AUXCMD_MEMDUMP_PHYSICAL=   0x0C,

	TOWNS_VNDRV_ERR_NO_ERROR=      0x00,
	TOWNS_VNDRV_ERR_FILE_NOT_FOUND=0x02,
	TOWNS_VNDRV_ERR_PATH_NOT_FOUND=0x03,
	TOWNS_VNDRV_ERR_NO_MORE_FILES= 0x12,
};

enum
{
	TBIOS_UNKNOWN,
	TBIOS_V31L22A,   // TBIOS that comes with Towns OS V1.1 L10
	TBIOS_V31L23A,   // TBIOS that comes with Towns OS V1.1 L30
	TBIOS_V31L31_90, // TBIOS that comes with Towns OS V1.1 L30
	TBIOS_V31L31_91, // TBIOS that comes with Towns OS V2.1 L10B
	TBIOS_V31L31_92, // TBIOS that comes with Towns OS V2.1 L20
	TBIOS_V31L31_93, // TBIOS that comes with Towns OS V2.1 L20A
	TBIOS_V31L35,    // TBIOS that comes with Towns OS V2.1 L30,L31,L50(Free Software Collection 11)
};

enum
{
	// Do not change order.  The number is saved in machine-state files.
	TOWNS_APPSPECIFIC_NONE,
	TOWNS_APPSPECIFIC_WINGCOMMANDER1,
	TOWNS_APPSPECIFIC_WINGCOMMANDER2,
	TOWNS_APPSPECIFIC_SUPERDAISEN,
	TOWNS_APPSPECIFIC_LEMMINGS,
	TOWNS_APPSPECIFIC_LEMMINGS2,
	TOWNS_APPSPECIFIC_STRIKECOMMANDER,
	TOWNS_APPSPECIFIC_AMARANTH3,
	TOWNS_APPSPECIFIC_ULTIMAUNDERWORLD,
	TOWNS_APPSPECIFIC_OPERATIONWOLF,
	TOWNS_APPSPECIFIC_BRANDISH,
	TOWNS_APPSPECIFIC_AIRWARRIOR_V2,
	TOWNS_APPSPECIFIC_DUNGEONMASTER_JP,
	TOWNS_APPSPECIFIC_DUNGEONMASTER_EN,
	TOWNS_APPSPECIFIC_DAIKOUKAIJIDAI,
	TOWNS_APPSPECIFIC_RASHINBAN,
	TOWNS_APPSPECIFIC_AFTERBURNER2,
	TOWNS_APPSPECIFIC_DAIKOUKAIJIDAI2,
	TOWNS_APPSPECIFIC_ORGEL,
	TOWNS_APPSPECIFIC_ROCKETRANGER,
	TOWNS_APPSPECIFIC_ASUKA120,

TOWNS_NUM_APPSPECIFIC
};

#ifdef __cplusplus
	unsigned int TownsStrToApp(std::string str);
	std::string TownsAppToStr(unsigned int i);
#endif


enum
{
	TOWNS_MEMCARD_TYPE_NONE,
	TOWNS_MEMCARD_TYPE_OLD,
	TOWNS_MEMCARD_TYPE_JEIDA4
};

#ifdef __cplusplus
	std::string TownsMemCardTypeToStr(unsigned int memCardType);
#endif


// Towns OS IO.SYS loads MSDOS.SYS in 1679H segment.
enum
{
	TOWNS_DOS_SEG=0x1679,

	TOWNS_DOS_LASTBUFF=0x001E,

	TOWNS_DOS_MCBPTR=0x0024,

	TOWNS_DOS_MCB_TYPE=0x00,
	TOWNS_DOS_MCB_PID= 0x01,
	TOWNS_DOS_MCB_SIZE=0x03,

	TOWNS_DOS_SYSVAR=0x0026,
	TOWNS_DOS_DPB_PTR=         0x0026,
	TOWNS_DOS_SFT_PTR=         0x002A,
	TOWNS_DOS_CLOCK_DEV_PTR=   0x002E,
	TOWNS_DOS_CON_DEV_PTR=     0x0032,
	TOWNS_DOS_BUF_SIZE=        0x0036,
	TOWNS_DOS_BUF_PTR=         0x0038,
	TOWNS_DOS_CDS_LIST_PTR=    0x003C,
	TOWNS_DOS_FCB_PTR=         0x0040,
	TOWNS_DOS_FCB_KEEP_COUNT=  0x0044,
	TOWNS_DOS_DPB_COUNT=       0x0046,
	TOWNS_DOS_CDS_COUNT=       0x0047,
	TOWNS_DOS_NUL_DEV_HEADER=  0x0048,
	TOWNS_DOS_JOINED_DRV_COUNT=0x005A,

	TOWNS_DOS_CURRENT_PDB=     0x02DE,
	TOWNS_DOS_ENDMEM=          0x02f2,

	TOWNS_DOS_PSP_INT20=         0x00,
	TOWNS_DOS_PSP_ENDSEG=        0x02,
	TOWNS_DOS_PSP_CALLF_DOS=     0x05,
	TOWNS_DOS_PSP_INT22VEC=      0x0A,
	TOWNS_DOS_PSP_INT23VEC=      0x0E,
	TOWNS_DOS_PSP_INT24VEC=      0x12,
	TOWNS_DOS_PSP_CALLER_PSP=    0x16,
	TOWNS_DOS_PSP_FILE_TABLE=    0x18,
	TOWNS_DOS_PSP_ENVSEG=        0x2C,
	TOWNS_DOS_PSP_STACK_PTR=     0x2E,
	TOWNS_DOS_PSP_FILE_TAB_SIZE= 0x32,
	TOWNS_DOS_PSP_FILE_TABLE_PTR=0x34,
	TOWNS_DOS_PSP_NEXT_PSP=      0x38,
	TOWNS_DOS_PSP_DOS_VERSION=   0x40,
	TOWNS_DOS_PSP_INT21_RETF=    0x50,
	TOWNS_DOS_PSP_FCB1=          0x5C,
	TOWNS_DOS_PSP_FCB2=          0x6C,
	TOWNS_DOS_PSP_COMMAND_TAIL=  0x80,

	TOWNS_DOS_DTA_ADDRESS=       0x02DA,

	TOWNS_DOS_DIRENT_ATTR_READONLY =0x01,
	TOWNS_DOS_DIRENT_ATTR_HIDDEN   =0x02,
	TOWNS_DOS_DIRENT_ATTR_SYSTEM   =0x04,
	TOWNS_DOS_DIRENT_ATTR_VOLLABEL =0x08,
	TOWNS_DOS_DIRENT_ATTR_DIRECTORY=0x10,
	TOWNS_DOS_DIRENT_ATTR_ARCHIVE  =0x20,


	TOWNS_DOSERR_NO_ERROR			=0x00,
	TOWNS_DOSERR_INVALID_FUNC		=0x01,
	TOWNS_DOSERR_FILE_NOT_FOUND		=0x02,
	TOWNS_DOSERR_DIR_NOT_FOUND		=0x03,
	TOWNS_DOSERR_TOO_MANY_OPEN_FILES=0x04,
	TOWNS_DOSERR_ACCESS_DENIED		=0x05,
	TOWNS_DOSERR_INVALID_HANDLE		=0x06,
	TOWNS_DOSERR_MCB_BROKEN			=0x07,
	TOWNS_DOSERR_OUT_OF_MEMORY		=0x08,
	TOWNS_DOSERR_INVALID_MCB		=0x09,
	TOWNS_DOSERR_BAD_ENV			=0x0A,
	TOWNS_DOSERR_BAD_FORMAT			=0x0B,
	TOWNS_DOSERR_INVALID_ACCESS		=0x0C,
	TOWNS_DOSERR_INVALID_DATA		=0x0D,
	TOWNS_DOSERR_UNUSED				=0x0E,
	TOWNS_DOSERR_INVALID_DRIVE		=0x0F,
	TOWNS_DOSERR_CANNOT_DEL_CUR_DIR	=0x10,
	TOWNS_DOSERR_NOT_SAME_DRIVE		=0x11,
	TOWNS_DOSERR_NO_MORE_FILES		=0x12,
	TOWNS_DOSERR_WRITE_PROTEDTED	=0x13,
	TOWNS_DOSERR_UNKNOWN_UNIT		=0x14,
	TOWNS_DOSERR_DRIVE_NOT_READY	=0x15,
	TOWNS_DOSERR_UNKNOWN_COMMAND	=0x16,
	TOWNS_DOSERR_CRC_ERROR			=0x17,
	TOWNS_DOSERR_BAD_REQ_LEN		=0x18,
	TOWNS_DOSERR_SEEK_ERROR			=0x19,
	TOWNS_DOSERR_UNKNOWN_MEDIUM		=0x1A,
	TOWNS_DOSERR_SECTOR_NOT_FOUND	=0x1B,
	TOWNS_DOSERR_OUT_OT_PAPER		=0x1C,
	TOWNS_DOSERR_WRITE_FAULT		=0x1D,
	TOWNS_DOSERR_READ_FAULT			=0x1E,
	TOWNS_DOSERR_GENERAL_FAULT		=0x1F,
	TOWNS_DOSERR_INVALID_DISK_CHANGE=0x22,
};

enum
{
	TOWNS_CMOSRAM_FASTMODE_FLAG     =0x615, // I/O 3C2Ah
};

/* } */
#endif
