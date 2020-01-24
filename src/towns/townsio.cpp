#include "townsio.h"



/* virtual */ void FMTowns::IOWriteByte(unsigned int ioport,unsigned int data)
{
}
/* virtual */ void FMTowns::IOWriteWord(unsigned int ioport,unsigned int data)
{
}
/* virtual */ void FMTowns::IOWriteDword(unsigned int ioport,unsigned int data)
{
}
/* virtual */ unsigned int FMTowns::IOReadByte(unsigned int ioport)
{
	switch(ioport)
	{
	case TOWNSIO_FREERUN_TIMER_LOW ://0x26,
		return state.townsTime&0xff;
	case TOWNSIO_FREERUN_TIMER_HIGH://0x28,
		return (state.townsTime>>8)&0xff;
	}
	return 0xff;
}
/* virtual */ unsigned int FMTowns::IOReadWord(unsigned int ioport)
{
	switch(ioport)
	{
	case TOWNSIO_FREERUN_TIMER:// 0x26
		return state.townsTime&0xffff;
	}
	return 0xffff;
}
/* virtual */ unsigned int FMTowns::IOReadDword(unsigned int ioport)
{
	return 0xffffffff;
}
