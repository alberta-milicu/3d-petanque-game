# include "ball.h"

glm::vec3 xRotate(1.0f, 0.0f, 0.0f);

Ball::Ball()
{
	this->ballPosition = glm::vec3(0.0f, 0.0f, 0.0f);
}

Ball::Ball(glm::vec3 ballPosition, int throwTrue, int calibrateTrue)
{
	this->ballPosition = ballPosition;
	this->throwTrue = throwTrue;
	this->calibrateTrue = calibrateTrue;

}

Ball::~Ball()
{
}

void Ball::drawBall(glm::mat4 MVP, glm::mat4 projection, glm::mat4 view, glm::mat4 model, GLuint programID)
{
	model = glm::translate(model, this->getBallPosition());
	
	unsigned int transformLoc0 = glGetUniformLocation(programID, "model");
	glUniformMatrix4fv(transformLoc0, 1, GL_FALSE, glm::value_ptr(model));

	unsigned int transformLoc1 = glGetUniformLocation(programID, "transform");
	glUniformMatrix4fv(transformLoc1, 1, GL_FALSE, glm::value_ptr(MVP));

	glDrawArrays(GL_QUADS, 0, 24);
}

glm::mat4 Ball::ballRotate(glm::mat4 model, float throwDist)
{
	if (this->getBallPositionZ() > throwDist)
		model = glm::rotate(model, (float)glfwGetTime() * -150, glm::vec3(1.0f, 0.0f, 0.0f));
	return model;
}

glm::mat4 Ball::ballAdvance(glm::mat4 model, float throwDist)
{
	if (this->getBallPositionZ() > throwDist)
		model = glm::translate(model, this->getBallPosition());
	else model = glm::translate(model, glm::vec3(this->getBallPositionX(), this->getBallPositionY(), throwDist));
	return model;
}

glm::mat4 Ball::ballThrow(glm::mat4 model, float throwDist)
{
	//this->setBallPositionZ(this->getBallPositionZ() - 0.1f);
	//this->ballPosition.z = this->ballPosition.z - 0.1f;
	model = this->ballAdvance(model, throwDist);
	model = this->ballRotate(model, throwDist);
	return model;
}

glm::mat4 Ball::ballCalibrate(glm::mat4 model)
{
	model = glm::translate(model, this->getBallPosition());
	return model;
}



glm::vec3 Ball::getBallPosition()
{
	return this->ballPosition;
}

float Ball::getBallPositionX()
{
	return this->ballPosition.x;
}

float Ball::getBallPositionY()
{
	return this->ballPosition.y;
}

float Ball::getBallPositionZ()
{
	return this->ballPosition.z;
}

int Ball::getBallThrowTrue()
{
	return this->throwTrue;
}

int Ball::getBallCalibrateTrue()
{
	return this->calibrateTrue;
}



void Ball::setBallPosition(glm::vec3 ballPosition)
{
	this->ballPosition = ballPosition;
}

void Ball::setBallPositionX(float x)
{
	this->ballPosition.x = x;
}

void Ball::setBallPositionY(float y)
{
	this->ballPosition.y = y;
}

void Ball::setBallPositionZ(float z)
{
	this->ballPosition.z = z;
}

void Ball::setBallThrowTrue(int throwTrue)
{
	this->throwTrue = throwTrue;
}

void Ball::setBallCalibrateTrue(int calibrateTrue)
{
	this->calibrateTrue = calibrateTrue;
}
