#include <fssimplewindow.h>
#include <math.h>



const double YsPi=3.14159265;

double func(double t,double lastValue,int FB)
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

const int xRes=800;

void MakePlot(int y[xRes],double dt,int FB)
{
	double lastY=0.0;
	int prevX=0;
	y[0]=300;
	for(double t=0.0; t<2.0; t+=dt)
	{
		double yValue=func(t,lastY,FB);
		lastY=yValue;

		int nextX=(int)(t*400.0);
		if(nextX!=prevX && nextX<xRes)
		{
			y[nextX]=300-(int)(yValue*200.0);
			prevX=nextX;
		}
	}
}

int main(void)
{
	double dt=0.00001;
	int FB=2;

	int y[xRes];
	MakePlot(y,dt,FB);

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
			MakePlot(y,dt,FB);
		}
		else if(FSKEY_A==key)
		{
			--FB;
			if(FB<0)
			{
				FB=0;
			}
			MakePlot(y,dt,FB);
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

