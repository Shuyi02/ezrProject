#version 330 core

layout(location = 0) out vec4 normalDepthColor;

in vec3 normal_camera;

void main(){
	vec3 n = normalize(normal_camera);
	
	//rgb normal, a depth
	normalDepthColor = vec4(n.x, n.y, n.z, gl_FragCoord.z);
}