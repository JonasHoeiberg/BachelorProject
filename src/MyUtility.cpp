//
// Created by jonas on 8/5/15.
//

#include "MyUtility.h"

void Timer::startTimer() {
	if(!active)
		return;

	glFlush();
	glFinish();

	startTime = glfwGetTime();
}

void Timer::endTimer() {
	if(!active)
		return;

	glFlush();
	glFinish();

	cumulativeTime += glfwGetTime() - startTime;

	sample++;

	if(sample >= 300) {
		averageTime = cumulativeTime / sample;
		sample = 0;
		cumulativeTime = 0.0;
	}
}

double *Timer::getAvgTimePtr() {
	return &averageTime;
}

std::string& Timer::getName() {
	return friendlyName;
}

Timer::Timer(std::string name) {
	friendlyName = name;
}

void Timer::activate() {
	active = true;
}

void Timer::deactivate() {
	active = false;
}

//Fullscreen quad definitions
glm::mat4 FSQuad::PV = glm::mat4();
GLuint FSQuad::VAO = 0;
GLuint FSQuad::noIndices = 0;

void FSQuad::setupQuad(){


	std::vector<Vertex> vertices = {Vertex(glm::vec3(-0.5f,-0.5f,0),glm::vec3(0,0,-1),glm::vec2(0,0)),
									Vertex(glm::vec3(-0.5f,0.5f,0),glm::vec3(0,0,-1),glm::vec2(0,1)),
									Vertex(glm::vec3(0.5f,-0.5f,0),glm::vec3(0,0,-1),glm::vec2(1,0)),
									Vertex(glm::vec3(0.5f,0.5f,0),glm::vec3(0,0,-1),glm::vec2(1,1))
	};

	std::vector<GLuint> indices = {0,1,2,1,2,3};

	MeshObject quad = MeshObject(vertices,indices);

	VAO = quad.getVAO();

	PV = glm::ortho( -0.5f,0.5f,-0.5f,0.5f,-1.0f,1.0f)*
		 glm::lookAt(glm::fvec3(0,0,0),glm::fvec3(0,0,-1),glm::fvec3(0,1,0));

	noIndices = quad.getNoIndices();
}

GLuint FSQuad::getVAO() {
	return VAO;
}

const glm::mat4 &FSQuad::getPV() {
	return PV;
}

GLuint FSQuad::getNoIndices() {
	return noIndices;
}
