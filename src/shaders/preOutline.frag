#version 330 core

//output into an texture
//layout(location = 0) out vec3 normalDepthColor;
layout(location = 0) out vec4 normalDepthColor;

in vec3 normal_camera;
in vec4 test;

void main(){
	//normalDepthColor = vec3(1.0,0.0,0.0); //rote outline 
	//normalDepthColor = vec4(normal_camera.x,normal_camera.y,normal_camera.z,gl_FragCoord.z); //rote outline 
	
	vec3 n = normalize(normal_camera);
//	normalDepthColor = vec4(1-n.x,1-n.y,1-n.z,1.0); //rote outline 
//	normalDepthColor = vec4(n.x, n.y, n.z, 1.0); //rote outline 
//	normalDepthColor = vec4(0.0, 0.0, gl_FragCoord.z, 1.0); //rote outline 
	normalDepthColor = vec4(0.0, 0.0, test.z, 1.0); //warum nicht blaustufen sondern 0,899-0,91101 ??
//	normalDepthColor = vec4((2*n.x)-1, (2*n.y)-1, (2*n.z)-1, 1.0); //rote outline 

	//normale 1-normale*255
	//normale und tiefe in textur
	
	//weiteres Framebuffer anlegen (enthält eine farb textur-> weitere texur im hauptporgramm) die nachher im hatch.frag anwenden.. siehe outline kram
	//neuer shader, ein weiterer durchlauf nach dem füllen der textur hier, als eingabe bekommt der shader die hier gefüllte textur
	//dann kann ich auf nachbarn zugreifen.. dann kann ich sobel dann ist die welt fast in ordnung :D
	
	
}