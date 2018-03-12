//
// Created by jonas on 7/20/15.
//

#ifndef BSCPROJECT_CAMERA_H
#define BSCPROJECT_CAMERA_H

#include "Message.h"

#include <vec3.hpp>
#include <vec2.hpp>
#include <mat4x4.hpp>
#include <gtx/transform.hpp>

class Camera {
public:
    Camera(glm::vec3 position,glm::vec3 direction);

    bool lock = false;

    float fov, farPlane, nearPlane;

    //Function is used to expose to the camera an int expressing
    //whether or not a given directional button is actively being pressed (>0)
    void setDesignation(camDirection dir, int* button_ptr);
    void setRotationControl(glm::vec2* diff);
    void update();

    glm::mat4 getLookAt();
    glm::mat4& getProjection();

    void setSpeed(float newSpeed);
    void updateDim(unsigned int width, unsigned int height);
    const glm::vec3 &getDirection() const {
        return direction;
    }
    const glm::vec3 &getPosition() const {
        return position;
    }
    const glm::vec3 &getUp() const {
        return up;
    }
private:
    glm::vec3 position;
    glm::vec3 direction;
    float speed = 500.0f;

    unsigned int width = 100;
    unsigned int height = 100;

    bool isInitialised = false;
    int* directions[4];
    glm::vec2* rotDiff = nullptr;

    glm::mat4 projection;

    const glm::vec3 up = glm::vec3(0,1,0);
};


#endif //BSCPROJECT_CAMERA_H
