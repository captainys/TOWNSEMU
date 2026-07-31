#ifndef HABITATMOUSE_IS_INCLUDED
#define HABITATMOUSE_IS_INCLUDED

class HabitatMouseAdapter
{
public:
	enum Plane
	{
		PLANE_UNKNOWN,
		PLANE_WORLD,
		PLANE_SIDEBAR,
	};

	static constexpr int WORLD_RAW_MAX_X=0x103;
	static constexpr int SIDEBAR_RAW_MIN_X=0x211;
	static constexpr int WORLD_HOST_MAX_X=519;
	static constexpr int SIDEBAR_HOST_MIN_X=544;
	static constexpr int WORLD_MIN_X=0;
	static constexpr int WORLD_MAX_X=0x103;
	static constexpr int WORLD_MIN_Y=2;
	static constexpr int WORLD_MAX_Y=0x9f;
	static constexpr int WORLD_HOST_Y_ORIGIN=0x96;

private:
	bool active=false;
	Plane plane=PLANE_UNKNOWN;
	int worldMinX=WORLD_MIN_X,worldMaxX=WORLD_MAX_X;
	int worldMinY=WORLD_MIN_Y,worldMaxY=WORLD_MAX_Y;
	bool pointerPositionValid=false;
	int pointerX=0,pointerY=0;

public:
	void Reset(void);
	bool IsActive(void) const;
	Plane GetPlane(void) const;
	void ObserveExtentX(int minX,int maxX);
	void ObserveExtentY(int minY,int maxY);
	void ObservePointerPosition(int x,int y);
	bool GetPointerPosition(int &x,int &y) const;
	void NormalizeRawPosition(int &rawX,int &rawY) const;

	/*! Maps a host target into Habitat's currently active coordinate system.
	    Returns false until Habitat's world pointer extents have been observed.
	*/
	bool Apply(int rawTBIOSX,int hostX,int hostY,int &mappedX,int &mappedY);
};

#endif
