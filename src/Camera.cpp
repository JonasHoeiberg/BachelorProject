//
// Created by jonas on 7/20/15.
//

#include "Camera.h"

#include <gtx/quaternion.hpp>
#include <geometric.hpp>
#include <GLFW/glfw3.h>

void Camera::setDesignation(camDirection dir, int* button_ptr) {
    directions[dir] = button_ptr;

    isInitialised = true;

    for(int i = 0; i < 4; i++)
        if(directions[i] == nullptr)
            isInitialised = false;

    if(rotDiff == nullptr)
        isInitialised = false;
}

Camera::Camera(glm::vec3 position, glm::vec3 direction) {
    this->position = position;
    this->direction = glm::normalize(direction);

    for(int i = 0; i < 4; i++)
        directions[i] = nullptr;

    fov = 45.0f;
    nearPlane = 1.0f;
    farPlane = 3000.0f;
}

glm::mat4 Camera::getLookAt() {
    return glm::lookAt(position,position + direction, up);
}

void Camera::update() {

    if(!isInitialised || lock)
        return;

    static double lastTime = glfwGetTime();
    glm::vec3 moveVector = glm::vec3(0,0,0);

    bool moveChange = false;

    if(*(directions[DIR_FORWARD]) > 0) {
        moveVector += direction;
        moveChange = true;
    }
    else if(*(directions[DIR_BACKWARD]) > 0) {
        moveVector -= direction;
        moveChange = true;
    }
    if(*(directions[DIR_RIGHT]) > 0) {
        moveVector -= glm::normalize(glm::cross(up,direction));
        moveChange = true;
    }
    else if(*(directions[DIR_LEFT]) > 0) {
        moveVector += glm::normalize(glm::cross(up,direction));
        moveChange = true;
    }

    double tempTime = glfwGetTime();
    //The difference is probably minimal, but this temp is used to make sure speed is a consistent concept

    if(moveChange) {
        position += ((float) tempTime - (float) lastTime) * speed * glm::normalize(moveVector);
    }

    glm::fquat rotateHor = glm::angleAxis(glm::radians(-rotDiff->x),up);

    glm::vec3 vertRotAxis = glm::normalize(glm::cross(up,direction));
    glm::fquat rotateVert = glm::angleAxis(glm::radians(rotDiff->y),vertRotAxis);

    glm::fquat totalRot = rotateHor * rotateVert;

    direction = glm::normalize(glm::vec3(glm::mat4_cast(totalRot) * glm::vec4(direction,1)));

    *rotDiff = glm::vec2(0);

    lastTime = tempTime;
}

void Camera::setSpeed(float newSpeed) {
    speed = newSpeed;
}

void Camera::setRotationControl(glm::vec2* diff) {
    rotDiff = diff;

    isInitialised = true;

    for(int i = 0; i < 4; i++)
        if(directions[i] == nullptr)
            isInitialised = false;

    if(rotDiff == nullptr)
        isInitialised = false;
}

void Camera::updateDim(unsigned int width, unsigned int height) {
    this->width = width;
    this->height = height;

    projection = glm::perspective(fov,(float) width / (float) height,nearPlane,farPlane);
}

glm::mat4& Camera::getProjection() {
    return projection;
}
