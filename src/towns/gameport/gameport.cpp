#include "device.h"
#include "towns.h"
#include "townsdef.h"
#include "gameport.h"
#include "cpputil.h"



void TownsGamePort::Port::Write(bool COM,bool T1,bool T2)
{
	if(MOUSE==device)
	{
		if((0==state || 2==state) && true!=COM)
		{
			++state;
		}
		else if((1==state || 3==state) && true==COM)
		{
			state=(state+1)&3;
		}
		this->COM=COM;
	}
}
unsigned char TownsGamePort::Port::Read(long long int townsTime)
{
	unsigned char data=0;
	if(GAMEPAD==device || MOUSE==device)
	{
		if(true==COM)
		{
			data|=0x40;
		}
		if(true!=button[0])
		{
			data|=0x10;
		}
		if(true!=button[1])
		{
			data|=0x20;
		}
		if(MOUSE==device)
		{
			if(200000<(townsTime-lastReadTime))
			{
				state=0;
			}
			switch(state)
			{
			case 0:
				data|=((mouseMotion.x()>>4)&0x0F);
				break;
			case 1:
				data|=((mouseMotion.x()   )&0x0F);
				break;
			case 2:
				data|=((mouseMotion.y()>>4)&0x0F);
				break;
			case 3:
				data|=((mouseMotion.y()   )&0x0F);
				mouseMotion.Set(0,0);
				break;
			}
		}
		else
		{
			data|=0x0F;
		}
	}
	lastReadTime=townsTime;
	return data;
}


////////////////////////////////////////////////////////////


void TownsGamePort::State::PowerOn(void)
{
}
void TownsGamePort::State::Reset(void)
{
	for(auto &p : ports)
	{
		p.state=0;
		p.button[0]=false;
		p.button[1]=false;
		p.COM=false;
		p.mouseMotion.Set(0,0);
		p.lastReadTime=0;
	}
}


////////////////////////////////////////////////////////////


/* virtual */ void TownsGamePort::PowerOn(void)
{
	state.PowerOn();
}
/* virtual */ void TownsGamePort::Reset(void)
{
	state.Reset();
}
/* virtual */ void TownsGamePort::IOWriteByte(unsigned int ioport,unsigned int data)
{
	switch(ioport)
	{
	case TOWNSIO_GAMEPORT_A_INPUT://        0x4D0,
		break;
	case TOWNSIO_GAMEPORT_B_INPUT://        0x4D2,
		break;
	case TOWNSIO_GAMEPORT_OUTPUT://         0x4D6,
		state.ports[0].Write(0!=(data&0x10),0!=(data&0x01),0!=(data&0x02));
		state.ports[1].Write(0!=(data&0x20),0!=(data&0x04),0!=(data&0x08));
		break;
	}
}
/* virtual */ unsigned int TownsGamePort::IOReadByte(unsigned int ioport)
{
	switch(ioport)
	{
	case TOWNSIO_GAMEPORT_A_INPUT://        0x4D0,
		return state.ports[0].Read(townsPtr->state.townsTime);
		break;
	case TOWNSIO_GAMEPORT_B_INPUT://        0x4D2,
		return state.ports[1].Read(townsPtr->state.townsTime);
		break;
	case TOWNSIO_GAMEPORT_OUTPUT://         0x4D6,
		break;
	}
	return 0xff;
}

