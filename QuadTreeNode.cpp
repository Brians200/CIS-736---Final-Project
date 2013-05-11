#include "QuadTreeNode.h"


QuadTreeNode  TopLeft;
QuadTreeNode  TopRight;
QuadTreeNode  BottomLeft;
QuadTreeNode  BottomRight;



QuadTreeNode::QuadTreeNode(void)
{
}

QuadTreeNode::~QuadTreeNode(void)
{
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

void QuadTreeNode::AddParticle(float x, float y, float z, float newMass)
{
	
	
	//create a single item node, taking up the whole node
	if(items == 0)
	{
		mass = newMass;
		particleCenter = Vector3(x,y,z);

		items++;
		return;
	}
	else if( items == 1)
	{
		//topRight
		Vector3 tr(TopRightCorner.x,TopRightCorner.y, 0.0f);
		Vector3 tl(middle.x,TopLeftCorner.y,0.0f);
		Vector3 br(BottomRightCorner.x,middle.y,0.0f);
		Vector3 bl(middle.x,middle.y,0.0f);

		TopRight = QuadTreeNode(tl,tr,bl,br);

		//topLeft
		tr = Vector3(middle.x,TopLeftCorner.y, 0.0f);
		tl = Vector3(TopLeftCorner.x,TopLeftCorner.y,0.0f);
		br = Vector3(middle.x,middle.y,0.0f);
		bl = Vector3(TopLeftCorner.x,middle.y,0.0f);

		TopLeft = QuadTreeNode(tl,tr,bl,br);

		//bottomLeft
		tr = Vector3(middle.x,middle.y, 0.0f);
		tl = Vector3(BottomLeftCorner.x,middle.y,0.0f);
		br = Vector3(middle.x,BottomLeftCorner.y,0.0f);
		bl = Vector3(BottomLeftCorner.x,BottomLeftCorner.y,0.0f);

		BottomLeft = QuadTreeNode(tl,tr,bl,br);

		//bottomRight
		tr = Vector3(BottomRightCorner.x,middle.y, 0.0f);
		tl = Vector3(middle.x,middle.y,0.0f);
		br = Vector3(BottomRightCorner.x,BottomRightCorner.y,0.0f);
		bl = Vector3(middle.x,BottomRightCorner.y,0.0f);

		BottomRight = QuadTreeNode(tl,tr,bl,br);

		if(particleCenter.x >= middle.x && particleCenter.y >= middle.y)  // top right
		{
			TopRight.AddParticle(particleCenter.x,particleCenter.y,particleCenter.z,mass);
		}
		else if(particleCenter.x >= middle.x && particleCenter.y < middle.y)  // bottom right
		{
			BottomRight.AddParticle(particleCenter.x,particleCenter.y,particleCenter.z,mass);
		}
		else if(particleCenter.x < middle.x && particleCenter.y >= middle.y)  // top left
		{
			TopLeft.AddParticle(particleCenter.x,particleCenter.y,particleCenter.z,mass);
		}
		else if(particleCenter.x < middle.x && particleCenter.y < middle.y)  // bottom left
		{
			BottomLeft.AddParticle(particleCenter.x,particleCenter.y,particleCenter.z,mass);
		}


	}
	//figure out which quadrant it is in and add it there
	//also update the mass and center of mass of this node
	if(x >= middle.x && y >= middle.y)  // top right
	{
		TopRight.AddParticle(x,y,z,newMass);
	}
	else if(x >= middle.x && y < middle.y)  // bottom right
	{
		BottomRight.AddParticle(x,y,z,newMass);
	}
	else if(x < middle.x && y >= middle.y)  // top left
	{
		TopLeft.AddParticle(x,y,z,newMass);
	}
	else if(x < middle.x && y < middle.y)  // bottom left
	{
		BottomLeft.AddParticle(x,y,z,newMass);
	}

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
	
}


