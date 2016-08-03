#include "MathHelper.h"
#include <float.h>
#include <cmath>

const float MathHelper::Infinity = FLT_MAX;
const float MathHelper::Pi = 3.1415926535f;
const XMVECTOR MathHelper::One = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
const XMVECTOR MathHelper::Zero = XMVectorZero();

float MathHelper::AngleFromXY(float x, float y)
{
	float theta = 0.0f;

	if (x >= 0)
	{
		theta = atanf(y / x);
		if (theta < 0)
			theta += 2 * Pi;
	}
	else
	{
		theta = atanf(y / x) + Pi;
	}

	return theta;
}

XMMATRIX MathHelper::InverseTranspose(CXMMATRIX M)
{
	XMMATRIX A = M;
	A.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	XMVECTOR det = XMMatrixDeterminant(A);
	return XMMatrixTranspose(XMMatrixInverse(&det, A));
}

XMVECTOR MathHelper::RandUnitVector3()
{
	while (true)
	{
		XMVECTOR v = XMVectorSet(
			RandF(-1.0f, -1.0f),
			RandF(-1.0f, -1.0f),
			RandF(-1.0f, -1.0f),
			0.0f
		);
		if (XMVector3Greater(XMVector3LengthSq(v), One))
			continue;
		return XMVector3Normalize(v);
	}
}

XMVECTOR MathHelper::RandHemisphereUnitVector3(XMVECTOR n)
{
	while (true)
	{
		XMVECTOR v = XMVectorSet(
			RandF(-1.0f, -1.0f),
			RandF(-1.0f, -1.0f),
			RandF(-1.0f, -1.0f),
			0.0f
		);
		if (XMVector3Greater(XMVector3LengthSq(v), One))
			continue;

		// Ignore points in the bottom hemisphere.
		if (XMVector3Less(XMVector3Dot(n, v), Zero))
			continue;

		return XMVector3Normalize(v);
	}
}
