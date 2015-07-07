#version 120

#define NUM_LIGHTS 3

struct Light{
	int type;
	vec3 pos;
	vec3 dir;
	vec3 intensity;
};

uniform Light L[NUM_LIGHTS];

uniform mat4 MV;
uniform mat4 P;

attribute vec3 a_Pos;
attribute vec3 a_Nrm;

varying vec3 v_Nrm;

void main(){
	gl_Position = P * MV * vec4(a_Pos, 1);
	v_Nrm = a_Nrm;
}