//
// Created by jonas on 7/3/15.
//

#ifndef BSCPROJECT_MESSAGE_H
#define BSCPROJECT_MESSAGE_H

//The message implementation requires the programmer to ensure that the right types are
//used for the DataMessage template both when a message is created and when it is
//received


enum messageType {
    NO_TYPE,
    CLEAR_COLOR_MSG,
    TIMER_MSG,
    CONSOLE_TOGGLE_MSG,
    EXIT_APP_MSG,
    WINDOW_SIZE_MSG,
    FULL_SCREEN_MSG,
    CAM_DESIGNATION_MSG,
    CAM_EXPOSE_MOUSE,
    CAM_SPEED_MSG,
    DRAW_TEXTURE_MSG,
    DRAW_LAYER_MSG,
    TOGGLE_TEST_MSG,
    TEST_BOUNCE_MSG,
    LIGHT_POS_MSG,
    STOP_ANIM_MSG
};

enum camDirection {
    DIR_FORWARD,
    DIR_BACKWARD,
    DIR_LEFT,
    DIR_RIGHT
};

template<typename T> class DataMessage;

class Message {
public:
    Message();
    Message(messageType type);

    //Templated function extracts data based on template argument provided at message recipient
    template<typename T>
    T& extractMsgData() {
        return static_cast<DataMessage<T>*>(this)->data;
    }

    const messageType type;
    bool kill = false;
};

template<typename T>
class DataMessage : public Message {
public:
    DataMessage(messageType type, T data) :
        Message(type),
        data(data)
    {
    }

    T data;
};

//Struct to attach a keyboard button to a camera direction
struct ExposeControlButton {
    ExposeControlButton(int* button, camDirection dir){
        this->button = button;
        this->dir = dir;
    }

    int* button;
    camDirection dir;
};
/*
class VectorMessage : public Message {
public:
    VectorMessage(messageType type, glm::vec3 color);

    const glm::vec3 color;
};

class FloatMessage : public Message {
public:
    FloatMessage(messageType type, float data);

    const float data;
};

class ControlExposeButtonMessage : public Message {
public:
    ControlExposeButtonMessage(camDirection dir, int* button);

    int* button;
    const camDirection dir;
};

class ControlExposeMouseMessage : public Message {
public:
    ControlExposeMouseMessage(glm::vec2* mouseMovement);

    glm::vec2* mouseMovement;
};

class StringMessage : public Message {
public:
    StringMessage(messageType type, std::string string);

    const std::string string;
};

class StringPtrMessage : public Message {
public:
    StringPtrMessage(std::string* consoleText);

    std::string* text;
};
 */


#endif //BSCPROJECT_MESSAGE_H
