#version 330

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;

uniform mat4 pvm;
uniform mat4 vm;

out vec3 outNormal;
out vec3 outPos;

void main() {
    outNormal = normal;
    outPos = position;
}