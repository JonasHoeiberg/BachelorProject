#version 330

in vec2 fragUV;
flat in int layer;

uniform sampler2DArray norm_texture;
uniform sampler2DArray depth_texture;

uniform float near_plane;
uniform float far_plane;

layout(location = 0) out vec2 edges;

float linearDepth(in float depth){
  return (2.0 * near_plane) / (far_plane + near_plane - depth * (far_plane - near_plane));
}

void main() {
    vec2 results = vec2(0);

    vec3 mainNorm = texture(norm_texture,vec3(fragUV,float(layer))).xyz;
    float mainDepth = linearDepth(texture(depth_texture,vec3(fragUV,float(layer))).r);

    if(mainDepth == 1.0f) {
        edges = vec2(1.0f,1.0f);
        return;
    }

    //horizontal
    vec3 leftNorm = textureOffset(norm_texture,vec3(fragUV,float(layer)),ivec2(-1,0)).xyz;
    float cos_left = dot(mainNorm,leftNorm);
    vec3 rightNorm = textureOffset(norm_texture,vec3(fragUV,float(layer)),ivec2(1,0)).xyz;
    float cos_right = dot(mainNorm,rightNorm);

    float leftDepth = linearDepth(textureOffset(depth_texture,vec3(fragUV,float(layer)),ivec2(-1,0)).r);
    float rightDepth = linearDepth(textureOffset(depth_texture,vec3(fragUV,float(layer)),ivec2(1,0)).r);

    if(cos_left < 0.9f || cos_right < 0.9f ||
       abs(leftDepth - mainDepth) > 0.01f || abs(rightDepth - mainDepth) > 0.01f) {
        results.x = 1.0f;
     }
    else
        results.x = 0.0f;

    //Vertical
    vec3 downNorm = textureOffset(norm_texture,vec3(fragUV,float(layer)),ivec2(0,-1)).xyz;
    float cos_down = dot(mainNorm,downNorm);
    vec3 upNorm = textureOffset(norm_texture,vec3(fragUV,float(layer)),ivec2(0,1)).xyz;
    float cos_up = dot(mainNorm,upNorm);

    float downDepth = linearDepth(textureOffset(depth_texture,vec3(fragUV,float(layer)),ivec2(0,-1)).r);
    float upDepth = linearDepth(textureOffset(depth_texture,vec3(fragUV,float(layer)),ivec2(0,1)).r);

    if(cos_up < 0.9f || cos_down < 0.9f ||
       abs(upDepth - mainDepth) > 0.01f || abs(downDepth - mainDepth) > 0.01f) {
        results.y = 1.0f;
     }
    else
        results.y = 0.0f;

     edges = results;
}