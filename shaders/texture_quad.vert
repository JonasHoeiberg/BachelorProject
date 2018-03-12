#version 330

layout(location=0)in vec3 position;
layout(location=2)in vec2 UVIn;

uniform mat4 pvm;

out vec2 UV;
out vec4 pos;

void main() {
    pos = pvm * vec4(position,1);
    gl_Position = pos;
    UV = UVIn;
}