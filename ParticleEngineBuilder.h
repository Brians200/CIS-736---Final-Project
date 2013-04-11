#pragma once
#include "ParticleEngine.h"
using namespace std;

class ParticleEngineBuilder
{
public:
	ParticleEngineBuilder(void);

	ParticleEngineBuilder* setGravitationalConstant(float value);
	ParticleEngineBuilder* setMinimumRadius(float value);
	ParticleEngineBuilder* setBlackHoleMass(float value);
	ParticleEngineBuilder* setBlackHoleRadius(float value);
	ParticleEngineBuilder* setDisappearingRadius(float value);
	ParticleEngineBuilder* setCollisions(bool value);
	ParticleEngineBuilder* setNumberOfThreads(int value);
	ParticleEngineBuilder* setMinSpawnRadius(float value);
	ParticleEngineBuilder* setmaxSpawnRadius(int value);
	ParticleEngineBuilder* setspawnVelocity(float value);
	ParticleEngineBuilder* setmaxZSpawnDistance(int value);
	ParticleEngineBuilder* setnumberOfParticles(int value);


	ParticleEngine Build();
private:
	


};