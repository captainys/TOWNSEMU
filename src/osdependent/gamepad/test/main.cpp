#include <math.h>
#include "ysgamepad.h"
#include "fssimplewindow.h"

double YsPi=3.14159265;

void DrawCircle(int cx,int cy,int rad)
{
	glBegin(GL_LINE_LOOP);
	for(int i=0; i<64; ++i)
	{
		double ang=(YsPi*2.0*(double)i)/64.0;
		double c=cos(ang);
		double s=sin(ang);
		glVertex2d((double)cx+c*(double)rad,(double)cy+s*(double)rad);
	}
	glEnd();
}

void DrawDirection(int cx,int cy,int rad,struct YsGamePadDirectionButton dir)
{
	glColor3ub(0,0,0);
	DrawCircle(cx,cy,rad);

	int dx=0,dy=0;
	if(dir.upDownLeftRight[0])
	{
		dy=-rad;
	}
	if(dir.upDownLeftRight[1])
	{
		dy=rad;
	}
	if(dir.upDownLeftRight[2])
	{
		dx=-rad;
	}
	if(dir.upDownLeftRight[3])
	{
		dx=rad;
	}
	glBegin(GL_LINES);
	glVertex2i(cx,cy);
	glVertex2i(cx+dx,cy+dy);
	glEnd();
}

void DrawAxis(int cx,int cy,int rad,float x,float y)
{
	glColor3ub(0,0,0);
	DrawCircle(cx,cy,rad);

	float x0=(float)cx+(int)(rad*x);
	float y0=(float)cy+(int)(rad*y);
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(x0-5,y0);
	glVertex2f(x0,y0-5);
	glVertex2f(x0+5,y0);
	glVertex2f(x0,y0+5);
	glEnd();
}

void Draw(struct YsGamePadReading &reading)
{
	DrawDirection(200,150,50,reading.dirs[0]);

	for(int i=0; i<YSGAMEPAD_MAX_NUM_BUTTONS; ++i)
	{
		if(0!=reading.buttons[i])
		{
			glBegin(GL_TRIANGLE_FAN);
		}
		else
		{
			glBegin(GL_LINE_LOOP);
		}
		int x0=200+i*50;
		int y0=450;
		glVertex2i(x0   ,y0);
		glVertex2i(x0+40,y0);
		glVertex2i(x0+40,y0+40);
		glVertex2i(x0   ,y0+40);
		glEnd();
	}
	DrawAxis(250,250,50,reading.axes[0],reading.axes[1]);
	DrawAxis(450,250,50,reading.axes[2],reading.axes[3]);
}

int main(void)
{
	FsOpenWindow(0,0,800,600,1);

	YsGamePadInitialize();
	YsGamePadWaitReady();

	for(;;)
	{
		FsPollDevice();
		auto key=FsInkey();
		if(FSKEY_ESC==key)
		{
			break;
		}

		YsGamePadReading reading;
		YsGamePadRead(&reading,0);

		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		Draw(reading);
		FsSwapBuffers();
	}

	YsGamePadTerminate();
}
