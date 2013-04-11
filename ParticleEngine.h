#pragma once
#include "Particle.h"
#include <vector>

using namespace std;

class ParticleEngine
{
public:
	ParticleEngine(float gp, float rMinp,float blackHoleMassp, float blackHoleRadiusp, float disappearingRadiusp,float minSpawnRadiusp, int maxSpawnRadiusp, float spawnVelocityp, int maxZSpawnDistancep, int threadsp, int particlesp, bool collisionsp);
	~ParticleEngine(void);
	void intializeEngine();
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
	ParticleEngine(void);

protected:
	

};

