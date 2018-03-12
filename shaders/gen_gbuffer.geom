#version 420

layout(triangles) in;
layout(triangle_strip, max_vertices=6) out;

in vec3 outNormal[];
in vec3 outPos[];

#ifdef TEXTURED
in vec2 outUV[];
out vec2 UVIn;
#endif

uniform mat4 pvm;
uniform mat4 vm;

out vec3 normalIn;
noperspective out vec2 screenUVFrag;
out vec3 screenPosFrag;
flat out int layer;

void main() {
    vec4 position[3] = { pvm * vec4(outPos[0],1.0),
                         pvm * vec4(outPos[1],1.0),
                         pvm * vec4(outPos[2],1.0)
    };

    vec3 normal[3] = {   mat3(vm) * outNormal[0],
                         mat3(vm) * outNormal[1],
                         mat3(vm) * outNormal[2]
    };

    vec2 screenUV[3] =  {vec4((position[0]/position[0].w + 1) / 2).xy,
                         vec4((position[1]/position[1].w + 1) / 2).xy,
                         vec4((position[2]/position[2].w + 1) / 2).xy
    };

    layer = 0;
    gl_Layer = 0;
    for(int i = 0; i < 3; i++) {
        normalIn = normal[i];
        screenUVFrag = screenUV[i];
        gl_Position = position[i];
        EmitVertex();
    }
    EndPrimitive();

    layer = 1;
    gl_Layer = 1;
    for(int i = 0; i < 3; i++) {
        normalIn = normal[i];
        screenUVFrag = screenUV[i];
        gl_Position = position[i];
        EmitVertex();
    }
    EndPrimitive();

}