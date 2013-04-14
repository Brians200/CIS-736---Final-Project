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
float g;
float rMin;
float blackHoleMass;
float blackHoleRadius;
float disappearingRadius;
bool collisions;
float previousAngle;
int numberOfThreads;
float minSpawnRadius;
int maxSpawnRadius;
float spawnVelocity;
int maxZSpawnDistance;

vector<Vector3> newPositions;
vector<Vector3> newVelocities;
vector<Vector3> newAccelerations;

ParticleEngine::ParticleEngine(void)
{
}

ParticleEngine::ParticleEngine(float gp, float rMinp,float blackHoleMassp, float blackHoleRadiusp, float disappearingRadiusp,float minSpawnRadiusp, int maxSpawnRadiusp, float spawnVelocityp, int maxZSpawnDistancep, int threadsp, int particlesp, bool collisionsp)
{
	g = gp;
	rMin = rMinp;
	blackHoleMass = blackHoleMassp;
	blackHoleRadius = blackHoleRadiusp;
	disappearingRadius = disappearingRadiusp;
	minSpawnRadius = minSpawnRadiusp;
	maxSpawnRadius = maxSpawnRadiusp - minSpawnRadiusp;
	spawnVelocity = spawnVelocityp;
	maxZSpawnDistance = maxZSpawnDistancep;
	collisions = collisionsp;
	numberOfThreads = threadsp;
	numberOfParticles = particlesp;

	srand ((unsigned int)time(NULL));
	previousAngle = 0.0f;

	intializeEngine();

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

	float velocity = (float)spawnVelocity;//((float)(rand()%maxSpawnVelocity));

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

void ParticleEngine::increaseNumberOfParticles(int number)
{
	numberOfParticles+=number;
	for(int i = 0 ;i<number;i++)
	{
		particleArray.push_back(generateNewParticle());
	}
}
void ParticleEngine::decreaseNumberOfParticles(int number)
{
	if(numberOfParticles-number>0)
	{
		for(int i=0;i<number;i++)
		{
			numberOfParticles--;
			particleArray.pop_back();
		}
	}
	else
	{
		numberOfParticles=1;
		particleArray.clear();
		particleArray.push_back(generateNewParticle());
	}
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


void ParticleEngine::intializeEngine()
{
	particleArray = vector<Particle>(numberOfParticles);
	for(int i = 0; i<numberOfParticles; i++)
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

Vector3 calculateParticleAcceleration(int particleNumber)
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
		float radiusInverse = 1.0f/(radius + rMin);

		//Figure out the magnitude of the total acceleration between these two
		float acceleration = g*thisParticle.mass*otherParticle.mass*radiusInverse*radiusInverse;

		//add the individual compents to the running total
		ax += acceleration * difference.x * radiusInverse;
		ay += acceleration * difference.y * radiusInverse;
		az += acceleration * difference.z * radiusInverse;

	}
	return Vector3(ax,ay,az);

}

Vector3 calculateBlackHoleAcceleration(int particleNumber)
{
	//TODO: decide if we want to remove stars that are too far away from the middle

	//TODO: decide if we are going to have the blackhole eat things that are too close

	//if there is no black hole, no point in checking
	if(blackHoleMass == 0.0) return Vector3(0,0,0);

	Particle thisParticle = particleArray[particleNumber];

	//find out how far away from the blackhole it is
	float radius = sqrtf(thisParticle.position.x*thisParticle.position.x + thisParticle.position.y*thisParticle.position.y + thisParticle.position.z*thisParticle.position.z);

	if(radius>blackHoleRadius && radius < disappearingRadius)
	{

		//increase the fudge factor more than the other, so the black hole isn't as strong
		float radiusInverse = 1.0f/ (radius + 2.0f*rMin);

		float acceleration = g*blackHoleMass*thisParticle.mass*radiusInverse*radiusInverse;

		float newx = -acceleration*thisParticle.position.x*radiusInverse;
		float newy = -acceleration*thisParticle.position.y*radiusInverse;
		float newz = -acceleration*thisParticle.position.z*radiusInverse;

		//particleArray[particleNumber].acceleration.x += newx;
		//particleArray[particleNumber].acceleration.y += newy;
		//particleArray[particleNumber].acceleration.z += newz;

		return Vector3(newx,newy,newz);
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

	return Vector3(0,0,0);
}

struct State
{
	Vector3 position;
	Vector3 velocity;
};

struct Derivative
{
	Vector3 dposition; // derivative of position = velocity
	Vector3 dvelocity; // derivative of velocity = acceleration;
};

Vector3 acceleration(const State &state,int i)
{
         Vector3 output = calculateParticleAcceleration(i);
		 output = VectorMath::add(output,calculateBlackHoleAcceleration(i));
		 return output;
}

Derivative evaluate(const State &initial, float dt, const Derivative &d,int i)
{
	State state;
	state.position = VectorMath::add(initial.position,VectorMath::multiply(dt,d.dposition));
	state.velocity = VectorMath::add(initial.velocity,VectorMath::multiply(dt,d.dvelocity));

	Derivative output;
	output.dposition = state.velocity;
	output.dvelocity = acceleration(state,i);
	return output;

}

//Runge kutta 4th order
void integrate4(State &state,  float dt,int i)
{
	Derivative a = evaluate(state,  0.0f, Derivative(),i);
	Derivative b = evaluate(state,  dt*0.5f, a,i);
	Derivative c = evaluate(state,  dt*0.5f, b,i);
	Derivative d = evaluate(state,  dt, c,i);

	Vector3 dxdt = VectorMath::multiply(1.0f/6.0f , (VectorMath::add(VectorMath::add(a.dposition, VectorMath::multiply(2.0f,(VectorMath::add(b.dposition,c.dposition)))) , d.dposition)));
	Vector3 dvdt = VectorMath::multiply(1.0f/6.0f , (VectorMath::add(VectorMath::add(a.dvelocity, VectorMath::multiply(2.0f,(VectorMath::add(b.dvelocity,c.dvelocity)))) , d.dvelocity)));

	state.position = VectorMath::add(state.position, VectorMath::multiply(dt, dxdt));
	state.velocity = VectorMath::add(state.velocity,VectorMath::multiply(dt, dvdt));
	
	newPositions[i] = state.position;
	newVelocities[i] = state.velocity;
	newAccelerations[i] = dvdt;

}

//Runge kutta 3rd order
void integrate3(State &state,  float dt,int i)
{
	Derivative a = evaluate(state,  0.0f, Derivative(),i);
	Derivative b = evaluate(state,  dt*0.5f, a,i);
	Derivative c = evaluate(state,  dt, b,i);
	

	Vector3 dxdt =  VectorMath::multiply(1.0f/6.0f , VectorMath::add(a.dposition, VectorMath::add(VectorMath::multiply(4.0f,b.dposition),c.dposition)));
	Vector3 dvdt =  VectorMath::multiply(1.0f/6.0f , VectorMath::add(a.dvelocity, VectorMath::add(VectorMath::multiply(4.0f,b.dvelocity),c.dvelocity)));

	state.position = VectorMath::add(state.position, VectorMath::multiply(dt, dxdt));
	state.velocity = VectorMath::add(state.velocity,VectorMath::multiply(dt, dvdt));
	
	newPositions[i] = state.position;
	newVelocities[i] = state.velocity;
	newAccelerations[i] = dvdt;

}

void parallelAcceleration(int start,int stop, float time)
{
	int a=3;
	for(int i=start; i<=stop; i++)
		{
			State state;
			state.position = particleArray[i].position;
			state.velocity = particleArray[i].velocity;
			integrate3(state,time,i);
	}
}




void ParticleEngine::step(float time)
{

	vector<std::thread> threads;

	newPositions = vector<Vector3>(numberOfParticles);
	newVelocities = vector<Vector3>(numberOfParticles);
	newAccelerations = vector<Vector3>(numberOfParticles);

	for(int i=0; i<numberOfThreads; i++)
	{ 
		threads.push_back(std::thread(parallelAcceleration,i*numberOfParticles/numberOfThreads, (i+1)*numberOfParticles/numberOfThreads-1, time));
	}

	for(int j = 0; j<numberOfThreads; j++)
	{
		threads[j].join();
	}

	for(int k=0; k<numberOfParticles;k++)
	{
		particleArray[k].position = newPositions[k];
		particleArray[k].velocity = newVelocities[k];
		particleArray[k].acceleration = newAccelerations[k];
	}
}
