# include "ball.h"
# include <math.h>

//frictionCoefficientOnDirt = 0.5;
//gravitationalAcceleration = 9.81;

glm::vec3 xRotate(1.0f, 0.0f, 0.0f);

Ball::Ball()
{
	this->ballPosition = glm::vec3(0.0f, 0.0f, 0.0f);
}

Ball::Ball(glm::vec3 ballPosition, int throwTrue, int calibrateTrue, 
	int spawnTrue, int throwDist, int throwSpeed, int disqualified)
{
	this->ballPosition = ballPosition;
	this->throwTrue = throwTrue;
	this->calibrateTrue = calibrateTrue;
	this->spawnTrue = spawnTrue;
	this->throwDist = throwDist;
	this->throwSpeed = throwSpeed;
	this->disqualified = disqualified;

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


glm::mat4 Ball::ballRotate(glm::mat4 model)
{
	if (this->getBallPositionZ() > -this->throwDist)
	{
		model = glm::rotate(model,(float)glfwGetTime() * -throwSpeed, glm::vec3(1.0f, 0.0f, 0.0f));
		
		this->throwSpeed = sqrt(pow(this->throwSpeed, 2) + pow(this->throwSpeed, 2) / (-2));
	}
	return model;
}

glm::mat4 Ball::ballAdvance(glm::mat4 model)
{
	if (this->getBallPositionZ() > -this->throwDist)
		model = glm::translate(model, this->getBallPosition());
	else model = glm::translate(model, 
		glm::vec3(this->getBallPositionX(), this->getBallPositionY(), -this->throwDist));
	return model;
}

glm::mat4 Ball::ballThrow(glm::mat4 model)
{
	model = this->ballAdvance(model);
	model = this->ballRotate(model);
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

int Ball::getBallSpawnTrue()
{
	return this->spawnTrue;
}

int Ball::getBallThrowDist()
{
	return this->throwDist;
}

int Ball::getBallThrowSpeed()
{
	return this->throwSpeed;
}

int Ball::getBallDisqualified()
{
	return this->disqualified;
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

void Ball::setBallSpawnTrue(int spawnTrue)
{
	this->spawnTrue = spawnTrue;
}

void Ball::setBallThrowDist(int throwDist)
{
	this->throwDist = throwDist;
}

void Ball::setBallThrowSpeed(int throwSpeed)
{
	this->throwSpeed = throwSpeed;
}

void Ball::setBallDisqualified(int disqualified)
{
	this->disqualified = disqualified;
}
