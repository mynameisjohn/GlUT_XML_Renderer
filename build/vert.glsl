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
varying vec3 v_Pos;

void main(){
	vec4 w_Pos = MV * vec4(a_Pos, 1);
	gl_Position = P * w_Pos;
	v_Nrm = a_Nrm;
	v_Pos = w_Pos.xyz;
}