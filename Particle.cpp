#include "Particle.h"
#include <math.h>

Particle::Particle(void)
{
	position = Vector3(0,0,0);
	velocity = Vector3(0,0,0);
	acceleration = Vector3(0,0,0);
	mass = 1.0f;
	radius = mass/100.0f;
}


Particle::~Particle(void)
{
}

void Particle::setMass(float newMass)
{
	mass = newMass;
	radius = log10f(newMass)/5.0f;
	if(radius <=0)
		radius = .01f;
}