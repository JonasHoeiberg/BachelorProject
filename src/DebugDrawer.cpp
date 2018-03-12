//
// Created by Jonas on 15-07-2015.
//

#include "DebugDrawer.h"

#define CONSOLE_X 2
#define CONSOLE_Y 8 //Slightly above the bottom of the screen to accommodate for characters that go under the line

DebugDrawer::DebugDrawer(int width, int height) :
    text("res/fonts/Inconsolata-LGC.otf",16,width,height)
{
    this->width = width;
    this->height = height;
}

DebugDrawer::DebugDrawer() :
    text()
{

}

void DebugDrawer::toggleConsole(std::string *consoleText) {
    consoleActive = !consoleActive;

    if(consoleActive)
        this->consoleText = consoleText;
    else
        consoleText = nullptr;
}

void DebugDrawer::draw() {
    if(consoleActive)
        text.renderString(">" + *consoleText,CONSOLE_X,CONSOLE_Y,glm::vec3(0.5f,0.5f,0.5f));

    int advY = 0;
    for(auto iter = timers.begin(); iter != timers.end(); iter++) {
        text.renderString(iter->first,width - 250, height - 20 - advY,glm::vec3(0,0,0));
        advY += 20;
    }

    advY = 0;
    for(auto iter = timers.begin(); iter != timers.end(); iter++) {
        text.renderString(std::to_string(*(iter->second)*1000.0).substr(0,4) + "ms",
                          width - 60,
                          height - 20 - advY,
                          glm::vec3(0.5,0.5,0.5));
        advY += 20;
    }
}

void DebugDrawer::setWindowSize( unsigned int width, unsigned int height) {
    text.setBufferSize(width,height);
    this->width = width;
    this->height = height;
}

void DebugDrawer::addTimer(std::string description, double *timePtr) {
    timers.push_back(std::pair<std::string,double*>(description,timePtr));
}

void DebugDrawer::clearTimers() {
    timers.clear();
}
