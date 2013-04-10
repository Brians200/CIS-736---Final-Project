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

private:
	
	
	//	void calculateParticleAcceleration(int particleNumber);
//	void calculateBlackHoleAcceleration(int particleNumber);
//	void updateVelocityAndPositions(float time);
//	Particle generateNewParticle();

};

