#version 330 core

layout(location = 0) in vec3 vert_modelspace;
layout(location = 1) in vec3 normal_modelspace;
layout(location = 2) in vec2 vertexUV;

uniform mat4 m;
uniform mat4 v;
uniform mat4 p;
uniform mat4 mv_ti;
uniform vec3 lightPos_Model;

out vec3 vertex_camera;
out vec3 normal_camera;
out vec2 uv;
out vec3 lightPos_camera;

void main(){

	gl_Position =  p * v * m * vec4(vert_modelspace, 1.);

	vertex_camera = (v * m * vec4(vert_modelspace, 1.)).xyz;
	
	normal_camera = (mv_ti * vec4(normal_modelspace, 0.)).xyz;
	
	lightPos_camera = (v * m * vec4(lightPos_Model,1.)).xyz;
	
	uv = vertexUV;

}