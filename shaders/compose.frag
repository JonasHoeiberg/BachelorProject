#version 330

in vec2 UV;

uniform sampler2D ao_tex;
uniform sampler2DArray rad_tex;
uniform sampler2DArray diff_texture;

out vec4 fragColor;

void main() {
    float ao = texture(ao_tex,UV).r;
    vec3 rad = texture(rad_tex,vec3(UV,0.0f)).rgb;
    vec3 diff = texture(diff_texture,vec3(UV,0.0f)).rgb;

    fragColor = vec4(rad + 0.1f * diff * ao,1);
}