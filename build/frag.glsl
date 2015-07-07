#version 120

#define NUM_LIGHTS 3

struct Light{
	int type;
	vec3 pos;
	vec3 dir;
	vec3 intensity;
};

uniform Light L[NUM_LIGHTS];

varying vec3 v_Nrm;

void main(){
	vec3 light;
	for (int i = 0; i < NUM_LIGHTS; i++){
		if (L[i].type == 0) // directional
		{
			float nDotVP = max(0, dot(v_Nrm, L[i].dir));
			light += nDotVP * L[0].intensity * vec3(1, 1, 0);
		}
		else if (L[i].type == 1){ // point
			light += L[i].pos*0.0000001; // ?
		}
		else if (L[i].type == 2){ // ambient
			light += L[i].intensity;
		}
	}
	
	gl_FragColor = vec4(light, 1);
}