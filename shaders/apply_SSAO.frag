#version 420

#define CONFERENCE

#ifndef SAMPLES
	#define SAMPLES 11
#endif //SAMPLES

#ifndef RADIUS
	#define RADIUS 30.0
#endif


#define M_PI 3.1415926535897932384626433832795

#define WIN_WIDTH 640.0
#define WIN_HEIGHT 480.0

//parameters
#ifdef CONFERENCE
    #define EPSILON 0.00002f
    #define BETA 7*10e-2f
    #define K 1.0f
    #define SIGMA 10.0f
    #define OBSCURANCE_RADIUS 100.0f
#else
    #define EPSILON 0.02f
    #define BETA 10e-4f
    #define K 1.0f
    #define SIGMA 1.0f
    #define OBSCURANCE_RADIUS 0.5f
#endif

in vec2 UV;

uniform sampler2DArray norm_texture;
uniform sampler2DArray depth_texture;
uniform sampler2D noise_tex;

uniform mat4 viewUnproject;

layout(location=0) out float AO;

const int tau_array[ ] = {1, 1, 2, 3, 2, 5, 2, 3, 2, 3, 3, 5, 5,
3, 4, 7, 5, 5, 7, 9, 8, 5, 5, 7, 7, 7, 8, 5, 8, 11, 12, 7,
10, 13, 8, 11, 8, 7, 14, 11, 11, 13, 12, 13, 19, 17, 13,
11, 18, 19, 11, 11, 14, 17, 21, 15, 16, 17, 18, 13, 17,
11, 17, 19, 18, 25, 18, 19, 19, 29, 21, 19, 27, 31, 29, 21,
18, 17, 29, 31, 31, 23, 18, 25, 26, 25, 23, 19, 34, 19, 27,
21, 25, 39, 29, 17, 21, 27};

vec3 regenPos(float tex_depth, vec2 winSpace) {

	vec4 v_screen = vec4(winSpace.xy,tex_depth,1.0);
	vec4 v_homo = viewUnproject * (2.0 * (v_screen - vec4(0.5)));

	return v_homo.xyz/v_homo.w;
}

void main() {
	float tau = float(tau_array[SAMPLES - 1]);

	float mainZ = texture(depth_texture,vec3(UV.xy,0.0f)).r;

	if(mainZ == 1.0f) {
		AO = 1.0f;
		return;
	}

	vec3 mainPos = regenPos(mainZ,UV.xy);
	vec3 mainNormal = texture(norm_texture,vec3(UV.xy,0.0f)).xyz;

	float AO_samples = 0.0f;

	float phi = texture(noise_tex,UV).r;

	float totalAO = 0.0f;

	for(int i = 0; i < SAMPLES; i++) {

		float sigma_i = (i + 0.5f/tau)/float(SAMPLES);
		float theta_i = 2*M_PI*sigma_i*tau + phi;
		vec2 u_i = vec2(cos(theta_i),sin(theta_i));
		float h_i = RADIUS * sigma_i;

		vec2 offSet = vec2(h_i*u_i);
		vec2 offSetUV = UV.xy + vec2(offSet.x/WIN_WIDTH,offSet.y/WIN_HEIGHT);


		float z_1 = texture(depth_texture,vec3(offSetUV,0.0)).r;
		vec3 pos_1 = regenPos(z_1,offSetUV);

		vec3 omega1 = pos_1 - mainPos;
		float geom1 = max(0,dot(omega1,mainNormal));


		float z_2 = texture(depth_texture,vec3(offSetUV,1.0)).r;
		vec3 pos_2 = regenPos(z_2,offSetUV);

		vec3 omega2 = pos_2 - mainPos;
		float geom2 = max(0,dot(omega2,mainNormal));

        if(geom1 > 0.0f &&
         abs(offSetUV.x - 0.5) <= 0.5 && abs(offSetUV.y - 0.5) <= 0.5 &&
         z_1 != 1.0f &&
         length(omega1) < OBSCURANCE_RADIUS) {
            totalAO += max(0,geom1 - mainZ * BETA)/(dot(omega1,omega1) + EPSILON);
            AO_samples += 1.0f;
        }

        if(geom2 > 0.0f &&
        abs(offSetUV.x - 0.5) <= 0.5 && abs(offSetUV.y - 0.5) <= 0.5 &&
        z_2 != 1.0f &&
        length(omega2) < OBSCURANCE_RADIUS) {
            totalAO += max(0,geom2 - mainZ * BETA)/(dot(omega2,omega2) + EPSILON);
            AO_samples += 1.0f;
        }

	}

    if(AO_samples > 0.1f) {
        AO = pow(max(0,
                      1 - (((2*SIGMA)/AO_samples) * totalAO)
                  ),K);
    } else
        AO = 1.0f;
}