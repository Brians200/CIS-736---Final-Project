#include "ParticleEngine.h"
#include <math.h>
#include <cstdlib>
#include "VectorMath.h"
#include <time.h>
#include <algorithm>
#include <thread>

const float PI = std::atanf(1.0f)*4.0f;
int numberOfParticles;
vector<Particle> particleArray;
float G;
float Rmin;
float blackHoleMass;
float blackHoleRadius;
float disappearingRadius;
bool collisions;
float previousAngle;
int numberOfThreads;
float minSpawnRadius;
int maxSpawnRadius;
int maxSpawnVelocity;
int maxZSpawnDistance;

ParticleEngine::ParticleEngine(void)
{
	//Gravitational Constant
	G=30;

	//Fudge factor, so we don't divide by really small numbers
	Rmin = 5;

	//How strong the central keeping force is, 0 to turn this off
	blackHoleMass = 000.0f;

	//How close the particles need to be before getting eaten, 0 to turn this off
	blackHoleRadius = 5.0f;

	//How far away from the origin before they are removed
	disappearingRadius = 100000.0f;
	
	//Should particles collide with each other or not
	collisions = false;

	//Minimum spawn radius
	minSpawnRadius = 70.0f;

	//Maximum spawn radius addition
	maxSpawnRadius = 200;

	//Max Spawn Velocity
	maxSpawnVelocity = 12;

	//Max Z spawn distance
	maxZSpawnDistance = 10;



	/******************/
	//Don't touch these
	/******************/
	srand ((unsigned int)time(NULL));
	previousAngle = 0.0f;
	numberOfThreads=1;
}

float ParticleEngine::getParticleSize(int particleNumber)
{
	return particleArray[particleNumber].radius;
}

Particle generateNewParticle()
{
	Particle retern = Particle();
	
	float radius = ((float)(rand()%maxSpawnRadius)+minSpawnRadius);

	retern.position.x = radius*cosf(previousAngle);
	retern.position.y = radius*sinf(previousAngle);
	retern.position.z = (float)(rand()%maxZSpawnDistance) - (float)maxZSpawnDistance/2;

	float velocity = (float)maxSpawnVelocity;//((float)(rand()%maxSpawnVelocity));

	retern.velocity.x = -velocity*sinf(previousAngle);
	retern.velocity.y = velocity*cosf(previousAngle);
	retern.velocity.z = 0.0f;


	previousAngle+= ((float)(rand()%100))/100.0f;

	int random = rand()%10;
	if(random == 9)
	{
		retern.setMass(3.0f);
	}
	else if(random>6)
	{
		retern.setMass(2.0f);
	}
	else
	{
		retern.setMass(1.0f);
	}

	return retern;
}


ParticleEngine::~ParticleEngine(void)
{
}

vector<float> ParticleEngine::getPositions()
{
	vector<float> retern(3*numberOfParticles);
	for(int i=0;i<numberOfParticles; i++)
	{
		retern[3*i]=(particleArray[i].position.x);
		retern[3*i+1]=(particleArray[i].position.y);
		retern[3*i+2]=(particleArray[i].position.z);
	}
	return retern;
}


void ParticleEngine::intializeEngine(int particles, int nthreads)
{
	if (nthreads>particles)
	{
		numberOfThreads = particles;
	}
	else
	{
		numberOfThreads=nthreads;
	}

	numberOfParticles = particles;
	particleArray = vector<Particle>(particles);
	for(int i = 0; i<particles; i++)
	{
		particleArray[i] = generateNewParticle();
	}
}


float ParticleEngine::getMass(int particleNumber)
{
	return particleArray[particleNumber].mass;
}

float ParticleEngine::getAcceleration(int particleNumber)
{
	float x = particleArray[particleNumber].acceleration.x;
	float y = particleArray[particleNumber].acceleration.y;
	float z = particleArray[particleNumber].acceleration.z;

	return sqrt(x*x+y*y+z*z);
}

float ParticleEngine::getVelocity(int particleNumber)
{
	float x = particleArray[particleNumber].velocity.x;
	float y = particleArray[particleNumber].velocity.y;
	float z = particleArray[particleNumber].velocity.z;

	return sqrt(x*x+y*y+z*z);
}


int ParticleEngine::getNumberOfParticles()
{
	return numberOfParticles;
}

void calculateParticleAcceleration(int particleNumber)
{
	float ax,ay,az;
	ax = ay = az = 0;

	Particle thisParticle = particleArray[particleNumber];

	//loop over all the particles
	for(int i =0 ;i < numberOfParticles; i++)
	{
		//skip this particle
		if(i==particleNumber) continue;

		Particle otherParticle = particleArray[i];

		//calculate differences between their coordinates
		Vector3 difference = VectorMath::difference(otherParticle.position, thisParticle.position);

		//calculate the distance between them
		float radius = sqrtf(difference.x*difference.x + difference.y *difference.y + difference.z*difference.z);

		//we are setting the radius of the particle to be the mass/100
		//so if the radius between the centers is less than the sum of both radii,
		//they have collided
		if(collisions && radius<thisParticle.radius+otherParticle.radius)
		{
			particleArray[particleNumber].velocity.x = (thisParticle.velocity.x * thisParticle.mass + otherParticle.velocity.x*otherParticle.mass) / (thisParticle.mass + otherParticle.mass);
			particleArray[particleNumber].velocity.y = (thisParticle.velocity.y * thisParticle.mass + otherParticle.velocity.y*otherParticle.mass) / (thisParticle.mass + otherParticle.mass);
			particleArray[particleNumber].velocity.z = (thisParticle.velocity.z * thisParticle.mass + otherParticle.velocity.z*otherParticle.mass) / (thisParticle.mass + otherParticle.mass);

			particleArray[particleNumber].setMass(thisParticle.mass+otherParticle.mass);

			//replace the second particle
			particleArray[i] = generateNewParticle();
		}
	
		//get the inverse, with a slight fudge factor to prevent dividing by really small numbers, cause the particles to fling off when they get close
		float radiusInverse = 1.0f/(radius + Rmin);

		//Figure out the magnitude of the total acceleration between these two
		float acceleration = G*thisParticle.mass*otherParticle.mass*radiusInverse*radiusInverse;

		//add the individual compents to the running total
		ax += acceleration * difference.x * radiusInverse;
		ay += acceleration * difference.y * radiusInverse;
		az += acceleration * difference.z * radiusInverse;

	}


	particleArray[particleNumber].acceleration.x = ax;
	particleArray[particleNumber].acceleration.y = ay;
	particleArray[particleNumber].acceleration.z = az;

}

void calculateBlackHoleAcceleration(int particleNumber)
{
	//TODO: decide if we want to remove stars that are too far away from the middle

	//TODO: decide if we are going to have the blackhole eat things that are too close

	//if there is no black hole, no point in checking
	if(blackHoleMass == 0.0) return;

	Particle thisParticle = particleArray[particleNumber];

	//find out how far away from the blackhole it is
	float radius = sqrtf(thisParticle.position.x*thisParticle.position.x + thisParticle.position.y*thisParticle.position.y + thisParticle.position.z*thisParticle.position.z);

	if(radius>blackHoleRadius && radius < disappearingRadius)
	{

		//increase the fudge factor more than the other, so the black hole isn't as strong
		float radiusInverse = 1.0f/ (radius + 2.0f*Rmin);

		float acceleration = G*blackHoleMass*thisParticle.mass*radiusInverse*radiusInverse;

		float newx = -acceleration*thisParticle.position.x*radiusInverse;
		float newy = -acceleration*thisParticle.position.y*radiusInverse;
		float newz = -acceleration*thisParticle.position.z*radiusInverse;

		particleArray[particleNumber].acceleration.x += newx;
		particleArray[particleNumber].acceleration.y += newy;
		particleArray[particleNumber].acceleration.z += newz;
	}
	else if(disappearingRadius> radius)
	{
		particleArray[particleNumber] = generateNewParticle();
	}
	else
	{
		
		blackHoleMass += thisParticle.mass;
		particleArray[particleNumber] = generateNewParticle();
	}
}

void updateVelocityAndPositions(float time)
{
	for(int i = 0; i < numberOfParticles; i++)
	{
		//delta V = A * delta T
		particleArray[i].velocity.x += particleArray[i].acceleration.x * time;
		particleArray[i].velocity.y += particleArray[i].acceleration.y * time;
		particleArray[i].velocity.z += particleArray[i].acceleration.z * time;

		//delta P = V * delta T
		particleArray[i].position.x += particleArray[i].velocity.x * time;
		particleArray[i].position.y += particleArray[i].velocity.y * time;
		particleArray[i].position.z += particleArray[i].velocity.z * time;

	}
}

void parallelAcceleration(int start,int stop)
{
    int a=3;
	for(int i=start; i<=stop; i++)
	{
		//calculate acceleration from the other stars
		calculateParticleAcceleration(i);

		//calculate the acceleration from the black hole
		//this is to help encourage the particles to stay towards the origin
		calculateBlackHoleAcceleration(i);
	}
}

void parallelVelocityAndPosition(int start, int stop, float time)
{
	//update start velocity and position
	for(int i = start; i < stop; i++)
	{
		//delta V = A * delta T
		particleArray[i].velocity.x += particleArray[i].acceleration.x * time;
		particleArray[i].velocity.y += particleArray[i].acceleration.y * time;
		particleArray[i].velocity.z += particleArray[i].acceleration.z * time;

		//delta P = V * delta T
		particleArray[i].position.x += particleArray[i].velocity.x * time;
		particleArray[i].position.y += particleArray[i].velocity.y * time;
		particleArray[i].position.z += particleArray[i].velocity.z * time;

	}
}

void ParticleEngine::step(float time)
{
	//Split all the particles into threads, so the acceleration can be calculated
	vector<std::thread> threads;
	for(int i=0;i<numberOfThreads;i++)
	{
		threads.push_back(std::thread(parallelAcceleration,i*numberOfParticles/numberOfThreads, (i+1)*numberOfParticles/numberOfThreads-1));
	}

	for(int j=0;j<numberOfThreads;j++)
	{
		threads[j].join();
	}

	//For some reason, it is faster not to parallelize the velocity update...

	/*threads.clear();
	for(int i=0;i<numberOfThreads;i++)
	{
		threads.push_back(std::thread(parallelVelocityAndPosition,i*numberOfParticles/numberOfThreads, (i+1)*numberOfParticles/numberOfThreads-1,time));
	}

	for(int j=0;j<numberOfThreads;j++)
	{
		threads[j].join();
	}*/
	
	updateVelocityAndPositions( time);
	

}