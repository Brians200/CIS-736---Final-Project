#pragma once
#include "QuadTreeNode.h"

class QuadTree
{
public:
QuadTree(void);
QuadTree(float maxDistance);
~QuadTree(void);

bool AddParticle(float x, float y, float z, float mass);
Vector3 calculateAcceleration(Vector3 position, float mass, float gravityCutOff, float rMin, float gravity);

private:
QuadTreeNode * qtn;
float maxDistance;

};