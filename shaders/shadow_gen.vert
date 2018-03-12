#version 330

layout(location=0) in vec3 position;
out vec4 outPos;

void main() {
    outPos = vec4(position,1.0f);
}