#pragma once
#include "Vector3.h"
#include "VectorMath.h"


class QuadTreeNode
{
public:
	QuadTreeNode(void);
	QuadTreeNode(Vector3 topLeft, Vector3 topRight, Vector3 bottomLeft, Vector3 bottomRight);
	~QuadTreeNode(void);

	void AddParticle(float x, float y, float z, float mass);

	Vector3 TopLeftCorner;
	Vector3 TopRightCorner;
	Vector3 BottomLeftCorner;
	Vector3 BottomRightCorner;
	Vector3 middle;
	

	int items;
	float mass;
	Vector3 particleCenter;

private:


};
