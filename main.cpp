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

	glfwSetFramebufferSizeCallback(window, window_callback);


	Ball jack = Ball(glm::vec3(0.0f, 0.0f, -1300.0f), 0, 0, 0, 0,0,0);

	Ball red1 = Ball(glm::vec3(0.0f, 0.0f, 0.0f), 0, 0, 0, 0,0,0);
	Ball blue1 = Ball(glm::vec3(0.0f, 0.0f, 0.0f), 0, 0, 0, 0,0,0);

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

		glm::mat4 model, modelJack, modelPlane;

		//Adding camera
		glm::mat4 view = glm::lookAt(camera.getCameraPosition(),
			camera.getCameraPosition() + camera.getCameraViewDirection(), camera.getCameraUp());

		glm::mat4 projection;
		projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 10000.0f);

		//draw the plane;


		glm::mat4 MVP;

		useShaderPack(programIDPlane, vaoPlane);


		modelPlane = glm::translate(model, glm::vec3(0.0f, -0.6f, -795.0f));

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
					red1.setBallThrowDist(pow((red1.getBallThrowSpeed() / 10), 2) / (2 * 0.5 * 9.81));
					
					red1.setBallThrowTrue(1);
					red1.setBallCalibrateTrue(1);

					red1.setBallPositionX(red1.getBallPositionX() * 2.0f);
				}

			}

			if (!red1.getBallThrowTrue() && !red1.getBallCalibrateTrue()
				&& glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
			{
				if (red1.getBallPositionX() > -1.7f)
					red1.setBallPositionX(red1.getBallPositionX() - 0.007f);
				red1.ballCalibrate(model);

			}

			if (!red1.getBallThrowTrue() && !red1.getBallCalibrateTrue()
				&& glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			{
				if (red1.getBallPositionX() < 1.7f)
					red1.setBallPositionX(red1.getBallPositionX() + 0.007f);
				red1.ballCalibrate(model);
			}

			if (!red1.getBallThrowTrue() && !red1.getBallCalibrateTrue())
			{

				model = red1.ballCalibrate(model);
			}

			if (red1.getBallThrowTrue() && red1.getBallCalibrateTrue())
			{
				red1.setBallPositionZ(red1.getBallPositionZ() - red1.getBallThrowDist()*0.1 / red1.getBallThrowSpeed());
				animateBall(red1, projection, view, model, MVP, programIDRed1);
			}
			else
			{

				model = glm::translate(model, red1.getBallPosition());

				MVP = projection * view * model;

				red1.drawBall(MVP, projection, view, model, programIDRed1);
			}

		}

		if (red1.getBallPositionZ() < -1590.0f)
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



		//use the shader for the light
		glUseProgram(programIDLight);

		model = glm::mat4();
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.0005f)); // a smaller cube
		MVP = projection * view * model;

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
