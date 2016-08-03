#ifndef MATHHELPER_H
#define MATHHELPER_H

#include <DirectXMath.h>
#include <Windows.h>
using namespace DirectX;

#define MAX(a,b) (a > b ? a : b)
#define MIN(a,b) (a < b ? a : b)
class MathHelper
{
public:
	static const float Infinity;
	static const float Pi;
	static const XMVECTOR One;
	static const XMVECTOR Zero;

public:
	// Returns random float in [0, 1).
	static float RandF()
	{
		return (float)( rand() ) / (float)RAND_MAX;
	}

	// Returns random float in [a, b).
	static float RandF(float a, float b)
	{
		return a + RandF()*( b - a );
	}

	template<typename T>
	static T Min(const T &a, const T &b)
	{
		return MIN(a, b);
	}

	template<typename T>
	static T Max(const T &a, const T &b)
	{
		return MAX(a, b);
	}

	template<typename T>
	static T Lerp(const T &a, const T &b, float t)
	{
		return a + ( b - a )*t;
	}

	template<typename T>
	static T Clamp(const T &x, const T &low, const T &high)
	{
		return MAX(low, MIN(x, high));
	}

public:
	// Returns the polar angle of the point (x,y) in [0, 2*PI).
	static float AngleFromXY(float x, float y);

	static XMMATRIX InverseTranspose(CXMMATRIX M);
	static XMVECTOR RandUnitVector3();
	static XMVECTOR RandHemisphereUnitVector3(XMVECTOR n);
};

#endif // MATHHELPER_H
