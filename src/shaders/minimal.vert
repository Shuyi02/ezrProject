#version 330 core

layout(location = 0) in vec3 vert_modelspace;
layout(location = 1) in vec3 normal_modelspace;
layout(location = 3) in vec2 vertexUV;

uniform mat4 mvp;
uniform mat4 mv;
uniform mat4 mv_ti;
uniform sampler2D textureSampler;

out vec3 vertex_pos;
out vec3 normal;
out vec2 uv;

void main(){

	gl_Position =  mvp * vec4(vert_modelspace, 1.);

	vertex_pos = (mv * vec4(vert_modelspace, 1.)).xyz;
	
	normal = (mv_ti * vec4(normal_modelspace, 0.)).xyz;
	
	uv = vertexUV;

}