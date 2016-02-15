#version 330 core

//output into an texture
//layout(location = 0) out vec3 normalDepthColor;
layout(location = 0) out vec4 outlineColor;

in vec3 normal_camera;
in vec4 test;
//input the filled texture from preOutline

void main(){

	// do some soble on textur we filled in preOutline
	// 
	// write 1 in outlineColor if there is no line and 0 if there is a line -> multiply in hatch.frag (fcolor *= texture(outlineColor,suv).xyz;)
	// outlineColor = vec4(xyz, 1.0); 

	

	
	//weiteres Framebuffer anlegen (enthält eine farb textur-> weitere texur im hauptporgramm) die nachher im hatch.frag anwenden.. siehe outline kram
	//neuer shader, ein weiterer durchlauf nach dem füllen der textur in preOutline, als eingabe bekommt der shader die in preOutline gefüllte textur
	//dann kann ich auf nachbarn zugreifen.. dann kann ich sobel dann ist die welt fast in ordnung :D
	
	
}