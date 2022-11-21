// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>

#define _USE_MATH_DEFINES

// Include Camera
#include "Camera\camera.h"

// Include GLEW
#include "dependente\glew\glew.h"

// Include GLFW
#include "dependente\glfw\glfw3.h"

// Include GLM
#include "dependente\glm\glm.hpp"
#include "dependente\glm\gtc\matrix_transform.hpp"
#include "dependente\glm\gtc\type_ptr.hpp"

#include "shader.hpp"
#include "Sphere/Sphere.h"
#include <vector>
#include <math.h>

#include "Ball/ball.h"

//variables
GLFWwindow* window;
const int width = 1024, height = 768;

glm::vec3 cameraPos = glm::vec3(0.0f, 3.2002f, 618.3f);
glm::vec3 cameraDir = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

Camera camera(cameraPos, cameraDir, cameraUp);

// time between current frame and last frame
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting

glm::vec3 lightPos(-3.0f, 0.0f, 2.0f);
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
glm::vec3 fragPos(1.0f, 1.0f, 0.0f);

glm::vec3 viewPos(camera.getCameraPosition());


void processKeyboardInput();

void setFrames()
{
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

}

void setLight(GLuint programID)
{
	unsigned int transformLoc2 = glGetUniformLocation(programID, "lightColor");
	glUniform3fv(transformLoc2, 1, glm::value_ptr(lightColor));


	unsigned int transformLoc3 = glGetUniformLocation(programID, "lightPos");
	glUniform3fv(transformLoc3, 1, glm::value_ptr(lightPos));


	unsigned int transformLoc4 = glGetUniformLocation(programID, "viewPos");
	glUniform3fv(transformLoc4, 1, glm::value_ptr(viewPos));
}

//functions for window set-up

void setWindow(GLFWwindow* window)
{
	// Swap buffers
	glfwSwapBuffers(window);

	// Check for events
	glfwPollEvents();
	processKeyboardInput();

	//enable depth buffer
	glEnable(GL_DEPTH_TEST);

	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


//here we bind the current used shaders

void useShaderPack(GLuint programID, GLuint vao)
{
	glUseProgram(programID);

	glBindVertexArray(vao);
}

//function to create ball motion

void animateBall(Ball allyBall1, 
	glm::mat4 projection, glm::mat4 view, glm::mat4 model, glm::mat4 MVP, GLuint programID)
{

	model = glm::mat4(1.0);

	model = allyBall1.ballThrow(model); //find in ball.cpp

	MVP = projection * view * model;

	allyBall1.drawBall(MVP, projection, view, model, programID); //find in ball.cpp

}


//function to check if there exists any collision between two boules on the plane

int checkCollision(Ball ball1, Ball ball2, float tolerance) //tolerance = collision  position accuracy corrector
{
	float xDif = abs(ball1.getBallPositionX() - ball2.getBallPositionX());
	float zDif = abs(ball1.getBallPositionZ() - ball2.getBallPositionZ());

	if (xDif <= tolerance && zDif <= tolerance)
		return 1; //if collision
	else return 0; //if no collision
}

//set view

void window_callback(GLFWwindow* window, int new_width, int new_height)
{
	glViewport(0, 0, new_width, new_height);
}


int main(void)
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(width, height, "Lighting", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window.");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		glfwTerminate();
		return -1;
	}

	//specify the size of the rendering window
	glViewport(0, 0, width, height);

	// Dark blue background
	glClearColor(0.24f, 0.47f, 0.67f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);


	GLuint VertexArrayIDPlane;
	glGenVertexArrays(1, &VertexArrayIDPlane);
	glBindVertexArray(VertexArrayIDPlane);

	//add dirt plane
	// Load shader
	GLuint programIDPlane = LoadShaders("Shaders/SimpleVertexShaderPlane.vertexshader", "Shaders/SimpleFragmentShaderPlane.fragmentshader");
	//GLuint programIDLight = LoadShaders("Shaders/LightVertexShader.vertexshader", "Shaders/LightFragmentShader.fragmentshader");

	float planeLength = 800.0f;

	GLfloat verticesPlane[] = {
		//we add coordinates for NORMALS 
		//back
		// position			//normals		
		-4.0f, -0.1f, -planeLength, 0.0f,  0.0f, -1.0f,  //0
		4.0f, -0.1f, -planeLength, 0.0f,  0.0f, -1.0f,  //1
		4.0f,  0.1f, -planeLength, 0.0f,  0.0f, -1.0f,  //2
		-4.0f,  0.1f, -planeLength,  0.0f,  0.0f, -1.0f,  //3

		//front
		-4.0f, -0.1f,  planeLength,  0.0f,  0.0f,  1.0f,  //4
		4.0f, -0.1f, planeLength,  0.0f,  0.0f,  1.0f,  //5
		4.0f,  0.1f,  planeLength,  0.0f,  0.0f,  1.0f,  //6
		-4.0f,  0.1f, planeLength,  0.0f,  0.0f,  1.0f, //7

		//left
	   -4.0f,  0.1f,  planeLength, -1.0f,  0.0f,  0.0f, //8
	   -4.0f,  0.1f, -planeLength, -1.0f,  0.0f,  0.0f, //9
	   -4.0f, -0.1f, -planeLength, -1.0f,  0.0f,  0.0f, //10
	   -4.0f, -0.1f,  planeLength, -1.0f,  0.0f,  0.0f, //11

	   // right
	   4.0f,  0.1f,  planeLength,  1.0f,  0.0f,  0.0f, //12
	   4.0f,  0.1f, -planeLength,  1.0f,  0.0f,  0.0f, //13
	   4.0f, -0.1f, -planeLength,  1.0f,  0.0f,  0.0f, //14
	   4.0f, -0.1f,  planeLength,  1.0f,  0.0f,  0.0f, //15

	   //bottom
	  -4.0f, -0.1f, -planeLength,  0.0f, -1.0f,  0.0f, //16
	  4.0f, -0.1f, -planeLength,  0.0f, -1.0f,  0.0f, //17
	  4.0f, -0.1f,  planeLength,  0.0f, -1.0f,  0.0f, //18
	  -4.0f, -0.1f, planeLength,  0.0f, -1.0f,  0.0f,//19

	  //top
	 -4.0f,  0.1f, -planeLength,  0.0f,  1.0f,  0.0f, //20
	 4.0f,  0.1f, -planeLength,  0.0f,  1.0f,  0.0f, //21
	 4.0f,  0.1f,  planeLength,  0.0f,  1.0f,  0.0f, //22
	 -4.0f,  0.1f,  planeLength,  0.0f, 1.0f,  0.0f,  //23
	};

	// A Vertex Array Object (VAO) is an object which contains one or more Vertex Buffer Objects and is designed to store the information for a complete rendered object. 
	GLuint vboPlane, vaoPlane;
	glGenVertexArrays(1, &vaoPlane);
	glGenBuffers(1, &vboPlane);

	glBindVertexArray(vaoPlane);

	glBindBuffer(GL_ARRAY_BUFFER, vboPlane);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesPlane), verticesPlane, GL_STATIC_DRAW);

	// 1rst attribute buffer : vertices position
	glVertexAttribPointer(
		0,                  // attribute 0, must match the layout in the shader.
		3,                  // size of each attribute
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		6 * sizeof(float),     // stride
		0            // array buffer offset
	);
	glEnableVertexAttribArray(0);

	// 2nd attribute buffer : normal coords
	glVertexAttribPointer(
		1,                  // attribute 1, must match the layout in the shader.
		3,                  // size of each attribute
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		6 * sizeof(float),                  // stride
		(void*)(3 * sizeof(float))            // array buffer offset
	);
	glEnableVertexAttribArray(1);



	GLuint VertexArrayIDGrass;
	glGenVertexArrays(1, &VertexArrayIDGrass);
	glBindVertexArray(VertexArrayIDGrass);

	// Load shader
	GLuint programIDGrass = LoadShaders("Shaders/SimpleVertexShaderGrass.vertexshader", "Shaders/SimpleFragmentShaderGrass.fragmentshader");
	//GLuint programIDLight = LoadShaders("Shaders/LightVertexShader.vertexshader", "Shaders/LightFragmentShader.fragmentshader");

	float grassLength = 800.0f;
	 //add grass
	GLfloat verticesGrass[] = {
		//we add coordinates for NORMALS 
		//back
		// position			//normals		
		-4.0f, -0.1f, -grassLength, 0.0f,  0.0f, -1.0f,  //0
		4.0f, -0.1f, -grassLength, 0.0f,  0.0f, -1.0f,  //1
		4.0f,  0.1f, -grassLength, 0.0f,  0.0f, -1.0f,  //2
		-4.0f,  0.1f, -grassLength,  0.0f,  0.0f, -1.0f,  //3

		//front
		-4.0f, -0.1f,  grassLength,  0.0f,  0.0f,  1.0f,  //4
		4.0f, -0.1f, grassLength,  0.0f,  0.0f,  1.0f,  //5
		4.0f,  0.1f,  grassLength,  0.0f,  0.0f,  1.0f,  //6
		-4.0f,  0.1f, grassLength,  0.0f,  0.0f,  1.0f, //7

		//left
	   -4.0f,  0.1f,  grassLength, -1.0f,  0.0f,  0.0f, //8
	   -4.0f,  0.1f, -grassLength, -1.0f,  0.0f,  0.0f, //9
	   -4.0f, -0.1f, -grassLength, -1.0f,  0.0f,  0.0f, //10
	   -4.0f, -0.1f,  grassLength, -1.0f,  0.0f,  0.0f, //11

	   // right
	   4.0f,  0.1f,  grassLength,  1.0f,  0.0f,  0.0f, //12
	   4.0f,  0.1f, -grassLength,  1.0f,  0.0f,  0.0f, //13
	   4.0f, -0.1f, -grassLength,  1.0f,  0.0f,  0.0f, //14
	   4.0f, -0.1f,  grassLength,  1.0f,  0.0f,  0.0f, //15

	   //bottom
	  -4.0f, -0.1f, -grassLength,  0.0f, -1.0f,  0.0f, //16
	  4.0f, -0.1f, -grassLength,  0.0f, -1.0f,  0.0f, //17
	  4.0f, -0.1f,  grassLength,  0.0f, -1.0f,  0.0f, //18
	  -4.0f, -0.1f, grassLength,  0.0f, -1.0f,  0.0f,//19

	  //top
	 -4.0f,  0.1f, -grassLength,  0.0f,  1.0f,  0.0f, //20
	 4.0f,  0.1f, -grassLength,  0.0f,  1.0f,  0.0f, //21
	 4.0f,  0.1f,  grassLength,  0.0f,  1.0f,  0.0f, //22
	 -4.0f,  0.1f,  grassLength,  0.0f, 1.0f,  0.0f,  //23
	};

	// A Vertex Array Object (VAO) is an object which contains one or more Vertex Buffer Objects and is designed to store the information for a complete rendered object. 
	GLuint vboGrass, vaoGrass;
	glGenVertexArrays(1, &vaoGrass);
	glGenBuffers(1, &vboGrass);

	glBindVertexArray(vaoGrass);

	glBindBuffer(GL_ARRAY_BUFFER, vboGrass);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesGrass), verticesGrass, GL_STATIC_DRAW);

	// 1rst attribute buffer : vertices position
	glVertexAttribPointer(
		0,                  // attribute 0, must match the layout in the shader.
		3,                  // size of each attribute
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		6 * sizeof(float),     // stride
		0            // array buffer offset
	);
	glEnableVertexAttribArray(0);

	// 2nd attribute buffer : normal coords
	glVertexAttribPointer(
		1,                  // attribute 1, must match the layout in the shader.
		3,                  // size of each attribute
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		6 * sizeof(float),                  // stride
		(void*)(3 * sizeof(float))            // array buffer offset
	);
	glEnableVertexAttribArray(1);


	GLuint programIDTrunk = LoadShaders("Shaders/SimpleVertexShaderPlaneTrunk.vertexshader", "Shaders/SimpleFragmentShaderPlaneTrunk.fragmentshader");

	float treeHeight = 5.0f;


	GLfloat verticesTrunk[] = {
		///we add coordinates for NORMALS 
		//back
		// position			//normals		
	-0.5f, -treeHeight, -0.5f, 0.0f,  0.0f, -1.0f,  //0
		0.5f, -treeHeight, -0.5f, 0.0f,  0.0f, -1.0f,  //1
		0.5f,  treeHeight, -0.5f, 0.0f,  0.0f, -1.0f,  //2
		-0.5f,  treeHeight, -0.5f,  0.0f,  0.0f, -1.0f,  //3

		//front
		-0.5f, -treeHeight,  0.5f,  0.0f,  0.0f,  1.0f,  //4
		0.5f, -treeHeight,  0.5f,  0.0f,  0.0f,  1.0f,  //5
		0.5f,  treeHeight,  0.5f,  0.0f,  0.0f,  1.0f,  //6
		-0.5f,  treeHeight,  0.5f,  0.0f,  0.0f,  1.0f, //7

		//left
	   -0.5f,  treeHeight,  0.5f, -1.0f,  0.0f,  0.0f, //8
	   -0.5f,  treeHeight, -0.5f, -1.0f,  0.0f,  0.0f, //9
	   -0.5f, -treeHeight, -0.5f, -1.0f,  0.0f,  0.0f, //10
	   -0.5f, -treeHeight,  0.5f, -1.0f,  0.0f,  0.0f, //11

	   // right
	   0.5f,  treeHeight,  0.5f,  1.0f,  0.0f,  0.0f, //12
	   0.5f,  treeHeight, -0.5f,  1.0f,  0.0f,  0.0f, //13
	   0.5f, -treeHeight, -0.5f,  1.0f,  0.0f,  0.0f, //14
	   0.5f, -treeHeight,  0.5f,  1.0f,  0.0f,  0.0f, //15

	   //bottom
	  -0.5f, -treeHeight, -0.5f,  0.0f, -1.0f,  0.0f, //16
	  0.5f, -treeHeight, -0.5f,  0.0f, -1.0f,  0.0f, //17
	  0.5f, -treeHeight,  0.5f,  0.0f, -1.0f,  0.0f, //18
	  -0.5f, -treeHeight,  0.5f,  0.0f, -1.0f,  0.0f,//19

	  //top
	 -0.5f,  treeHeight, -0.5f,  0.0f,  1.0f,  0.0f, //20
	 0.5f,  treeHeight, -0.5f,  0.0f,  1.0f,  0.0f, //21
	 0.5f,  treeHeight,  0.5f,  0.0f,  1.0f,  0.0f, //22
	 -0.5f,  treeHeight,  0.5f,  0.0f, 1.0f,  0.0f,  //23
	};

	glm::vec3 trunkPositions[]
	{
		glm::vec3(8.0f, -0.5f, -500.5f),
		glm::vec3(-8.0f, -0.5f, -785.5f),
		glm::vec3(5.0f, -0.5f, -2400.5f),
		glm::vec3(-7.0f, -0.5f, -4000.5f)
	};


	// A Vertex Array Object (VAO) is an object which contains one or more Vertex Buffer Objects and is designed to store the information for a complete rendered object. 
	GLuint vboTrunk, vaoTrunk;
	glGenVertexArrays(1, &vaoTrunk);
	glGenBuffers(1, &vboTrunk);

	glBindVertexArray(vaoTrunk);

	glBindBuffer(GL_ARRAY_BUFFER, vboTrunk);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesTrunk), verticesTrunk, GL_STATIC_DRAW);

	// 1rst attribute buffer : vertices position
	glVertexAttribPointer(
		0,                  // attribute 0, must match the layout in the shader.
		3,                  // size of each attribute
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		6 * sizeof(float),     // stride
		0            // array buffer offset
	);
	glEnableVertexAttribArray(0);

	// 2nd attribute buffer : normal coords
	glVertexAttribPointer(
		1,                  // attribute 1, must match the layout in the shader.
		3,                  // size of each attribute
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		6 * sizeof(float),                  // stride
		(void*)(3 * sizeof(float))            // array buffer offset
	);
	glEnableVertexAttribArray(1);

	GLuint programIDTree = LoadShaders("Shaders/SimpleVertexShaderTree.vertexshader", "Shaders/SimpleFragmentShaderGrassTree.fragmentshader");

	float treeWidth = 3.0f;
	float treeHeight2 = 4.0f;

	GLfloat verticesTree[] = {
		///we add coordinates for NORMALS 
		//back
		// position			//normals		
		//we add coordinates for NORMALS 
		//back
		// position			//normals		
		-treeWidth, -treeHeight2, -0.5f, 0.0f,  0.0f, -1.0f,  //0
		treeWidth, -treeHeight2, -0.5f, 0.0f,  0.0f, -1.0f,  //1
		treeWidth,  treeHeight2, -0.5f, 0.0f,  0.0f, -1.0f,  //2
		-treeWidth,  treeHeight2, -0.5f,  0.0f,  0.0f, -1.0f,  //3

		//front
		-treeWidth, -treeHeight2,  0.5f,  0.0f,  0.0f,  1.0f,  //4
		treeWidth, -treeHeight2,  0.5f,  0.0f,  0.0f,  1.0f,  //5
		treeWidth,  treeHeight2,  0.5f,  0.0f,  0.0f,  1.0f,  //6
		-treeWidth,  treeHeight2,  0.5f,  0.0f,  0.0f,  1.0f, //7

		//left
	   -treeWidth,  treeHeight2,  0.5f, -1.0f,  0.0f,  0.0f, //8
	   -treeWidth,  treeHeight2, -0.5f, -1.0f,  0.0f,  0.0f, //9
	   -treeWidth, -treeHeight2, -0.5f, -1.0f,  0.0f,  0.0f, //10
	   -treeWidth, -treeHeight2,  0.5f, -1.0f,  0.0f,  0.0f, //11

	   // right
	   treeWidth,  treeHeight2,  0.5f,  1.0f,  0.0f,  0.0f, //12
	   treeWidth,  treeHeight2, -0.5f,  1.0f,  0.0f,  0.0f, //13
	   treeWidth, -treeHeight2, -0.5f,  1.0f,  0.0f,  0.0f, //14
	   treeWidth, -treeHeight2,  0.5f,  1.0f,  0.0f,  0.0f, //15

	   //bottom
	  -treeWidth, -treeHeight2, -0.5f,  0.0f, -1.0f,  0.0f, //16
	  treeWidth, -treeHeight2, -0.5f,  0.0f, -1.0f,  0.0f, //17
	  treeWidth, -treeHeight2,  0.5f,  0.0f, -1.0f,  0.0f, //18
	  -treeWidth, -treeHeight2,  0.5f,  0.0f, -1.0f,  0.0f,//19

	  //top
	 -treeWidth,  treeHeight2, -0.5f,  0.0f,  1.0f,  0.0f, //20
	 treeWidth,  treeHeight2, -0.5f,  0.0f,  1.0f,  0.0f, //21
	 treeWidth,  treeHeight2,  0.5f,  0.0f,  1.0f,  0.0f, //22
	 -treeWidth,  treeHeight2,  0.5f,  0.0f, 1.0f,  0.0f,  //23
	};


	glm::vec3 treePositions[]
	{
		glm::vec3(8.0f, 5.0f, -500.5f),
		glm::vec3(-8.0f, 5.0f, -785.5f),
		glm::vec3(5.0f, 5.0f, -2400.5f),
		glm::vec3(-7.0f, 5.0f, -4050.5f)
	};



	// A Vertex Array Object (VAO) is an object which contains one or more Vertex Buffer Objects and is designed to store the information for a complete rendered object. 
	GLuint vboTree, vaoTree;
	glGenVertexArrays(1, &vaoTree);
	glGenBuffers(1, &vboTree);

	glBindVertexArray(vaoTree);

	glBindBuffer(GL_ARRAY_BUFFER, vboTree);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesTree), verticesTree, GL_STATIC_DRAW);

	// 1rst attribute buffer : vertices position
	glVertexAttribPointer(
		0,                  // attribute 0, must match the layout in the shader.
		3,                  // size of each attribute
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		6 * sizeof(float),     // stride
		0            // array buffer offset
	);
	glEnableVertexAttribArray(0);

	// 2nd attribute buffer : normal coords
	glVertexAttribPointer(
		1,                  // attribute 1, must match the layout in the shader.
		3,                  // size of each attribute
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		6 * sizeof(float),                  // stride
		(void*)(3 * sizeof(float))            // array buffer offset
	);
	glEnableVertexAttribArray(1);


	// add the jack

	GLuint programIDJack = LoadShaders("Shaders/SimpleVertexShaderJack.vertexshader", "Shaders/SimpleFragmentShaderJack.fragmentshader");


	Sphere sphereJack(0.5f, 72, 24);
	

	GLuint vboJack, iboJack, vaoJack;
	glGenVertexArrays(1, &vaoJack);
	glGenBuffers(1, &vboJack);
	glGenBuffers(1, &iboJack);

	glBindVertexArray(vaoJack);

	glBindBuffer(GL_ARRAY_BUFFER, vboJack);           // for vertex data
	glBufferData(GL_ARRAY_BUFFER,                   // target
		sphereJack.getVertexSize() * sizeof(float), // data size, # of bytes
		sphereJack.getVertices(),   // ptr to vertex data
		GL_STATIC_DRAW);                   // usage


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboJack);   // for index data
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,           // target
		sphereJack.getIndexSize() * sizeof(int),             // data size, # of bytes
		sphereJack.getIndices(),               // ptr to index data
		GL_STATIC_DRAW);                   // usage

	glVertexAttribPointer(
		0,                  // attribute 0, must match the layout in the shader.
		3,                  // size of each attribute
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		3 * sizeof(float),                  // stride
		(void*)0            // array buffer offset
	);
	glEnableVertexAttribArray(0);



	// add 3 red boules and 3 blue boules

	GLuint programIDRed1 = LoadShaders("Shaders/SimpleVertexShaderRed1.vertexshader", "Shaders/SimpleFragmentShaderRed1.fragmentshader");


	Sphere sphereRed1(1.0f, 72, 24);


	GLuint vboRed1, iboRed1, vaoRed1;
	glGenVertexArrays(1, &vaoRed1);
	glGenBuffers(1, &vboRed1);
	glGenBuffers(1, &iboRed1);

	glBindVertexArray(vaoRed1);

	glBindBuffer(GL_ARRAY_BUFFER, vboRed1);           // for vertex data
	glBufferData(GL_ARRAY_BUFFER,                   // target
		sphereRed1.getVertexSize() * sizeof(float), // data size, # of bytes
		sphereRed1.getVertices(),   // ptr to vertex data
		GL_STATIC_DRAW);                   // usage


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboRed1);   // for index data
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,           // target
		sphereRed1.getIndexSize() * sizeof(int),             // data size, # of bytes
		sphereRed1.getIndices(),               // ptr to index data
		GL_STATIC_DRAW);                   // usage

	glVertexAttribPointer(
		0,                  // attribute 0, must match the layout in the shader.
		3,                  // size of each attribute
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		3 * sizeof(float),                  // stride
		(void*)0            // array buffer offset
	);
	glEnableVertexAttribArray(0);



	// Load shader
	GLuint programIDBlue1 = LoadShaders("Shaders/SimpleVertexShaderBlue1.vertexshader", "Shaders/SimpleFragmentShaderBlue1.fragmentshader");


	Sphere sphereBlue1(1.0f, 72, 24);


	GLuint vboBlue1, iboBlue1, vaoBlue1;
	glGenVertexArrays(1, &vaoBlue1);
	glGenBuffers(1, &vboBlue1);
	glGenBuffers(1, &iboBlue1);

	glBindVertexArray(vaoBlue1);

	glBindBuffer(GL_ARRAY_BUFFER, vboBlue1);           // for vertex data
	glBufferData(GL_ARRAY_BUFFER,                   // target
		sphereBlue1.getVertexSize() * sizeof(float), // data size, # of bytes
		sphereBlue1.getVertices(),   // ptr to vertex data
		GL_STATIC_DRAW);                   // usage


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboBlue1);   // for index data
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,           // target
		sphereBlue1.getIndexSize() * sizeof(int),             // data size, # of bytes
		sphereBlue1.getIndices(),               // ptr to index data
		GL_STATIC_DRAW);                   // usage

	glVertexAttribPointer(
		0,                  // attribute 0, must match the layout in the shader.
		3,                  // size of each attribute
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		3 * sizeof(float),                  // stride
		(void*)0            // array buffer offset
	);
	glEnableVertexAttribArray(0);




	GLuint programIDRed2 = LoadShaders("Shaders/SimpleVertexShaderRed2.vertexshader", "Shaders/SimpleFragmentShaderRed2.fragmentshader");


	Sphere sphereRed2(1.0f, 72, 24);


	GLuint vboRed2, iboRed2, vaoRed2;
	glGenVertexArrays(1, &vaoRed2);
	glGenBuffers(1, &vboRed2);
	glGenBuffers(1, &iboRed2);

	glBindVertexArray(vaoRed2);

	glBindBuffer(GL_ARRAY_BUFFER, vboRed2);           // for vertex data
	glBufferData(GL_ARRAY_BUFFER,                   // target
		sphereRed2.getVertexSize() * sizeof(float), // data size, # of bytes
		sphereRed2.getVertices(),   // ptr to vertex data
		GL_STATIC_DRAW);                   // usage


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboRed2);   // for index data
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,           // target
		sphereRed2.getIndexSize() * sizeof(int),             // data size, # of bytes
		sphereRed2.getIndices(),               // ptr to index data
		GL_STATIC_DRAW);                   // usage

	glVertexAttribPointer(
		0,                  // attribute 0, must match the layout in the shader.
		3,                  // size of each attribute
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		3 * sizeof(float),                  // stride
		(void*)0            // array buffer offset
	);
	glEnableVertexAttribArray(0);




	GLuint programIDBlue2 = LoadShaders("Shaders/SimpleVertexShaderBlue2.vertexshader", "Shaders/SimpleFragmentShaderBlue2.fragmentshader");


	Sphere sphereBlue2(1.0f, 72, 24);


	GLuint vboBlue2, iboBlue2, vaoBlue2;
	glGenVertexArrays(1, &vaoBlue2);
	glGenBuffers(1, &vboBlue2);
	glGenBuffers(1, &iboBlue2);

	glBindVertexArray(vaoBlue2);

	glBindBuffer(GL_ARRAY_BUFFER, vboBlue2);           // for vertex data
	glBufferData(GL_ARRAY_BUFFER,                   // target
		sphereBlue2.getVertexSize() * sizeof(float), // data size, # of bytes
		sphereBlue2.getVertices(),   // ptr to vertex data
		GL_STATIC_DRAW);                   // usage


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboBlue2);   // for index data
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,           // target
		sphereBlue2.getIndexSize() * sizeof(int),             // data size, # of bytes
		sphereBlue2.getIndices(),               // ptr to index data
		GL_STATIC_DRAW);                   // usage

	glVertexAttribPointer(
		0,                  // attribute 0, must match the layout in the shader.
		3,                  // size of each attribute
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		3 * sizeof(float),                  // stride
		(void*)0            // array buffer offset
	);
	glEnableVertexAttribArray(0);




	GLuint programIDRed3 = LoadShaders("Shaders/SimpleVertexShaderRed3.vertexshader", "Shaders/SimpleFragmentShaderRed3.fragmentshader");


	Sphere sphereRed3(1.0f, 72, 24);


	GLuint vboRed3, iboRed3, vaoRed3;
	glGenVertexArrays(1, &vaoRed3);
	glGenBuffers(1, &vboRed3);
	glGenBuffers(1, &iboRed3);

	glBindVertexArray(vaoRed3);

	glBindBuffer(GL_ARRAY_BUFFER, vboRed3);           // for vertex data
	glBufferData(GL_ARRAY_BUFFER,                   // target
		sphereRed3.getVertexSize() * sizeof(float), // data size, # of bytes
		sphereRed3.getVertices(),   // ptr to vertex data
		GL_STATIC_DRAW);                   // usage


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboRed3);   // for index data
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,           // target
		sphereRed3.getIndexSize() * sizeof(int),             // data size, # of bytes
		sphereRed3.getIndices(),               // ptr to index data
		GL_STATIC_DRAW);                   // usage

	glVertexAttribPointer(
		0,                  // attribute 0, must match the layout in the shader.
		3,                  // size of each attribute
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		3 * sizeof(float),                  // stride
		(void*)0            // array buffer offset
	);
	glEnableVertexAttribArray(0);



	GLuint programIDBlue3 = LoadShaders("Shaders/SimpleVertexShaderBlue3.vertexshader", "Shaders/SimpleFragmentShaderBlue3.fragmentshader");


	Sphere sphereBlue3(1.0f, 72, 24);


	GLuint vboBlue3, iboBlue3, vaoBlue3;
	glGenVertexArrays(1, &vaoBlue3);
	glGenBuffers(1, &vboBlue3);
	glGenBuffers(1, &iboBlue3);

	glBindVertexArray(vaoBlue3);

	glBindBuffer(GL_ARRAY_BUFFER, vboBlue3);           // for vertex data
	glBufferData(GL_ARRAY_BUFFER,                   // target
		sphereBlue3.getVertexSize() * sizeof(float), // data size, # of bytes
		sphereBlue3.getVertices(),   // ptr to vertex data
		GL_STATIC_DRAW);                   // usage


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboBlue3);   // for index data
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,           // target
		sphereBlue3.getIndexSize() * sizeof(int),             // data size, # of bytes
		sphereBlue3.getIndices(),               // ptr to index data
		GL_STATIC_DRAW);                   // usage

	glVertexAttribPointer(
		0,                  // attribute 0, must match the layout in the shader.
		3,                  // size of each attribute
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		3 * sizeof(float),                  // stride
		(void*)0            // array buffer offset
	);
	glEnableVertexAttribArray(0);



	glfwSetFramebufferSizeCallback(window, window_callback);


	//item setup

	Ball jack = Ball(glm::vec3(0.0f, -0.5f, -1300.0f), 0, 0, 0, 0, 0, 0,0, sphereJack);

	Ball red1 = Ball(glm::vec3(0.0f, 0.0f, 0.0f), 0, 0, 0, 0, 0, 0, 0, sphereRed1);
	Ball blue1 = Ball(glm::vec3(0.0f, 0.0f, 0.0f), 0, 0, 0, 0, 0, 0, 0, sphereBlue1);

	Ball red2 = Ball(glm::vec3(0.0f, 0.0f, 0.0f), 0, 0, 0, 0, 0, 0, 0, sphereRed2);
	Ball blue2 = Ball(glm::vec3(0.0f, 0.0f, 0.0f), 0, 0, 0, 0, 0, 0, 0, sphereBlue2);

	Ball red3 = Ball(glm::vec3(0.0f, 0.0f, 0.0f), 0, 0, 0, 0, 0, 0, 0, sphereRed3);
	Ball blue3 = Ball(glm::vec3(0.0f, 0.0f, 0.0f), 0, 0, 0, 0, 0, 0, 0, sphereBlue3);

	Ball boules[6] = { red1, blue1, red2, blue2, red3, blue3 };
	GLuint vao[6] = { vaoRed1, vaoBlue1, vaoRed2, vaoBlue2, vaoRed3, vaoBlue3 };
	GLuint programIDBoules[6] = { programIDRed1, programIDBlue1, programIDRed2, programIDBlue2, 
		programIDRed3, programIDBlue3 };

	float redScores[3] = { 0,0,0 };
	float blueScores[3] = { 0,0,0 };

	int gameRounds[4] = { 0,1,2,3 };

	int currentRound = 0;

	//int rewind;

	std::cout << "|----------------------------------------------------|" << std::endl;
	std::cout << "|                                                    |" << std::endl;
	std::cout << "|                   GAME CONTROLS                    |" << std::endl;
	std::cout << "|                                                    |" << std::endl;
	std::cout << "|----------------------------------------------------|" << std::endl;

	std::cout << "|-------------------------|" << "|-------------------------|" << std::endl;
	std::cout << "|                         |" << "|                         |" << std::endl;
	std::cout << "|    PRESS B TO SPAWN     |" << "|    PRESS P TO THROW     |" << std::endl;
	std::cout << "|                         |" << "|                         |" << std::endl;
	std::cout << "|-------------------------|" << "|-------------------------|" << std::endl;
	std::cout << "|-------------------------|" << "|-------------------------|" << std::endl;
	std::cout << "|                         |" << "|                         |" << std::endl;
	std::cout << "|    PRESS O TO RESET     |" << "| PRESS N FOR NEXT ROUND  |" << std::endl;
	std::cout << "|                         |" << "|                         |" << std::endl;
	std::cout << "|-------------------------|" << "|-------------------------|" << std::endl;

	std::cout << "|----------------------------------------------------|" << std::endl;
	std::cout << "|                                                    |" << std::endl;
	std::cout << "|                     GAME RULES                     |" << std::endl;
	std::cout << "|                                                    |" << std::endl;
	std::cout << "|----------------------------------------------------|" << std::endl;

	std::cout << "|----------------------------------------------------|" << std::endl;
	std::cout << "|  1. THE LONGER YOU HOLD P, THE HARDER YOU THROW    |" << std::endl;
	std::cout << "|  2. IF THE JACK IS HIT, THE ROUND IS RESET         |" << std::endl;
	std::cout << "|  3. THE TEAM WITH THE SHORTEST DISTANCE BETWEEN    |" << std::endl;
	std::cout << "|  THE BOULES AND THE JACK WINS.                     |" << std::endl;
	std::cout << "|----------------------------------------------------|" << std::endl;
	std::cout << "| !!! IF A BOULE FALLS OFF THE END OF THE PLANE,     |" << std::endl;
	std::cout << "| ITS DISTANCE TO THE JACK IS COMPUTED FROM (0,0,0)  |" << std::endl;
	std::cout << "|----------------------------------------------------|" << std::endl;




	// Check if the window was closed
	while (!glfwWindowShouldClose(window) && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
	{

		setWindow(window);

		setFrames();

		//models for each object

		glm::mat4 modelJack, modelPlane, modelLight, modelGrass;
		glm::mat4 modelRed1, modelBlue1;
		glm::mat4 modelRed2, modelBlue2, modelRed3, modelBlue3;

		glm::mat4 modelTrunk, modelTree;

		glm::mat4 modelBoules[6] = { modelRed1, modelBlue1, modelRed2, modelBlue2, modelRed3, modelBlue3 };

		//Adding camera
		glm::mat4 view = glm::lookAt(camera.getCameraPosition(),
			camera.getCameraPosition() + camera.getCameraViewDirection(), camera.getCameraUp());

		glm::mat4 projection;
		projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 10000.0f);

		//draw the plane;


		glm::mat4 MVP;

		useShaderPack(programIDGrass, vaoGrass);


		modelGrass = glm::translate(modelGrass, glm::vec3(0.0f, -2.5f, -795.0f));

		modelGrass = glm::scale(modelGrass, glm::vec3(10.0f));

		MVP = projection * view * modelGrass;

		unsigned int transformLocGrass0 = glGetUniformLocation(programIDGrass, "model");
		glUniformMatrix4fv(transformLocGrass0, 1, GL_FALSE, glm::value_ptr(modelGrass));

		unsigned int transformGrass1 = glGetUniformLocation(programIDGrass, "transform");
		glUniformMatrix4fv(transformGrass1, 1, GL_FALSE, glm::value_ptr(MVP));

		glDrawArrays(GL_QUADS, 0, 24);




		useShaderPack(programIDPlane, vaoPlane);


		modelPlane = glm::translate(modelPlane, glm::vec3(0.0f, -1.1f, -795.0f));

		MVP = projection * view * modelPlane;

		unsigned int transformLoc100 = glGetUniformLocation(programIDPlane, "model");
		glUniformMatrix4fv(transformLoc100, 1, GL_FALSE, glm::value_ptr(modelPlane));

		unsigned int transformLoc111 = glGetUniformLocation(programIDPlane, "transform");
		glUniformMatrix4fv(transformLoc111, 1, GL_FALSE, glm::value_ptr(MVP));

		glDrawArrays(GL_QUADS, 0, 24);


		//tree trunks

		useShaderPack(programIDTrunk, vaoTrunk);

		for(int i = 0; i < 4; i++)
		{
			
			modelTrunk = glm::mat4(1.0f);

			modelTrunk = glm::translate(modelTrunk, trunkPositions[i]);


			MVP = projection * view * modelTrunk;

			unsigned int transformLocT0 = glGetUniformLocation(programIDTrunk, "model");
			glUniformMatrix4fv(transformLocT0, 1, GL_FALSE, glm::value_ptr(modelTrunk));

			unsigned int transformLocT1 = glGetUniformLocation(programIDTrunk, "transform");
			glUniformMatrix4fv(transformLocT1, 1, GL_FALSE, glm::value_ptr(MVP));

			glDrawArrays(GL_QUADS, 0, 24);
		}



		useShaderPack(programIDTree, vaoTree);

		//trees
		for (int i = 0; i < 4; i++)
		{
			modelTree = glm::mat4(1.0f);

			modelTree = glm::translate(modelTree, treePositions[i]);

			MVP = projection * view * modelTree;

			unsigned int transformLocT2 = glGetUniformLocation(programIDTree, "model");
			glUniformMatrix4fv(transformLocT2, 1, GL_FALSE, glm::value_ptr(modelTree));

			unsigned int transformLocT3 = glGetUniformLocation(programIDTree, "transform");
			glUniformMatrix4fv(transformLocT3, 1, GL_FALSE, glm::value_ptr(MVP));


			glDrawArrays(GL_QUADS, 0, 24);
		}


		//draw the Jack


		useShaderPack(programIDJack, vaoJack);

		modelJack = jack.ballCalibrate(modelJack);

		MVP = projection * view * modelJack;

		jack.drawBall(MVP, projection, view, modelJack, programIDJack);

		//setLight(programIDJack);


		//add reset button on O
		if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
		{

			for (int j = 0; j < 6; j++)
				boules[j].ballReset();
			currentRound = 0;

			for (int k = 0; k < 3; k++)
			{
				redScores[k] = 0;
				blueScores[k] = 0;
			}

		}


		//start round

		if (currentRound <= 2)
		{

			for(int i = 0; i < 6; i++)
			{
				useShaderPack(programIDBoules[i], vao[i]);

				//spawn the ball on B

				if (!boules[i].getBallSpawnTrue() && !boules[i].getBallDisqualified() && boules[i - 1].getBallDisqualified()
					&& glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
				{
					
					boules[i].setBallSpawnTrue(1);
					//std::cout << "ROUND " << currentRound + 1 << std::endl << "BOULE " << i + 1 << std::endl;

					std::cout << "|-------------------------|" << std::endl;
					std::cout << "|                         |" << std::endl;
					std::cout << "|     ROUND " << currentRound + 1 << ": BOULE " << i + 1 << "    |" << std::endl;
					std::cout << "|                         |" << std::endl;
					std::cout << "|-------------------------|" << std::endl;

				}

				//check if the ball was spawned

				if (boules[i].getBallSpawnTrue())
				{
					
					//while in calibration mode, set left/right boule pos

					if (!boules[i].getBallThrowTrue() && !boules[i].getBallCalibrateTrue()
						&& glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
					{
						if (boules[i].getBallPositionX() > -1.7f)
							boules[i].setBallPositionX(boules[i].getBallPositionX() - 0.0009f);
						boules[i].ballCalibrate(modelBoules[i]);

					}

					
					if (!boules[i].getBallThrowTrue() && !boules[i].getBallCalibrateTrue()
						&& glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
					{
						if (boules[i].getBallPositionX() < 1.7f)
							boules[i].setBallPositionX(boules[i].getBallPositionX() + 0.0009f);
						boules[i].ballCalibrate(modelBoules[i]);
					}

					
					if (!boules[i].getBallThrowTrue() && !boules[i].getBallCalibrateTrue())
					{

						modelBoules[i] = boules[i].ballCalibrate(modelBoules[i]);
					}

					//throw on P
					
					if (!boules[i].getBallThrowTrue())
					{
						if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
						{
							boules[i].setBallThrowSpeed(boules[i].getBallThrowSpeed() + 2);

						}

						//compute by scaling with press length and physics formula v^2=2ugd;

						if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE && boules[i].getBallThrowSpeed() != 0)
						{
							boules[i].setBallThrowDist(pow((boules[i].getBallThrowSpeed()/10), 2) / (2 * 0.5 * 9.81));

							boules[i].setBallThrowTrue(1);
							boules[i].setBallCalibrateTrue(1);

							boules[i].setBallPositionX(boules[i].getBallPositionX() * 2.0f);
						}

					}

					//roll until disqualified or stop occurs

					if (boules[i].getBallThrowTrue() && boules[i].getBallCalibrateTrue())
					{
						
						
						if (boules[i].getBallPositionZ() >= -boules[i].getBallThrowDist())
							boules[i].setBallPositionZ(boules[i].getBallPositionZ() - boules[i].getBallThrowDist() * 0.1 / boules[i].getBallThrowSpeed());
						else boules[i].setBallDisqualified(1);
				
						//end on hitting the jack

						if (checkCollision(boules[i], jack, 1.0f))
						{
							boules[i].setBallThrowDist(-boules[i].getBallPositionZ() - 2.0f);

							//GAME RESET

							for (int j = 0; j < 6; j++)
								boules[j].ballReset();
						}

						//check collision with all stationary boules on the plane

						for (int h = 0; h < i; h++)
						{
							if (boules[h].getBallPosition() != glm::vec3(0.0f, 0.0f, 0.0f))
								if (checkCollision(boules[i], boules[h], 2.0f))
								{
									boules[i].setBallThrowDist(-boules[i].getBallPositionZ() - 2.0f);

									float dist = boules[h].getBallPositionZ() - boules[i].getBallThrowSpeed()/20;

									boules[h].setBallThrowDist(-dist);


									//COLLISION DETECTED!

									

									if(boules[h].getBallPositionZ() <= dist)
									{

										useShaderPack(programIDBoules[h], vao[h]);

										boules[h].setBallThrowSpeed(boules[i].getBallThrowSpeed() / 20);

										boules[h].ballThrow(modelBoules[h]);

										boules[h].drawBall(MVP, projection, view, modelBoules[h], programIDBoules[h]);

									}
								}

							//block if stop occcurs

							boules[h].ballBlock();

						}

						//maintain game animation each frame

						animateBall(boules[i], projection, view, modelBoules[i], MVP, programIDBoules[i]);

					}
					else
					{
						if (!boules[i].getBallDisqualified())
						{
							modelBoules[i] = glm::translate(modelBoules[i], boules[i].getBallPosition());

							MVP = projection * view * modelBoules[i];

							boules[i].drawBall(MVP, projection, view, modelBoules[i], programIDBoules[i]);
						}
					}

				}

			
				//setLight(programIDBoules[i]);

				
				boules[i].ballBlock();

		

			}

		}

		//compute score when all boules have been disqualified == stopped moving and no longer eligible for spawn

		if (boules[0].getBallDisqualified() && boules[1].getBallDisqualified() && boules[2].getBallDisqualified()
			&& boules[3].getBallDisqualified() && boules[4].getBallDisqualified() && boules[5].getBallDisqualified())
		{

			float allScores[6] = { 0,0,0,0,0,0 };

			for (int j = 0; j < 6; j++)
			{
				//math formula for dist between 2 points in a cartesian plane (we use xOz here)
				float dist = sqrt(pow(jack.getBallPositionX() - boules[j].getBallPositionX(), 2) +
					pow(jack.getBallPositionZ() - boules[j].getBallPositionZ(), 2));

				if (dist == 0)
					dist += 0.001;
				
				allScores[j] = float(1 / dist) * 10000;


			}
			//score update
			for(int j = 0; j < 5; j += 2)
			{
				if (allScores[j] > allScores[j + 1])
				{
					allScores[j] = 1;
					allScores[j + 1] = 0;
				}

				if (allScores[j] < allScores[j + 1])
				{
					allScores[j] = 0;
					allScores[j + 1] = 1;
				}

				if (allScores[j] == allScores[j + 1])
				{
					allScores[j] = 1;
					allScores[j + 1] = 1;
				}

			}

			redScores[currentRound] = allScores[0] + allScores[2] + allScores[4];

			//std::cout << "RED: " << redScores[currentRound] << std::endl;
			blueScores[currentRound] = allScores[1] + allScores[3] + allScores[5];
			//std::cout << "BLUE: " << blueScores[currentRound] << std::endl;

			std::cout << "|-------------------------|" << "|-------------------------|" << std::endl;
			std::cout << "|                         |" << "|                         |" << std::endl;
			std::cout << "| TEAM RED ROUND SCORE : " << redScores[currentRound] << "||TEAM BLUE ROUND SCORE : " << blueScores[currentRound] << "|" << std::endl;
			std::cout << "|                         |" << "|                         |" << std::endl;
			std::cout << "|-------------------------|" << "|-------------------------|" << std::endl;
			
			//reset for next round
			for (int j = 0; j < 6; j++)
				boules[j].ballReset();

			if (currentRound == 2)
			{
				float redScore = redScores[0] + redScores[1] + redScores[2];
				float blueScore = blueScores[0] + blueScores[1] + blueScores[2];

				std::cout << "|-------------------------|" << "|-------------------------|" << std::endl;
				std::cout << "|                         |" << "|                         |" << std::endl;
				std::cout << "|TEAM RED FINAL SCORE : " << redScore<<" ||TEAM BLUE FINAL SCORE : "<< blueScore<< "|"<< std::endl;
				std::cout << "|                         |" << "|                         |" << std::endl;
				std::cout << "|-------------------------|" << "|-------------------------|" << std::endl;

				if((int)blueScore > (int)redScore)
				{
					std::cout << "|----------------------------------------------------|" << std::endl;
					std::cout << "|                                                    |" << std::endl;
					std::cout << "|                  WINNER TEAM BLUE                  |" << std::endl;
					std::cout << "|                                                    |" << std::endl;
					std::cout << "|----------------------------------------------------|" << std::endl;
				}

				if ((int)blueScore < (int)redScore)
				{
					std::cout << "|----------------------------------------------------|" << std::endl;
					std::cout << "|                                                    |" << std::endl;
					std::cout << "|                  WINNER TEAM RED                   |" << std::endl;
					std::cout << "|                                                    |" << std::endl;
					std::cout << "|----------------------------------------------------|" << std::endl;
				}

				if ((int)blueScore == (int)redScore)
				{
					std::cout << "|----------------------------------------------------|" << std::endl;
					std::cout << "|                                                    |" << std::endl;
					std::cout << "|                     WINNER TIE                     |" << std::endl;
					std::cout << "|                                                    |" << std::endl;
					std::cout << "|----------------------------------------------------|" << std::endl;
				}
			}

			//inc round
			currentRound = gameRounds[currentRound + 1];
			

		}

	}

	// Cleanup VBO
	glDeleteBuffers(1, &vboJack);
	glDeleteBuffers(1, &iboJack);
	glDeleteProgram(programIDJack);

	glDeleteBuffers(1, &vboRed1);
	glDeleteBuffers(1, &iboRed1);
	glDeleteProgram(programIDRed1);

	glDeleteBuffers(1, &vboRed2);
	glDeleteBuffers(1, &iboRed2);
	glDeleteProgram(programIDRed2);

	glDeleteBuffers(1, &vboRed3);
	glDeleteBuffers(1, &iboRed3);
	glDeleteProgram(programIDRed3);

	glDeleteBuffers(1, &vboBlue1);
	glDeleteBuffers(1, &iboBlue1);
	glDeleteProgram(programIDBlue1);

	glDeleteBuffers(1, &vboBlue2);
	glDeleteBuffers(1, &iboBlue2);
	glDeleteProgram(programIDBlue2);

	glDeleteBuffers(1, &vboBlue3);
	glDeleteBuffers(1, &iboBlue3);
	glDeleteProgram(programIDBlue3);



	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

//Process input for camera rotation
void processKeyboardInput()
{
	float cameraSpeed = deltaTime * 10;


	if (glfwGetKey(window, GLFW_KEY_W))
	{
		camera.keyboardMoveFront(cameraSpeed);
	}
	if (glfwGetKey(window, GLFW_KEY_S))
	{
		camera.keyboardMoveBack(cameraSpeed);
	}

	if (glfwGetKey(window, GLFW_KEY_R))
	{
		camera.keyboardMoveUp(cameraSpeed);
	}
	if (glfwGetKey(window, GLFW_KEY_F))
	{
		camera.keyboardMoveDown(cameraSpeed);
	}

	if (glfwGetKey(window, GLFW_KEY_UP))
	{
		camera.rotateOx(cameraSpeed / 2);
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN))
	{
		camera.rotateOx(-cameraSpeed / 2);
	}

}
