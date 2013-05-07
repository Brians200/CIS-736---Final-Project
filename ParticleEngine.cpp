#include "ParticleEngine.h"

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
int integrator;

vector<Vector3> newPositions;
vector<Vector3> newVelocities;
vector<Vector3> newAccelerations;

list<vector<int>> listOfCollisions;
list<vector<int>> blackHoleRemovals;


mutex mutexLock;
mutex mutexLock2;

ParticleEngine::ParticleEngine(void)
{
}

ParticleEngine::ParticleEngine(float gp, float rMinp,float blackHoleMassp, float blackHoleRadiusp, float disappearingRadiusp,float minSpawnRadiusp, int maxSpawnRadiusp, float spawnVelocityp, int maxZSpawnDistancep, int threadsp, int particlesp, bool collisionsp, int integratorp)
{
	g = gp;
	rMin = rMinp;
	blackHoleMass = blackHoleMassp;
	blackHoleRadius = blackHoleRadiusp;
	disappearingRadius = disappearingRadiusp;
	minSpawnRadius = minSpawnRadiusp;
	maxSpawnRadius = (int)(maxSpawnRadiusp - minSpawnRadiusp);
	spawnVelocity = spawnVelocityp;
	maxZSpawnDistance = maxZSpawnDistancep;
	collisions = collisionsp;
	numberOfThreads = threadsp;
	numberOfParticles = particlesp;
	integrator = integratorp;

	srand ((unsigned int)time(NULL));
	previousAngle = 0.0f;

	intializeEngine();

}

void ParticleEngine::setIntegrator(int integratorNumber)
{
	integrator = integratorNumber;
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

void ParticleEngine::addParticle(Vector3 position, Vector3 velocity, float mass)
{
	numberOfParticles++;
	Particle result;
	result.position = position;
	result.velocity = velocity;
	result.mass = mass;
	particleArray.push_back(result);
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

Vector3 calculateParticleAcceleration(int particleNumber,Vector3 position)
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
		Vector3 difference = VectorMath::difference(otherParticle.position, position);

		//calculate the distance between them
		float radius = sqrtf(difference.x*difference.x + difference.y *difference.y + difference.z*difference.z);

		//we are setting the radius of the particle to be the mass/100
		//so if the radius between the centers is less than the sum of both radii,
		//they have collided
		if(collisions && radius<thisParticle.radius+otherParticle.radius && particleNumber < i)
		{
			particleArray[particleNumber].velocity.x = (thisParticle.velocity.x * thisParticle.mass + otherParticle.velocity.x*otherParticle.mass) / (thisParticle.mass + otherParticle.mass);
			particleArray[particleNumber].velocity.y = (thisParticle.velocity.y * thisParticle.mass + otherParticle.velocity.y*otherParticle.mass) / (thisParticle.mass + otherParticle.mass);
			particleArray[particleNumber].velocity.z = (thisParticle.velocity.z * thisParticle.mass + otherParticle.velocity.z*otherParticle.mass) / (thisParticle.mass + otherParticle.mass);

			particleArray[particleNumber].setMass(thisParticle.mass+otherParticle.mass);
			particleArray[i].setMass(0);

			//Place the particles in a list to be removed
			mutexLock.lock();
			
			vector<int> coll;
			coll.push_back(particleNumber);
			coll.push_back(i);

			listOfCollisions.push_back(coll);

			mutexLock.unlock();
			
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

Vector3 calculateBlackHoleAcceleration(int particleNumber, Vector3 position)
{
	//if there is no black hole, no point in checking
	if(blackHoleMass == 0.0) return Vector3(0,0,0);

	Particle thisParticle = particleArray[particleNumber];

	//find out how far away from the blackhole it is
	float radius = sqrtf(position.x*position.x + position.y*position.y + position.z*position.z);

	if(radius>blackHoleRadius && radius < disappearingRadius)
	{

		//increase the fudge factor more than the other, so the black hole isn't as strong
		float radiusInverse = 1.0f/ (radius + 2.0f*rMin);

		float acceleration = g*blackHoleMass*thisParticle.mass*radiusInverse*radiusInverse;

		float newx = -acceleration*position.x*radiusInverse;
		float newy = -acceleration*position.y*radiusInverse;
		float newz = -acceleration*position.z*radiusInverse;

		return Vector3(newx,newy,newz);
	}
	else if(disappearingRadius> radius) //too far away
	{
		//particleArray[particleNumber] = generateNewParticle();
		mutexLock2.lock();
			
		vector<int> coll;
		coll.push_back(-1);
		coll.push_back(particleNumber);

		blackHoleRemovals.push_back(coll);

		mutexLock2.unlock();
	}
	else // inside the black hole
	{

		mutexLock2.lock();
		blackHoleMass += thisParticle.mass;
		thisParticle.setMass(0);

		//particleArray[particleNumber] = generateNewParticle();
				
		vector<int> coll;
		coll.push_back(-1);
		coll.push_back(particleNumber);

		blackHoleRemovals.push_back(coll);

		mutexLock2.unlock();
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
         Vector3 output = calculateParticleAcceleration(i,state.position);
		 output = VectorMath::add(output,calculateBlackHoleAcceleration(i,state.position));
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

//Improved Euler's method / Heun's method
void integrate2(State &state,  float dt,int i)
{
	Derivative a = evaluate(state, 0.0f, Derivative(),i);
	Derivative b = evaluate(state, dt, a, i);

	Vector3 dxdt = VectorMath::multiply(1/2.0f,VectorMath::add(a.dposition,b.dposition));
	Vector3 dvdt = VectorMath::multiply(1/2.0f,VectorMath::add(a.dvelocity,b.dvelocity));

	state.position = VectorMath::add(state.position, VectorMath::multiply(dt, dxdt));
	state.velocity = VectorMath::add(state.velocity,VectorMath::multiply(dt, dvdt));
	
	newPositions[i] = state.position;
	newVelocities[i] = state.velocity;
	newAccelerations[i] = dvdt;

}


//Euler's method
void integrate1(State &state,  float dt,int i)
{
	Derivative a = evaluate(state, 0.0f, Derivative(),i);
	
	Vector3 dxdt = a.dposition;
	Vector3 dvdt = a.dvelocity;

	state.position = VectorMath::add(state.position, VectorMath::multiply(dt, dxdt));
	state.velocity = VectorMath::add(state.velocity,VectorMath::multiply(dt, dvdt));
	
	newPositions[i] = state.position;
	newVelocities[i] = state.velocity;
	newAccelerations[i] = dvdt;

}

void parallelAcceleration(int start,int stop, float time, int integrator)
{
	
	if(integrator == 1)
	{//Euler's Method
		for(int i=start; i<=stop; i++)
			{
				State state;
				state.position = particleArray[i].position;
				state.velocity = particleArray[i].velocity;
				integrate1(state,time,i);
		}
	}
	else if(integrator == 2)
	{//Improved Euler / Hoen
		for(int i=start; i<=stop; i++)
			{
				State state;
				state.position = particleArray[i].position;
				state.velocity = particleArray[i].velocity;
				integrate2(state,time,i);
		}
	}
	else if(integrator == 3)
	{//Runge Kutta 3
		for(int i=start; i<=stop; i++)
			{
				State state;
				state.position = particleArray[i].position;
				state.velocity = particleArray[i].velocity;
				integrate3(state,time,i);
		}
	}
	else if(integrator == 4)
	{//Runge Kutta 4
		for(int i=start; i<=stop; i++)
			{
				State state;
				state.position = particleArray[i].position;
				state.velocity = particleArray[i].velocity;
				integrate4(state,time,i);
		}
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
		threads.push_back(std::thread(parallelAcceleration,i*numberOfParticles/numberOfThreads, (i+1)*numberOfParticles/numberOfThreads-1, time,integrator));
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

	//remove collided particles
	if(listOfCollisions.size() > 0)
	{
		list<int> removing;
		while(0<listOfCollisions.size())
		{
			removing.push_back(listOfCollisions.front()[1]);
			listOfCollisions.pop_front();
		}

		while(0<blackHoleRemovals.size())
		{
			removing.push_back(blackHoleRemovals.front()[1]);
			blackHoleRemovals.pop_front();
		}

		removing.sort();
		removing.unique();


		while(0<removing.size())
		{
			particleArray.erase(particleArray.begin()+removing.back());
			removing.pop_back();
			numberOfParticles -- ;
		}
	}
}

vector<float> ParticleEngine::getParticleEngineCenter()
{
	float x,y,z;
	x=y=z=0.0f;

	for(int i=0;i<numberOfParticles;i++)
	{
		Vector3 pos = particleArray[i].position;
		x+=pos.x;
		y+=pos.y;
		z+=pos.z;
	}

	if(numberOfParticles!=0)
	{
		x/=numberOfParticles;
		y/=numberOfParticles;
		z/=numberOfParticles;
	}

	vector<float> retern;
	retern.push_back(x);
	retern.push_back(y);
	retern.push_back(z);

}
