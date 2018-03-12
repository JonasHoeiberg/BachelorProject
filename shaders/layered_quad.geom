#version 330

layout(triangles) in;
layout(triangle_strip, max_vertices=6) out;

in vec2 UV[];
in vec4 pos[];

//uniform vec2 plane_dim;

out vec2 fragUV;
out vec2 windowSpace;
flat out int layer;

void main() {
    gl_Layer = 0;
    layer = 0;
    for(int i = 0; i < 3; i++) {
        gl_Position = pos[i];
        fragUV = UV[i];
        //windowSpace = (UV[i] - 0.5)*plane_dim;
        EmitVertex();
    }
    EndPrimitive();

    gl_Layer = 1;
    layer = 1;
    for(int i = 0; i < 3; i++) {
        gl_Position = pos[i];
        fragUV = UV[i];
        //windowSpace = (UV[i] - 0.5)*plane_dim;
        EmitVertex();
    }
    EndPrimitive();
}