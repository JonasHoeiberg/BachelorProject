#version 330

layout(triangles) in;
layout(triangle_strip, max_vertices=18) out;

in vec4 outPos[];

uniform mat4 pvm[6];
uniform mat4 vm[6];

out vec3 fragPos;

void main() {
	for(int i = 0; i < 6; i++) {
		gl_Layer = i;
		for(int j = 0; j < 3; j++) {
			fragPos = (vm[i] * outPos[j]).xyz;

			gl_Position = pvm[i] * outPos[j];
			EmitVertex();
		}
		EndPrimitive();
	}
}