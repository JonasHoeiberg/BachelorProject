#version 330

in vec2 UV;

uniform sampler2D texture_uniform;

out vec4 fragColor;

void main() {
    fragColor = texture(texture_uniform, UV).rgba;
}