// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

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


	Ball jack = Ball(glm::vec3(0.0f, 0.0f, -1300.0f), 0, 0, 0, 0,0,0);

	Ball red1 = Ball(glm::vec3(0.0f, 0.0f, 0.0f), 0, 0, 0, 0, 0, 0);
	Ball blue1 = Ball(glm::vec3(0.0f, 0.0f, 0.0f), 0, 0, 0, 0, 0, 0);

	Ball red2 = Ball(glm::vec3(0.0f, 0.0f, 0.0f), 0, 0, 0, 0, 0, 0);
	Ball blue2 = Ball(glm::vec3(0.0f, 0.0f, 0.0f), 0, 0, 0, 0, 0, 0);

	Ball red3 = Ball(glm::vec3(0.0f, 0.0f, 0.0f), 0, 0, 0, 0, 0, 0);
	Ball blue3 = Ball(glm::vec3(0.0f, 0.0f, 0.0f), 0, 0, 0, 0, 0, 0);

	Ball players[2] = {red1, blue1};

	// Check if the window was closed
	while (!glfwWindowShouldClose(window) && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
	{

		setWindow(window);

		setFrames();

		glm::mat4 modelJack, modelPlane, modelLight, modelRed1, modelBlue1, modelRed2, modelBlue2, modelRed3, modelBlue3;

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


		useShaderPack(programIDRed1, vaoRed1);

		if (!red1.getBallSpawnTrue() && !red1.getBallDisqualified() && glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		{
			red1.setBallSpawnTrue(1);
		}

		if (red1.getBallSpawnTrue())
		{
			
			if (!red1.getBallThrowTrue())
			{
				if(glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
				{
					red1.setBallThrowSpeed(red1.getBallThrowSpeed() + 1);

				}

				if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE && red1.getBallThrowSpeed() != 0)
				{
					red1.setBallThrowDist(pow((red1.getBallThrowSpeed()/10), 2) / (2 * 0.5 * 9.81));
					
					red1.setBallThrowTrue(1);
					red1.setBallCalibrateTrue(1);

					red1.setBallPositionX(red1.getBallPositionX() * 2.0f);
				}

			}

			if (!red1.getBallThrowTrue() && !red1.getBallCalibrateTrue()
				&& glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
			{
				if (red1.getBallPositionX() > -1.7f)
					red1.setBallPositionX(red1.getBallPositionX() - 0.0009f);
				red1.ballCalibrate(modelRed1);

			}

			if (!red1.getBallThrowTrue() && !red1.getBallCalibrateTrue()
				&& glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			{
				if (red1.getBallPositionX() < 1.7f)
					red1.setBallPositionX(red1.getBallPositionX() + 0.0009f);
				red1.ballCalibrate(modelRed1);
			}

			if (!red1.getBallThrowTrue() && !red1.getBallCalibrateTrue())
			{

				modelRed1 = red1.ballCalibrate(modelRed1);
			}

			if (red1.getBallThrowTrue() && red1.getBallCalibrateTrue())
			{			
				
				if (red1.getBallPositionZ() >= -red1.getBallThrowDist())
					red1.setBallPositionZ(red1.getBallPositionZ() - red1.getBallThrowDist() * 0.1 / red1.getBallThrowSpeed());
				else red1.setBallDisqualified(1);
				animateBall(red1, projection, view, modelRed1, MVP, programIDRed1);
	

			}
			else
			{
				if(!red1.getBallDisqualified())
				{
					modelRed1 = glm::translate(modelRed1, red1.getBallPosition());

					MVP = projection * view * modelRed1;

					red1.drawBall(MVP, projection, view, modelRed1, programIDRed1);
				}
			}

		}

		if (red1.getBallPositionZ() < -1600.0f)
		{
			red1.setBallSpawnTrue(0);
			red1.setBallCalibrateTrue(0);
			red1.setBallThrowTrue(0);
			red1.setBallThrowSpeed(0);
			red1.setBallThrowDist(0);
			red1.setBallDisqualified(1);

			red1.setBallPosition(glm::vec3(0.0f, 0.0f, 0.0f));
		}

		setLight(programIDRed1);




		useShaderPack(programIDBlue1, vaoBlue1);

		if (!blue1.getBallSpawnTrue() && !blue1.getBallDisqualified() 
			&& red1.getBallDisqualified() && glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		{
			blue1.setBallSpawnTrue(1);
		}

		if (blue1.getBallSpawnTrue())
		{

			if (!blue1.getBallThrowTrue())
			{
				if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
				{
					blue1.setBallThrowSpeed(blue1.getBallThrowSpeed() + 1);

				}

				if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE && blue1.getBallThrowSpeed() != 0)
				{
					blue1.setBallThrowDist(pow((blue1.getBallThrowSpeed() / 10), 2) / (2 * 0.5 * 9.81));

					blue1.setBallThrowTrue(1);
					blue1.setBallCalibrateTrue(1);

					blue1.setBallPositionX(blue1.getBallPositionX() * 2.0f);
				}

			}

			if (!blue1.getBallThrowTrue() && !blue1.getBallCalibrateTrue()
				&& glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
			{
				if (blue1.getBallPositionX() > -1.7f)
					blue1.setBallPositionX(blue1.getBallPositionX() - 0.0009f);
				blue1.ballCalibrate(modelBlue1);

			}

			if (!blue1.getBallThrowTrue() && !blue1.getBallCalibrateTrue()
				&& glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			{
				if (blue1.getBallPositionX() < 1.7f)
					blue1.setBallPositionX(blue1.getBallPositionX() + 0.0009f);
				blue1.ballCalibrate(modelBlue1);
			}

			if (!blue1.getBallThrowTrue() && !blue1.getBallCalibrateTrue())
			{

				modelBlue1 = blue1.ballCalibrate(modelBlue1);
			}

			if (blue1.getBallThrowTrue() && blue1.getBallCalibrateTrue())
			{

				if (blue1.getBallPositionZ() >= -blue1.getBallThrowDist())
					blue1.setBallPositionZ(blue1.getBallPositionZ() - blue1.getBallThrowDist() * 0.1 / blue1.getBallThrowSpeed());
				else blue1.setBallDisqualified(1);
				animateBall(blue1, projection, view, modelBlue1, MVP, programIDBlue1);


			}
			else
			{
				if (!blue1.getBallDisqualified())
				{
					modelBlue1 = glm::translate(modelBlue1, blue1.getBallPosition());

					MVP = projection * view * modelBlue1;

					blue1.drawBall(MVP, projection, view, modelBlue1, programIDBlue1);
				}
			}

		}

		if (blue1.getBallPositionZ() < -1600.0f)
		{
			blue1.setBallSpawnTrue(0);
			blue1.setBallCalibrateTrue(0);
			blue1.setBallThrowTrue(0);
			blue1.setBallThrowSpeed(0);
			blue1.setBallThrowDist(0);
			blue1.setBallDisqualified(1);

			blue1.setBallPosition(glm::vec3(0.0f, 0.0f, 0.0f));
		}

		setLight(programIDBlue1);




		useShaderPack(programIDRed2, vaoRed2);

		if (!red2.getBallSpawnTrue() && !red2.getBallDisqualified()
			&& red1.getBallDisqualified() && blue1.getBallDisqualified() && glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		{
			red2.setBallSpawnTrue(1);
		}

		if (red2.getBallSpawnTrue())
		{

			if (!red2.getBallThrowTrue())
			{
				if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
				{
					red2.setBallThrowSpeed(red2.getBallThrowSpeed() + 1);

				}

				if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE && red2.getBallThrowSpeed() != 0)
				{
					red2.setBallThrowDist(pow((red2.getBallThrowSpeed() / 10), 2) / (2 * 0.5 * 9.81));

					red2.setBallThrowTrue(1);
					red2.setBallCalibrateTrue(1);

					red2.setBallPositionX(red2.getBallPositionX() * 2.0f);
				}

			}

			if (!red2.getBallThrowTrue() && !red2.getBallCalibrateTrue()
				&& glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
			{
				if (red2.getBallPositionX() > -1.7f)
					red2.setBallPositionX(red2.getBallPositionX() - 0.0009f);
				red2.ballCalibrate(modelRed2);

			}

			if (!red2.getBallThrowTrue() && !red2.getBallCalibrateTrue()
				&& glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			{
				if (red2.getBallPositionX() < 1.7f)
					red2.setBallPositionX(red2.getBallPositionX() + 0.0009f);
				red2.ballCalibrate(modelRed2);
			}

			if (!red2.getBallThrowTrue() && !red2.getBallCalibrateTrue())
			{

				modelRed2 = red2.ballCalibrate(modelRed2);
			}

			if (red2.getBallThrowTrue() && red2.getBallCalibrateTrue())
			{

				if (red2.getBallPositionZ() >= -red2.getBallThrowDist())
					red2.setBallPositionZ(red2.getBallPositionZ() - red2.getBallThrowDist() * 0.1 / red2.getBallThrowSpeed());
				else red2.setBallDisqualified(1);
				animateBall(red2, projection, view, modelRed2, MVP, programIDRed2);


			}
			else
			{
				if (!red2.getBallDisqualified())
				{
					modelRed2 = glm::translate(modelRed2, red2.getBallPosition());

					MVP = projection * view * modelRed2;

					red2.drawBall(MVP, projection, view, modelRed2, programIDRed2);
				}
			}

		}

		if (red2.getBallPositionZ() < -1600.0f)
		{
			red2.setBallSpawnTrue(0);
			red2.setBallCalibrateTrue(0);
			red2.setBallThrowTrue(0);
			red2.setBallThrowSpeed(0);
			red2.setBallThrowDist(0);
			red2.setBallDisqualified(1);

			red2.setBallPosition(glm::vec3(0.0f, 0.0f, 0.0f));
		}

		setLight(programIDRed2);



		useShaderPack(programIDBlue2, vaoBlue2);

		if (!blue2.getBallSpawnTrue() && !blue2.getBallDisqualified()
			&& red1.getBallDisqualified() && blue1.getBallDisqualified()
			&& red2.getBallDisqualified()
			&& glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		{
			blue2.setBallSpawnTrue(1);
		}

		if (blue2.getBallSpawnTrue())
		{

			if (!blue2.getBallThrowTrue())
			{
				if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
				{
					blue2.setBallThrowSpeed(blue2.getBallThrowSpeed() + 1);

				}

				if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE && blue2.getBallThrowSpeed() != 0)
				{
					blue2.setBallThrowDist(pow((blue2.getBallThrowSpeed() / 10), 2) / (2 * 0.5 * 9.81));

					blue2.setBallThrowTrue(1);
					blue2.setBallCalibrateTrue(1);

					blue2.setBallPositionX(blue2.getBallPositionX() * 2.0f);
				}

			}

			if (!blue2.getBallThrowTrue() && !blue2.getBallCalibrateTrue()
				&& glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
			{
				if (blue2.getBallPositionX() > -1.7f)
					blue2.setBallPositionX(blue2.getBallPositionX() - 0.0009f);
				blue2.ballCalibrate(modelBlue2);

			}

			if (!blue2.getBallThrowTrue() && !blue2.getBallCalibrateTrue()
				&& glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			{
				if (blue2.getBallPositionX() < 1.7f)
					blue2.setBallPositionX(blue2.getBallPositionX() + 0.0009f);
				blue2.ballCalibrate(modelBlue2);
			}

			if (!blue2.getBallThrowTrue() && !blue2.getBallCalibrateTrue())
			{

				modelBlue2 = blue2.ballCalibrate(modelBlue2);
			}

			if (blue2.getBallThrowTrue() && blue2.getBallCalibrateTrue())
			{

				if (blue2.getBallPositionZ() >= -blue2.getBallThrowDist())
					blue2.setBallPositionZ(blue2.getBallPositionZ() - blue2.getBallThrowDist() * 0.1 / blue2.getBallThrowSpeed());
				else blue2.setBallDisqualified(1);
				animateBall(blue2, projection, view, modelBlue2, MVP, programIDBlue2);


			}
			else
			{
				if (!blue2.getBallDisqualified())
				{
					modelBlue2 = glm::translate(modelBlue2, blue2.getBallPosition());

					MVP = projection * view * modelBlue2;

					blue2.drawBall(MVP, projection, view, modelBlue2, programIDBlue2);
				}
			}

		}

		if (blue2.getBallPositionZ() < -1600.0f)
		{
			blue2.setBallSpawnTrue(0);
			blue2.setBallCalibrateTrue(0);
			blue2.setBallThrowTrue(0);
			blue2.setBallThrowSpeed(0);
			blue2.setBallThrowDist(0);
			blue2.setBallDisqualified(1);

			blue2.setBallPosition(glm::vec3(0.0f, 0.0f, 0.0f));
		}

		setLight(programIDBlue2);


		useShaderPack(programIDRed3, vaoRed3);

		if (!red3.getBallSpawnTrue() && !red3.getBallDisqualified()
			&& red1.getBallDisqualified() && blue1.getBallDisqualified()
			&& red2.getBallDisqualified() && blue2.getBallDisqualified()
			&& glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		{
			red3.setBallSpawnTrue(1);
		}

		if (red3.getBallSpawnTrue())
		{

			if (!red3.getBallThrowTrue())
			{
				if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
				{
					red3.setBallThrowSpeed(red3.getBallThrowSpeed() + 1);

				}

				if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE && red3.getBallThrowSpeed() != 0)
				{
					red3.setBallThrowDist(pow((red3.getBallThrowSpeed() / 10), 2) / (2 * 0.5 * 9.81));

					red3.setBallThrowTrue(1);
					red3.setBallCalibrateTrue(1);

					red3.setBallPositionX(red3.getBallPositionX() * 2.0f);
				}

			}

			if (!red3.getBallThrowTrue() && !red3.getBallCalibrateTrue()
				&& glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
			{
				if (red3.getBallPositionX() > -1.7f)
					red3.setBallPositionX(red3.getBallPositionX() - 0.0009f);
				red3.ballCalibrate(modelRed3);

			}

			if (!red3.getBallThrowTrue() && !red3.getBallCalibrateTrue()
				&& glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			{
				if (red3.getBallPositionX() < 1.7f)
					red3.setBallPositionX(red3.getBallPositionX() + 0.0009f);
				red3.ballCalibrate(modelRed3);
			}

			if (!red3.getBallThrowTrue() && !red3.getBallCalibrateTrue())
			{

				modelRed3 = red3.ballCalibrate(modelRed3);
			}

			if (red3.getBallThrowTrue() && red3.getBallCalibrateTrue())
			{

				if (red3.getBallPositionZ() >= -red3.getBallThrowDist())
					red3.setBallPositionZ(red3.getBallPositionZ() - red3.getBallThrowDist() * 0.1 / red3.getBallThrowSpeed());
				else red3.setBallDisqualified(1);
				animateBall(red3, projection, view, modelRed3, MVP, programIDRed3);


			}
			else
			{
				if (!red3.getBallDisqualified())
				{
					modelRed3 = glm::translate(modelRed3, red3.getBallPosition());

					MVP = projection * view * modelRed3;

					red3.drawBall(MVP, projection, view, modelRed3, programIDRed3);
				}
			}

		}

		if (red3.getBallPositionZ() < -1600.0f)
		{
			red3.setBallSpawnTrue(0);
			red3.setBallCalibrateTrue(0);
			red3.setBallThrowTrue(0);
			red3.setBallThrowSpeed(0);
			red3.setBallThrowDist(0);
			red3.setBallDisqualified(1);

			red3.setBallPosition(glm::vec3(0.0f, 0.0f, 0.0f));
		}

		setLight(programIDRed3);



		useShaderPack(programIDBlue3, vaoBlue3);

		if (!blue3.getBallSpawnTrue() && !blue3.getBallDisqualified()
			&& red1.getBallDisqualified() && blue1.getBallDisqualified()
			&& red2.getBallDisqualified() && blue2.getBallDisqualified()
			&& red3.getBallDisqualified()
			&& glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		{
			blue3.setBallSpawnTrue(1);
		}

		if (blue3.getBallSpawnTrue())
		{

			if (!blue3.getBallThrowTrue())
			{
				if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
				{
					blue3.setBallThrowSpeed(blue3.getBallThrowSpeed() + 1);

				}

				if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE && blue3.getBallThrowSpeed() != 0)
				{
					blue3.setBallThrowDist(pow((blue3.getBallThrowSpeed() / 10), 2) / (2 * 0.5 * 9.81));

					blue3.setBallThrowTrue(1);
					blue3.setBallCalibrateTrue(1);

					blue3.setBallPositionX(blue3.getBallPositionX() * 2.0f);
				}

			}

			if (!blue3.getBallThrowTrue() && !blue3.getBallCalibrateTrue()
				&& glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
			{
				if (blue3.getBallPositionX() > -1.7f)
					blue3.setBallPositionX(blue3.getBallPositionX() - 0.0009f);
				blue3.ballCalibrate(modelBlue3);

			}

			if (!blue3.getBallThrowTrue() && !blue3.getBallCalibrateTrue()
				&& glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			{
				if (blue3.getBallPositionX() < 1.7f)
					blue3.setBallPositionX(blue3.getBallPositionX() + 0.0009f);
				blue3.ballCalibrate(modelBlue3);
			}

			if (!blue3.getBallThrowTrue() && !blue3.getBallCalibrateTrue())
			{

				modelBlue3 = blue3.ballCalibrate(modelBlue3);
			}

			if (blue3.getBallThrowTrue() && blue3.getBallCalibrateTrue())
			{

				if (blue3.getBallPositionZ() >= -blue3.getBallThrowDist())
					blue3.setBallPositionZ(blue3.getBallPositionZ() - blue3.getBallThrowDist() * 0.1 / blue3.getBallThrowSpeed());
				else blue3.setBallDisqualified(1);
				animateBall(blue3, projection, view, modelBlue3, MVP, programIDBlue3);


			}
			else
			{
				if (!blue3.getBallDisqualified())
				{
					modelBlue3 = glm::translate(modelBlue3, blue3.getBallPosition());

					MVP = projection * view * modelBlue3;

					blue3.drawBall(MVP, projection, view, modelBlue3, programIDBlue3);
				}
			}

		}

		if (blue3.getBallPositionZ() < -1600.0f)
		{
			blue3.setBallSpawnTrue(0);
			blue3.setBallCalibrateTrue(0);
			blue3.setBallThrowTrue(0);
			blue3.setBallThrowSpeed(0);
			blue3.setBallThrowDist(0);
			blue3.setBallDisqualified(1);

			blue3.setBallPosition(glm::vec3(0.0f, 0.0f, 0.0f));
		}

		setLight(programIDBlue3);





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
