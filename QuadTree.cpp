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
	this->maxDistance = maxDistance;
}


bool QuadTree::AddParticle(float x, float y, float z, float mass)
{
	if(abs(x)>maxDistance || abs(y) > maxDistance || abs(z) > maxDistance)
	{
		//off the map
		//do not add it, return false so it can be removed
		return false;
	}

	return qtn->AddParticle(x,y,z,mass);
}

Vector3 QuadTree::calculateAcceleration(Vector3 position, float mass, float gravityCutOff, float rMin, float gravity)
{
	Vector3 retern = qtn->calculateAcceleration(position,mass, gravityCutOff, rMin, gravity);
	return retern;
}