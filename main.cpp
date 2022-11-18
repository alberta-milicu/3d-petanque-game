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

void animateBall(Ball allyBall1, glm::mat4 projection, glm::mat4 view, glm::mat4 model, glm::mat4 MVP, GLuint programID)
{

	model = glm::mat4(1.0);

	model = allyBall1.ballThrow(model, -600.f);

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
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	GLuint VertexArrayIDPlane;
	glGenVertexArrays(1, &VertexArrayIDPlane);
	glBindVertexArray(VertexArrayIDPlane);

	// Load shader
	GLuint programIDPlane = LoadShaders("SimpleVertexShaderPlane.vertexshader", "SimpleFragmentShaderPlane.fragmentshader");
	GLuint programIDLight = LoadShaders("LightVertexShader.vertexshader", "LightFragmentShader.fragmentshader");

	GLfloat verticesPlane[] = {
		//we add coordinates for NORMALS 
		//back
		// position			//normals		
		-4.0f, -0.1f, -800.0f, 0.0f,  0.0f, -1.0f,  //0
		4.0f, -0.1f, -800.0f, 0.0f,  0.0f, -1.0f,  //1
		4.0f,  0.1f, -800.0f, 0.0f,  0.0f, -1.0f,  //2
		-4.0f,  0.1f, -800.0f,  0.0f,  0.0f, -1.0f,  //3

		//front
		-4.0f, -0.1f,  800.0f,  0.0f,  0.0f,  1.0f,  //4
		4.0f, -0.1f,  800.0f,  0.0f,  0.0f,  1.0f,  //5
		4.0f,  0.1f,  800.0f,  0.0f,  0.0f,  1.0f,  //6
		-4.0f,  0.1f,  800.0f,  0.0f,  0.0f,  1.0f, //7

		//left
	   -4.0f,  0.1f,  800.0f, -1.0f,  0.0f,  0.0f, //8
	   -4.0f,  0.1f, -800.0f, -1.0f,  0.0f,  0.0f, //9
	   -4.0f, -0.1f, -800.0f, -1.0f,  0.0f,  0.0f, //10
	   -4.0f, -0.1f,  800.0f, -1.0f,  0.0f,  0.0f, //11

	   // right
	   4.0f,  0.1f,  800.0f,  1.0f,  0.0f,  0.0f, //12
	   4.0f,  0.1f, -800.0f,  1.0f,  0.0f,  0.0f, //13
	   4.0f, -0.1f, -800.0f,  1.0f,  0.0f,  0.0f, //14
	   4.0f, -0.1f,  800.0f,  1.0f,  0.0f,  0.0f, //15

	   //bottom
	  -4.0f, -0.1f, -800.0f,  0.0f, -1.0f,  0.0f, //16
	  4.0f, -0.1f, -800.0f,  0.0f, -1.0f,  0.0f, //17
	  4.0f, -0.1f,  800.0f,  0.0f, -1.0f,  0.0f, //18
	  -4.0f, -0.1f,  800.0f,  0.0f, -1.0f,  0.0f,//19

	  //top
	 -4.0f,  0.1f, -800.0f,  0.0f,  1.0f,  0.0f, //20
	 4.0f,  0.1f, -800.0f,  0.0f,  1.0f,  0.0f, //21
	 4.0f,  0.1f,  800.0f,  0.0f,  1.0f,  0.0f, //22
	 -4.0f,  0.1f,  800.0f,  0.0f, 1.0f,  0.0f,  //23
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
	GLuint programIDJack = LoadShaders("SimpleVertexShaderJack.vertexshader", "SimpleFragmentShaderJack.fragmentshader");
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

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Load shader
	GLuint programID = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");
	//GLuint programIDLight = LoadShaders("LightVertexShader.vertexshader", "LightFragmentShader.fragmentshader");

	GLfloat vertices[] = {
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
	GLuint vbo, vao;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

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


	Ball allyBall1 = Ball(glm::vec3(0.0f, 0.0f, 0.0f), 0, 0);
	Ball jack = Ball(glm::vec3(0.0f, 0.0f, -800.0f), 0, 0);
	Ball plane = Ball(glm::vec3(0.0f, -0.6f, -795.0f), 0, 0);


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

		//glm::mat4 MVP;


		glm::mat4 MVP;

		useShaderPack(programIDPlane, vaoPlane);

		modelPlane = plane.ballCalibrate(modelPlane);

		MVP = projection * view * modelPlane;

		plane.drawBall(MVP, projection, view, modelPlane, programIDPlane);

		setLight(programIDPlane);



		useShaderPack(programIDJack, vaoJack);

		modelJack = jack.ballCalibrate(modelJack);

		MVP = projection * view * modelJack;

		jack.drawBall(MVP, projection, view, modelJack, programIDJack);

		setLight(programIDJack);




		useShaderPack(programID, vao);

		if (!allyBall1.getBallThrowTrue() && glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		{
			
			allyBall1.setBallThrowTrue(1);
			allyBall1.setBallCalibrateTrue(1);

			allyBall1.setBallPositionX(allyBall1.getBallPositionX() * 2.0f);
			
		}

		if (!allyBall1.getBallThrowTrue() && !allyBall1.getBallCalibrateTrue()
			&& glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		{
			allyBall1.setBallPositionX(allyBall1.getBallPositionX() - 0.007f);
			allyBall1.ballCalibrate(model);

		}

		if (!allyBall1.getBallThrowTrue() && !allyBall1.getBallCalibrateTrue()
			&& glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		{
			allyBall1.setBallPositionX(allyBall1.getBallPositionX() + 0.007f);
			allyBall1.ballCalibrate(model);
		}

		if (!allyBall1.getBallThrowTrue() && !allyBall1.getBallCalibrateTrue())
		{

			model = allyBall1.ballCalibrate(model);


		}

		if (allyBall1.getBallThrowTrue() && allyBall1.getBallCalibrateTrue())
		{
			allyBall1.setBallPositionZ(allyBall1.getBallPositionZ() - 0.07f);
			animateBall(allyBall1, projection, view, model, MVP, programID);
		}
		else
		{

			model = glm::translate(model, allyBall1.getBallPosition());

			MVP = projection * view * model;

			allyBall1.drawBall(MVP, projection, view, model, programID);
		}


		setLight(programID);



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
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(programID);

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

	/*if (glfwGetKey(window, GLFW_KEY_W))
	{
		camera.keyboardMoveFront(cameraSpeed);
		std::cout << camera.getCameraPosition().x << " " << camera.getCameraPosition().y << " " << camera.getCameraPosition().z << std::endl;
	}
	if (glfwGetKey(window, GLFW_KEY_S))
	{
		camera.keyboardMoveBack(cameraSpeed);
		std::cout << camera.getCameraPosition().x << " " << camera.getCameraPosition().y << " " << camera.getCameraPosition().z << std::endl;
	}

	if (glfwGetKey(window, GLFW_KEY_R))
	{
		camera.keyboardMoveUp(cameraSpeed);
		std::cout << camera.getCameraPosition().x << " " << camera.getCameraPosition().y << " " << camera.getCameraPosition().z << std::endl;
	}
	if (glfwGetKey(window, GLFW_KEY_F))
	{
		camera.keyboardMoveDown(cameraSpeed);
		std::cout << camera.getCameraPosition().x << " " << camera.getCameraPosition().y << " " << camera.getCameraPosition().z << std::endl;
	}

	if (glfwGetKey(window, GLFW_KEY_UP))
	{
		camera.rotateOx(cameraSpeed / 100);
		std::cout << camera.getCameraPosition().x << " " << camera.getCameraPosition().y << " " << camera.getCameraPosition().z << std::endl;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN))
	{
		camera.rotateOx(-cameraSpeed / 100);
		std::cout << camera.getCameraPosition().x << " " << camera.getCameraPosition().y << " " << camera.getCameraPosition().z << std::endl;
	}*/
	
	//toggle top view -> rotateOX 90deg(-) and focus camera pos
}
