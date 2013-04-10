#include "VectorMath.h"


VectorMath::VectorMath(void)
{
}


VectorMath::~VectorMath(void)
{
}


Vector3 VectorMath::add(Vector3 left,Vector3 right)
{
	Vector3 retern(left.x+right.x, left.y + right.y, left.z+right.z);
	return retern;
}

Vector3 VectorMath::multiply(float scaler,Vector3 vector)
{
	Vector3 retern(scaler*vector.x, scaler*vector.y, scaler*vector.z);
	return retern;
}

Vector3 VectorMath::difference(Vector3 v1,Vector3 v2)
{
	Vector3 retern(v1.x-v2.x, v1.y-v2.y, v1.z-v2.z);
	return retern;
}

