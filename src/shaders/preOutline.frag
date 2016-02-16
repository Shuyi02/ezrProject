#version 330 core

//output into an texture
//Ergebniss: out->Textur die Normalmap und Tiefenmap enthält
//normalDepthColor {Normale.x, Normale.y, Normale.z, Tiefenwert}

layout(location = 0) out vec4 normalDepthColor;

in vec3 normal_camera;

void main(){
	vec3 n = normalize(normal_camera);
	
	normalDepthColor = vec4(n.x, n.y, n.z, gl_FragCoord.z); //Textur with Normalmap and Dephtvalue in RGBA (RGB->Normalmap, A->Dephtvalue) 

	
	//weiteres Framebuffer anlegen (enthält eine farb textur-> weitere texur im hauptporgramm) die nachher im hatch.frag anwenden.. siehe outline kram
	//neuer shader, ein weiterer durchlauf nach dem füllen der textur hier, als eingabe bekommt der shader die hier gefüllte textur
	//dann kann ich auf nachbarn zugreifen.. dann kann ich sobel dann ist die welt fast in ordnung :D
	
	
}