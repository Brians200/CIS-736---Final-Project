#pragma once
#include "Particle.h"
#include <vector>

using namespace std;

class ParticleEngine
{
public:
	ParticleEngine(void);
	~ParticleEngine(void);
	void intializeEngine(int particles);
	void step(float time);
	vector<float> getPositions();
	int getNumberOfParticles();

private:
	int numberOfParticles;
	vector<Particle> particleArray;
	float G;
	float Rmin;
	float blackHoleMass;
	float blackHoleRadius;
	float disappearingRadius;
	bool collisions;

	float previousAngle;
	float centerX;
	float centerY;
	float centerZ;
	float totalMass;


	void calculateParticleAcceleration(int particleNumber);
	void calculateBlackHoleAcceleration(int particleNumber);
	void updateVelocityAndPositions(float time);
	Particle generateNewParticle();
	void findCenterOfMass();
	

};

