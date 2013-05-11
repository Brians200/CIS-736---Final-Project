#define _WIN32_WINNT 0x0502
// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include <GL/glew.h>
#include <GL/glfw.h>
#include <GL/glut.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "shader.hpp"
#include "controls.hpp"
#include "texture.hpp"

#include "ParticleEngine.h"
#include "ParticleEngineBuilder.h"
#include "ColorPicker.h"
#include "TGAWriter.h"

#include <vector>
#include <sstream>
//#include <iostream>

int main( void )
{
	_SYSTEM_INFO sysinfo;
	GetNativeSystemInfo( &sysinfo );
	int numCPU = sysinfo.dwNumberOfProcessors;

	int threads = numCPU;
	int particles = 2000;
	ParticleEngine pe = (new ParticleEngineBuilder())->
						setGravitationalConstant(7.0f)->
						setMinimumRadius(30.0f)->
						setBlackHoleMass(0.0f)->
						setBlackHoleRadius(5.0f)->
						setDisappearingRadius(10000.0f)->
						setCollisions(true)->
						setNumberOfThreads(threads)->
						setMinSpawnRadius(70)->
						setmaxSpawnRadius(270)->
						setspawnVelocity(9.0f)->
						setmaxZSpawnDistance(20)->
						setnumberOfParticles(particles)->
						setIntegrator(4)->
						Build();
	// Initialise GLFW
	if( !glfwInit() )	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		return -1;
	}

	glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	if( !glfwOpenWindow( 1920, 1080, 0,0,0,0, 32,0, GLFW_FULLSCREEN ) )	{
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		glfwTerminate();
		return -1;
	}

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	glfwSetWindowTitle( "Particle System" );

	// Ensure we can capture the escape key being pressed below
	glfwEnable( GLFW_STICKY_KEYS );
	glfwSetMousePos(1920/2, 1080/2);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "Particle.vertexshader", "Particle.fragmentshader" );

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	//vector<float> datav = pe.getPositions();
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	//glBufferData(GL_ARRAY_BUFFER, 3*pe.getNumberOfParticles()*sizeof(float), &datav[0], GL_STREAM_DRAW);

	GLuint colorBuffer;
	glGenBuffers(1, &colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	
	GLuint radiusBuffer;
	glGenBuffers(1, &radiusBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, radiusBuffer);

	//enable multisampling
	glEnable( GL_MULTISAMPLE );

	// Load the texture
	// And Get a handle for our "myTextureSampler" uniform
	GLuint Texture = loadBMP_custom("particle.bmp");
	GLuint TextureID  = glGetUniformLocation(programID, "myTexture");

	glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glEnable(GL_POINT_SPRITE);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glAlphaFunc(GL_GREATER,0.1f);
	glEnable(GL_ALPHA_TEST);

	glBindTexture(GL_TEXTURE_2D, TextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int renderStep = 0;
	int colorOption = 3;
	ColorPicker cp;
	int numParticles;
	//double lastTime = glfwGetTime();
	//double currentTime;
	
	//camera animation variables
	//glm::vec3 position = glm::vec3(0,0,0); 
	//int quadrantXZ = 4;
	//int quadrantYZ = 4;
	//int quadrantXYZ = 4;
	//Projection/View Matrix variables
	//glm::mat4 ViewMatrix;
	//glm::mat4 ProjectionMatrix;
	//vector<float> centerVector;

	/*
   void* bitmap_fonts[7] = {
      GLUT_BITMAP_9_BY_15,
      GLUT_BITMAP_8_BY_13,
      GLUT_BITMAP_TIMES_ROMAN_10,
      GLUT_BITMAP_TIMES_ROMAN_24,
      GLUT_BITMAP_HELVETICA_10,
      GLUT_BITMAP_HELVETICA_12,
      GLUT_BITMAP_HELVETICA_18     
   };
   */

	do{

		//currentTime = glfwGetTime();
		if(renderStep % 900 == 0){
			if((renderStep/900) % 4 == 0){
				//No Variable Color
				colorOption = 0;
			}
			else if((renderStep/900) % 2 == 0){
				//Velocity Variable Color
				//colorOption = 2;
				colorOption = 0;
			}
			else if((renderStep/900) % 3 == 0){
				//Mass Variable Color
				colorOption = 3;
			}
			else{
				colorOption = 1;
			}
		}
		
		/*
		if(renderStep <= 100){
			position[2]+=20;
		}
		
		if(renderStep == 500){
			position[2] = 2000;
		}
		if(renderStep > 900 && renderStep <= 1800)
		{
			if(quadrantXZ == 1){
				if(position[0] == 0){
					quadrantXZ=2;
				}else
				{
					position[0]-=10;
					position[2]-=10;
				}
			}
			if(quadrantXZ == 2){
				if(position[0] == -2000){
					quadrantXZ=3;
				}else
				{
					position[0]-=10;
					position[2]+=10;
				}
			}
			if(quadrantXZ == 3){
				if(position[2] == 2000){
					quadrantXZ=4;
				}else
				{
					position[0]+=10;
					position[2]+=10;
				}
			}
			if(quadrantXZ == 4){
				if(position[2] == 0){
					quadrantXZ=1;
				}else
				{
					position[0]+=10;
					position[2]-=10;
				}
			}
		}
		if(renderStep == 1801){	
			position[0] = 0;
			position[1] = 0;
			position[2] = 2000;
		}
		if(renderStep > 2700 && renderStep <= 3600)
		{
			if(quadrantYZ == 1){
				if(position[1] == 0){
					quadrantYZ=2;
				}else
				{
					position[1]-=10;
					position[2]-=10;
				}
			}
			if(quadrantYZ == 2){
				if(position[1] == -2000){
					quadrantYZ=3;
				}else
				{
					position[1]-=10;
					position[2]+=10;
				}
			}
			if(quadrantYZ == 3){
				if(position[2] == 2000){
					quadrantYZ=4;
				}else
				{
					position[1]+=10;
					position[2]+=10;
				}
			}
			if(quadrantYZ == 4){
				if(position[2] == 0){
					quadrantYZ=1;
				}else
				{
					position[1]+=10;
					position[2]-=10;
				}
			}
		}
		if(renderStep == 3601){	
			position[0] = 0;
			position[1] = 0;
			position[2] = 1700;
		}
		if(renderStep > 4500 && renderStep <= 7000)
		{
			if(quadrantXYZ == 1){
				if(position[1] == 0){
					quadrantXYZ=2;
				}else
				{
					position[0]-=10;
					position[1]-=10;
					position[2]-=10;
				}
			}
			if(quadrantXYZ == 2){
				if(position[1] == -1700){
					quadrantXYZ=3;
				}else
				{
					position[0]-=10;
					position[1]-=10;
					position[2]+=10;
				}
			}
			if(quadrantXYZ == 3){
				if(position[2] == 1700){
					quadrantXYZ=4;
				}else
				{
					position[0]+=10;
					position[1]+=10;
					position[2]+=10;
				}
			}
			if(quadrantXYZ == 4){
				if(position[2] == 0){
					quadrantXYZ=1;
				}else
				{
					position[0]+=10;
					position[1]+=10;
					position[2]-=10;
				}
			}
		}
		if(renderStep == 7001){	
			position[0] = 0;
			position[1] = 0;
			position[2] = 1200;
		}
		centerVector = pe.getParticleEngineCenter();
		glm::vec3 center = glm::vec3(centerVector[0],centerVector[1],centerVector[2]);
		glm::vec3 direction = glm::normalize(glm::vec3(center-position));
		glm::mat3 yRotate(
			0, 0.0, -1,
			0.0, 1.0, 0.0f,
			1, 0.0, 0.0f);
		glm::vec3 right = glm::vec3(yRotate[0][0]*direction[0] + yRotate[1][0]*direction[1] + yRotate[2][0]*direction[2],
			yRotate[0][1]*direction[0] + yRotate[1][1]*direction[1] + yRotate[2][1]*direction[2],
			yRotate[0][2]*direction[0] + yRotate[1][2]*direction[1] + yRotate[2][2]*direction[2]);
		glm::vec3 up = glm::cross( right, direction );
		ProjectionMatrix = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100000.0f);
		ViewMatrix = glm::lookAt(position, center, up);
		glm::mat4 ModelMatrix = glm::mat4(1.0f);
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		*/

		//End of Camera Movement for Movie

		// Clear the screen
		glClear( GL_COLOR_BUFFER_BIT );

		// Use our shader
		glUseProgram(programID);

		
		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();
		// Model matrix : an identity matrix (model will be at the origin)
		glm::mat4 ModelMatrix = glm::mat4(1.0f);
		// Our ModelViewProjection : multiplication of our 3 matrices
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		pe.step((float)(0.33333333333));	
		//pe.step((float)(currentTime-lastTime));
		//lastTime = currentTime;
	
		numParticles = pe.getNumberOfParticles();

		vector<float> datav = pe.getPositions();
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, 3*numParticles*sizeof(float), &datav[0], GL_STREAM_DRAW);

		
		// Decrease Number of Particles
		if (glfwGetKey( GLFW_KEY_KP_SUBTRACT ) == GLFW_PRESS){
			pe.decreaseNumberOfParticles(50);
			numParticles = pe.getNumberOfParticles();
		}
		// Increase Number of Particles
		if (glfwGetKey( GLFW_KEY_KP_ADD ) == GLFW_PRESS){
			pe.increaseNumberOfParticles(50);
			numParticles = pe.getNumberOfParticles();
		}

		// Set Integrator Euler
		if (glfwGetKey( '1' ) == GLFW_PRESS){pe.setIntegrator(1);}
		// Set Integrator Euler
		if (glfwGetKey( '2' ) == GLFW_PRESS){pe.setIntegrator(2);}
		// Set Integrator Euler
		if (glfwGetKey( '3' ) == GLFW_PRESS){pe.setIntegrator(3);}	
		// Set Integrator Euler
		if (glfwGetKey( '4' ) == GLFW_PRESS){pe.setIntegrator(4);}
		// Set Color Acceleration
		if (glfwGetKey( 'A' ) == GLFW_PRESS){colorOption = 1;}
		// Set Color Velocity
		if (glfwGetKey( 'V' ) == GLFW_PRESS){colorOption = 2;}
		// Set Color Mass
		if (glfwGetKey( 'M' ) == GLFW_PRESS){colorOption = 3;}
		// Set Color None All particles White
		if (glfwGetKey( 'N' ) == GLFW_PRESS){colorOption = 0;}
		

		//if( (renderStep % 30) == 0 )
		if( true )
		{
		vector<float> colorData(3*numParticles);
		if(colorOption == 0){//No Coloring
			for(int i=0; i<numParticles; i++){
				int size = pe.getParticleSize(i)*100;
				float hue = 0.0f;
				float saturation = 0.0f;
				float value = 0.0f;
				if(size>9){
					hue = 1;
					saturation = 1;
					value = 1;
				}
				else if(size>8){
					hue = 30;
					saturation = 1;
					value = 1;
				}
				else if(size>7){
					hue = 60;
					saturation = 1;
					value = 1;
				}
				else if(size>6){
					hue = 240;
					saturation = 0.5;
					value = 0.5;
				}
				else if(size>5){
					hue = 180;
					saturation = 0.5;
					value = 0.5;
				}
				else{
					hue = 0;
					saturation = 0;
					value = 1;

				}
				vector<float> newColor = cp.getColor(hue,saturation,value);
				float blue = newColor.back();
				newColor.pop_back();
				float green = newColor.back();
				newColor.pop_back();
				float red = newColor.back();
				newColor.pop_back();
				colorData[3*i]=red;
				colorData[3*i+1]=green;
				colorData[3*i+2]=blue;
			}
		}
		else if(colorOption == 1){//Color Acceleration

			float maxAcceleration = 0.0f;
			for(int i=0;i<numParticles;i++)
			{
				if(pe.getAcceleration(i) > maxAcceleration)
				{
					maxAcceleration = pe.getAcceleration(i);
				}
			}
			for(int i=0; i<numParticles; i++){
				float acceleration = pe.getAcceleration(i);
				vector<float> newColor = cp.getColor(120.0f*acceleration/maxAcceleration+240.0f,1,1);
				float blue = newColor.back();
				newColor.pop_back();
				float green = newColor.back();
				newColor.pop_back();
				float red = newColor.back();
				newColor.pop_back();
				colorData[3*i]=red;
				colorData[3*i+1]=green;
				colorData[3*i+2]=blue;
			}
		}
		else if(colorOption == 2){//Color Velocity

			float maxVelocity = 0.0f;
			for(int i=0;i<numParticles;i++)
			{
				if(pe.getVelocity(i) > maxVelocity)
				{
					maxVelocity = pe.getVelocity(i);
				}
			}
			for(int i=0; i<numParticles; i++){
				float velocity = pe.getVelocity(i);
				vector<float> newColor = cp.getColor(120.0f-120.0f * velocity/maxVelocity,1,1);
				float blue = newColor.back();
				newColor.pop_back();
				float green = newColor.back();
				newColor.pop_back();
				float red = newColor.back();
				newColor.pop_back();
				colorData[3*i]=red;
				colorData[3*i+1]=green;
				colorData[3*i+2]=blue;
			}
		}

		else if(colorOption == 3){//Color Mass
			float maxMass = 0.0f;
			float minMass = 0.0f;
			for(int i=0;i<numParticles;i++)
			{
				float massTester = pe.getMass(i);
				if(massTester > maxMass)
				{
					maxMass = massTester;
				}
				if(massTester < minMass)
				{
					minMass = massTester;
				}
			}
			for(int i=0; i<numParticles; i++){
				float massRange = maxMass - minMass;
				float mass = pe.getMass(i);
				float value = (((massRange - mass) / massRange) - 1) * -1;
				vector<float> newColor = cp.getColor(180,mass/maxMass,value);
				float blue = newColor.back();
				newColor.pop_back();
				float green = newColor.back();
				newColor.pop_back();
				float red = newColor.back();
				newColor.pop_back();
				colorData[3*i]=red;
				colorData[3*i+1]=green;
				colorData[3*i+2]=blue;
			}
		}
		glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
		glBufferData(GL_ARRAY_BUFFER, 3*numParticles*sizeof(float), &colorData[0], GL_STREAM_DRAW);

		std::vector<float> radiusData; 
		for(int i=0; i<numParticles; i++){
			float rData = pe.getParticleSize(i);
			radiusData.push_back(rData);
		}
		glBindBuffer(GL_ARRAY_BUFFER, radiusBuffer);
		glBufferData(GL_ARRAY_BUFFER, numParticles*sizeof(float), &radiusData[0], GL_STREAM_DRAW);
		
		}

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute. 
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : colors
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
		glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,(void*)0);

		// 2nd attribute buffer : radi
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, radiusBuffer);
		glVertexAttribPointer(2,1,GL_FLOAT,GL_FALSE,0,(void*)0);

		// Draw the Particles !
		glDrawArrays(GL_POINTS, 0, numParticles);

		string fileString;
		fileString.append("images/");
		stringstream ss;
		ss << renderStep;
		fileString.append(ss.str());
		fileString.append(".tga");
		char *fileName = (char*)fileString.c_str();

		TakeScreenshot(fileName);
		renderStep++;

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		// Swap buffers
		glfwSwapBuffers();
		
		/*
		glDisable(GL_TEXTURE);
		glDisable(GL_TEXTURE_2D);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glPushMatrix();
		glLoadIdentity();
		//glTranslatef(0.0f, 0.0f, -5.0f);
		glRasterPos3f(110, 110, 500);
		glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24, 105 );
		glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24, 97 );
		glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24, 100 );
		glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24, 140 );
		glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24, 130 );
		glPopMatrix();
		glutSwapBuffers();
		glEnable(GL_TEXTURE);
		glEnable(GL_TEXTURE_2D);
		*/

	} // Check if the ESC key was pressed or the window was closed
	while( (glfwGetKey( GLFW_KEY_ESC ) != GLFW_PRESS && glfwGetWindowParam( GLFW_OPENED )) && renderStep <= 9000 );

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &colorBuffer);
	glDeleteBuffers(1, &radiusBuffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

