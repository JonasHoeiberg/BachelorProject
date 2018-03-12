#version 330

#define BIAS 0.001f
#define M_PI 3.1415926535897932384626433832795

in vec2 fragUV;
flat in int layer;

uniform sampler2DArray norm_texture;
uniform sampler2DArray depth_texture;
uniform sampler2DArray diff_texture;
uniform samplerCube shadowMap;

uniform float far_plane;

uniform mat4 viewUnproject;
uniform mat4 invView;

uniform vec3 light_pos;
uniform vec3 light_intensity;

layout(location=0) out vec3 nextBounce;
layout(location=1) out vec3 total;

/*
vec3 regenNormal(vec3 normIn) {

    return normalize(normIn - vec3(1.0));
}
*/
vec3 regenPos(float tex_depth, vec2 winSpace) {
	vec4 v_screen = vec4(winSpace.xy,tex_depth,1.0);
	vec4 v_homo = viewUnproject * (2.0 * (v_screen - vec4(0.5)));

	return v_homo.xyz/v_homo.w;
}

void main() {
    float Z_value = texture(depth_texture,vec3(fragUV.xy,float(layer))).r;

    if(Z_value == 1.0f) {
        nextBounce = vec3(0.0f);
        total = vec3(0.0f);
        discard;
    }

    vec3 viewPos = regenPos(Z_value,fragUV.xy);
    vec4 diff_color = texture(diff_texture,vec3(fragUV.xy,float(layer)));
    vec3 viewNormal = texture(norm_texture,vec3(fragUV.xy,float(layer))).xyz;

    vec3 worldPos = vec4(invView * vec4(viewPos,1.0f)).xyz;
    vec3 worldNorm = mat3(invView) * viewNormal;

    vec3 plVector = light_pos - worldPos;
    vec3 lVector = normalize(plVector);
    float atten = length(plVector);

    float shadowDepth = texture(shadowMap,-lVector).r;

    float depthToCompare = length(plVector)/far_plane;

    float V = 0.0f;

    if(depthToCompare - BIAS <= shadowDepth)
        V = 1.0f;

    vec3 radValue = max(0,dot(lVector,worldNorm))*(vec3(diff_color)/M_PI)*light_intensity; //lambertian

    nextBounce = max(vec3(0),(1.0f/(0.999f + 0.000001f * atten*atten)) * radValue * V);
    total = max(vec3(0),(1.0f/(0.999f + 0.000001f * atten*atten)) * radValue * V);
}