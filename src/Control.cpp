//
// Created by jonas on 7/15/15.
//

#include <iostream>
#include <functional>
#include "Control.h"

#define FORWARD_BINDING 'w'
#define BACK_BINDING 's'
#define LEFT_BINDING 'a'
#define RIGHT_BINDING 'd'

Control::Control(Input *inputInfo, std::vector<Message*>* messages) {
    this->input = inputInfo;
    this->messages = messages;
    consoleText = std::string();

    messages->push_back(new DataMessage<ExposeControlButton>(CAM_DESIGNATION_MSG,
                                                             ExposeControlButton(&(input->characters[FORWARD_BINDING - 'a']),DIR_FORWARD))
    );
    messages->push_back(new DataMessage<ExposeControlButton>(CAM_DESIGNATION_MSG,
                                                             ExposeControlButton(&(input->characters[BACK_BINDING - 'a']),DIR_BACKWARD))
    );
    messages->push_back(new DataMessage<ExposeControlButton>(CAM_DESIGNATION_MSG,
                                                             ExposeControlButton(&(input->characters[LEFT_BINDING - 'a']),DIR_LEFT))
    );
    messages->push_back(new DataMessage<ExposeControlButton>(CAM_DESIGNATION_MSG,
                                                             ExposeControlButton(&(input->characters[RIGHT_BINDING - 'a']),DIR_RIGHT))
    );

    messages->push_back(new DataMessage<glm::vec2*>(CAM_EXPOSE_MOUSE,&(input->mouseMovement)));
}

void Control::update() {
    if(consoleActive) {
        for(int i = 0; i < 26; i++) {
            if(input->characters[i] == 2) {
                consoleText += ('a' + i);
                input->characters[i] = 1;
            }
        }
        for(int i = 0; i < 10; i++) {
            if(input->numbers[i] && !input->l_shift) {
                consoleText += ('0' + i);
            }
        }
        if(input->space)
            consoleText += ' ';
        if(input->point)
            consoleText += '.';
        if(input->frwdslash || (input->l_shift && input->numbers[7]))
            consoleText += '/';
        if(input->backspace && !consoleText.empty())
            consoleText.erase(consoleText.length() - 1, 1);
        if(input->enter)
            if(consoleActive)
                deployCommand();

    } else {
    }

    if(input->tab) {
        consoleActive = !consoleActive;
        messages->push_back(new DataMessage<std::string*>(CONSOLE_TOGGLE_MSG,&consoleText));
        consoleText.clear();
    }
}

void Control::deployCommand() {
    long baseCommandEnd = consoleText.find(' ');
    std::string baseCommand;
    if(baseCommandEnd > 0)
        baseCommand = consoleText.substr(0,baseCommandEnd);
    else {
        baseCommand = consoleText;
    }

    if(baseCommand == "exit")
        messages->push_back(new Message(EXIT_APP_MSG));
    else if(baseCommand == "fullscreen")
        messages->push_back(new Message(FULL_SCREEN_MSG));
    else if(baseCommand == "setwindowsize") {
        if(baseCommandEnd <= 0)
            return;

        glm::vec2 newSize = glm::vec2();
        long arg1End = consoleText.find(' ',baseCommandEnd + 1);

        if(arg1End <= 0)
            return;

        newSize.x = std::stof(consoleText.substr(baseCommandEnd));
        newSize.y = std::stof(consoleText.substr(arg1End));

        messages->push_back(new DataMessage<glm::vec2>(WINDOW_SIZE_MSG,newSize));
    }
    else if(baseCommand == "setclearcolor") {
        if(baseCommandEnd <= 0)
            return;
        glm::vec3 newClearColor = glm::vec3();
        long arg1End = consoleText.find(' ',baseCommandEnd + 1);
        long arg2End = consoleText.find(' ',arg1End + 1);

        if(arg1End <= 0 || arg2End <= 0)
            return;

        newClearColor.x = std::stof(consoleText.substr(baseCommandEnd));
        newClearColor.y = std::stof(consoleText.substr(arg1End));
        newClearColor.z = std::stof(consoleText.substr(arg2End));

        messages->push_back(new DataMessage<glm::vec3>(CLEAR_COLOR_MSG,newClearColor));
    }
    else if(baseCommand == "setcamspeed") {
        if (baseCommandEnd <= 0)
            return;

        float camSpeed = std::stof(consoleText.substr(baseCommandEnd));

        messages->push_back(new DataMessage<float>(CAM_SPEED_MSG,camSpeed));
    }
    else if(baseCommand == "drawtexture") {
        if(baseCommandEnd <= 0)
            return;

        messages->push_back(new DataMessage<std::string>(DRAW_TEXTURE_MSG,consoleText.substr(baseCommandEnd + 1)));
    }
    else if(baseCommand == "drawlayer") {
        if(baseCommandEnd <= 0)
            return;

        messages->push_back(new DataMessage<float>(DRAW_LAYER_MSG,std::stof(consoleText.substr(baseCommandEnd))));
    }
    else if(baseCommand == "bounces") {
        messages->push_back(new DataMessage<int>(TEST_BOUNCE_MSG,std::stoi(consoleText.substr(baseCommandEnd))));
    }
    else if(baseCommand == "setlightpos") {
        if(baseCommandEnd <= 0)
            return;
        glm::vec3 newLightPos = glm::vec3();
        long arg1End = consoleText.find(' ',baseCommandEnd + 1);
        long arg2End = consoleText.find(' ',arg1End + 1);

        if(arg1End <= 0 || arg2End <= 0)
            return;

        newLightPos.x = std::stof(consoleText.substr(baseCommandEnd));
        newLightPos.y = std::stof(consoleText.substr(arg1End));
        newLightPos.z = std::stof(consoleText.substr(arg2End));

        messages->push_back(new DataMessage<glm::vec3>(LIGHT_POS_MSG,newLightPos));
    }
    else if(baseCommand == "stopanim") {
        messages->push_back(new Message(STOP_ANIM_MSG));
    }


    consoleActive = false;
    consoleText.clear();
    messages->push_back(new DataMessage<std::string*>(CONSOLE_TOGGLE_MSG,&consoleText));
}
