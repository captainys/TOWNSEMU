#ifndef CHEAPMATH_IS_INCLUDED
#define CHEAPMATH_IS_INCLUDED
/* { */


template <class ComponentType,const int Dimension>
class VectorTemplate
{
public:
	ComponentType v[Dimension];

	inline ComponentType &operator[](unsigned int i)
	{
		return v[i];
	}
	inline ComponentType operator[](unsigned int i) const
	{
		return v[i];
	}
};

template <class ComponentType>
class Vec2Template : public VectorTemplate <ComponentType,2>
{
public:
	inline ComponentType x(void) const
	{
		return this->v[0];
	}
	inline ComponentType y(void) const
	{
		return this->v[1];
	}
	inline ComponentType &x(void)
	{
		return this->v[0];
	}
	inline ComponentType &y(void)
	{
		return this->v[1];
	}
	inline void Set(ComponentType x,ComponentType y)
	{
		this->v[0]=x;
		this->v[1]=y;
	}
	inline bool operator==(const Vec2Template &incoming) const
	{
		return incoming.v[0]==this->v[0] && incoming.v[1]==this->v[1];
	}
	inline bool operator!=(const Vec2Template &incoming) const
	{
		return incoming.v[0]!=this->v[0] || incoming.v[1]!=this->v[1];
	}
};

template <class ComponentType>
class Vec3Template : public VectorTemplate <ComponentType,3>
{
public:
	inline ComponentType x(void) const
	{
		return this->v[0];
	}
	inline ComponentType y(void) const
	{
		return this->v[1];
	}
	inline ComponentType z(void) const
	{
		return this->v[2];
	}
	inline ComponentType &x(void)
	{
		return this->v[0];
	}
	inline ComponentType &y(void)
	{
		return this->v[1];
	}
	inline ComponentType &z(void)
	{
		return this->v[2];
	}

	inline ComponentType r(void) const
	{
		return this->v[0];
	}
	inline ComponentType g(void) const
	{
		return this->v[1];
	}
	inline ComponentType b(void) const
	{
		return this->v[2];
	}
	inline ComponentType &r(void)
	{
		return this->v[0];
	}
	inline ComponentType &g(void)
	{
		return this->v[1];
	}
	inline ComponentType &b(void)
	{
		return this->v[2];
	}
	inline void Set(ComponentType x,ComponentType y,ComponentType z)
	{
		this->v[0]=x;
		this->v[1]=y;
		this->v[2]=z;
	}
	inline bool operator==(const Vec3Template &incoming) const
	{
		return incoming.v[0]==this->v[0] && incoming.v[1]==this->v[1] && incoming.v[2]==this->v[2];
	}
	inline bool operator!=(const Vec3Template &incoming) const
	{
		return incoming.v[0]!=this->v[0] || incoming.v[1]!=this->v[1] || incoming.v[2]!=this->v[2];
	}
};



class Vec2i : public Vec2Template <int>
{
public:
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
};

class Vec3ub : public Vec3Template <unsigned char>
{
public:
	static inline Vec3ub Make(unsigned int x,unsigned int y,unsigned int z)
	{
		Vec3ub v;
		v.v[0]=x;
		v.v[1]=y;
		v.v[3]=z;
		return v;
	}

	static inline Vec3ub Origin(void)
	{
		Vec3ub v;
		v.v[0]=0;
		v.v[1]=0;
		v.v[2]=0;
		return v;
	}
};



/* } */
#endif
