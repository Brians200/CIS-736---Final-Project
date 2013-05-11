#include "QuadTree.h"

QuadTree::QuadTree(void)
{
}

QuadTree::~QuadTree(void)
{
	delete qtn;
}

QuadTree::QuadTree(float maxDistance)
{
	Vector3 topleft(-maxDistance,maxDistance,0);
	Vector3 topright(maxDistance,maxDistance,0);
	Vector3 bottomleft(-maxDistance,-maxDistance,0);
	Vector3 bottomright(maxDistance,-maxDistance,0);
	qtn = new QuadTreeNode(topleft,topright,bottomleft,bottomright);
}

bool QuadTree::AddParticle(float x, float y, float z, float mass)
{
	//TODO: do some bounds checking to see if we can remove this particle or not

	qtn->AddParticle(x,y,z,mass);
	return true;
}