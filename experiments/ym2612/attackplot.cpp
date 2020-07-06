#include <fssimplewindow.h>
#include <math.h>
#include <iostream>






// f=1.0-pow(2.0,a*t)   t={0..1}
// f=0.5 at t=0.1
// 0.5=1.0-pow(2.0,0.1*a)
// pow(2.0,0.1*a)=0.5
// 0.1*a=-1
// a=-10

double Exponential(double t,double a)
{
	auto max=1.0-pow(2.0,a); // Force it to be 0 to 1
	return (1.0-pow(2.0,a*t))/max;
}

void Print(double a)
{
	printf("unsigned int attackExp[]={\n");
	for(int x=0; x<4096; ++x)
	{
		double t=(double)x/4096.0;
		auto y=Exponential(t,a);
		int Y=(int)(y*4096.0);
		printf("%4d,",Y);
		if(0==(x+1)%16)
		{
			printf("\n");
		}
	}
	printf("};\n");
}

int main(void)
{
	FsOpenWindow(0,0,800,600,1);
	double a=-4.0; // I don't know what a should be used.  It looks close to YM3438 application manual.

	Print(a);

	for(;;)
	{
		FsPollDevice();
		auto key=FsInkey();
		if(FSKEY_ESC==key)
		{
			break;
		}
		if(FSKEY_Q==key)
		{
			a-=1.0;
			printf("%lf\n",a);
		}
		else if(FSKEY_A==key)
		{
			a+=1.0;
			printf("%lf\n",a);
		}

		const int xRes=256;
		int y[xRes];
		for(int x=0; x<xRes; ++x)
		{
			double t=(double)x/(double)xRes;
			y[x]=(int)(400.0-200.0*Exponential(t,a));
		}

		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		glBegin(GL_LINE_STRIP);
		for(int x=0; x<xRes; ++x)
		{
			glVertex2i(x,y[x]);
		}
		glEnd();
		FsSwapBuffers();
	}
	return 0;
}

