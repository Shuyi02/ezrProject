#version 330 core

layout(location = 0) in vec3 vertexPos_modelspace;
layout(location = 2) in vec3 normal;

uniform mat4 mvp;

out vec3 normal_vert;

void main(){

	gl_Position =  mvp * vec4(vertexPos_modelspace, 1.);
	
	normal_vert = (mvp * vec4(normal, 0.)).xyz;
}