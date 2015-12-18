#version 330 core

layout(location = 0) in vec3 vertexPos_modelspace;
layout(location = 1) in vec3 vertexNor_modelspace;
uniform mat4 mvp;
uniform mat4 mv;
uniform mat4 mv_ti;
out vec4 pos;
out vec4 nor;

void main(){

	gl_Position =  mvp * vec4(vertexPos_modelspace,1);
	pos = mv * vec4(vertexPos_modelspace,1);

	nor = mv_ti * vec4(vertexNor_modelspace,1);
}