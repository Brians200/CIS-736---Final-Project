#pragma once

#include "Vector3.h"
class Particle
{
public:
	Particle(void);
	~Particle(void);
private:

public:
	Vector3 position;
	Vector3 velocity;
	Vector3 acceleration;
	float mass;
	float radius;

	void setMass(float newMass);
};

