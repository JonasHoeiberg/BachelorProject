#version 330

layout(location=0)in vec3 position;
layout(location=1)in vec3 normal;

uniform mat4 pvm;
uniform mat4 vm;

out vec3 vertOut;
out vec3 normOut;

void main() {
    gl_Position = pvm * vec4(position,1);

    vertOut = vec3(vm * vec4(position,1));
    normOut = vec3(vm * vec4(normal,0));
}