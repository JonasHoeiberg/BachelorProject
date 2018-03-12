#version 330

in vec3 vertOut;
in vec3 normOut;

uniform vec4 diff_color;

out vec4 fragColor;

void main() {
    vec3 lightPos = vec3(0,200,-500);

    vec3 lVector = lightPos - vertOut;

    fragColor = vec4((1/(0.000007f*length(lVector)*length(lVector))) * vec3(1,1,1) * vec3(diff_color) * max(dot(normalize(lVector),normOut),0),1);
}