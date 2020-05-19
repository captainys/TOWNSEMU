#ifndef SPRITE_IS_INCLUDED
#define SPRITE_IS_INCLUDED
/* { */

#include "device.h"
class TownsSprite : public Device
{
private:
	class FMTowns *townsPtr;
public:
	virtual const char *DeviceName(void) const{return "SPRITE";}

	enum
	{
		MAX_NUM_SPRITE_INDEX=1024,
		MAX_NUM_SPRITE_PATTERN=896,
		SPRITE_HALF_VRAM_SIZE=128*1024,
		SPRITE_VRAM_BYTES_PER_LINE=512,
		SPRITERAM_INDEX_OFFSET=0,
		SPRITERAM_PALETTE_OFFSET=8192,
		SPRITE_DIMENSION=16,
		SPRITE_PTN32K_BYTES_PER_LINE=32,
		SPRITE_PTN16_BYTES_PER_LINE=32,
		SPRITE_BUSY_TIME= 1000000,         // 1/60 sec cycle?  16ms=16000us=16000000ns
		SPRITE_IDLE_TIME=15000000,
	};
	enum
	{
		ATTR_OFFS=0x8000,
		ATTR_ROT_MASK=0x7000,
		ATTR_ROT_SHIFT=12,
		ATTR_SUY=0x0800,
		ATTR_SUX=0x0400,
		ATTR_PAT_MASK=0x03FF,
	};
	enum
	{
		PALETTE_INDEX_MASK=0xFFF,
		PALETTE_DISP=0x2000,
		PALETTE_SPYS=0x4000,
		PALETTE_CTEN=0x8000,
	};

	enum
	{
		REG_CONTROL0=0,
		REG_CONTROL1=1,
		REG_HORIZONTAL_OFFSET0=2,
		REG_HORIZONTAL_OFFSET1=3,
		REG_VERTICAL_OFFSET0=4,
		REG_VERTICAL_OFFSET1=5,
		REG_DISPLAY_PAGE=6,   // [2] pp.130  DP1 write -> Bit 7, DP1 read -> Bit 4.  state.reg[REG_DISPLAY_PAGE] retains a wirtten value.
		REG_DUMMY=7,
	NUM_REGS=8,
	};

	class State
	{
	public:
		unsigned char addressLatch;
		unsigned char reg[NUM_REGS];
		bool spriteBusy;

		void PowerOn(void);
		void Reset(void);
	};

	State state;
	class TownsPhysicalMemory *physMemPtr;

	TownsSprite(class FMTowns *townsPtr,class TownsPhysicalMemory *physMemPtr);
	void Start(void);
	void Stop(void);

	inline bool SpriteActive(void) const
	{
		return 0!=(state.reg[REG_CONTROL1]&0x80);
	}
	inline unsigned int NumSpritesToDraw(void) const
	{
		unsigned int n=((state.reg[REG_CONTROL1]<<8)|state.reg[REG_CONTROL0])&(MAX_NUM_SPRITE_INDEX-1);
		if(0==n)
		{
			return MAX_NUM_SPRITE_INDEX;
		}
		return n;
	}
	inline unsigned int FirstSpriteIndex(void) const
	{
		return ((state.reg[REG_CONTROL1]<<8)|state.reg[REG_CONTROL0])&(MAX_NUM_SPRITE_INDEX-1);
	}
	inline unsigned int HOffset(void) const
	{
		return ((state.reg[REG_HORIZONTAL_OFFSET1]<<8)|state.reg[REG_HORIZONTAL_OFFSET0])&0x1FF;
	}
	inline unsigned int VOffset(void) const
	{
		return ((state.reg[REG_VERTICAL_OFFSET1]<<8)|state.reg[REG_VERTICAL_OFFSET0])&0x1FF;
	}
	inline unsigned int DisplayPage(void) const
	{
		return (state.reg[REG_DISPLAY_PAGE]>>7);
	}


	void RunScheduledTask(unsigned long long int townsTime);

	void Render(unsigned char VRAM[],const unsigned char spriteRAM[]) const;
private:
	inline static void Transform(unsigned int &X,unsigned int &Y,unsigned int x,unsigned int y,unsigned char ROT)
	{
		switch(ROT)
		{
		default:
		case 0:
			// No transformation
			X=x;
			Y=y;
			break;
		case 1:
			X=x;
			Y=15-y;
			break;
		case 2:
			X=15-x;
			Y=y;
			break;
		case 3:
			X=15-x;
			Y=15-y;
			break;
		case 4:
			X=y;
			Y=x;
			break;
		case 5:
			X=y;
			Y=15-x;
			break;
		case 6:
			X=15-y;
			Y=x;
			break;
		case 7:
			X=15-y;
			Y=15-x;
			break;
		}
	}

public:
	inline bool SPD0(void) const   // For CRTC I/O
	{
		return state.spriteBusy;
	}

	inline unsigned char WritingPage(void) const // For CRTC I/O
	{
		if(0!=(state.reg[REG_DISPLAY_PAGE]&0x10))
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}

	virtual void PowerOn(void);
	virtual void Reset(void);

	virtual void IOWriteByte(unsigned int ioport,unsigned int data);

	virtual unsigned int IOReadByte(unsigned int ioport);

	std::vector <std::string> GetStatusText(const unsigned char spriteRAM[]) const;
};

/* } */
#endif
