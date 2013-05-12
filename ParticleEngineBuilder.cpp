#include "ParticleEngineBuilder.h"

float gBuilder;
float rMinBuilder;
float blackHoleMassBuilder;
float blackHoleRadiusBuilder;
float disappearingRadiusBuilder;
bool collisionsBuilder;
int numberOfThreadsBuilder;
float minSpawnRadiusBuilder;
int maxSpawnRadiusBuilder;
float spawnVelocityBuilder;
int maxZSpawnDistanceBuilder;
int numberOfParticlesBuilder;
int integratorBuilder;
float gravityCutOffBuilder;

ParticleEngineBuilder::ParticleEngineBuilder(){
gBuilder=30.0f;
rMinBuilder = 5.0f;
blackHoleMassBuilder = 0.0f;
blackHoleRadiusBuilder = 5.0f;
disappearingRadiusBuilder = 100000.0f;
collisionsBuilder = true;
numberOfThreadsBuilder = 1;
numberOfParticlesBuilder=150;
minSpawnRadiusBuilder = 70;
maxSpawnRadiusBuilder = 270;
spawnVelocityBuilder = 12.0f;
maxZSpawnDistanceBuilder = 10;
integratorBuilder = 4;
gravityCutOffBuilder = .0001f;
}

ParticleEngineBuilder* ParticleEngineBuilder::setGravitationalConstant(float value){gBuilder=value; return this;}
ParticleEngineBuilder* ParticleEngineBuilder::setMinimumRadius(float value){rMinBuilder = value; return this;}
ParticleEngineBuilder* ParticleEngineBuilder::setBlackHoleMass(float value){blackHoleMassBuilder = value; return this;}
ParticleEngineBuilder* ParticleEngineBuilder::setBlackHoleRadius(float value){blackHoleRadiusBuilder = value; return this;}
ParticleEngineBuilder* ParticleEngineBuilder::setDisappearingRadius(float value){disappearingRadiusBuilder = value; return this;}
ParticleEngineBuilder* ParticleEngineBuilder::setCollisions(bool value){collisionsBuilder = value; return this;}
ParticleEngineBuilder* ParticleEngineBuilder::setNumberOfThreads(int value){numberOfThreadsBuilder= value; return this;}
ParticleEngineBuilder* ParticleEngineBuilder::setMinSpawnRadius(float value){minSpawnRadiusBuilder = value; return this;}
ParticleEngineBuilder* ParticleEngineBuilder::setmaxSpawnRadius(int value){maxSpawnRadiusBuilder = value; return this;}
ParticleEngineBuilder* ParticleEngineBuilder::setspawnVelocity(float value){spawnVelocityBuilder = value; return this;}
ParticleEngineBuilder* ParticleEngineBuilder::setmaxZSpawnDistance(int value){maxZSpawnDistanceBuilder = value; return this;}
ParticleEngineBuilder* ParticleEngineBuilder::setnumberOfParticles(int value){numberOfParticlesBuilder = value; return this;}
ParticleEngineBuilder* ParticleEngineBuilder::setIntegrator(int value){integratorBuilder = value; return this;}
ParticleEngineBuilder* ParticleEngineBuilder::setGravityCutOff(float value){gravityCutOffBuilder = value; return this;}

ParticleEngine ParticleEngineBuilder::Build(){ return ParticleEngine(gBuilder,rMinBuilder,blackHoleMassBuilder,blackHoleRadiusBuilder,disappearingRadiusBuilder,minSpawnRadiusBuilder,maxSpawnRadiusBuilder,spawnVelocityBuilder,maxZSpawnDistanceBuilder,numberOfThreadsBuilder,numberOfParticlesBuilder,collisionsBuilder,integratorBuilder, gravityCutOffBuilder);}