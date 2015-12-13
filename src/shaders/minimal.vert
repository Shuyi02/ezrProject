#version 330 core

// Input data
layout(location = 0) in vec3 vertexPos_modelspace;
layout(location = 1) in vec2 vertexUV;

// Output data
out vec2 UV;

// Constant data
uniform mat4 mvp;

void main(){

	gl_Position =  mvp * vec4(vertexPos_modelspace,1);
	
	// UV of the vertex
	UV = vertexUV;

}