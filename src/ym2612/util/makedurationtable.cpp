#include <iostream>

// Attack:
//   i=10 -> Actual measurement: 2677ms (267700)
//   i=8  -> Estimated=attadkTime[10]*1.5=4015
//   i=4  -> Estimated=attackTime[8]*2=8030
//   i=0  -> Estimated=attackTime[4]*2=16060 (Actually infinity)

// Decay/Sustain/Release:
//   i=8 ->  Actual measurement: 56656ms (5665600)
//   i=4 ->  Estimated=data[8]*2= 113312ms
//   i=0 ->  Estimated=data[4]*2= 226624ms


int main(void)
{
	unsigned int level=1606000;

	std::cout << "// Attack" << std::endl;
	for(int i=0; i<64; i+=4)
	{
		std::cout << level << "," << std::endl;
		std::cout << level*4/5 <<  "," << std::endl;
		std::cout << level*4/6 <<  "," << std::endl;
		std::cout << level*4/7 <<  "," << std::endl;
		level/=2;
	}

	level=22662400;
	std::cout << "// decay/sustain/release" << std::endl;
	for(int i=0; i<64; i+=4)
	{
		std::cout << level << "," << std::endl;
		std::cout << level*4/5 <<  "," << std::endl;
		std::cout << level*4/6 <<  "," << std::endl;
		std::cout << level*4/7 <<  "," << std::endl;
		level/=2;
	}

	return 0;
}
