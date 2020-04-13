#include <stdio.h>
#include <math.h>
int main(void)
{
	const int PHASE_STEPS=64;
	const int UNSCALED_MAX=32;

	const double PI=3.14159265358979323;
	for(int i=0; i<PHASE_STEPS; ++i)
	{
		double a=2.0*PI*(double)i/(double)PHASE_STEPS;
		double s=sin(a);
		s*=(double)(UNSCALED_MAX);
		printf("%+-3d,",(int)s);
		if(i%16==15)
		{
			printf("\n");
		}
	}

	return 0;
}
