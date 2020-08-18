#ifndef CYBERSTICK_IS_INCLUDED
#define CYBERSTICK_IS_INCLUDED
/* { */

class CyberStick
{
public:
	enum
	{
		PHASE_IDLE,
		PHASE_COMRESET,
		PHASE_COMSET,

		STROBE_MASK=0x10,
		STROBE_ACKBIT=0x20,

		DATA_TRANSMISSION_BYTES=11,
	};

	class State
	{
	public:
		float x=0,y=0,pow=0;
		unsigned int buttons=0;

		bool COMOutFromCPU=false;
		unsigned int phase=0,readCount=0;
		unsigned char STROBE=0;
		bool ackRead=false;
	};
	State state;

	void COMWrite(bool COMOutFromCPU);
	unsigned char Read(void);
	unsigned char DataByte(unsigned int readCount) const;
};

/* } */
#endif
