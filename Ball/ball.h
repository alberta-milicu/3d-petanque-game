#pragma once

#include "..\dependente\glew\glew.h"

#include "..\dependente\glfw\glfw3.h"

#include "..\dependente\glm\glm.hpp"
#include "..\dependente\glm\gtc\matrix_transform.hpp"
#include "..\dependente\glm\gtc\type_ptr.hpp"


class Ball
{
private:
	glm::vec3 ballPosition;
	int throwTrue;
	int calibrateTrue;

public:
	Ball();
	Ball(glm::vec3 ballPosition, int throwTrue, int calibrateTrue);
	~Ball();

	void drawBall(glm::mat4 MVP, glm::mat4 projection, glm::mat4 view, glm::mat4 model, GLuint programID);
	glm::mat4 ballRotate(glm::mat4 model, float throwDist);
	glm::mat4 ballAdvance(glm::mat4 model, float throwDist);
	glm::mat4 ballThrow(glm::mat4 model, float throwDist);
	glm::mat4 ballCalibrate(glm::mat4 model);

	glm::vec3 getBallPosition();
	float getBallPositionX();
	float getBallPositionY();
	float getBallPositionZ();
	int getBallThrowTrue();
	int getBallCalibrateTrue();
	

	void setBallPosition(glm::vec3 ballPostion);
	void setBallPositionX(float x);
	void setBallPositionY(float y);
	void setBallPositionZ(float z);
	void setBallThrowTrue(int throwTrue);
	void setBallCalibrateTrue(int calibrateTrue);

};