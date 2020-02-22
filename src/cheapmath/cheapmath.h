#ifndef CHEAPMATH_IS_INCLUDED
#define CHEAPMATH_IS_INCLUDED
/* { */


class Vec2i
{
public:
	int v[2];

	static inline Vec2i Make(int x,int y)
	{
		Vec2i v;
		v.v[0]=x;
		v.v[1]=y;
		return v;
	}

	static inline Vec2i Origin(void)
	{
		Vec2i v;
		v.v[0]=0;
		v.v[1]=0;
		return v;
	}

	inline int x(void) const
	{
		return v[0];
	}
	inline int y(void) const
	{
		return v[1];
	}
	inline int &x(void)
	{
		return v[0];
	}
	inline int &y(void)
	{
		return v[1];
	}
};


/* } */
#endif
