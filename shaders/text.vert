#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 UV;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 posOut;
out vec2 UVOut;

void main() {
    gl_Position = projection * view * model * vec4(position,1);

    posOut = vec4(model * vec4(position,1)).xyz;
    UVOut = UV;
}