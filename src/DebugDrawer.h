//
// Created by Jonas on 15-07-2015.
//

#ifndef BSCPROJECT_DEBUGDRAWER_H
#define BSCPROJECT_DEBUGDRAWER_H

#include "TextRender.h"
#include <string>
#include <vector>
#include <utility>


class DebugDrawer {
public:
    DebugDrawer(int width, int height);
    DebugDrawer();

    void toggleConsole(std::string* consoleText);

    void draw();
    void setWindowSize(unsigned int width,  unsigned int height);

    void addTimer(std::string description, double* timePtr);
    void clearTimers();

private:
    TextRender text;
    bool consoleActive = false;
    std::string* consoleText;

    unsigned int width, height;

    std::vector<std::pair<std::string,double*>> timers; //first = descriptor, second = time in seconds
};


#endif //BSCPROJECT_DEBUGDRAWER_H
