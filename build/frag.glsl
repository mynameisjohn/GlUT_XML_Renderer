#version 120

struct Light{
	int type;
	vec3 pos;
	vec3 dir;
	vec3 intensity;
};

uniform Light L;

varying vec3 v_Nrm;

void main(){
	float nDotVP = max(0, dot(v_Nrm, L.dir));
	gl_FragColor = vec4(nDotVP * L.intensity * vec3(1, 1, 0), 1)+vec4(0.2);
}