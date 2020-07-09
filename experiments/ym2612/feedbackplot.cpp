#include <fssimplewindow.h>
#include <math.h>
#include <iostream>



const double YsPi=3.14159265;
double adjust=1.0;

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
		lastValue/=2;   // Oscillation
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
	lastValue*=adjust;
	return sin(t*YsPi*2.0+lastValue*YsPi);
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

const int xRes=8000;

void MakePlot1(double T[xRes],double y[xRes],double lastY[xRes],double dt,int FB)
{
	double prevY=0.0;
	int prevX=0;
	double t=0;
	y[0]=0.0;
	for(int x=0; x<xRes; ++x)
	{
		double yValue=func1(t,prevY,FB);
		T[x]=t;
		y[x]=yValue;
		lastY[x]=prevY;
		prevY=yValue;
		t+=dt;
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

#define PLOTFUNC(t,y,lastY,dt,FB) MakePlot1(t,y,lastY,dt,FB);

int main(void)
{
	// dt: Relative to one wave cycle.
	//     440Hz -> cycle=0.002272727272... second
	//     If YM2612 internal frequency is 600KHz as explained in FM Towns Technical Databook,
	//     (1/600K)/0.02272727272=(1/600K)*440=440/600K=0.000733
	//     If it is 690KHz as my obbservation,
	//     440/690K=0.000638
	double dt=0.000638;
	int FB=0;

	int redlineX=0;

	double t[xRes],y[xRes],lastY[xRes];
	PLOTFUNC(t,y,lastY,dt,FB);

	// PrintTable();

	int x0=0;

	FsOpenWindow(0,0,800,600,1);
	for(;;)
	{
		auto prevRedlineX=redlineX;

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
			PLOTFUNC(t,y,lastY,dt,FB);
			printf("FB %d\n",FB);
		}
		else if(FSKEY_A==key)
		{
			--FB;
			if(FB<0)
			{
				FB=0;
			}
			PLOTFUNC(t,y,lastY,dt,FB);
			printf("FB %d\n",FB);
		}
		else if(FSKEY_W==key)
		{
			dt*=2.0;
			printf("%lf\n",dt);
			PLOTFUNC(t,y,lastY,dt,FB);
		}
		else if(FSKEY_S==key)
		{
			dt/=2.0;
			printf("%lf\n",dt);
			PLOTFUNC(t,y,lastY,dt,FB);
		}
		else if(FSKEY_E==key)
		{
			dt*=1.1;
			printf("%lf\n",dt);
			PLOTFUNC(t,y,lastY,dt,FB);
		}
		else if(FSKEY_D==key)
		{
			dt/=1.1;
			printf("%lf\n",dt);
			PLOTFUNC(t,y,lastY,dt,FB);
		}
		else if(FSKEY_R==key)
		{
			adjust+=0.025;
			PLOTFUNC(t,y,lastY,dt,FB);
			printf("Adjust %lf\n",adjust);
		}
		else if(FSKEY_F==key)
		{
			adjust-=0.025;
			PLOTFUNC(t,y,lastY,dt,FB);
			printf("Adjust %lf\n",adjust);
		}
		else if(FSKEY_DOT==key)
		{
			++redlineX;
		}
		else if(FSKEY_COMMA==key)
		{
			--redlineX;
		}


		int lb,mb,rb,mx,my;
		FsGetMouseEvent(lb,mb,rb,mx,my);
		if(0!=lb)
		{
			redlineX=mx;
		}

		if(prevRedlineX!=redlineX)
		{
			auto XX=redlineX+x0;
			if(0<=XX && XX<xRes)
			{
				const double C[]=
				{
					0.0,1.0/16.0,1.0/8.0,1.0/4.0,1.0/2.0,1.0,2.0,4.0
				};
				printf("i=%d  t=%lf  Y=%lf  LastY=%lf  sin(%.4lfPI)=sin(%.4lf+%.4lf)\n",
				    XX,t[XX],y[XX],lastY[XX],(t[XX]*2.0+C[FB&7]*lastY[XX]),t[XX]*2.0,C[FB&7]*lastY[XX]);
			}
		}


		int wid,hei;
		FsGetWindowSize(wid,hei);
		glViewport(0,0,wid,hei);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0,wid-1,hei-1,0,-1,1);

		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		glColor3ub(0,0,0);
		glBegin(GL_LINE_STRIP);
		for(int x=0; x<wid; ++x)
		{
			auto XX=x+x0;
			if(0<=XX && XX<xRes)
			{
				glVertex2i(x,300-(hei/2)*y[XX]);
			}
		}
		glEnd();

		glBegin(GL_LINES);
		glColor3ub(0,0,0);
		glVertex2i(wid/2,0);
		glVertex2i(wid/2,hei);
		glColor3ub(255,0,0);
		glVertex2i(redlineX,0);
		glVertex2i(redlineX,hei);
		glEnd();


		FsSwapBuffers();
	}
	return 0;
}

