#version 330

#define FILTER_RADIUS 50

in vec2 fragUV;
flat in int layer;

uniform sampler2DArray prev_bounce;
uniform sampler2DArray depth_texture;
uniform sampler2DArray edge_texture;

layout(location = 0) out vec3 filtered;
layout(location = 1) out vec3 total;

float gaussian(float x, float sigma) {
    return exp(-(x*x)/
                (2 * sigma * sigma));
}

void main() {
    vec3 total_contr = vec3(0);
    float totalWeight = 0.0f;

    float mainWeight = abs(gaussian(0,float(FILTER_RADIUS)/3.0f));
    totalWeight += mainWeight;
    total_contr += mainWeight * max(vec3(0),textureOffset(prev_bounce,vec3(fragUV,float(layer)),ivec2(0,0)).rgb);

    float mainZ = texture(depth_texture,vec3(fragUV,float(layer))).r;

    if(mainZ == 1.0f) {
        filtered = vec3(0,0,0);
        total = vec3(0,0,0);
        return;
    }
/*

    for(int i = -FILTER_RADIUS;i <= FILTER_RADIUS; i++) {
        vec3 reading = max(vec3(0),textureOffset(prev_bounce,vec3(fragUV,float(layer)),ivec2(0,i)).rgb);

        float Z_reading = textureOffset(depth_texture,vec3(fragUV,float(layer)),ivec2(0,i)).r;

        if(Z_reading == 1.0f)
            continue;

        float weight = abs(gaussian(float(i),float(FILTER_RADIUS)/3.0f))
                        * max(0,(1 - sqrt(abs(mainZ - Z_reading))));

        total_contr += weight * reading;
        totalWeight += weight;
    }
*/

    for(int i = -1; i <= 1; i += 2) {
        for(int j = 1; j <= FILTER_RADIUS; j++) {
            float edge = textureOffset(edge_texture,vec3(fragUV,float(layer)),ivec2(0,i*j)).g;

            vec3 reading = max(vec3(0),textureOffset(prev_bounce,vec3(fragUV,float(layer)),ivec2(0,i*j)).rgb);
            float weight = abs(gaussian(float(i*j),float(FILTER_RADIUS)/3.0f));

            total_contr += weight * reading;
            totalWeight += weight;

            if(edge == 1.0f)
                break;
        }
    }


    if(totalWeight <= 0.0f) {
        filtered = vec3(0,0,0);
        total = vec3(0,0,0);
    } else {
        total_contr /= totalWeight;

        filtered = total_contr.rgb;
        total = total_contr.rgb;
    }
}