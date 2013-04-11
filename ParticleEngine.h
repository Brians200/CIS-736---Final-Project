#pragma once
#include "Particle.h"
#include <vector>

using namespace std;

class ParticleEngine
{
public:
	ParticleEngine(void);
	~ParticleEngine(void);
	void intializeEngine(int particles, int nthread);
	void step(float time);
	vector<float> getPositions();
	int getNumberOfParticles();
	float getParticleSize(int particleNumber);
	float getMass(int particleNumber);
	float getAcceleration(int particleNumber);
	float getVelocity(int particleNumber);

private:

};

