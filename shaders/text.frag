#version 330

in vec3 posOut;
in vec2 UVOut;

uniform sampler2D texture_uniform;
uniform float x_offset;
uniform float glyph_width;
uniform float glyph_height;
uniform vec3 color;

out vec4 fragColor;

void main() {
    vec4 read = texture(texture_uniform,vec2(x_offset + glyph_width * UVOut.x,glyph_height * (1 - UVOut.y)));

    fragColor = vec4(color,read.r);
}