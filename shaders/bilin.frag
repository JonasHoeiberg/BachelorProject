#version 330

#define FILTER_RADIUS 5

in vec2 fragUV;
flat in int layer;

uniform sampler2DArray prev_bounce;

layout(location = 0) out vec3 filtered;
layout(location = 1) out vec3 total;

float gaussian(float x, float sigma) {
    return exp(-(x*x)/
                (2 * sigma * sigma));
}

void main() {
    vec3 total_contr = vec3(0);
    float totalWeight = 0.0f;

    vec3 mainReading = texture(prev_bounce,vec3(fragUV,float(layer))).rgb;

    for(int i = -FILTER_RADIUS;i <= FILTER_RADIUS; i++) {
        for(int j = -FILTER_RADIUS;j <= FILTER_RADIUS; j++) {
            vec3 reading = textureOffset(prev_bounce,vec3(fragUV,float(layer)),ivec2(i,j)).rgb;

            float weight = gaussian(length(vec2(float(i),float(j))),2.0f);

            total_contr += weight * reading;
            totalWeight += weight;
        }
    }

    total_contr /= totalWeight;

    filtered = total_contr.rgb;
    total = total_contr.rgb;
}