#pragma once
#include "Vector3.h"
#include <math.h>
class VectorMath
{
public:
	VectorMath(void);
	~VectorMath(void);
private:
	float x,y,z;

public:
	static Vector3 add(Vector3,Vector3);
	static Vector3 difference(Vector3,Vector3);
	static Vector3 multiply(float,Vector3);
	static Vector3 midpoint(Vector3,Vector3);
	//static Vector3 divide(Vector3,Vector3);

};