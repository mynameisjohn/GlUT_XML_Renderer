#version 120

uniform mat4 MV;
uniform mat4 P;

attribute vec3 a_Pos;

void main(){
	gl_Position = P * MV * vec4(a_Pos, 1);
}