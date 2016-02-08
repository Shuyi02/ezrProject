#version 330 core

//output into an texture
out float fragmentdepth;

void main(){
	fragmentdepth = gl_FragCoord.z;
}