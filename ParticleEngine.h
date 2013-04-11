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

	void increaseNumberOfParticles(int number);
	void decreaseNumberOfParticles(int number);

	int getNumberOfParticles();
	float getParticleSize(int particleNumber);
	float getMass(int particleNumber);
	float getAcceleration(int particleNumber);
	float getVelocity(int particleNumber);


private:

};

