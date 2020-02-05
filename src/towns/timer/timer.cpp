#include "timer.h"
#include "towns.h"
#include "pic.h"



void TownsTimer::State::PowerOn(void)
{
}
void TownsTimer::State::Reset(void)
{
}


////////////////////////////////////////////////////////////


/* virtual */ void TownsTimer::PowerOn(void)
{
	state.PowerOn();
}
/* virtual */ void TownsTimer::Reset(void)
{
	state.Reset();
}
/* virtual */ void TownsTimer::IOWriteByte(unsigned int ioport,unsigned int data)
{
	switch(ioport)
	{
	case TOWNSIO_TIMER0_COUNT_LOW://         0x40,
		break;
	case TOWNSIO_TIMER0_COUNT_HIGH://        0x41,
		break;
	case TOWNSIO_TIMER1_COUNT_LOW://         0x42,
		break;
	case TOWNSIO_TIMER1_COUNT_HIGH://        0x43,
		break;
	case TOWNSIO_TIMER2_COUNT_LOW://         0x44,
		break;
	case TOWNSIO_TIMER2_COUNT_HIGH://        0x45,
		break;
	case TOWNSIO_TIMER_0_1_2_CTRL://         0x46,
		break;
	case TOWNSIO_TIMER3_COUNT_LOW://         0x50,
		break;
	case TOWNSIO_TIMER3_COUNT_HIGH://        0x51,
		break;
	case TOWNSIO_TIMER4_COUNT_LOW://         0x52,
		break;
	case TOWNSIO_TIMER4_COUNT_HIGH://        0x53,
		break;
	case TOWNSIO_TIMER5_COUNT_LOW://         0x54,
		break;
	case TOWNSIO_TIMER5_COUNT_HIGH://        0x55,
		break;
	case TOWNSIO_TIMER_3_4_5_CTRL://         0x56,
		break;
	}
}
/* virtual */ unsigned int TownsTimer::IOReadByte(unsigned int ioport)
{
	unsigned char data=0xff;
	switch(ioport)
	{
	case TOWNSIO_TIMER0_COUNT_LOW://         0x40,
		break;
	case TOWNSIO_TIMER0_COUNT_HIGH://        0x41,
		break;
	case TOWNSIO_TIMER1_COUNT_LOW://         0x42,
		break;
	case TOWNSIO_TIMER1_COUNT_HIGH://        0x43,
		break;
	case TOWNSIO_TIMER2_COUNT_LOW://         0x44,
		break;
	case TOWNSIO_TIMER2_COUNT_HIGH://        0x45,
		break;
	case TOWNSIO_TIMER_0_1_2_CTRL://         0x46,
		break;
	case TOWNSIO_TIMER3_COUNT_LOW://         0x50,
		break;
	case TOWNSIO_TIMER3_COUNT_HIGH://        0x51,
		break;
	case TOWNSIO_TIMER4_COUNT_LOW://         0x52,
		break;
	case TOWNSIO_TIMER4_COUNT_HIGH://        0x53,
		break;
	case TOWNSIO_TIMER5_COUNT_LOW://         0x54,
		break;
	case TOWNSIO_TIMER5_COUNT_HIGH://        0x55,
		break;
	case TOWNSIO_TIMER_3_4_5_CTRL://         0x56,
		break;
	}
	return data;
}

/* virtual */ void TownsTimer::RunScheduledTask(unsigned long long int townsTime)
{
}
