#include <algorithm>
#include <cstdint>
#include "habitatmouse.h"

namespace
{
int SignedWord(int value)
{
	return (int)(int16_t)(uint16_t)value;
}
}

void HabitatMouseAdapter::Reset(void)
{
	active=false;
	plane=PLANE_UNKNOWN;
	worldMinX=WORLD_MIN_X;
	worldMaxX=WORLD_MAX_X;
	worldMinY=WORLD_MIN_Y;
	worldMaxY=WORLD_MAX_Y;
	pointerPositionValid=false;
	pointerX=0;
	pointerY=0;
}

bool HabitatMouseAdapter::IsActive(void) const
{
	return active;
}

HabitatMouseAdapter::Plane HabitatMouseAdapter::GetPlane(void) const
{
	return plane;
}

void HabitatMouseAdapter::ObserveExtentX(int minX,int maxX)
{
	// Habitat's launcher, configuration, and dialler programs use the full
	// 640-pixel pointer plane.  Returning to it must disarm world integration.
	if(0==minX && 0x27f==maxX)
	{
		Reset();
	}
	else if(true==active && 0==minX && maxX<=WORLD_MAX_X)
	{
		worldMinX=minX;
		worldMaxX=maxX;
	}
	else if(true==active && 0==minX && 0x26f==maxX)
	{
		// The idle router restores the shared world/sidebar horizontal range.
		worldMinX=WORLD_MIN_X;
		worldMaxX=WORLD_MAX_X;
	}
}

void HabitatMouseAdapter::ObserveExtentY(int minY,int maxY)
{
	// These are the distinctive extents programmed by the in-world pointer
	// router.  They are a behavioral signature and do not depend on an
	// executable name, load address, or client memory layout.
	if(2==minY && maxY<=WORLD_MAX_Y)
	{
		active=true;
		plane=PLANE_WORLD;
		worldMinY=minY;
		worldMaxY=maxY;
	}
	else if(WORLD_HOST_Y_ORIGIN==minY && 0x1cf==maxY)
	{
		active=true;
		plane=PLANE_SIDEBAR;
	}
	else if(0==minY && 0x1df==maxY)
	{
		Reset();
	}
}

void HabitatMouseAdapter::ObservePointerPosition(int x,int y)
{
	pointerPositionValid=true;
	pointerX=x;
	pointerY=y;
}

bool HabitatMouseAdapter::GetPointerPosition(int &x,int &y) const
{
	if(true==pointerPositionValid)
	{
		x=pointerX;
		y=pointerY;
	}
	return pointerPositionValid;
}

bool HabitatMouseAdapter::Apply(int rawTBIOSX,int hostX,int hostY,int &mappedX,int &mappedY)
{
	if(true!=active)
	{
		return false;
	}

	// The TBIOS work area exposes 16-bit words.  A one-step west-edge
	// overshoot is therefore read as 65535 unless it is interpreted as signed.
	int classifiedX=SignedWord(rawTBIOSX);
	if(classifiedX<=WORLD_RAW_MAX_X)
	{
		plane=PLANE_WORLD;
	}
	else if(SIDEBAR_RAW_MIN_X<=classifiedX)
	{
		plane=PLANE_SIDEBAR;
	}

	if(PLANE_UNKNOWN==plane)
	{
		return false;
	}

	Plane targetPlane=plane;
	if(hostX<=WORLD_HOST_MAX_X)
	{
		targetPlane=PLANE_WORLD;
	}
	else if(SIDEBAR_HOST_MIN_X<=hostX)
	{
		targetPlane=PLANE_SIDEBAR;
	}

	if(PLANE_WORLD==targetPlane)
	{
		// The host-side transition band has no representation in the world
		// plane.  Keep the target on the last world pixel until x<=519.
		hostX=std::min(hostX,WORLD_HOST_MAX_X);
		mappedX=std::clamp(hostX/2,worldMinX,worldMaxX);
		mappedY=std::clamp((hostY-WORLD_HOST_Y_ORIGIN)/2,worldMinY,worldMaxY);
	}
	else
	{
		// Likewise, do not ask Habitat to leave the sidebar while the host is
		// in the unrepresentable transition band.
		mappedX=std::max(hostX,SIDEBAR_HOST_MIN_X);
		mappedY=hostY;
	}
	return true;
}

void HabitatMouseAdapter::NormalizeRawPosition(int &rawX,int &rawY) const
{
	if(PLANE_WORLD==plane)
	{
		// Absolute integration must compare its target with the nearest valid
		// world coordinate.  Otherwise a transient 16-bit under/overshoot at an
		// extent produces a large reverse correction on the next poll.
		rawX=std::clamp(SignedWord(rawX),worldMinX,worldMaxX);
		rawY=std::clamp(SignedWord(rawY),worldMinY,worldMaxY);
	}
}
