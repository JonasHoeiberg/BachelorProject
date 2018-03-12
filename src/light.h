//
// Created by Jonas on 18-12-2015.
//

#ifndef BSCPROJECT_LIGHT_H
#define BSCPROJECT_LIGHT_H

#include <GL/glew.h>
#include <vec3.hpp>
#include "shader.h"
#include "Camera.h"
#include "MeshObject.h"
#include <gtx/quaternion.hpp>
#include <vector>


class Light {
public:
    Light(const glm::vec3& position, const glm::vec3& intensity);

    void genShadowMap(Camera *cam, std::vector<MeshObject> *meshes);

    GLuint getShadowMap();
    void setPosition(glm::vec3 position);
    const glm::vec3& getPosition();
    const glm::vec3& getIntensity();

    void setAnimation(glm::vec3 startPoint, glm::vec3 axis, float speed);
    void animate(float dt);
private:
    GLuint shadowDepth, shadowFBO;
    Shader::ShaderProgram program;
    glm::vec3 position;
    glm::vec3 lightIntensity;

    float animSpeed = 1.0f;
    glm::vec3 animAxis;
    float animTime = 0.0f;
    float radius = 3.0f;
};


#endif //BSCPROJECT_LIGHT_H
