#version 330 core

layout(location = 0) in vec4 positionAttribute;
layout(location = 1) in vec3 normalAttribute;
layout(location = 2) in vec2 uv;
uniform mat4 view;
uniform mat4 projection;

out vec4 passPosition;
out vec3 passNormal;
out vec2 passUV;

void main(){
    gl_Position = projection * view *positionAttribute;


	passPosition = view * positionAttribute;
	passNormal  = mat3(transpose(inverse(view)))*normalAttribute;
	passUV = uv;    
}