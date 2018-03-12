#version 330

in vec3 fragPos;

uniform float far_plane;

void main() {
	gl_FragDepth = length(fragPos)/far_plane;
}