#version 330 core

layout(location = 0) in vec3 vert_modelspace;
layout(location = 1) in vec3 normal_modelspace;

uniform mat4 mvp;
uniform mat4 mv;
uniform mat4 mv_ti;

out vec3 vertex_pos;
out vec3 normal;

void main(){

	gl_Position =  mvp * vec4(vert_modelspace, 1.);

	vertex_pos = (mv * vec4(vert_modelspace, 1.)).xyz;
	
	normal = (mv_ti * vec4(normal_modelspace, 0.)).xyz;

}