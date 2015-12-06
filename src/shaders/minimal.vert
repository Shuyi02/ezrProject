#version 330 core

layout(location = 0) in vec3 vertexPos_modelspace;

uniform mat4 mvp;

void main(){

	gl_Position =  mvp * vec4(vertexPos_modelspace,1);

}