#pragma once
#include "Particle.h"
#include <vector>
#include <list>
#include <math.h>
#include <cstdlib>
#include "VectorMath.h"
#include <time.h>
#include <algorithm>
#include <thread>
#include <mutex>
#include "QuadTree.h"
using namespace std;

class ParticleEngine
{
public:
	ParticleEngine(float gp, float rMinp,float blackHoleMassp, float blackHoleRadiusp, float disappearingRadiusp,float minSpawnRadiusp, int maxSpawnRadiusp, float spawnVelocityp, int maxZSpawnDistancep, int threadsp, int particlesp, bool collisionsp, int integratorp, float gravityCutOff);
	~ParticleEngine(void);
	void intializeEngine();
	void step(float time);
	vector<float> getPositions();

	void increaseNumberOfParticles(int number);
	void decreaseNumberOfParticles(int number);
	void addParticle(Vector3 position, Vector3 velocity, float mass);

	int getNumberOfParticles();
	float getParticleSize(int particleNumber);
	float getMass(int particleNumber);
	float getAcceleration(int particleNumber);
	float getVelocity(int particleNumber);

	void setIntegrator(int integratorNumber);

private:
	ParticleEngine(void);

};