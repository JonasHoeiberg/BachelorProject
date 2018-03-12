//
// Created by jonas on 7/3/15.
//

#include "Message.h"

Message::Message(messageType type) :
type(type)
{

}

Message::Message() :
type(NO_TYPE)
{

}

/*

VectorMessage::VectorMessage(messageType type, glm::vec3 color) :
Message(type),
color(color)
{

}

StringMessage::StringMessage(messageType type, std::string string) :
Message(type),
string(string)
{

}

StringPtrMessage::StringPtrMessage(std::string *consoleText) :
Message(CONSOLE_TOGGLE_MSG),
text(consoleText)
{

}

ControlExposeButtonMessage::ControlExposeButtonMessage(camDirection dir, int *button) :
Message(CAM_DESIGNATION_MSG),
dir(dir),
button(button)
{

}

FloatMessage::FloatMessage(messageType type, float data) :
Message(type),
data(data)
{

}

ControlExposeMouseMessage::ControlExposeMouseMessage(glm::vec2 *mouseMovement) :
Message(CAM_EXPOSE_MOUSE),
mouseMovement(mouseMovement)
{

}
*/