//
// Created by jonas on 7/3/15.
//

#ifndef BSCPROJECT_RENDERENGINE_H
#define BSCPROJECT_RENDERENGINE_H

#include "Message.h"
#include "DebugDrawer.h"
#include "Camera.h"
#include "GBuffer.h"
#include "MyUtility.h"
#include "filters.h"
#include "light.h"

#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
class RenderEngine {
public:
    RenderEngine(std::vector<Message*>*  messages,GLFWwindow* window);
    bool initEngine(int width, int height);
    void draw();

    int getWidth();
    int getHeight();

    void setContext(GLFWwindow* window);
    void updateBuffers(GLFWwindow* context, int width, int height);
private:
    void parseRenderMessages();
    void parseImportFile(const std::string& filePath);
    void renderTextureLevel(GLuint texture, unsigned short int layer);
    void renderComposite();
    void initTextures(int width, int height);

    std::vector<Message*>* messages;
    GLFWwindow* context;

    std::vector<MeshObject> meshes;
    std::vector<Light> lights;

    GLuint backBufferTexture = -1;
    short unsigned int backBufferLayer = 0;
    bool isLayered = false;

    GBuffer buffer;

    double genTime;

    Camera cam;
    bool doTest = false;
    int bounces = 0;
    bool startNewTest = false;

    DebugDrawer dbg;

    Timer time;

    //GLuint radNext = 0;
    GLuint radTot = 0;
    GLuint radPrev = 0;
    GLuint alchAO = 0;
    GLuint edges = 0;
    double dt;
    double prevTime;

    int width = 100;
    int height = 100;
    bool animate = true;
};


#endif //BSCPROJECT_RENDERENGINE_H
