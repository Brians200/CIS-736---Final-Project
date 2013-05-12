#include "QuadTreeNode.h"






QuadTreeNode::QuadTreeNode(void)
{
}

QuadTreeNode::~QuadTreeNode(void)
{
if(TopLeft == nullptr)
{
delete TopLeft;
delete TopRight;
delete BottomLeft;
delete BottomRight;
}

}

QuadTreeNode::QuadTreeNode(Vector3 topLeft, Vector3 topRight, Vector3 bottomLeft, Vector3 bottomRight)
{
TopLeftCorner = topLeft;
TopRightCorner = topRight;
BottomLeftCorner = bottomLeft;
BottomRightCorner = bottomRight;

middle = VectorMath::midpoint(topLeft, bottomRight);

mass = 0.0f;
particleCenter = Vector3(0.0f,0.0f,0.0f);

items = 0;
}

bool QuadTreeNode::AddParticle(float x, float y, float z, float newMass)
{
bool added = true;

if(TopRightCorner.x == TopLeftCorner.x || TopRightCorner.y == BottomRightCorner.y)
{
return false;
}

//create a single item node, taking up the whole node
if(items == 0)
{
mass = newMass;
particleCenter = Vector3(x,y,z);

items++;
return true;
}
else if( items == 1)
{
//topRight
Vector3 tr(TopRightCorner.x,TopRightCorner.y, 0.0f);
Vector3 tl(middle.x,TopLeftCorner.y,0.0f);
Vector3 br(BottomRightCorner.x,middle.y,0.0f);
Vector3 bl(middle.x,middle.y,0.0f);

TopRight = new QuadTreeNode(tl,tr,bl,br);

//topLeft
tr = Vector3(middle.x,TopLeftCorner.y, 0.0f);
tl = Vector3(TopLeftCorner.x,TopLeftCorner.y,0.0f);
br = Vector3(middle.x,middle.y,0.0f);
bl = Vector3(TopLeftCorner.x,middle.y,0.0f);

TopLeft = new QuadTreeNode(tl,tr,bl,br);

//bottomLeft
tr = Vector3(middle.x,middle.y, 0.0f);
tl = Vector3(BottomLeftCorner.x,middle.y,0.0f);
br = Vector3(middle.x,BottomLeftCorner.y,0.0f);
bl = Vector3(BottomLeftCorner.x,BottomLeftCorner.y,0.0f);

BottomLeft =new QuadTreeNode(tl,tr,bl,br);

//bottomRight
tr = Vector3(BottomRightCorner.x,middle.y, 0.0f);
tl = Vector3(middle.x,middle.y,0.0f);
br = Vector3(BottomRightCorner.x,BottomRightCorner.y,0.0f);
bl = Vector3(middle.x,BottomRightCorner.y,0.0f);

BottomRight = new QuadTreeNode(tl,tr,bl,br);

if(particleCenter.x >= middle.x && particleCenter.y >= middle.y) // top right
{
TopRight->AddParticle(particleCenter.x,particleCenter.y,particleCenter.z,mass);
}
else if(particleCenter.x >= middle.x && particleCenter.y < middle.y) // bottom right
{
BottomRight->AddParticle(particleCenter.x,particleCenter.y,particleCenter.z,mass);
}
else if(particleCenter.x < middle.x && particleCenter.y >= middle.y) // top left
{
TopLeft->AddParticle(particleCenter.x,particleCenter.y,particleCenter.z,mass);
}
else if(particleCenter.x < middle.x && particleCenter.y < middle.y) // bottom left
{
BottomLeft->AddParticle(particleCenter.x,particleCenter.y,particleCenter.z,mass);
}


}
//figure out which quadrant it is in and add it there
//also update the mass and center of mass of this node
if(x >= middle.x && y >= middle.y) // top right
{
added = TopRight->AddParticle(x,y,z,newMass);
}
else if(x >= middle.x && y < middle.y) // bottom right
{
added = BottomRight->AddParticle(x,y,z,newMass);
}
else if(x < middle.x && y >= middle.y) // top left
{
added = TopLeft->AddParticle(x,y,z,newMass);
}
else if(x < middle.x && y < middle.y) // bottom left
{
added = BottomLeft->AddParticle(x,y,z,newMass);
}

if(!added)
return false;

float newX = particleCenter.x * mass;
float newY = particleCenter.y * mass;
float newZ = particleCenter.z * mass;

newX += x*newMass;
newY += y*newMass;
newZ += z*newMass;

mass += newMass;

newX /= mass;
newY /= mass;
newZ /= mass;

particleCenter = Vector3(newX, newY, newZ);
items++;

return true;

}

Vector3 QuadTreeNode::calculateAcceleration(Vector3 position, float massp, float gravityCutOff, float rMin, float gravity)
{

float ax, ay, az;
ax = ay = az = 0.0f;

//if 0 particles, return 0,0,0
if(items == 0)
return Vector3(ax,ay,az);

if(position.x == particleCenter.x && position.y == particleCenter.y && position.z == particleCenter.z)
return Vector3(ax,ay,az);

//figure out the acceleration from this node, if it is above the cut off, we need to go deeper
Vector3 difference = VectorMath::difference(particleCenter,position);
float radius = sqrtf(difference.x*difference.x + difference.y *difference.y + difference.z*difference.z);
float radiusInverse = 1.0f/(radius + rMin);
float acceleration = gravity*mass*massp*radiusInverse*radiusInverse;

//if 1 particle, return
// if < cutoff, return
if(items == 1 )
{
ax = acceleration * difference.x * radiusInverse;
ay = acceleration * difference.y * radiusInverse;
az = acceleration * difference.z * radiusInverse;
}
else if(acceleration < gravityCutOff)
{
ax = acceleration * difference.x * radiusInverse;
ay = acceleration * difference.y * radiusInverse;
az = acceleration * difference.z * radiusInverse;
}
else //if > cutoff, add up the acceleration from the 4 parts
{
Vector3 tl = TopLeft->calculateAcceleration(position,massp,gravityCutOff,rMin,gravity);
Vector3 bl = BottomLeft->calculateAcceleration(position,massp,gravityCutOff,rMin,gravity);
Vector3 tr = TopRight->calculateAcceleration(position,massp,gravityCutOff,rMin,gravity);
Vector3 br = BottomRight->calculateAcceleration(position,massp,gravityCutOff,rMin,gravity);

ax = tl.x + bl.x + tr.x + br.x;
ay = tl.y + bl.y + tr.y + br.y;
az = tl.z + bl.z + tr.z + br.z;
}

return Vector3(ax,ay,az);
}