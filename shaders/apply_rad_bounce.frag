#version 420

#ifndef SAMPLES
	#define SAMPLES 11
#endif //SAMPLES

#ifndef RADIUS
	#define RADIUS 350.0
#endif


#define M_PI 3.1415926535897932384626433832795

#define WIN_WIDTH 640.0
#define WIN_HEIGHT 480.0


in vec2 fragUV;
//in vec2 windowSpace;
flat in int layer;

uniform sampler2DArray norm_texture;
uniform sampler2DArray depth_texture;
uniform sampler2DArray diff_texture;
uniform sampler2DArray prev_bounce;
uniform sampler2D noise_tex;
//uniform vec2 samplePoints[SAMPLES];

//uniform float near_plane;
//uniform float far_plane;

uniform mat4 viewUnproject;

layout(location=0) out vec3 nextBounce;
//layout(location=1) out float AO;
//layout(location=1) out vec4 total;

const int tau_array[ ] = {1, 1, 2, 3, 2, 5, 2, 3, 2, 3, 3, 5, 5,
3, 4, 7, 5, 5, 7, 9, 8, 5, 5, 7, 7, 7, 8, 5, 8, 11, 12, 7,
10, 13, 8, 11, 8, 7, 14, 11, 11, 13, 12, 13, 19, 17, 13,
11, 18, 19, 11, 11, 14, 17, 21, 15, 16, 17, 18, 13, 17,
11, 17, 19, 18, 25, 18, 19, 19, 29, 21, 19, 27, 31, 29, 21,
18, 17, 29, 31, 31, 23, 18, 25, 26, 25, 23, 19, 34, 19, 27,
21, 25, 39, 29, 17, 21, 27};

//const vec2 offsets[] = {vec2(0.05,0),vec2(0,0.05),-vec2(0.05,0),-vec2(0,0.05)};

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
    float tau = float(tau_array[SAMPLES - 1]);

    float mainZ = texture(depth_texture,vec3(fragUV.xy,float(layer))).r;

    if(mainZ == 1.0f) {
        nextBounce = vec3(0.0f);
        return;
    }

    vec3 mainPos = regenPos(mainZ,fragUV.xy);
    vec4 mainColor = texture(diff_texture,vec3(fragUV.xy,float(layer)));
    vec3 mainNormal = texture(norm_texture,vec3(fragUV.xy,float(layer))).xyz;

    vec3 totalIrradiance = vec3(0);
    float M = 0.0f;

    float phi = texture(noise_tex,fragUV).r;
/*
    float z[SAMPLES*2];
    vec3 rad[SAMPLES*2];
    vec3 pos[SAMPLES*2];
    //vec3 norm[SAMPLES*2];
    vec2 offSetUV[SAMPLES];

    for(int i = 0; i < SAMPLES; i++) {
        float sigma_i = (float(i) + 0.5f/tau)/float(SAMPLES);
        float theta_i = 2*M_PI*sigma_i*tau + phi;
        vec2 u_i = vec2(cos(theta_i),sin(theta_i));
        float h_i = RADIUS * sigma_i;

        vec2 offSet = vec2(h_i*u_i);

        offSetUV[i] = fragUV.xy + vec2(offSet.x/WIN_WIDTH,offSet.y/WIN_HEIGHT);
    }

    for(int i = 0; i < SAMPLES; i++) {
        for(float j = 0.0f; j <= 1.0f; j += 1.0f) {
            z[2*i + int(j)] = texture(depth_texture,vec3(offSetUV[i],j)).z;
            pos[2*i + int(j)] = regenPos(z[2*i + int(j)],offSetUV[i]);
        }
    }

    for(int i = 0; i < SAMPLES; i++) {
        for(float j = 0.0f; j < 1.0f; j += 1.0f) {
            norm[2*i + int(j)] = texture(norm_texture,vec3(offSetUV[i],j)).xyz;
        }
    }

    for(int i = 0; i < SAMPLES; i++) {
        for(float j = 0.0f; j <= 1.0f; j += 1.0f) {
            rad[2*i + int(j)] = texture(prev_bounce,vec3(offSetUV[i],j)).rgb;
        }
    }

    for(int i = 0; i < SAMPLES; i++) {

        for(float j = 0.0f; j <= 1.0f; j += 1.0f) {

            vec3 omega1 = pos[2*i + int(j)] - mainPos;
            vec3 omega1_dir = normalize(omega1);

            float cos_theta_1 = max(0,dot(omega1_dir,mainNormal));
            //float cos_phi_1 = max(0,dot(-omega1_dir,norm[2*i + int(j)]));

            //float delta_omega_1 = (cos_phi_1/(M_PI * dot(omega1,omega1)));
            float geom1 = cos_theta_1;// * delta_omega_1;

            if(cos_theta_1 > 0 &&
             abs(offSetUV[i].x - 0.5) <= 0.5 && abs(offSetUV[i].y - 0.5) <= 0.5 &&
             z[2*i + int(j)] != 1.0f &&
             dot(omega1,omega1) <= 500.0f*500.0f) {
                M += 1.0f;//delta_omega_1;
                totalIrradiance += rad[2*i + int(j)] * geom1;
            }
        }
    }
*/

    for(int i = 0; i < SAMPLES; i++) {
        float sigma_i = (i + 0.5f/tau)/float(SAMPLES);
        float theta_i = 2*M_PI*sigma_i*tau + phi;
        vec2 u_i = vec2(cos(theta_i),sin(theta_i));
        float h_i = RADIUS * sigma_i;

        vec2 offSet = vec2(h_i*u_i);

        vec2 offSetUV = fragUV.xy + vec2(offSet.x/WIN_WIDTH,offSet.y/WIN_HEIGHT);

        for(float i = 0.0f; i <= 1.0f; i += 1.0f) {

            //Layer 1 calculations
            vec3 rad_1 = texture(prev_bounce,vec3(offSetUV,i)).rgb;
            float z_1 = texture(depth_texture,vec3(offSetUV,i)).r;
            vec3 pos_1 = regenPos(z_1,offSetUV);
            vec3 norm_1 = texture(norm_texture,vec3(offSetUV,i)).rgb;

            vec3 omega1 = pos_1 - mainPos;
            vec3 omega1_dir = normalize(omega1);

            float cos_theta_1 = max(0,dot(omega1_dir,mainNormal));
            float cos_phi_1 = max(0,dot(-omega1_dir,norm_1));

            float r_sq = dot(omega1,omega1);

            float delta_omega_1 = cos_phi_1/r_sq;
            float geom1 = cos_theta_1 * delta_omega_1;

            if( geom1 > 0.0f &&
             abs(offSetUV.x - 0.5) <= 0.5 && abs(offSetUV.y - 0.5) <= 0.5 &&
             z_1 != 1.0f/* &&
             r_sq < pow(1000.0,2)*/) {
                M += delta_omega_1;
                totalIrradiance += rad_1.rgb * geom1;
            }
        }
    }

    if(M > 0.0f)
        totalIrradiance = ((2.0f*M_PI)/M) * totalIrradiance;

    vec3 radiosity = totalIrradiance * (mainColor.rgb/M_PI);

    nextBounce = max(vec3(0),radiosity.rgb);
}