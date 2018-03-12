//
// Created by Jonas on 18-12-2015.
//

#include "light.h"
#include <mat4x4.hpp>
#include <gtx/transform.hpp>
#include <geometric.hpp>
#include <gtc/type_ptr.hpp>
#include "GLError.h"

#define SHADOW_WIDTH 1024
#define SHADOW_HEIGHT 1024

Light::Light(const glm::vec3& position, const glm::vec3& intensity) :
program("../shaders/shadow_gen.vert","../shaders/shadow_gen.frag","../shaders/shadow_gen.geom") {
	this->position = position;
	this->lightIntensity = intensity;

	glGenTextures(1, &shadowDepth);
	glGenFramebuffers(1, &shadowFBO);

	glBindTexture(GL_TEXTURE_CUBE_MAP, shadowDepth);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	for(GLuint i = 0; i < 6; i++) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT32F, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);
	}

	glBindTexture(GL_TEXTURE_CUBE_MAP,0);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER,shadowFBO);

	glFramebufferTexture(GL_DRAW_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,shadowDepth,0);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);

	check_gl_error();
}

void Light::genShadowMap(Camera *cam, std::vector<MeshObject> *meshes) {
	glViewport(0,0,SHADOW_WIDTH,SHADOW_HEIGHT);
	glEnable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER,shadowFBO);
	program.use();

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glm::mat4 projection = glm::perspective(glm::pi<float>()/2.0f,1.0f,cam->nearPlane,cam->farPlane);

    //Chances are something here will have to be swapped around. Let's see.
    glm::mat4 pvms[6] = {projection * glm::lookAt(position,position + glm::vec3(1.0f,0.0f,0.0f),glm::vec3(0.0f,-1.0f,0.0f)),
                         projection * glm::lookAt(position,position + glm::vec3(-1.0f,0.0f,0.0f),glm::vec3(0.0f,-1.0f,0.0f)),
                         projection * glm::lookAt(position,position + glm::vec3(0.0f,1.0f,0.0f),glm::vec3(0.0f,0.0f,1.0f)),
                         projection * glm::lookAt(position,position + glm::vec3(0.0f,-1.0f,0.0f),glm::vec3(0.0f,0.0f,-1.0f)),
                         projection * glm::lookAt(position,position + glm::vec3(0.0f,0.0f,1.0f),glm::vec3(0.0f,-1.0f,0.0f)),
                         projection * glm::lookAt(position,position + glm::vec3(0.0f,0.0f,-1.0f),glm::vec3(0.0f,-1.0f,0.0f))
    };

    glm::mat4 vms[6] = {glm::lookAt(position,position + glm::vec3(1.0f,0.0f,0.0f),glm::vec3(0.0f,-1.0f,0.0f)),
                        glm::lookAt(position,position + glm::vec3(-1.0f,0.0f,0.0f),glm::vec3(0.0f,-1.0f,0.0f)),
                        glm::lookAt(position,position + glm::vec3(0.0f,1.0f,0.0f),glm::vec3(0.0f,0.0f,1.0f)),
                        glm::lookAt(position,position + glm::vec3(0.0f,-1.0f,0.0f),glm::vec3(0.0f,0.0f,-1.0f)),
                        glm::lookAt(position,position + glm::vec3(0.0f,0.0f,1.0f),glm::vec3(0.0f,-1.0f,0.0f)),
                        glm::lookAt(position,position + glm::vec3(0.0f,0.0f,-1.0f),glm::vec3(0.0f,-1.0f,0.0f))
    };


    glUniformMatrix4fv(program.get_U_Location(Shader::U_M_PVM),6,GL_FALSE,glm::value_ptr(pvms[0]));

    glUniformMatrix4fv(program.get_U_Location(Shader::U_M_VM),6,GL_FALSE,glm::value_ptr(vms[0]));

    glUniform1f(program.get_U_Location(Shader::U_F_FARPLANE),cam->farPlane);

    for(std::vector<MeshObject>::iterator iter = meshes->begin(); iter != meshes->end();iter++) {
        glBindVertexArray(iter->getVAO());

        glDrawElements(GL_TRIANGLES,iter->getNoIndices(),GL_UNSIGNED_INT,(void*) 0);
    }

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);

	check_gl_error();
}

GLuint Light::getShadowMap() {
	return shadowDepth;
}

const glm::vec3 &Light::getPosition() {
	return position;
}

const glm::vec3 &Light::getIntensity() {
	return lightIntensity;
}

void Light::setPosition(glm::vec3 position) {
	this->position = position;
}

void Light::setAnimation(glm::vec3 startPoint, glm::vec3 axis, float speed) {
	animAxis = axis;
	animSpeed = speed;
	position = startPoint;
}

void Light::animate(float dt) {
	animTime += dt;

	float rotation = animTime * animSpeed;

	position = glm::vec3(radius * cos(rotation),1.0f,radius * sin(rotation));
}
