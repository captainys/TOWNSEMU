#include <fssimplewindow.h>
#include <math.h>
#include <iostream>



const double YsPi=3.14159265;

double func1(double t,double lastValue,int FB)
{
	switch(FB)
	{
	case 0:
		lastValue=0;
		break;
	case 1:
		lastValue/=16;
		break;
	case 2:
		lastValue/=8;
		break;
	case 3:
		lastValue/=4;
		break;
	case 4:
		lastValue/=2;
		break;
	case 5:
		break;
	case 6:
		lastValue*=2;
		break;
	case 7:
		lastValue*=4;
		break;
	}
	return sin((t+lastValue)*YsPi*2.0);
}

double func2(double t,double dummy,int FB)
{
	double fb=sin(t*YsPi*2.0);
	switch(FB)
	{
	case 0:
		fb=0;
		break;
	case 1:
		fb/=16;
		break;
	case 2:
		fb/=8;
		break;
	case 3:
		fb/=4;
		break;
	case 4:
		fb/=2;
		break;
	case 5:
		break;
	case 6:
		fb*=2;
		break;
	case 7:
		fb*=4;
		break;
	}
	return sin((t+fb)*YsPi*2.0);
}

// Based on YM2608 OPNA Application Manual Section 2-1 Equation (3)
double func3(double t,int FB)
{
	double beta=1.0;
	switch(FB)
	{
	case 0:
		beta=0;
		break;
	case 1:
		beta/=16;
		break;
	case 2:
		beta/=8;
		break;
	case 3:
		beta/=4;
		break;
	case 4:
		beta/=2;
		break;
	case 5:
		break;
	case 6:
		beta*=2;
		break;
	case 7:
		beta*=4;
		break;
	}

	int rangeMin,rangeMax;
	if(fmod(t,1.0)<0.5)  // Limit range because there are two roots.
	{
		rangeMin=0;
		rangeMax=2048;
	}
	else
	{
		rangeMin=-2048;
		rangeMax=0;
	}

	double minErr=100.0;
	double minY=-4096;
	for(int i=rangeMin; i<=rangeMax; ++i)
	{
		double y=(double)i/(double)2048.0; // y should be -1.0 to 1.0
		double err=fabs(y-sin(t*YsPi*2.0+beta*y));
		if(err<minErr)
		{
			minErr=err;
			minY=y;
		}
	}

	return minY;
}

double func4(double t,int FB)
{
	double out=sin(t*YsPi*2.0);
	for(int i=0; i<FB; ++i)
	{
		out=sin(t*YsPi*2.0+out);
	}
	return out;
}

const int xRes=800;

void MakePlot1(int y[xRes],double dt,int FB)
{
	double lastY=0.0;
	int prevX=0;
	y[0]=300;
	for(double t=0.0; t<2.0; t+=dt)
	{
		double yValue=func1(t,lastY,FB);
		lastY=yValue;

		int nextX=(int)(t*400.0);
		if(nextX!=prevX && nextX<xRes)
		{
			y[nextX]=300-(int)(yValue*200.0);
			prevX=nextX;
		}
	}
}

void MakePlot2(int y[xRes],double dt,int FB)
{
	double lastY=0.0;
	int prevX=0;
	y[0]=300;
	for(double t=0.0; t<2.0; t+=dt)
	{
		double yValue=func2(t,lastY,FB);
		lastY=yValue;

		int nextX=(int)(t*400.0);
		if(nextX!=prevX && nextX<xRes)
		{
			y[nextX]=300-(int)(yValue*200.0);
			prevX=nextX;
		}
	}
}

void MakePlot3(int y[xRes],double dummy,int FB)
{
	for(int x=0; x<xRes; ++x)
	{
		double t=(double)x/(double)400.0;
		y[x]=300-(int)(func3(t,FB)*200.0);
	}
}

void MakePlot4(int y[xRes],double dummy,int FB)
{
	for(int x=0; x<xRes; ++x)
	{
		double t=(double)x/(double)400.0;
		y[x]=300-(int)(func4(t,FB)*200.0);
	}
}

void PrintTable(void)
{
	for(int FB=1; FB<8; ++FB)
	{
		printf("int FBTable%d[]={\n",FB);
		for(int i=0; i<4096; ++i)
		{
			double t=(double)i/4096.0;
			double value=func4(t,FB);
			int iValue=(int)(value*2048.0);
			printf("% 5d,",iValue);
			if(0==(i+1)%16)
			{
				printf("\n");
			}
		}
		printf("};\n");
	}
}

#define PLOTFUNC MakePlot3

int main(void)
{
	double dt=0.00001;
	int FB=0;

	int y[xRes];
	PLOTFUNC(y,dt,FB);

	// PrintTable();

	FsOpenWindow(0,0,800,600,1);
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
			++FB;
			if(7<FB)
			{
				FB=7;
			}
			PLOTFUNC(y,dt,FB);
		}
		else if(FSKEY_A==key)
		{
			--FB;
			if(FB<0)
			{
				FB=0;
			}
			PLOTFUNC(y,dt,FB);
		}

		int wid,hei;
		FsGetWindowSize(wid,hei);
		glViewport(0,0,wid,hei);

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

