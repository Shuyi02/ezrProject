#version 330 core

// Input vertex data, different for all executions of this shader.

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 normal_modelspace;

uniform mat4 normalDepth;
uniform mat4 mv_ti;


out vec3 normal_camera;

void main(){
	gl_Position =  normalDepth * vec4(vertexPosition_modelspace,1.);
	normal_camera = (mv_ti * vec4(normal_modelspace, 0.)).xyz;
	
}
