#include <fssimplewindow.h>
#include <math.h>
#include <iostream>
#include <ysbitmap.h>



void Print(double a)
{
	printf("unsigned int attackExp[]={\n");
	for(int x=0; x<4096; ++x)
	{
		double t=(double)x/4096.0;
		auto y=0.0;
		int Y=(int)(y*4096.0);
		printf("%4d,",Y);
		if(0==(x+1)%16)
		{
			printf("\n");
		}
	}
	printf("};\n");
}

const int iStep=4096;
int dBAtTimeStep(int i,double final) // 4096 scale.  final is ratio error from 1.0.
{
	double t=pow(final,1.0/(double)iStep);
	double y=1.0-pow(t,(double)i);
	return (int)(y*4096);
}

int main(void)
{
	FsOpenWindow(0,0,800,600,1);
	double a=0.01;

	YsBitmap bmp;
	bmp.LoadPng("realsample.png");

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
			a-=0.001;
			printf("%lf (1/955=%lf, 1/4096=%lf)\n",a,1.0/955,1.0/4096);
		}
		else if(FSKEY_A==key)
		{
			a+=0.001;
			printf("%lf (1/955=%lf, 1/4096=%lf)\n",a,1.0/955,1.0/4096);
		}

		// dB_drop starts from 1.0 and for each time step, dB_drop(i+1)=dB_drop(i)*a. {0<C<1}
		// Two parameters: What time step is the end of the attack phase, and how close the
		// dB_drop at the end of the attack phase.

		const int xRes=800;
		int yDB[xRes],yAmp[xRes],yAmpScale[xRes];
		for(int x=0; x<xRes; ++x)
		{
			double i=(double)iStep*(double)x/(double)xRes;

			int y=dBAtTimeStep(i,a);

			double dB96Scale=(double)y*96.0/4096.0;

			yDB[x]=(int)(500.0-400.0*(dB96Scale/96.0));

			yAmp[x]=(int)(500.0-400.0*(1.0/pow(10,(96.0-dB96Scale)/20.0)));
			yAmpScale[x]=(int)(500.0-400.0*(1.0/pow(10,(96.0-dB96Scale/(1.0-a))/20.0)));
		}

		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		glRasterPos2i(0,500);
		glDrawPixels(bmp.GetWidth(),bmp.GetHeight(),GL_RGBA,GL_UNSIGNED_BYTE,bmp.GetRGBABitmapPointer());

		glColor3ub(255,0,0);
		glBegin(GL_LINE_STRIP);
		for(int x=0; x<xRes; ++x)
		{
			glVertex2i(x,yDB[x]);
		}
		glEnd();

		glColor3ub(0,255,0);
		glBegin(GL_LINE_STRIP);
		for(int x=0; x<xRes; ++x)
		{
			glVertex2i(x,yAmp[x]);
		}
		glEnd();

		glColor3ub(255,255,0);
		glBegin(GL_LINE_STRIP);
		for(int x=0; x<xRes; ++x)
		{
			glVertex2i(x,yAmpScale[x]);
		}
		glEnd();

		FsSwapBuffers();
	}
	return 0;
}

