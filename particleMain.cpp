#define _WIN32_WINNT 0x0502
// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GL/glfw.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "shader.hpp"
#include "controls.hpp"

#include "ParticleEngine.h"
#include "ParticleEngineBuilder.h"
#include "ColorPicker.h"

#include <vector>

int main( void )
{
	int colorOption = 0;
	ColorPicker cp;
	vector<float> color = cp.getColor(0,0,1); //white
	color = cp.getColor(0,0,0); //black
	color = cp.getColor(0,1,1); //Neon red
	color = cp.getColor(120,1,1); //Neon green
	color = cp.getColor(240,1,1); //Neon Blue

	_SYSTEM_INFO sysinfo;
	GetNativeSystemInfo( &sysinfo );
	int numCPU = sysinfo.dwNumberOfProcessors;

	int threads = 1;//numCPU;
	int particles = numCPU*100;
	ParticleEngine pe = (new ParticleEngineBuilder())->
						setGravitationalConstant(30.0f)->
						setMinimumRadius(5.0f)->
						setBlackHoleMass(0.0f)->
						setBlackHoleRadius(5.0f)->
						setDisappearingRadius(10000.0f)->
						setCollisions(true)->
						setNumberOfThreads(threads)->
						setMinSpawnRadius(70)->
						setmaxSpawnRadius(270)->
						setspawnVelocity(12.0f)->
						setmaxZSpawnDistance(10)->
						setnumberOfParticles(particles)->
						setGravityCutOff(.0001f)->
						setIntegrator(4)->
						Build();
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		return -1;
	}

	glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	if( !glfwOpenWindow( 1024, 768, 0,0,0,0, 32,0, GLFW_WINDOW ) )
	{
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
	glfwSetMousePos(1024/2, 768/2);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Enable depth test
	//glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	//glDepthFunc(GL_LESS);
	
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "Particle.vertexshader", "Particle.fragmentshader" );

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	static const GLfloat g_vertex_buffer_data[] = { 
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 0.0f,  1.0f, 0.0f,
	};
	static const GLushort g_element_buffer_data[] = { 0, 1, 2 };

	vector<float> datav = pe.getPositions();
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, 3*pe.getNumberOfParticles()*sizeof(float), &datav[0], GL_STREAM_DRAW);

	GLuint colorBuffer;
	glGenBuffers(1, &colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	
	 double lastTime = glfwGetTime();
	
	do{
		double currentTime = glfwGetTime();
		pe.step((float)(currentTime-lastTime));
		lastTime = currentTime;
	
		int numParticles = pe.getNumberOfParticles();

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
		if (glfwGetKey( '1' ) == GLFW_PRESS){
			pe.setIntegrator(1);
		}
		// Set Integrator Euler
		if (glfwGetKey( '2' ) == GLFW_PRESS){
			pe.setIntegrator(2);
		}
		// Set Integrator Euler
		if (glfwGetKey( '3' ) == GLFW_PRESS){
			pe.setIntegrator(3);
		}
		// Set Integrator Euler
		if (glfwGetKey( '4' ) == GLFW_PRESS){
			pe.setIntegrator(4);
		}

		// Set Color Acceleration
		if (glfwGetKey( 'A' ) == GLFW_PRESS){
			colorOption = 1;
		}
		// Set Color Velocity
		if (glfwGetKey( 'V' ) == GLFW_PRESS){
			colorOption = 2;
		}
		// Set Color Mass
		if (glfwGetKey( 'M' ) == GLFW_PRESS){
			colorOption = 3;
		}
		// Set Color None All particles White
		if (glfwGetKey( 'N' ) == GLFW_PRESS){
			colorOption = 0;
		}

		vector<float> colorData(3*numParticles);

		ColorPicker cp;
		if(colorOption == 0){//No Coloring
			for(int i=0; i<numParticles; i++){
				vector<float> newColor = cp.getColor(0,0,1);
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
			for(int i=0;i<numParticles;i++)
			{
				if(pe.getMass(i) > maxMass)
				{
					maxMass = pe.getMass(i);
				}
			}

			for(int i=0; i<numParticles; i++){
				float mass = pe.getMass(i);
				vector<float> newColor = cp.getColor(120,mass/maxMass,1);
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


		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);


		// 2nd attribute buffer : colors
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
		glVertexAttribPointer(
			1,                  
			3,                  
			GL_FLOAT,           
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		

		// Draw the Particles !
		glDrawArrays(GL_POINTS, 0, numParticles);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		// Swap buffers
		glfwSwapBuffers();
		
	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey( GLFW_KEY_ESC ) != GLFW_PRESS &&
		   glfwGetWindowParam( GLFW_OPENED ) );

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &colorBuffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

