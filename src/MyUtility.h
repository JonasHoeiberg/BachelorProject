//
// Created by jonas on 8/5/15.
//

#ifndef BSCPROJECT_UTILITY_H
#define BSCPROJECT_UTILITY_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <mat4x4.hpp>
#include "MeshObject.h"
#include <gtx/transform.hpp>

class Timer {
public:
    Timer(std::string name);

    void startTimer();
    void endTimer();
    double* getAvgTimePtr();

    void activate();
    void deactivate();

    std::string& getName();
private:
    unsigned int sample = 0;
    double cumulativeTime = 0.0;
    double averageTime = 0.0;
    double startTime;

    std::string friendlyName;

    bool active = false;
};

class FSQuad {
public:
    static void setupQuad();
    static GLuint getVAO();
    static const glm::mat4& getPV();
    static GLuint getNoIndices();
private:
    static glm::mat4 PV;
    static GLuint VAO;
    static GLuint noIndices;
};

#endif //BSCPROJECT_UTILITY_H
