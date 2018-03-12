//
// Created by jonas on 7/15/15.
//

#ifndef BSCPROJECT_CONTROL_H
#define BSCPROJECT_CONTROL_H

#include "Message.h"
#include <vector>
#include <vec2.hpp>
#include <vec3.hpp>

struct Input {
    bool tab = false;
    bool frwdslash = false;
    bool l_shift = false;
    bool point = false;
    bool backspace = false;
    bool space = false;
    int characters[26];
    bool numbers[10];
    bool enter = false;

    glm::vec2 mouseMovement = glm::vec2(0);
    //Only one class will be using the mouse at a time,
    //so it will be reset by the receiver when it has taken the data.

    void reset() {
        tab = false;
        frwdslash = false;
        point = false;
        backspace = false;
        space = false;
        enter = false;
        for(int i = 0; i < 10; i++) {
            numbers[i] = false;
        }
    }
};


class Control {
public:
    Control(Input* inputInfo, std::vector<Message*>* messages);

    void update();

private:
    Input* input;
    std::vector<Message*>* messages;
    bool consoleActive = false;
    std::string consoleText;

    void deployCommand();
};


#endif //BSCPROJECT_CONTROL_H
