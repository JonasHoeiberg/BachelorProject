#version 330

in vec2 UV;

uniform sampler2DArray texture_uniform;
uniform int layer;

out vec4 fragColor;

void main() {
    fragColor = texture(texture_uniform, vec3(UV,layer)).rgba;
}