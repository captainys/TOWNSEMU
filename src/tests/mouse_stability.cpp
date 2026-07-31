#include <iostream>

#include "habitatmouse.h"
#include "townsdef.h"

namespace
{
int numFailed=0;

void Check(bool condition,const char message[])
{
	if(true!=condition)
	{
		++numFailed;
		std::cout << "FAIL: " << message << std::endl;
	}
}

void TestHabitatMouse(void)
{
	HabitatMouseAdapter adapter;
	int x=1234,y=5678;
	int pointerX=0,pointerY=0;
	Check(true!=adapter.GetPointerPosition(pointerX,pointerY),
	      "Habitat pointer position starts unknown");
	adapter.ObservePointerPosition(123,45);
	Check(true==adapter.GetPointerPosition(pointerX,pointerY) &&
	      123==pointerX && 45==pointerY,
	      "Habitat pointer position captures TBIOS return values");
	adapter.Reset();
	Check(true!=adapter.GetPointerPosition(pointerX,pointerY),
	      "Habitat pointer position is runtime-only");

	Check(true!=adapter.Apply(300,530,240,x,y),"Habitat startup dead band is ignored");
	Check(1234==x && 5678==y,"ignored Habitat sample leaves target unchanged");
	Check(true!=adapter.Apply(100,400,350,x,y),"ordinary pre-Habitat coordinates are ignored");
	Check(true!=adapter.IsActive(),"Habitat adapter starts inactive");

	adapter.ObserveExtentY(2,0x9f);
	Check(adapter.IsActive() && HabitatMouseAdapter::PLANE_WORLD==adapter.GetPlane(),
	      "Habitat world extent activates the adapter");
	Check(true==adapter.Apply(100,400,350,x,y),"Habitat world plane is recognized");
	Check(200==x && 100==y,"Habitat world scale and vertical origin");
	Check(true==adapter.Apply(100,0,0,x,y) && 0==x && 2==y,"Habitat world minimum clamp");
	Check(true==adapter.Apply(100,519,1000,x,y) && 259==x && 159==y,"Habitat world maximum clamp");

	int rawX=0xffff,rawY=0xffff;
	Check(true==adapter.Apply(rawX,0,150,x,y) && HabitatMouseAdapter::PLANE_WORLD==adapter.GetPlane(),
	      "signed west overshoot remains on the Habitat world plane");
	adapter.NormalizeRawPosition(rawX,rawY);
	Check(0==rawX && 2==rawY,"Habitat north/west raw overshoot clamps to world minimum");
	rawX=260;
	rawY=160;
	adapter.NormalizeRawPosition(rawX,rawY);
	Check(259==rawX && 159==rawY,"Habitat east/south raw overshoot clamps to world maximum");
	adapter.ObserveExtentX(0,0xf7);
	adapter.ObserveExtentY(2,0x97);
	Check(true==adapter.Apply(100,519,1000,x,y) && 247==x && 151==y,
	      "Habitat pressed world uses its temporary verb-menu extents");
	rawX=259;
	rawY=159;
	adapter.NormalizeRawPosition(rawX,rawY);
	Check(247==rawX && 151==rawY,"Habitat pressed raw position clamps to temporary extents");
	adapter.ObserveExtentX(0,0x26f);
	adapter.ObserveExtentY(2,0x9f);
	Check(true==adapter.Apply(100,519,1000,x,y) && 259==x && 159==y,
	      "Habitat release restores idle world extents");

	for(int hostX=520; hostX<=543; ++hostX)
	{
		adapter.Apply(300,hostX,300,x,y);
		Check(259==x && 75==y,"Habitat world remains stable in host transition band");
	}
	Check(true==adapter.Apply(300,544,300,x,y) && 544==x && 300==y,
	      "Habitat requests world-to-sidebar transition at x=544");
	Check(true==adapter.Apply(550,600,310,x,y) && 600==x && 310==y,
	      "Habitat sidebar uses screen coordinates");

	for(int hostX=520; hostX<=543; ++hostX)
	{
		adapter.Apply(400,hostX,310,x,y);
		Check(544==x && 310==y,"Habitat sidebar remains stable in host transition band");
	}
	Check(true==adapter.Apply(400,519,310,x,y) && 259==x && 80==y,
	      "Habitat requests sidebar-to-world transition at x=519");

	adapter.ObserveExtentX(0,0x27f);
	Check(true!=adapter.IsActive(),"full-width launcher extent deactivates Habitat adapter");
	Check(true!=adapter.Apply(100,400,350,x,y),"launcher coordinates remain unmodified after world exit");

	Check(TOWNS_APPSPECIFIC_HABITAT==TownsStrToApp("HABITAT"),"HABITAT application name");
	Check(TOWNS_APPSPECIFIC_HABITAT==TownsStrToApp("FUJITSUHABITAT"),"FUJITSUHABITAT alias");
	Check("HABITAT"==TownsAppToStr(TOWNS_APPSPECIFIC_HABITAT),"HABITAT canonical name");
	Check(TOWNS_APPSPECIFIC_NONE==TownsStrToApp("NONE"),"NONE does not select Habitat");
	Check(TOWNS_APPSPECIFIC_DRAKKEN==TownsStrToApp("DRAKKEN"),"another app does not select Habitat");
}

}

int main(void)
{
	TestHabitatMouse();
	if(0==numFailed)
	{
		std::cout << "mouse_stability: all checks passed" << std::endl;
	}
	return (0==numFailed ? 0 : 1);
}
