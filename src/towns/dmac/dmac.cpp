#include "device.h"
#include "townsdef.h"
#include "towns.h"
#include "cpputil.h"
#include "dmac.h"


void TownsDMAC::State::PowerOn(void)
{
	Reset();
}
void TownsDMAC::State::Reset(void)
{
	for(auto &c : ch)
	{
		c.baseCount=0;
		c.currentCount=0;
		c.baseAddr=0;
		c.currentAddr=0;
		c.modeCtrl=0;
	}

	bitSize=16;
	BASE=true;
	SELCH=0;
	devCtrl[0]=0;
	devCtrl[1]=0;
	temporaryReg[0]=0;
	temporaryReg[1]=0;
	req=0;
	mask=0;
}


////////////////////////////////////////////////////////////


/* virtual */ void TownsDMAC::PowerOn(void)
{
	state.PowerOn();
}
/* virtual */ void TownsDMAC::Reset(void)
{
	state.Reset();
}
/* virtual */ void TownsDMAC::IOWriteByte(unsigned int ioport,unsigned int data)
{
	switch(ioport)
	{
	case TOWNSIO_DMAC_INITIALIZE://          0xA0,
		if(0!=(data&1))
		{
			state.Reset();
		}
		if(0!=(data&2))
		{
			state.bitSize=16;
		}
		else
		{
			state.bitSize=8;
		}
		break;
	case TOWNSIO_DMAC_CHANNEL://             0xA1,
		state.BASE=(0!=(data&4));
		state.SELCH=data&3;
		break;
	case TOWNSIO_DMAC_COUNT_LOW://           0xA2,
		state.ch[state.SELCH].currentCount&=0xff00;
		state.ch[state.SELCH].currentCount|=(data&0xff);
		if(true==state.BASE)
		{
			state.ch[state.SELCH].baseCount=state.ch[state.SELCH].currentCount;
		}
		break;
	case TOWNSIO_DMAC_COUNT_HIGH://          0xA3,
		state.ch[state.SELCH].currentCount&=0xff;
		state.ch[state.SELCH].currentCount|=((data&0xff)<<8);
		if(true==state.BASE)
		{
			state.ch[state.SELCH].baseCount=state.ch[state.SELCH].currentCount;
		}
		break;
	case TOWNSIO_DMAC_ADDRESS_LOWEST://      0xA4,
		state.ch[state.SELCH].currentAddr&=0xffffff00;
		state.ch[state.SELCH].currentAddr|=(data&0xff);
		if(true==state.BASE)
		{
			state.ch[state.SELCH].baseAddr=state.ch[state.SELCH].currentAddr;
		}
		break;
	case TOWNSIO_DMAC_ADDRESS_MIDLOW://      0xA5,
		state.ch[state.SELCH].currentAddr&=0xffff00ff;
		state.ch[state.SELCH].currentAddr|=((data&0xff)<<8);
		if(true==state.BASE)
		{
			state.ch[state.SELCH].baseAddr=state.ch[state.SELCH].currentAddr;
		}
		break;
	case TOWNSIO_DMAC_ADDRESS_MIDHIGH://     0xA6,
		state.ch[state.SELCH].currentAddr&=0xff00ffff;
		state.ch[state.SELCH].currentAddr|=((data&0xff)<<16);
		if(true==state.BASE)
		{
			state.ch[state.SELCH].baseAddr=state.ch[state.SELCH].currentAddr;
		}
		break;
	case TOWNSIO_DMAC_ADDRESS_HIGHEST://     0xA7,
		state.ch[state.SELCH].currentAddr&=0x00ffffff;
		state.ch[state.SELCH].currentAddr|=((data&0xff)<<24);
		if(true==state.BASE)
		{
			state.ch[state.SELCH].baseAddr=state.ch[state.SELCH].currentAddr;
		}
		break;
	case TOWNSIO_DMAC_DEVICE_CONTROL_LOW://  0xA8,
		data&=0x08;
		data|=DEVICE_CONTROL_LOW_FIXED;
		state.devCtrl[0]=data;
		break;
	case TOWNSIO_DMAC_DEVICE_CONTROL_HIGH:// 0xA9,
		state.devCtrl[1]=data;
		break;
	case TOWNSIO_DMAC_MODE_CONTROL://        0xAA,
		state.ch[state.SELCH].modeCtrl=data;
		break;
	case TOWNSIO_DMAC_STATUS://              0xAB,
		break;
	case TOWNSIO_DMAC_TEMPORARY_REG_LOW://   0xAC,
		break;
	case TOWNSIO_DMAC_TEMPORARY_REG_HIGH://  0xAD,
		break;
	case TOWNSIO_DMAC_REQUEST://             0xAE,
		state.req=data;
		break;
	case TOWNSIO_DMAC_MASK://                0xAF,
		state.mask=data;
		break;
	}
}
/* virtual */ unsigned int TownsDMAC::IOReadByte(unsigned int ioport)
{
	unsigned char data=0;
	switch(ioport)
	{
	case TOWNSIO_DMAC_INITIALIZE://          0xA0,
		break;
	case TOWNSIO_DMAC_CHANNEL://             0xA1,
		return (1<<state.SELCH)|(state.BASE ? 0x10 : 0);
	case TOWNSIO_DMAC_COUNT_LOW://           0xA2,
		break;
	case TOWNSIO_DMAC_COUNT_HIGH://          0xA3,
		break;
	case TOWNSIO_DMAC_ADDRESS_LOWEST://      0xA4,
		break;
	case TOWNSIO_DMAC_ADDRESS_MIDLOW://      0xA5,
		break;
	case TOWNSIO_DMAC_ADDRESS_MIDHIGH://     0xA6,
		break;
	case TOWNSIO_DMAC_ADDRESS_HIGHEST://     0xA7,
		break;
	case TOWNSIO_DMAC_DEVICE_CONTROL_LOW://  0xA8,
		break;
	case TOWNSIO_DMAC_DEVICE_CONTROL_HIGH:// 0xA9,
		break;
	case TOWNSIO_DMAC_MODE_CONTROL://        0xAA,
		break;
	case TOWNSIO_DMAC_STATUS://              0xAB,
		break;
	case TOWNSIO_DMAC_TEMPORARY_REG_LOW://   0xAC,
		break;
	case TOWNSIO_DMAC_TEMPORARY_REG_HIGH://  0xAD,
		break;
	case TOWNSIO_DMAC_REQUEST://             0xAE,
		break;
	case TOWNSIO_DMAC_MASK://                0xAF,
		break;
	}
	return 0xff;
}

