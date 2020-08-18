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
	};

	class State
	{
		float x,y,pow;
		bool buttons[16];

		bool COMOut;  // COM out from Towns
		unsigned int phase,readCount;
		unsigned char STROBE;
	};
	State state;
};

/* } */
#endif
