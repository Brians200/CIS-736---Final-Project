#pragma once
#include "QuadTreeNode.h"

class QuadTree
{
public:
	QuadTree(void);
	QuadTree(float maxDistance);
	~QuadTree(void);

	bool AddParticle(float x, float y, float z, float mass);
private:
	QuadTreeNode qtn;

};
