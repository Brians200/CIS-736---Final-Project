#pragma once
#include "Vector3.h"
#include "VectorMath.h"


class QuadTreeNode
{
public:
	QuadTreeNode(void);
	QuadTreeNode(Vector3 topLeft, Vector3 topRight, Vector3 bottomLeft, Vector3 bottomRight);
	~QuadTreeNode(void);

	bool AddParticle(float x, float y, float z, float mass);
	Vector3 calculateAcceleration(Vector3 position, float mass, float gravityCutOff, float rMin, float gravity);


	Vector3 TopLeftCorner;
	Vector3 TopRightCorner;
	Vector3 BottomLeftCorner;
	Vector3 BottomRightCorner;
	Vector3 middle;

	QuadTreeNode * TopLeft;
	QuadTreeNode * TopRight;
	QuadTreeNode * BottomLeft;
	QuadTreeNode * BottomRight;
	

	int items;
	float mass;
	Vector3 particleCenter;

private:


};
