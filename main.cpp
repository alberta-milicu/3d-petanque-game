// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>

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

#include "Ball/ball.h"

//variables
GLFWwindow* window;
const int width = 1024, height = 768;

glm::vec3 cameraPos = glm::vec3(0.0f, 3.2002f, 545.3f);
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

void useShaderPack(GLuint programID, GLuint vao)
{
	glUseProgram(programID);

	glBindVertexArray(vao);
}

void animateBall(Ball allyBall1, 
	glm::mat4 projection, glm::mat4 view, glm::mat4 model, glm::mat4 MVP, GLuint programID)
{

	model = glm::mat4(1.0);

	model = allyBall1.ballThrow(model);

	MVP = projection * view * model;

	allyBall1.drawBall(MVP, projection, view, model, programID);

}

int checkCollision(Ball ball1, Ball ball2, float tolerance)
{
	float xDif = abs(ball1.getBallPositionX() - ball2.getBallPositionX());
	float zDif = abs(ball1.getBallPositionZ() - ball2.getBallPositionZ());

	if (xDif <= tolerance && zDif <= tolerance)
		return 1;
	else return 0;
}



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
	glClearColor(0.0f, 0.2f, 0.2f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	GLuint VertexArrayIDPlane;
	glGenVertexArrays(1, &VertexArrayIDPlane);
	glBindVertexArray(VertexArrayIDPlane);

	// Load shader
	GLuint programIDPlane = LoadShaders("Shaders/SimpleVertexShaderPlane.vertexshader", "Shaders/SimpleFragmentShaderPlane.fragmentshader");
	GLuint programIDLight = LoadShaders("Shaders/LightVertexShader.vertexshader", "Shaders/LightFragmentShader.fragmentshader");

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


	GLuint VertexArrayIDJack;
	glGenVertexArrays(1, &VertexArrayIDJack);
	glBindVertexArray(VertexArrayIDJack);

	// Load shader
	GLuint programIDJack = LoadShaders("Shaders/SimpleVertexShaderJack.vertexshader", "Shaders/SimpleFragmentShaderJack.fragmentshader");
	//GLuint programIDLight = LoadShaders("LightVertexShader.vertexshader", "LightFragmentShader.fragmentshader");

	GLfloat verticesJack[] = {
		//we add coordinates for NORMALS 
		//back
		// position			//normals		
		-0.5f, -0.5f, -0.5f, 0.0f,  0.0f, -1.0f,  //0
		0.5f, -0.5f, -0.5f, 0.0f,  0.0f, -1.0f,  //1
		0.5f,  0.5f, -0.5f, 0.0f,  0.0f, -1.0f,  //2
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  //3

		//front
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  //4
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  //5
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  //6
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, //7

		//left
	   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, //8
	   -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, //9
	   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, //10
	   -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, //11

	   // right
	   0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, //12
	   0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, //13
	   0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, //14
	   0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, //15

	   //bottom
	  -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, //16
	  0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, //17
	  0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, //18
	  -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,//19

	  //top
	 -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, //20
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, //21
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, //22
	 -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  //23
	};

	// A Vertex Array Object (VAO) is an object which contains one or more Vertex Buffer Objects and is designed to store the information for a complete rendered object. 
	GLuint vboJack, vaoJack;
	glGenVertexArrays(1, &vaoJack);
	glGenBuffers(1, &vboJack);

	glBindVertexArray(vaoJack);

	glBindBuffer(GL_ARRAY_BUFFER, vboJack);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesJack), verticesJack, GL_STATIC_DRAW);

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

	GLuint VertexArrayIDRed1;
	glGenVertexArrays(1, &VertexArrayIDRed1);
	glBindVertexArray(VertexArrayIDRed1);

	// Load shader
	GLuint programIDRed1 = LoadShaders("Shaders/SimpleVertexShaderRed1.vertexshader", "Shaders/SimpleFragmentShaderRed1.fragmentshader");
	//GLuint programIDLight = LoadShaders("LightVertexShader.vertexshader", "LightFragmentShader.fragmentshader");

	GLfloat verticesRed1[] = {
		//we add coordinates for NORMALS 
		//back
		// position			//normals		
		-0.5f, -0.5f, -0.5f, 0.0f,  0.0f, -1.0f,  //0
		0.5f, -0.5f, -0.5f, 0.0f,  0.0f, -1.0f,  //1
		0.5f,  0.5f, -0.5f, 0.0f,  0.0f, -1.0f,  //2
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  //3

		//front
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  //4
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  //5
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  //6
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, //7

		//left
	   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, //8
	   -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, //9
	   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, //10
	   -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, //11

	   // right
	   0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, //12
	   0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, //13
	   0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, //14
	   0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, //15

	   //bottom
	  -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, //16
	  0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, //17
	  0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, //18
	  -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,//19

	  //top
	 -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, //20
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, //21
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, //22
	 -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  //23
	};

	// A Vertex Array Object (VAO) is an object which contains one or more Vertex Buffer Objects and is designed to store the information for a complete rendered object. 
	GLuint vboRed1, vaoRed1;
	glGenVertexArrays(1, &vaoRed1);
	glGenBuffers(1, &vboRed1);

	glBindVertexArray(vaoRed1);

	glBindBuffer(GL_ARRAY_BUFFER, vboRed1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesRed1), verticesRed1, GL_STATIC_DRAW);

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

	GLuint VertexArrayIDBlue1;
	glGenVertexArrays(1, &VertexArrayIDBlue1);
	glBindVertexArray(VertexArrayIDBlue1);

	// Load shader
	GLuint programIDBlue1 = LoadShaders("Shaders/SimpleVertexShaderBlue1.vertexshader", "Shaders/SimpleFragmentShaderBlue1.fragmentshader");
	//GLuint programIDLight = LoadShaders("LightVertexShader.vertexshader", "LightFragmentShader.fragmentshader");

	GLfloat verticesBlue1[] = {
		//we add coordinates for NORMALS 
		//back
		// position			//normals		
		-0.5f, -0.5f, -0.5f, 0.0f,  0.0f, -1.0f,  //0
		0.5f, -0.5f, -0.5f, 0.0f,  0.0f, -1.0f,  //1
		0.5f,  0.5f, -0.5f, 0.0f,  0.0f, -1.0f,  //2
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  //3

		//front
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  //4
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  //5
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  //6
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, //7

		//left
	   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, //8
	   -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, //9
	   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, //10
	   -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, //11

	   // right
	   0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, //12
	   0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, //13
	   0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, //14
	   0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, //15

	   //bottom
	  -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, //16
	  0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, //17
	  0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, //18
	  -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,//19

	  //top
	 -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, //20
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, //21
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, //22
	 -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  //23
	};

	// A Vertex Array Object (VAO) is an object which contains one or more Vertex Buffer Objects and is designed to store the information for a complete rendered object. 
	GLuint vboBlue1, vaoBlue1;
	glGenVertexArrays(1, &vaoBlue1);
	glGenBuffers(1, &vboBlue1);

	glBindVertexArray(vaoBlue1);

	glBindBuffer(GL_ARRAY_BUFFER, vboBlue1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesBlue1), verticesBlue1, GL_STATIC_DRAW);

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

	GLuint VertexArrayIDRed2;
	glGenVertexArrays(1, &VertexArrayIDRed2);
	glBindVertexArray(VertexArrayIDRed2);

	// Load shader
	GLuint programIDRed2 = LoadShaders("Shaders/SimpleVertexShaderRed2.vertexshader", "Shaders/SimpleFragmentShaderRed2.fragmentshader");
	//GLuint programIDLight = LoadShaders("LightVertexShader.vertexshader", "LightFragmentShader.fragmentshader");

	GLfloat verticesRed2[] = {
		//we add coordinates for NORMALS 
		//back
		// position			//normals		
		-0.5f, -0.5f, -0.5f, 0.0f,  0.0f, -1.0f,  //0
		0.5f, -0.5f, -0.5f, 0.0f,  0.0f, -1.0f,  //1
		0.5f,  0.5f, -0.5f, 0.0f,  0.0f, -1.0f,  //2
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  //3

		//front
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  //4
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  //5
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  //6
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, //7

		//left
	   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, //8
	   -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, //9
	   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, //10
	   -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, //11

	   // right
	   0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, //12
	   0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, //13
	   0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, //14
	   0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, //15

	   //bottom
	  -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, //16
	  0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, //17
	  0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, //18
	  -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,//19

	  //top
	 -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, //20
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, //21
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, //22
	 -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  //23
	};

	// A Vertex Array Object (VAO) is an object which contains one or more Vertex Buffer Objects and is designed to store the information for a complete rendered object. 
	GLuint vboRed2, vaoRed2;
	glGenVertexArrays(1, &vaoRed2);
	glGenBuffers(1, &vboRed2);

	glBindVertexArray(vaoRed2);

	glBindBuffer(GL_ARRAY_BUFFER, vboRed2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesRed2), verticesRed2, GL_STATIC_DRAW);

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


	GLuint VertexArrayIDBlue2;
	glGenVertexArrays(1, &VertexArrayIDBlue2);
	glBindVertexArray(VertexArrayIDBlue2);

	// Load shader
	GLuint programIDBlue2 = LoadShaders("Shaders/SimpleVertexShaderBlue2.vertexshader", "Shaders/SimpleFragmentShaderBlue2.fragmentshader");
	//GLuint programIDLight = LoadShaders("LightVertexShader.vertexshader", "LightFragmentShader.fragmentshader");

	GLfloat verticesBlue2[] = {
		//we add coordinates for NORMALS 
		//back
		// position			//normals		
		-0.5f, -0.5f, -0.5f, 0.0f,  0.0f, -1.0f,  //0
		0.5f, -0.5f, -0.5f, 0.0f,  0.0f, -1.0f,  //1
		0.5f,  0.5f, -0.5f, 0.0f,  0.0f, -1.0f,  //2
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  //3

		//front
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  //4
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  //5
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  //6
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, //7

		//left
	   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, //8
	   -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, //9
	   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, //10
	   -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, //11

	   // right
	   0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, //12
	   0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, //13
	   0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, //14
	   0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, //15

	   //bottom
	  -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, //16
	  0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, //17
	  0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, //18
	  -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,//19

	  //top
	 -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, //20
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, //21
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, //22
	 -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  //23
	};

	// A Vertex Array Object (VAO) is an object which contains one or more Vertex Buffer Objects and is designed to store the information for a complete rendered object. 
	GLuint vboBlue2, vaoBlue2;
	glGenVertexArrays(1, &vaoBlue2);
	glGenBuffers(1, &vboBlue2);

	glBindVertexArray(vaoBlue2);

	glBindBuffer(GL_ARRAY_BUFFER, vboBlue2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesBlue2), verticesBlue2, GL_STATIC_DRAW);

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



	GLuint VertexArrayIDRed3;
	glGenVertexArrays(1, &VertexArrayIDRed3);
	glBindVertexArray(VertexArrayIDRed3);

	// Load shader
	GLuint programIDRed3 = LoadShaders("Shaders/SimpleVertexShaderRed3.vertexshader", "Shaders/SimpleFragmentShaderRed3.fragmentshader");
	//GLuint programIDLight = LoadShaders("LightVertexShader.vertexshader", "LightFragmentShader.fragmentshader");

	GLfloat verticesRed3[] = {
		//we add coordinates for NORMALS 
		//back
		// position			//normals		
		-0.5f, -0.5f, -0.5f, 0.0f,  0.0f, -1.0f,  //0
		0.5f, -0.5f, -0.5f, 0.0f,  0.0f, -1.0f,  //1
		0.5f,  0.5f, -0.5f, 0.0f,  0.0f, -1.0f,  //2
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  //3

		//front
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  //4
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  //5
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  //6
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, //7

		//left
	   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, //8
	   -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, //9
	   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, //10
	   -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, //11

	   // right
	   0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, //12
	   0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, //13
	   0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, //14
	   0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, //15

	   //bottom
	  -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, //16
	  0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, //17
	  0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, //18
	  -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,//19

	  //top
	 -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, //20
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, //21
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, //22
	 -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  //23
	};

	// A Vertex Array Object (VAO) is an object which contains one or more Vertex Buffer Objects and is designed to store the information for a complete rendered object. 
	GLuint vboRed3, vaoRed3;
	glGenVertexArrays(1, &vaoRed3);
	glGenBuffers(1, &vboRed3);

	glBindVertexArray(vaoRed3);

	glBindBuffer(GL_ARRAY_BUFFER, vboRed3);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesRed3), verticesRed3, GL_STATIC_DRAW);

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



	GLuint VertexArrayIDBlue3;
	glGenVertexArrays(1, &VertexArrayIDBlue3);
	glBindVertexArray(VertexArrayIDBlue3);

	// Load shader
	GLuint programIDBlue3 = LoadShaders("Shaders/SimpleVertexShaderBlue3.vertexshader", "Shaders/SimpleFragmentShaderBlue3.fragmentshader");
	//GLuint programIDLight = LoadShaders("LightVertexShader.vertexshader", "LightFragmentShader.fragmentshader");

	GLfloat verticesBlue3[] = {
		//we add coordinates for NORMALS 
		//back
		// position			//normals		
		-0.5f, -0.5f, -0.5f, 0.0f,  0.0f, -1.0f,  //0
		0.5f, -0.5f, -0.5f, 0.0f,  0.0f, -1.0f,  //1
		0.5f,  0.5f, -0.5f, 0.0f,  0.0f, -1.0f,  //2
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  //3

		//front
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  //4
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  //5
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  //6
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, //7

		//left
	   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, //8
	   -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, //9
	   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, //10
	   -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, //11

	   // right
	   0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, //12
	   0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, //13
	   0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, //14
	   0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, //15

	   //bottom
	  -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, //16
	  0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, //17
	  0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, //18
	  -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,//19

	  //top
	 -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, //20
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, //21
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, //22
	 -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  //23
	};

	// A Vertex Array Object (VAO) is an object which contains one or more Vertex Buffer Objects and is designed to store the information for a complete rendered object. 
	GLuint vboBlue3, vaoBlue3;
	glGenVertexArrays(1, &vaoBlue3);
	glGenBuffers(1, &vboBlue3);

	glBindVertexArray(vaoBlue3);

	glBindBuffer(GL_ARRAY_BUFFER, vboBlue3);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesBlue3), verticesBlue3, GL_STATIC_DRAW);

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




	glfwSetFramebufferSizeCallback(window, window_callback);


	Ball jack = Ball(glm::vec3(0.0f, 0.0f, -1300.0f), 0, 0, 0, 0, 0, 0,0);

	Ball red1 = Ball(glm::vec3(0.0f, 0.0f, 0.0f), 0, 0, 0, 0, 0, 0, 0);
	Ball blue1 = Ball(glm::vec3(0.0f, 0.0f, 0.0f), 0, 0, 0, 0, 0, 0, 0);

	Ball red2 = Ball(glm::vec3(0.0f, 0.0f, 0.0f), 0, 0, 0, 0, 0, 0, 0);
	Ball blue2 = Ball(glm::vec3(0.0f, 0.0f, 0.0f), 0, 0, 0, 0, 0, 0, 0);

	Ball red3 = Ball(glm::vec3(0.0f, 0.0f, 0.0f), 0, 0, 0, 0, 0, 0, 0);
	Ball blue3 = Ball(glm::vec3(0.0f, 0.0f, 0.0f), 0, 0, 0, 0, 0, 0, 0);

	Ball boules[6] = { red1, blue1, red2, blue2, red3, blue3 };
	GLuint vao[6] = { vaoRed1, vaoBlue1, vaoRed2, vaoBlue2, vaoRed3, vaoBlue3 };
	GLuint programIDBoules[6] = { programIDRed1, programIDBlue1, programIDRed2, programIDBlue2, 
		programIDRed3, programIDBlue3 };

	float redScores[3] = { 0,0,0 };
	float blueScores[3] = { 0,0,0 };

	int gameRounds[4] = { 0,1,2,3 };

	int currentRound = 0;

	//int rewind;

	// Check if the window was closed
	while (!glfwWindowShouldClose(window) && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
	{

		setWindow(window);

		setFrames();

		glm::mat4 modelJack, modelPlane, modelLight;
		glm::mat4 modelRed1, modelBlue1;
		glm::mat4 modelRed2, modelBlue2, modelRed3, modelBlue3;

		glm::mat4 modelBoules[6] = { modelRed1, modelBlue1, modelRed2, modelBlue2, modelRed3, modelBlue3 };

		//Adding camera
		glm::mat4 view = glm::lookAt(camera.getCameraPosition(),
			camera.getCameraPosition() + camera.getCameraViewDirection(), camera.getCameraUp());

		glm::mat4 projection;
		projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 10000.0f);

		//draw the plane;


		glm::mat4 MVP;

		useShaderPack(programIDPlane, vaoPlane);


		modelPlane = glm::translate(modelPlane, glm::vec3(0.0f, -0.6f, -795.0f));

		MVP = projection * view * modelPlane;

		unsigned int transformLoc100 = glGetUniformLocation(programIDPlane, "model");
		glUniformMatrix4fv(transformLoc100, 1, GL_FALSE, glm::value_ptr(modelPlane));

		unsigned int transformLoc111 = glGetUniformLocation(programIDPlane, "transform");
		glUniformMatrix4fv(transformLoc111, 1, GL_FALSE, glm::value_ptr(MVP));

		glDrawArrays(GL_QUADS, 0, 24);

		setLight(programIDPlane);


		//draw the Jack


		useShaderPack(programIDJack, vaoJack);

		modelJack = jack.ballCalibrate(modelJack);

		MVP = projection * view * modelJack;

		jack.drawBall(MVP, projection, view, modelJack, programIDJack);

		setLight(programIDJack);


		//draw the ball
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

		if (currentRound <= 2)
		{

			for(int i = 0; i < 6; i++)
			{
				//int collisionIndex = i;

				useShaderPack(programIDBoules[i], vao[i]);


				if (!boules[i].getBallSpawnTrue() && !boules[i].getBallDisqualified() && boules[i - 1].getBallDisqualified()
					&& glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
				{
					boules[i].setBallSpawnTrue(1);
					std::cout << "ROUND " << currentRound + 1 << std::endl << "BOULE " << i + 1 << std::endl;
				}


				if (boules[i].getBallSpawnTrue())
				{
					
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

					
					if (!boules[i].getBallThrowTrue())
					{
						if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
						{
							boules[i].setBallThrowSpeed(boules[i].getBallThrowSpeed() + 1);

						}

						if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE && boules[i].getBallThrowSpeed() != 0)
						{
							boules[i].setBallThrowDist(pow((boules[i].getBallThrowSpeed() / 10), 2) / (2 * 0.5 * 9.81));

							boules[i].setBallThrowTrue(1);
							boules[i].setBallCalibrateTrue(1);

							boules[i].setBallPositionX(boules[i].getBallPositionX() * 2.0f);
						}

					}

					

					if (boules[i].getBallThrowTrue() && boules[i].getBallCalibrateTrue())
					{
						
						
						if (boules[i].getBallPositionZ() >= -boules[i].getBallThrowDist())
							boules[i].setBallPositionZ(boules[i].getBallPositionZ() - boules[i].getBallThrowDist() * 0.1 / boules[i].getBallThrowSpeed());
						else boules[i].setBallDisqualified(1);
				
							
						


						for (int h = 0; h < i; h++)
						{
							if (boules[h].getBallPosition() != glm::vec3(0.0f, 0.0f, 0.0f))
								if (checkCollision(boules[i], boules[h], 1.0f))
								{
									boules[i].setBallThrowDist(-boules[i].getBallPositionZ() - 2.0f);

									float dist = boules[h].getBallPositionZ() - boules[i].getBallThrowSpeed()/20;

									boules[h].setBallThrowDist(-dist);


									std::cout << "I: " << boules[i].getBallPositionZ() << std::endl << "H: " << boules[h].getBallPositionZ() << std::endl;

									

									if(boules[h].getBallPositionZ() <= dist)
									{

										useShaderPack(programIDBoules[h], vao[h]);

										boules[h].setBallThrowSpeed(boules[i].getBallThrowSpeed() / 20);

										boules[h].ballThrow(modelBoules[h]);

										boules[h].drawBall(MVP, projection, view, modelBoules[h], programIDBoules[h]);

									}
								}

							boules[h].ballBlock();

						}

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

			
				setLight(programIDBoules[i]);

				
				boules[i].ballBlock();

				//i = collisionIndex;


				/*if(glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && boules[i].getBallDisqualified())

				{

					float dist = sqrt(pow(jack.getBallPositionX() - boules[i].getBallPositionX(), 2) +
						pow(jack.getBallPositionZ() - boules[i].getBallPositionZ(), 2));

					if (i % 2 == 0)
					{
						int score = float(1 / dist) * 1000;
						std::cout << "RED NO. " << i + 1 << "DIST : " << dist << " SCORE: " << score << std::endl;
					}

					else
					{
						int score = float(1 / dist) * 1000;
						std::cout << "BLUE NO. " << i + 1 << "DIST : " << dist << " SCORE: " << score << std::endl;
					}
				}*/

			}

		}


		if (boules[0].getBallDisqualified() && boules[1].getBallDisqualified() && boules[2].getBallDisqualified()
			&& boules[3].getBallDisqualified() && boules[4].getBallDisqualified() && boules[5].getBallDisqualified())
		{

			float allScores[6] = { 0,0,0,0,0,0 };

			for (int j = 0; j < 6; j++)
			{
				
				float dist = sqrt(pow(jack.getBallPositionX() - boules[j].getBallPositionX(), 2) +
					pow(jack.getBallPositionZ() - boules[j].getBallPositionZ(), 2));

				if (dist == 0)
					dist += 0.001;
				
				allScores[j] = float(1 / dist) * 10000;

				std::cout<<allScores[j]<<std::endl;

			}

			redScores[currentRound] = allScores[0] + allScores[2] + allScores[4];
			blueScores[currentRound] = allScores[1] + allScores[3] + allScores[5];
			
			for (int j = 0; j < 6; j++)
				boules[j].ballReset();

			if (currentRound == 2)
			{
				float redScore = redScores[0] + redScores[1] + redScores[2];
				float blueScore = blueScores[0] + blueScores[1] + blueScores[2];
				std::cout << "TEAM RED FINAL SCORE: " << (int)redScore << std::endl;
				std::cout << "TEAM BLUE FINAL SCORE: " << (int)blueScore << std::endl;

				if((int)blueScore > (int)redScore)
					std::cout << "WINNER: TEAM BLUE" << std::endl;

				if ((int)blueScore < (int)redScore)
					std::cout << "WINNER: TEAM RED" << std::endl;

				if ((int)blueScore == (int)redScore)
					std::cout << "WINNER: TIE" << std::endl;
			}

			/*std::cout << "TEAM RED SCORE: " << redScores[currentRound] << std::endl;
			std::cout << "TEAM BLUE SCORE: " << blueScores[currentRound] << std::endl;*/

			currentRound = gameRounds[currentRound + 1];
			

		}



		//use the shader for the light
		glUseProgram(programIDLight);

		modelLight = glm::mat4();
		modelLight = glm::translate(modelLight, lightPos);
		modelLight = glm::scale(modelLight, glm::vec3(0.0005f)); // a smaller cube
		MVP = projection * view * modelLight;

		unsigned int transformLoc5 = glGetUniformLocation(programIDLight, "transform");
		glUniformMatrix4fv(transformLoc5, 1, GL_FALSE, glm::value_ptr(MVP));


		unsigned int transformLoc6 = glGetUniformLocation(programIDLight, "lightColor");
		glUniform4fv(transformLoc6, 1, glm::value_ptr(lightColor));

		glDrawArrays(GL_QUADS, 0, 24);

	}

	// Cleanup VBO
	glDeleteBuffers(1, &vboRed1);
	glDeleteVertexArrays(1, &VertexArrayIDRed1);
	glDeleteProgram(programIDRed1);

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
