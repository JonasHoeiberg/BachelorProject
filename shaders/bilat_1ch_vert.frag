#version 330

#define FILTER_RADIUS 20

in vec2 UV;

uniform sampler2D texture_uniform;
uniform sampler2DArray depth_texture;

layout(location = 0) out float filtered;

float gaussian(float x, float sigma) {
    return exp(-(x*x)/
                (2 * sigma * sigma));
}

void main() {
    float total_contr = 0.0f;
    float totalWeight = 0.0f;

    float mainZ = texture(depth_texture,vec3(UV,0.0f)).r;

    if(mainZ == 1.0f) {
        filtered = 0.0f;
        return;
    }

    for(int i = -FILTER_RADIUS;i <= FILTER_RADIUS; i++) {
        float reading = textureOffset(texture_uniform,UV,ivec2(0,i)).r;

        float Z_reading = textureOffset(depth_texture,vec3(UV,0.0f),ivec2(0,i)).r;

        if(Z_reading == 1.0f)
            continue;

        float weight = gaussian(float(i),float(FILTER_RADIUS)/3.0f)
                        * max(0,(1 - sqrt(abs(mainZ - Z_reading))));

        total_contr += weight * reading;
        totalWeight += weight;
    }

    if(totalWeight == 0.0f) {
        filtered = 0.0f;
    } else {
        total_contr /= totalWeight;

        filtered = total_contr;
    }
}