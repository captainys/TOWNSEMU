#include <stdio.h>
#include <math.h>

double Scale(double cent)
{
	double oneTone=pow(2.0,1.0/12.0);
	return pow(oneTone,cent/100.0);
}

void Print(double cent)
{
	unsigned int scale16384=(unsigned int)((Scale(cent)-1.0)*16384.0);
	printf("%u,// scale(%lf)=%lf, (%lf-1.0)*16384.0=%u\n",scale16384,cent,Scale(cent),Scale(cent),scale16384);
}

int main(void)
{
	Print(3.4);
	Print(6.7);
	Print(10.0);
	Print(14.0);
	Print(20.0);
	Print(40.0);
	Print(80.0);
	return 0;
}

