#include "cyberstick.h"



void CyberStick::COMWrite(bool COMOutFromCPU)
{
	if(false==COMOutFromCPU)
	{
		state.phase=PHASE_COMRESET;
	}
	else
	{
		if(PHASE_COMRESET==state.phase)
		{
			state.phase=PHASE_COMSET;
			state.readCount=0;
			state.STROBE=STROBE_MASK;
			state.ackRead=false;
		}
	}
	state.COMOutFromCPU=COMOutFromCPU;
}
unsigned char CyberStick::Read(void)
{
	if(PHASE_IDLE==state.phase)
	{
		return 0xFF;  // CPU will reset COM to start.
	}
	else if(PHASE_COMRESET==state.phase)
	{
		return 0x0F;  // CPU will set COM when 0==(read_data&0x30)
	}
	else if(PHASE_COMSET==state.phase)
	{
		unsigned char dataByte=DataByte(state.readCount);
		dataByte|=state.STROBE;
		if(true==state.ackRead)
		{
			dataByte|=STROBE_ACKBIT;
			state.STROBE=~state.STROBE;
			state.STROBE&=STROBE_MASK;
			++state.readCount;
			if(DATA_TRANSMISSION_BYTES<=state.readCount)
			{
				state.phase=PHASE_IDLE;
			}
		}
		state.ackRead=(state.ackRead!=true);
		return dataByte;
	}
	return 0xFF;
}

unsigned char CyberStick::DataByte(unsigned int readCount) const
{
	//  +0 Btn high 4 bits
	//  +1 Btn low 4 bits
	//  +2 Y high 4 bits
	//  +3 X high 4 bits
	//  +4 Pow high 4 bits
	//  +5 Unused?
	//  +6 Y low 4 bits
	//  +7 X low 4 bits
	//  +8 Pow low 4 bits
	//  +9 Unused?
	// +10 Unused?
	// +11 Unused?
	switch(readCount)
	{
	case 0:
		return (state.buttons>>4)&0x0F;
	case 1:
		return state.buttons&0x0F;
	case 2:
		return (((unsigned int)((state.y+1.0F)*255.0F/2.0F))>>4)&0x0F;
	case 3:
		return (((unsigned int)((state.x+1.0F)*255.0F/2.0F))>>4)&0x0F;
	case 4:
		return (((unsigned int)((state.pow)*255.0F))>>4)&0x0F;
	case 6:
		return (((unsigned int)((state.y+1.0F)*255.0F/2.0F)))&0x0F;
	case 7:
		return (((unsigned int)((state.x+1.0F)*255.0F/2.0F)))&0x0F;
	case 8:
		return (((unsigned int)((state.pow)*255.0F)))&0x0F;
	}
	return 0;
}
