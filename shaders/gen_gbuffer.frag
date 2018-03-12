#version 330

in vec3 normalIn;
noperspective in vec2 screenUVFrag;
flat in int layer;

uniform sampler2DArray depth_texture;
uniform float near_plane;
uniform float far_plane;

#ifdef TEXTURED
in vec2 UVIn;
uniform sampler2D diff_texture;
#else
uniform vec4 diff_color;
uniform vec4 spec_color;
#endif

layout(location=0) out vec3 normal;
layout(location=1) out vec4 diffColor;
//diffcolor.w contains the sign for the normal z-componentz
layout(location=2) out vec4 specColor;

float linearDepth(in float depth){
  return (2.0 * near_plane) / (far_plane + near_plane - depth * (far_plane - near_plane));
}

void main() {
    #ifdef TEXTURED
    vec4 diff_color = texture(diff_texture,UVIn);
    #endif

    switch(layer) {
        case 0:
            normal = normalize(normalIn).xyz;
            diffColor = vec4(diff_color.xyz,1.0f);
            specColor = vec4(spec_color.xyz,spec_color.w/128.0f);
            break;
        case 1:

            if(linearDepth(gl_FragCoord.z) <= linearDepth(texture(depth_texture,vec3(screenUVFrag,0)).r) + 0.01f)
                discard;
            else {
                normal = normalize(normalIn).xyz;
                diffColor = vec4(diff_color.xyz,1.0f);
                specColor = vec4(spec_color.xyz,spec_color.w/128.0f);
            }
            break;
    }

}

