#include <iostream>

#include "RenderEngine.h"
#include "TextRender.h"
#include "Control.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define WIDTH 640
#define HEIGHT 480

double varWidth = WIDTH;
double varHeight = HEIGHT;

Input input;

void processInput(GLFWwindow* window, int key, int scancode, int action, int mods);
void processMouse(GLFWwindow* window, double x, double y);

int main() {

    bool isFullScreen = false;

    input = Input();

    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(WIDTH, HEIGHT, "Deep G-Buffers",
                              isFullScreen ? glfwGetPrimaryMonitor() : NULL,
                              NULL);

    glfwMakeContextCurrent(window);

    glfwSetCursorPos(window,varWidth/2,varHeight/2);

    glfwSetKeyCallback(window,processInput);
    glfwSetCursorPosCallback(window,processMouse);

    if (!window)
    {
        glfwTerminate();
        return -1;
    }



    std::vector<Message*> messages = std::vector<Message*>();

    Control controller(&input,&messages);

    GLenum err = glewInit();

    if(err != GLEW_OK) {
        std::cout << "ERROR: GLEW did not initialise" << std::endl;
        return false;
    }

    RenderEngine render(&messages,window);
    render.initEngine(WIDTH,HEIGHT);

    glfwSwapInterval(0);

    while (!glfwWindowShouldClose(window)) {
        render.draw();

        input.reset();
        glfwPollEvents();
        controller.update();

        bool isEmpty = true;


        //The rest of the while loop is message reception, handling and killing.
        for(int i = 0; i < messages.size(); i++) {

            if(messages[i] == nullptr)
                continue;

            if(messages[i]->type == EXIT_APP_MSG) {
                glfwSetWindowShouldClose(window, GL_TRUE);
                messages[i]->kill = true;
            }

            if(messages[i]->type == FULL_SCREEN_MSG) {

                messages[i]->kill = true;
            }

            if(messages[i]->type == WINDOW_SIZE_MSG) {
                glm::vec2& size = messages[i]->extractMsgData<glm::vec2>();
                varWidth = (int) size.x;
                varHeight = (int) size.y;

                glfwSetWindowSize(window,varWidth,varHeight);

                render.updateBuffers(window,varWidth,varHeight);

                messages[i]->kill = true;
            }

            if (messages[i]->kill) {
                delete messages[i];
                messages[i] = nullptr;
            }
            if(messages[i] != nullptr)
                isEmpty = false;
        }

        if(isEmpty && messages.size() > 0) {
            messages.clear();
            std::cout << "Messages cleared"<< std::endl;
        }
    }

    glfwTerminate();

    return 0;
}

void processInput(GLFWwindow* window, int key, int scancode, int action, int mods) {

    if(action==GLFW_PRESS && key==GLFW_KEY_TAB)
        input.tab = true;
    if(action==GLFW_PRESS && key==GLFW_KEY_KP_DIVIDE)
        input.frwdslash = true;
    if(action==GLFW_PRESS && (key==GLFW_KEY_PERIOD || key==GLFW_KEY_KP_DECIMAL))
        input.point = true;
    if(action==GLFW_PRESS && key==GLFW_KEY_BACKSPACE)
        input.backspace = true;
    if(action==GLFW_PRESS && key==GLFW_KEY_SPACE)
        input.space = true;
    if(key==GLFW_KEY_LEFT_SHIFT) {
        if(action==GLFW_PRESS)
            input.l_shift = true;
        else if(action==GLFW_RELEASE)
            input.l_shift = false;
    }
    if(action==GLFW_PRESS && (key==GLFW_KEY_ENTER || key==GLFW_KEY_KP_ENTER))
        input.enter = true;

    if((key <= GLFW_KEY_Z) && (key >= GLFW_KEY_A)) {
        if(action==GLFW_PRESS)
            input.characters[key - GLFW_KEY_A] = 2;
        if(action==GLFW_RELEASE)
            input.characters[key - GLFW_KEY_A] = 0;
    }

    if(action==GLFW_PRESS && (
                              ((key <= GLFW_KEY_9) && (key >= GLFW_KEY_0))
                              || ((key <= GLFW_KEY_KP_9) && (key >= GLFW_KEY_KP_0))
                             )
       ) {
        if((key <= GLFW_KEY_9) && (key >= GLFW_KEY_0))
            input.numbers[key - GLFW_KEY_0] = true;
        else
            input.numbers[key - GLFW_KEY_KP_0] = true;
    }
}

void processMouse(GLFWwindow* window, double x, double y) {
    if(glfwGetInputMode(window,GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
        input.mouseMovement.x += x - (varWidth/2);
        input.mouseMovement.y += y - (varHeight/2);

        glfwSetCursorPos(window,varWidth/2,varHeight/2);
    }
}