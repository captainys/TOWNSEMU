#ifndef RF5C68_IS_INCLUDED
#define RF5C68_IS_INCLUDED
/* { */


#include <vector>

class RF5C68
{
public:
	enum
	{
		WAVERAM_SIZE=65536
	};

	std::vector <unsigned char> waveRAM;

	RF5C68();
};


/* } */
#endif
